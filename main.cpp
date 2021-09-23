#include "rf.cpp"
#include "dm.cpp"
#include "pm.cpp"
#include "al.cpp"

#define REG_ADDR_SIZE 4
#define MEM_ADDR_SIZE 16
#define DATA_SIZE 16
#define ALU_OP_SIZE 3
#define PSR_SIZE 3

template<int reg_addr_size, int mem_addr_size, int data_size> class stimulus:public sc_module {
	public:
		//PORTS
		sc_out<bool> clock;
		sc_out<sc_uint<reg_addr_size> > rf_addr_r1, rf_addr_r2, rf_addr_rw;
		sc_out<bool> rf_en_r1, rf_en_r2, rf_en_rw;
		sc_out<sc_uint<data_size> > rf_data_rw;
		
		sc_out<sc_uint<mem_addr_size> > dm_addr;
		sc_out<bool> dm_en_write;
		sc_out<sc_uint<data_size> > dm_data_in;
		
		sc_out<bool> al_c1, al_c2, al_c3, al_c4, al_c5, al_c6;
		sc_out<sc_uint<ALU_OP_SIZE> > al_cop;
		sc_out<sc_uint<DATA_SIZE> > al_inout_mdr;
		sc_out<sc_uint<DATA_SIZE> > al_input_imm, al_input_con;

		//CONSTRUCTOR
		SC_HAS_PROCESS(stimulus);

		stimulus(sc_module_name name):sc_module(name) {
			SC_THREAD(main);
		}

		//PROCESS
		void main() {
			
		}
};

