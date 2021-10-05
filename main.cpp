#include "rf.cpp"
#include "dm.cpp"
#include "al.cpp"
#include "pm.cpp"
#include "controller.cpp"

#define REG_ADDR_SIZE 4
#define MEM_ADDR_SIZE 16
#define DATA_SIZE 16
#define ALU_OP_SIZE 3
#define PSR_SIZE 3

#include "systemc.h"

template<int program_size> class stimulus:public sc_module {
	public:
		//PORTS
		sc_out<bool> clock;

		//CONSTRUCTOR
		SC_HAS_PROCESS(stimulus);

		stimulus(sc_module_name name):sc_module(name) {
			SC_THREAD(main);
		}

		//PROCESS
		void main() {
			for (int i = 0; i < program_size*4; i++) {
				clock = 0;
				wait(2.5, SC_NS);
				clock = 1;
				wait(2.5, SC_NS);
			}
		}
};

int sc_main(int argc, char* argv[]) {
    //SIGNALS
    sc_signal<bool> clock;
    
	// sc_signal<sc_uint<DATA_SIZE> > data_in;
	// sc_signal<sc_uint<DATA_SIZE> > data_out;

	sc_signal<sc_uint<MEM_ADDR_SIZE> > pm_addr;
	sc_signal<bool> pm_en_read;
	sc_signal<sc_uint<DATA_SIZE> > pm_data_out;

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
	sc_signal<sc_uint<DATA_SIZE> > al_out_mdr;
	
	sc_signal<sc_uint<DATA_SIZE> > con_imm, con_output, con_input;
	
	//LOCAL VAR
	const int rf_data[] = {0, 5, 2, 3, 4, 5, 6, 7,
						   8, 9, 10, 11, 12, 13, 14, 15};
	const int rf_size = sizeof(rf_data) / sizeof(rf_data[0]);
	
	const int dm_data[] = {15, 14, 13, 12, 11, 10, 9, 8,
						   7, 6, 5, 4, 3, 2, 1, 0};
	const int dm_size = sizeof(dm_data) / sizeof(dm_data[0]);

	//PROGRAM 1
	// const int pm_data[] = {0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0100001000000011,
	// 					   0b0100010000000101,
	// 					   0b0101100000010001,
	// 					   0b0000000000000000,
	// 					   0b0000001101010010,
	// 					   0b0100011000000111,
	// 					   0b1001100000001111,
	// 					   0b0100001000000101,
	// 					   0b0000000000000000,
	// 					   0b0000001001011000,
	// 					   0b0100100001000011,
	// 					   0b0100010001000101,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,};

	// PROGRAM 2
	// const int pm_data[] = {0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0000001001010001,
	// 					   0b0000001101010001,
	// 					   0b0000010001010001,
	// 					   0b0000000101010101,
	// 					   0b1011000100001010,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0100000111001010,
	// 					   0b0000000000000000,
	// 					   0b0000000101010110,
	// 					   0b0000000101010111,
	// 					   0b0000000101011000,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,
	// 					   0b0000000000000000,};

	//PROGRAM 3
	const int pm_data[] = {0b0000000000000000,
						   0b0000000000000000,
						   0b0000000000000000,
						   0b0000000000000000,
						   0b0000000000000000,
						   0b0000001001010001,
						   0b0000001101010001,
						   0b0000010001010001,
						   0b0000000101010101,
						   0b1011000100001100,
						   0b0000000000000000,
						   0b0000000000000000,
						   0b0100000111001010,
						   0b0000000000000000,
						   0b0000000101010110,
						   0b0000000101010111,
						   0b0000000101011000,
						   0b0000000000000000,
						   0b0000000000000000,
						   0b0000000000000000,
						   0b0000000000000000,
						   0b0000000000000000,};
	const int pm_size = sizeof(pm_data) / sizeof(pm_data[0]);

    //MODULES
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
    alu.output_mdr(dm_data_in);

	prgm_mem<pm_size, MEM_ADDR_SIZE, DATA_SIZE> prgm_mem("prgm_mem", &pm_data[0]);
	prgm_mem.clock(clock);
	prgm_mem.addr(pm_addr);
	prgm_mem.en_read(pm_en_read);
	prgm_mem.data_out(pm_data_out);

	con<REG_ADDR_SIZE, MEM_ADDR_SIZE, DATA_SIZE, ALU_OP_SIZE, PSR_SIZE> con("controller", 0);
	con.clock(clock);
	con.pm_pc(pm_addr);
	con.pm_en_read(pm_en_read);
	con.pm_inst_read(pm_data_out);
	con.rf_addr_r1(rf_addr_r1);
	con.rf_addr_r2(rf_addr_r2);
	con.rf_addr_rw(rf_addr_rw);
	con.rf_en_r1(rf_en_r1);
	con.rf_en_r2(rf_en_r2);
	con.rf_en_rw(rf_en_rw);
	con.dm_en_write(dm_en_write);
	con.dm_data_read(dm_data_out);
	con.al_c1(al_c1);
    con.al_c2(al_c2);
    con.al_c3(al_c3);
    con.al_c4(al_c4);
    con.al_c5(al_c5);
    con.al_c6(al_c6);
	con.al_cop(al_cop);
	con.al_output_psr(al_output_psr);
	con.al_imm(con_imm);
	con.al_input(con_output);
	con.al_output(con_input);

	stimulus<pm_size> stimulus("stim");
	stimulus.clock(clock);

    //TRACES
    sc_trace_file* tf = sc_create_vcd_trace_file("cpu_trace");
    sc_trace(tf, clock, "clock");
	sc_trace(tf, pm_addr, "pm_addr");
	sc_trace(tf, pm_en_read, "pm_en_read");
	sc_trace(tf, pm_data_out, "pm_data_out");
	sc_trace(tf, rf_en_r1, "rf_en_r1");
    sc_trace(tf, rf_addr_r1, "rf_addr_r1");
    sc_trace(tf, rf_data_r1, "rf_data_r1");
    sc_trace(tf, rf_en_r2, "rf_en_r2");
    sc_trace(tf, rf_addr_r2, "rf_addr_r2");
    sc_trace(tf, rf_data_r2, "rf_data_r2");
    sc_trace(tf, rf_en_rw, "rf_en_rw");
    sc_trace(tf, rf_addr_rw, "rf_addr_rw");
    sc_trace(tf, rf_data_rw, "rf_data_rw");
    sc_trace(tf, dm_addr, "dm_addr");
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
	cout << "PROGRAM 3" << endl;
	cout << "Before: " << endl;
	cout << "PSR: ";
	alu.psr_print();
	cout << "RF: ";
	reg_file.rf_read();
	cout << "DM: ";
	data_mem.dm_read();

    sc_start(3.5*pm_size*5, SC_NS);

	cout << "After: " << endl;
	cout << "PSR: ";
	alu.psr_print();
	cout << "RF: ";
	reg_file.rf_read();
	cout << "DM: ";
	data_mem.dm_read();
    
    sc_close_vcd_trace_file(tf);

    return 0;
}
