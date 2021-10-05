#include "systemc.h"
#define PIPELINE_STAGES 5

template<int reg_addr_size, int mem_addr_size, int data_size, int alu_op_size, int alu_psr_size> class con:public sc_module{
    public:
        //PORTS
        sc_in<bool> clock;

        sc_out<sc_uint<mem_addr_size> > pm_pc;
        sc_out<bool> pm_en_read;
        sc_in<sc_uint<data_size> > pm_inst_read;

        sc_out<sc_uint<reg_addr_size> > rf_addr_r1, rf_addr_r2, rf_addr_rw;
        sc_out<bool> rf_en_r1, rf_en_r2, rf_en_rw;

        sc_out<bool> dm_en_write;
        sc_in<sc_uint<data_size> > dm_data_read;

        sc_out<bool> al_c1, al_c2, al_c3, al_c4, al_c5, al_c6;
        sc_out<sc_uint<alu_op_size> > al_cop;
        sc_in<sc_uint<alu_psr_size> > al_output_psr;
        sc_out<sc_uint<data_size> > al_imm, al_input;
        sc_in<sc_uint<data_size> > al_output;


        SC_HAS_PROCESS(con);

        con(sc_module_name name, int pc_init):sc_module(name), _pc(pc_init) {
            SC_METHOD(instruction_fetch);
            sensitive << clock.pos();
            
            SC_METHOD(read);
            sensitive << clock.pos();

            SC_METHOD(alu_execute);
            sensitive << clock.pos();

            SC_METHOD(memory_access);
            sensitive << clock.pos();

            SC_METHOD(writeback);
            sensitive << clock.pos();

            _pc = pc_init;
        }
    private:
        sc_uint<mem_addr_size> _pc, _prev_pc;
        sc_uint<data_size> _instruction[PIPELINE_STAGES];
        sc_uint<alu_psr_size> _psr;
        sc_uint<4> _cond;       //For Jcond, Bcond
        bool bubble[2];


        void instruction_fetch() {
            if (!bubble[0] && !bubble[1]) {
                pm_en_read = 1;
                //pm_pc = _pc;
                for (int i = PIPELINE_STAGES-1; i > 0; i--) {
                    _instruction[i] = _instruction[i-1];
                }
                _instruction[0] = pm_inst_read.read();
                
                sc_uint<4> op_code = _instruction[0].range(15,12);
                sc_uint<4> op_code_ext = _instruction[0].range(7, 4);
                //insert bubble if instruction was LOAD or STOR
                //if (op_code == 0b0100 && (op_code_ext == 0b0000 || op_code_ext == 0b0100)) {
                if (bubble[1] == false) {
                    bubble[0] = true;
                }
            }
            else if (bubble[0]) {
                for (int i = PIPELINE_STAGES-1; i > 0; i--) {
                    _instruction[i] = _instruction[i-1];
                }
                _instruction[0] = 0x0000;
                bubble[0] = false;
                bubble[1] = true;

                sc_uint<4> op_code = _instruction[3].range(15,12);
                sc_uint<data_size> immediate = _instruction[3].range(7, 0);
                sc_uint<mem_addr_size> pc_new = (op_code == 0b1100) ? (sc_uint<data_size>)(_pc + immediate) : al_output.read();
                _psr = al_output_psr;
                _prev_pc = _pc;

                switch(_cond) {
                    case 0b0000:                //EQ
                        if (_psr[2] == 1) {
                            _pc = pc_new;
                        }
                        break;
                    case 0b0001:                //NE
                        if (_psr[2] == 0) {
                            _pc = pc_new;
                        }
                        break;
                    case 0b1101:                //GE
                        if (_psr[2] == 1 || _psr[1] == 1) {
                            _pc = pc_new;
                        }
                        break;
                    case 0b0010:                //CS
                        if (_psr[0] == 1) {
                            _pc = pc_new;
                        }
                        break;
                    case 0b0011:                //CC
                        if (_psr[0] == 0) {
                            _pc = pc_new;
                        }
                        break;
                    case 0b0110:                //GT
                        if (_psr[1] == 1) {
                            _pc = pc_new;
                        }
                        break;
                    case 0b0111:                //LE
                        if (_psr[1] == 0) {
                            _pc = pc_new;
                        }
                        break;
                    case 0b1100:                //LT
                        if (_psr[1] == 0 && _psr[2] == 0) {
                            _pc = pc_new;
                        }
                        break;
                    case 0b1110:                //US
                        _pc = pc_new;
                        break;
                    case 0b1111:
                    default:
                        _pc = _pc + 1;
                        break;
                }
                _cond = 0b1111;
            }
            else {
                for (int i = PIPELINE_STAGES-1; i > 0; i--) {
                    _instruction[i] = _instruction[i-1];
                }
                _instruction[0] = 0x0000;
                bubble[1] = false;

                pm_pc = _pc;
            }
        }

        void read() {
            sc_uint<4> op_code = _instruction[1].range(15,12);
            sc_uint<4> op_code_ext = _instruction[1].range(7, 4);
            sc_uint<4> r_dst = _instruction[1].range(11,8);
            sc_uint<4> r_src = _instruction[1].range(3, 0);

            if(op_code == 0b0000) {
                switch(op_code_ext) {
                    case 0b0101:            //ADD
                    case 0b1001:            //SUB
                    case 0b1011:            //CMP
                    case 0b0001:            //AND
                    case 0b0010:            //OR
                    case 0b0011:            //XOR
                        rf_en_r1 = 1;
                        rf_en_r2 = 1;
                        break;
                    case 0b1101:            //MOV
                        rf_en_r1 = 1;
                        rf_en_r2 = 0;
                        break;
                    case 0b0000:            //NOP
                    default:
                        rf_en_r1 = 0;
                        rf_en_r2 = 0;
                        break;
                }
            }
            else {
                switch(op_code) {
                    case 0b0101:            //ADDI
                    case 0b1001:            //SUBI
                    case 0b1011:            //CMPI
                    case 0b0001:            //ANDI
                    case 0b0010:            //ORI
                    case 0b0011:            //XORI
                        rf_en_r1 = 0;
                        rf_en_r2 = 1;
                        break;
                    case 0b1101:            //MOVI
                    case 0b1111:            //LUI
                    case 0b1100:            //Bcond
                        rf_en_r1 = 0;
                        rf_en_r2 = 0;
                        break;
                    case 0b1000:            //shifts
                        switch(op_code_ext) {
                            case 0b0100:        //LSH
                            case 0b0110:        //ASH
                                rf_en_r1 = 1;
                                rf_en_r2 = 1;
                                break;
                            case 0b0000:        //LSHI
                            case 0b0001:
                            case 0b0010:        //ASHI
                            case 0b0011:
                                rf_en_r1 = 0;
                                rf_en_r2 = 1;
                                break;
                            default:
                                rf_en_r1 = 0;
                                rf_en_r2 = 0;
                                break;
                        }
                        break;
                    case 0b0100:            //data memory and jumps
                        if (op_code_ext == 0b0000) {        //LOAD
                            rf_en_r1 = 1;
                            rf_en_r2 = 0;
                        }
                        else if (op_code_ext == 0b0100) {   //STOR
                            rf_en_r1 = 1;
                            rf_en_r2 = 1;
                        }
                        else if (op_code_ext == 0b1100) {   //Jcond
                            rf_en_r1 = 1;
                            rf_en_r2 = 0;
                        }
                        else if (op_code_ext == 0b1000) {   //JAL
                            rf_en_r1 = 1;
                            rf_en_r2 = 0;
                        }
                        else {
                            rf_en_r1 = 0;
                            rf_en_r2 = 0;
                        }
                        break;
                    default:
                        rf_en_r1 = 0;
                        rf_en_r2 = 0;
                        break;
                }
            }

            rf_addr_r1 = r_src;
            rf_addr_r2 = r_dst;

            //_pipe_signal[1] = 0;
            //_pipe_signal[2] = 1;
        }

        void alu_execute() {
            sc_uint<4> op_code = _instruction[2].range(15,12);
            sc_uint<4> op_code_ext = _instruction[2].range(7, 4);
            sc_uint<data_size> immediate = _instruction[2].range(7, 0);

            if(op_code == 0b0000) {
                switch(op_code_ext) {
                    case 0b0101:            //ADD
                        alu_control_config(0, 0, 0, 0, 0, 0);
                        al_cop = 0b011;
                        break;
                    case 0b1001:            //SUB
                        alu_control_config(0, 0, 1, 0, 0, 0);
                        al_cop = 0b011;
                        break;
                    case 0b1011:            //CMP
                        alu_control_config(0, 0, 0, 1, 0, 0);
                        al_cop = 0b011;
                        break;
                    case 0b0001:            //AND
                        alu_control_config(0, 0, 0, 0, 0, 0);
                        al_cop = 0b000;
                        break;
                    case 0b0010:            //OR
                        alu_control_config(0, 0, 0, 0, 0, 0);
                        al_cop = 0b001;
                        break;
                    case 0b0011:            //XOR
                        alu_control_config(0, 0, 0, 0, 0, 0);
                        al_cop = 0b010;
                        break;
                    case 0b1101:            //MOV
                        alu_control_config(0, 1, 0, 0, 0, 0);
                        al_cop = 0b011;
                        break;
                    case 0b0000:            //NOP
                    default:
                        alu_control_config(0, 0, 0, 0, 0, 0);
                        al_cop = 0b010;
                        break;
                }
            }
            else {
                switch(op_code) {
                    case 0b0101:            //ADDI
                        alu_control_config(0, 0, 1, 0, 0, 0);
                        al_cop = 0b011;
                        immediate = immediate[7] ? (sc_uint<data_size>)(immediate | 0xFF00) : immediate;
                        break;
                    case 0b1001:            //SUBI
                        alu_control_config(0, 0, 1, 1, 0, 0);
                        al_cop = 0b011;
                        immediate = immediate[7] ? (sc_uint<data_size>)(immediate | 0xFF00) : immediate;
                        break;
                    case 0b1011:            //CMPI
                        alu_control_config(0, 0, 1, 1, 0, 0);
                        al_cop = 0b011;
                        immediate = immediate[7] ? (sc_uint<data_size>)(immediate | 0xFF00) : immediate;
                        break;
                    case 0b0001:            //ANDI
                        alu_control_config(0, 0, 1, 0, 0, 0);
                        al_cop = 0b000;
                        break;
                    case 0b0010:            //ORI
                        alu_control_config(0, 0, 1, 0, 0, 0);
                        al_cop = 0b001;
                        break;
                    case 0b0011:            //XORI
                        alu_control_config(0, 0, 1, 0, 0, 0);
                        al_cop = 0b010;
                        break;
                    case 0b1101:            //MOVI
                        alu_control_config(0, 1, 1, 0, 0, 0);
                        al_cop = 0b011;
                        break;
                    case 0b1000:            //shifts
                        switch(op_code_ext) {
                            case 0b0100:        //LSH
                                alu_control_config(0, 0, 0, 0, 0, 0);
                                al_cop = 0b100;
                                break;
                            case 0b0000:        //LSHI
                            case 0b0001:
                                alu_control_config(0, 0, 1, 0, 0, 0);
                                al_cop = 0b100;
                                immediate = immediate[4] ? immediate : (sc_uint<data_size>)(immediate | 0xFFF0);
                                break;
                            case 0b0110:        //ASH
                                alu_control_config(0, 0, 0, 0, 1, 0);
                                al_cop = 0b100;
                                break;
                            case 0b0010:        //ASHI
                            case 0b0011:
                                alu_control_config(0, 0, 1, 0, 1, 0);
                                al_cop = 0b100;
                                immediate = immediate[4] ? immediate : (sc_uint<data_size>)(immediate | 0xFFF0);
                                break;
                            default:
                                alu_control_config(0, 0, 0, 0, 0, 0);
                                al_cop = 0b010;
                                break;
                        }
                        break;
                    case 0b1111:            //LUI
                        alu_control_config(0, 1, 1, 0, 0, 0);
                        al_cop = 0b011;
                        immediate = immediate << 8;
                        break;
                    case 0b0100:            //data memory and jumps
                        if (op_code_ext == 0b0000) {        //LOAD
                            alu_control_config(1, 0, 0, 0, 0, 1);
                            al_cop = 0b101;
                        }
                        else if (op_code_ext == 0b0100) {   //STOR
                            alu_control_config(1, 0, 0, 0, 0, 0);
                            al_cop = 0b101;
                        }
                        else if (op_code_ext == 0b1100) {   //Jcond
                            _cond = _instruction[2].range(11,8);
                            alu_control_config(1, 0, 0, 0, 0, 0);
                            al_cop = 0b111;
                        }
                        else if (op_code_ext == 0b1000) {   //JAL
                            _cond = 0b1110;
                            alu_control_config(1, 0, 0, 0, 0, 1);
                            al_cop = 0b111;
                            al_input = _pc;
                        }
                        else {
                            alu_control_config(0, 0, 0, 0, 0, 0);
                            al_cop = 0b010;
                        }
                        break;
                    case 0b1100:            //Bcond
                        _cond = _instruction[2].range(11,8);
                        alu_control_config(0, 0, 1, 0, 0, 0);
                        al_cop = 0b111;
                        break;
                    default:
                        alu_control_config(0, 0, 0, 0, 0, 0);
                        al_cop = 0b010;
                        break;
                }
            }

            al_imm = immediate;

            //_pipe_signal[2] = 0;
            //_pipe_signal[3] = 1;
        }

        void memory_access() {
            sc_uint<4> op_code = _instruction[2].range(15,12);
            sc_uint<4> op_code_ext = _instruction[2].range(7, 4);

            if(op_code == 0b0000) {
                switch(op_code_ext) {
                    case 0b0101:            //ADD
                    case 0b1001:            //SUB
                    case 0b1011:            //CMP
                    case 0b0001:            //AND
                    case 0b0010:            //OR
                    case 0b0011:            //XOR
                    case 0b1101:            //MOV
                    case 0b0000:            //NOP
                    default:
                        dm_en_write = 0;
                        break;
                }
            }
            else {
                switch(op_code) {
                    case 0b0101:            //ADDI
                    case 0b1001:            //SUBI
                    case 0b1011:            //CMPI
                    case 0b0001:            //ANDI
                    case 0b0010:            //ORI
                    case 0b0011:            //XORI
                    case 0b1101:            //MOVI
                    case 0b1111:            //LUI
                    case 0b1100:            //Bcond
                        dm_en_write = 0;
                        break;
                    case 0b1000:            //shifts
                        switch(op_code_ext) {
                            case 0b0100:        //LSH
                            case 0b0000:        //LSHI
                            case 0b0001:
                            case 0b0110:        //ASH
                            case 0b0010:        //ASHI
                            case 0b0011:
                            default:
                                dm_en_write = 0;
                                break;
                        }
                        break;
                    case 0b0100:            //data memory and jumps
                        if (op_code_ext == 0b0000) {        //LOAD
                            dm_en_write = 0;
                        }
                        else if (op_code_ext == 0b0100) {   //STOR
                            dm_en_write = 1;
                        }
                        else if (op_code_ext == 0b1100) {   //Jcond
                            dm_en_write = 0;
                        }
                        else if (op_code_ext == 0b1000) {   //JAL
                            dm_en_write = 0;
                        }
                        else {
                            dm_en_write = 0;
                        }
                        break;
                    default:
                        dm_en_write = 0;
                        break;
                }
            }

            //_pipe_signal[3] = 0;
            //_pipe_signal[4] = 1;
        }

        void writeback() {
            sc_uint<4> op_code = _instruction[4].range(15,12);
            sc_uint<4> op_code_ext = _instruction[4].range(7, 4);
            sc_uint<4> r_dst = _instruction[4].range(11,8);

            al_c6 = 0;
            if(op_code == 0b0000) {
                switch(op_code_ext) {
                    case 0b0101:            //ADD
                    case 0b1001:            //SUB
                        rf_en_rw = 1;
                        break;
                    case 0b1011:            //CMP
                        rf_en_rw = 0;
                        break;
                    case 0b0001:            //AND
                    case 0b0010:            //OR
                    case 0b0011:            //XOR
                    case 0b1101:            //MOV
                        rf_en_rw = 1;
                        break;
                    case 0b0000:            //NOP
                    default:
                        rf_en_rw = 0;
                        break;
                }
            }
            else {
                switch(op_code) {
                    case 0b0101:            //ADDI
                    case 0b1001:            //SUBI
                        rf_en_rw = 1;
                        break;
                    case 0b1011:            //CMPI
                        rf_en_rw = 0;
                        break;
                    case 0b0001:            //ANDI
                    case 0b0010:            //ORI
                    case 0b0011:            //XORI
                    case 0b1101:            //MOVI
                    case 0b1111:            //LUI
                        rf_en_rw = 1;
                        break;
                    case 0b1100:            //Bcond
                        rf_en_rw = 0;
                        break;
                    case 0b1000:            //shifts
                        switch(op_code_ext) {
                            case 0b0100:        //LSH
                            case 0b0000:        //LSHI
                            case 0b0001:
                            case 0b0110:        //ASH
                            case 0b0010:        //ASHI
                            case 0b0011:
                                rf_en_rw = 1;
                                break;
                            default:
                                rf_en_rw = 0;
                                break;
                        }
                        break;
                    case 0b0100:            //data memory and jumps
                        if (op_code_ext == 0b0000) {        //LOAD
                            rf_en_rw = 1;
                            al_input = dm_data_read.read();
                            al_c6 = 1;
                        }
                        else if (op_code_ext == 0b0100) {   //STOR
                            rf_en_rw = 0;
                        }
                        else if (op_code_ext == 0b1100) {   //Jcond
                            rf_en_rw = 0;
                        }
                        else if (op_code_ext == 0b1000) {   //JAL
                            rf_en_rw = 1;
                            al_c6 = 1;
                        }
                        else {
                            rf_en_rw = 0;
                        }
                        break;
                    default:
                        rf_en_rw = 0;
                        break;
                }
            }

            rf_addr_rw = r_dst;
            //_pipe_signal[4] = 0;
        }

        void alu_control_config(bool c1, bool c2, bool c3, bool c4, bool c5, bool c6) {
            al_c1 = c1;
            al_c2 = c2;
            al_c3 = c3;
            al_c4 = c4;
            al_c5 = c5;
        }
};