int sc_main(int argc, char* argv[]) {
    //SIGNALS
    sc_signal<bool> clock;
    
	sc_signal<sc_uint<REG_ADDR_SIZE> > rf_addr_r1, rf_addr_r2, rf_addr_rw;
	sc_signal<bool> rf_en_r1, rf_en_r2, rf_en_rw;
	sc_signal<sc_uint<DATA_SIZE> > rf_data_r1, rf_data_r2;
	sc_signal<sc_uint<DATA_SIZE> > rf_data_rw;
	
	sc_signal<sc_uint<MEM_ADDR_SIZE> > dm_addr;
	sc_signal<bool> dm_en_write;
	sc_signal<sc_uint<DATA_SIZE> > dm_data_in;
	sc_signal<sc_uint<DATA_SIZE> > dm_data_out;
	
	sc_signal<bool> al_c1, al_c2, al_c3, al_c4, al_c5, al_c6;
	sc_signal<sc_uint<ALU_OP_SIZE> > al_cop;
	sc_signal<sc_uint<PSR_SIZE> > al_output_psr;
	sc_signal<sc_uint<DATA_SIZE> > al_inout_mdr;
	
	sc_signal<sc_uint<DATA_SIZE> > con_imm, con_output, con_input;
	
	//LOCAL VAR
	const int rf_data[] = {0, 12, 24, 24, 30, 0, 0, 0,
						12, 34, 56, 0, -24, -12, 0, 1};
	const int rf_size = sizeof(rf_data) / sizeof(rf_data[0]);
	
	const int dm_data[] = {0, 1, 2, 3, 4, 5, 6, 7,
						8, 9, 10, 11, 12, 13, 14, 15};
	const int dm_size = sizeof(dm_data) / sizeof(dm_data[0]);

    //MODULES
    alu<ALU_OP_SIZE, PSR_SIZE, MEM_ADDR_SIZE, DATA_SIZE> alu("alu");
    alu.clock(clock);
    alu.c1(al_c1);
    alu.c2(al_c2);
    alu.c3(al_c3);
    alu.c4(al_c4);
    alu.c5(al_c5);
    alu.c6(al_c6);
    alu.cop(al_cop);
    alu.output_psr(al_output_psr);
    alu.input_r1(rf_data_r1);
    alu.input_r2(rf_data_r2);
    alu.output_rw(rf_data_rw);
    alu.input_imm(con_imm);
    alu.input_con(con_output);
    alu.output_con(con_input);
    alu.output_mar(dm_addr);
    alu.inout_mdr(dm_data_in);
    alu.inout_mdr(dm_data_out);
    
    reg_file<rf_size, REG_ADDR_SIZE, DATA_SIZE> reg_file("reg_file", &rf_data[0]);
	reg_file.clock(clock);
	reg_file.addr_r1(rf_addr_r1);
	reg_file.addr_r2(rf_addr_r2);
	reg_file.addr_rw(rf_addr_rw);
	reg_file.en_r1(rf_en_r1);
	reg_file.en_r2(rf_en_r2);
	reg_file.en_rw(rf_en_rw);
	reg_file.data_r1(rf_data_r1);
	reg_file.data_r2(rf_data_r2);
	reg_file.data_rw(rf_data_rw);
	
	data_mem<dm_size, MEM_ADDR_SIZE, DATA_SIZE> data_mem("data_mem", &dm_data[0]);
	data_mem.clock(clock);
	data_mem.addr(dm_addr);
	data_mem.en_write(dm_en_write);
	data_mem.data_in(dm_data_in);
	data_mem.data_out(dm_data_out);

	stimulus<REG_ADDR_SIZE, MEM_ADDR_SIZE, DATA_SIZE> stim("stim");
	stim.clock(clock);
	stim.rf_addr_r1(rf_addr_r1);
	stim.rf_addr_r2(rf_addr_r2);
	stim.rf_addr_rw(rf_addr_rw);
	stim.rf_en_r1(rf_en_r1);
	stim.rf_en_r2(rf_en_r2);
	stim.rf_en_rw(rf_en_rw);
	stim.rf_data_rw(rf_data_rw);
	stim.dm_addr(dm_addr);
	stim.dm_en_write(dm_en_write);
	stim.dm_data_in(dm_data_in);
    stim.al_c1(al_c1);
    stim.al_c2(al_c2);
    stim.al_c3(al_c3);
    stim.al_c4(al_c4);
    stim.al_c5(al_c5);
    stim.al_c6(al_c6);
    stim.al_cop(al_cop);
    stim.al_input_imm(con_imm);
    stim.al_input_con(con_output);

    //TRACES
    sc_trace_file* tf = sc_create_vcd_trace_file("datapath_trace");
    sc_trace(tf, clock, "clock");
	sc_trace(tf, rf_en_r1, "rf_en_r1");
    sc_trace(tf, rf_addr_r1, "rf_addr_r1");
    sc_trace(tf, rf_data_r1, "rf_data_r1");
    sc_trace(tf, rf_en_r2, "rf_en_r2");
    sc_trace(tf, rf_addr_r2, "rf_addr_r2");
    sc_trace(tf, rf_data_r2, "rf_data_r2");
    sc_trace(tf, rf_en_rw, "rf_en_rw");
    sc_trace(tf, rf_addr_rw, "rf_addr_rw");
    sc_trace(tf, rf_data_rw, "rf_data_rw");
    sc_trace(tf, dm_addr, "addr");
	sc_trace(tf, dm_en_write, "dm_en_write");
    sc_trace(tf, dm_data_in, "dm_data_in");
	sc_trace(tf, dm_data_out, "dm_data_out");
	sc_trace(tf, al_c1, "al_c1");
	sc_trace(tf, al_c2, "al_c2");
	sc_trace(tf, al_c3, "al_c3");
	sc_trace(tf, al_c4, "al_c4");
	sc_trace(tf, al_c5, "al_c5");
	sc_trace(tf, al_c6, "al_c6");
	sc_trace(tf, al_cop, "al_cop");
	sc_trace(tf, al_output_psr, "al_psr");
	sc_trace(tf, con_imm, "alu_imm");
	sc_trace(tf, con_output, "alu_input_con");
	sc_trace(tf, con_input, "alu_output_con");

    //START SIM
    sc_start(10 * 10, SC_NS);
    
    sc_close_vcd_trace_file(tf);

    return 0;
}
