#include "systemc.h"

template<int reg_addr_size, int mem_addr_size, int data_size, int alu_op_size> class stimulus:public sc_module {
	public:
		//PORTS
		sc_out<bool> clock;
		sc_out<sc_uint<reg_addr_size> > rf_addr_r1, rf_addr_r2, rf_addr_rw;
		sc_out<bool> rf_en_r1, rf_en_r2, rf_en_rw;
		
		sc_out<bool> dm_en_write;
		
		sc_out<bool> al_c1, al_c2, al_c3, al_c4, al_c5, al_c6;
		sc_out<sc_uint<alu_op_size> > al_cop;
		sc_out<sc_uint<data_size> > al_input_imm, al_input_con;

		//CONSTRUCTOR
		SC_HAS_PROCESS(stimulus);

		stimulus(sc_module_name name, int rf_size, int dm_size):sc_module(name), _size_of_rf(rf_size), _size_of_dm(dm_size) {
			SC_THREAD(main);
		}

		//PROCESS
		void main() {
			//ADD r1, r2, r1
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(1, 1, 0, 1, 2, 1);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b011);
			rf_config(1, 1, 1, 1, 2, 1);
			dm_config(0);
			al_input_imm = 0;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);
			
			rf_read();
			
			//ADDI 12, r2
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(0, 1, 0, 0, 2, 2);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 1, 0, 0, 0, 0b011);
			rf_config(0, 1, 1, 0, 2, 2);
			dm_config(0);
			al_input_imm = 12;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);
			
			rf_read();

			//SUB r13, r12
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(1, 1, 0, 13, 12, 12);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 1, 0, 0, 0b011);
			rf_config(1, 1, 1, 13, 12, 12);
			dm_config(0);
			al_input_imm = 0;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			rf_read();

			//SUBI 16, r12
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(0, 1, 0, 0, 12, 12);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 1, 1, 0, 0, 0b011);
			rf_config(0, 1, 1, 0, 12, 12);
			dm_config(0);
			al_input_imm = 16;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			rf_read();

			//CMP r14, r11
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(1, 1, 0, 14, 11, 0);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 1, 0, 0, 0b011);
			rf_config(1, 1, 0, 14, 11, 0);
			dm_config(0);
			al_input_imm = 0;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			rf_read();

			//CMPI 16, r11
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(0, 1, 0, 0, 11, 0);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 1, 1, 0, 0, 0b011);
			rf_config(0, 1, 0, 0, 11, 0);
			dm_config(0);
			al_input_imm = 16;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			rf_read();

			//AND r9, r10
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(1, 1, 0, 9, 10, 10);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b000);
			rf_config(1, 1, 1, 9, 10, 10);
			dm_config(0);
			al_input_imm = 0;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			rf_read();

			//ANDI 0, r10
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(0, 1, 0, 0, 10, 10);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 1, 0, 0, 0, 0b000);
			rf_config(0, 1, 1, 0, 10, 10);
			dm_config(0);
			al_input_imm = 0;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			rf_read();

			//OR r5, r4
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(1, 1, 0, 5, 4, 4);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b001);
			rf_config(1, 1, 1, 5, 4, 4);
			dm_config(0);
			al_input_imm = 0;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			rf_read();

			//ORI 0xFF, r4
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(0, 1, 0, 0, 4, 4);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 1, 0, 0, 0, 0b001);
			rf_config(0, 1, 1, 0, 4, 4);
			dm_config(0);
			al_input_imm = 0x00FF;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			rf_read();

			//XOR r5, r4
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(1, 1, 0, 5, 4, 4);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(1, 1, 1, 5, 4, 4);
			dm_config(0);
			al_input_imm = 0;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			rf_read();

			//XORI 4, r4
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(0, 1, 0, 0, 4, 4);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 1, 0, 0, 0, 0b010);
			rf_config(0, 1, 1, 0, 4, 4);
			dm_config(0);
			al_input_imm = 4;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			rf_read();

			//MOV r6, r5
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(1, 0, 0, 6, 0, 5);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 1, 0, 0, 0, 0, 0b011);
			rf_config(1, 0, 1, 6, 0, 5);
			dm_config(0);
			al_input_imm = 0;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);
			
			rf_read();

			//MOVI 8, r5
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(0, 0, 0, 0, 0, 5);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 1, 1, 0, 0, 0, 0b011);
			rf_config(0, 0, 1, 0, 0, 5);
			dm_config(0);
			al_input_imm = 8;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);
			
			rf_read();

			//LSH r7, r6
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(1, 1, 0, 7, 6, 6);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b100);
			rf_config(1, 1, 1, 7, 6, 6);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);
			
			rf_read();

			//LSHI 7, r6
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(0, 1, 0, 0, 6, 6);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 1, 0, 0, 0, 0b100);
			rf_config(0, 1, 1, 0, 6, 6);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);
			
			rf_read();

			//ASH r15, r7
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(1, 1, 0, 15, 7, 7);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 1, 1, 0, 0b100);
			rf_config(1, 1, 1, 15, 7, 7);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);
			
			rf_read();

			//ASHI 2, r8
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(0, 1, 0, 0, 8, 8);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 1, 1, 1, 0, 0b100);
			rf_config(0, 1, 1, 0, 8, 8);
			dm_config(0);
			al_input_imm = 2;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);
			
			rf_read();

			//LUI 0xFF, r4
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(0, 0, 0, 0, 0, 4);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 1, 1, 0, 0, 0, 0b011);
			rf_config(0, 0, 1, 0, 0, 4);
			dm_config(0);
			al_input_imm = 0xFF00;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);
			
			rf_read();

			//STOR r15, r6		dm[rf[r15]] = rf[r6]
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(1, 1, 0, 15, 6, 0);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(1, 0, 0, 0, 0, 0, 0b101);
			rf_config(1, 1, 0, 15, 6, 0);
			dm_config(1);
			al_input_imm = 0;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);
			
			rf_read();

			//LOAD r6, r15		rf[r6] = dm[rf[r15]]
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(1, 0, 0, 15, 0, 6);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(1, 0, 0, 0, 0, 1, 0b101);
			rf_config(1, 0, 1, 15, 0, 6);
			dm_config(0);
			al_input_imm = 0;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);
			
			rf_read();

			//Jcond r4
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(1, 0, 0, 4, 0, 0);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(1, 0, 0, 0, 0, 0, 0b111);
			rf_config(1, 0, 0, 4, 0, 0);
			dm_config(0);
			al_input_imm = 0;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);
			
			rf_read();

			//JAL r6, r4
			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);
			rf_config(1, 0, 0, 4, 0, 6);
			dm_config(0);
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);

			clock = 0;
			wait(2.5, SC_NS);
			alu_operation(1, 0, 0, 0, 0, 1, 0b111);
			rf_config(1, 0, 1, 4, 0, 6);
			dm_config(0);
			al_input_imm = 0;
			al_input_con = 100;
			wait(2.5, SC_NS);
			clock = 1;
			wait(5, SC_NS);
			
			rf_read();
		}
	private:
		int _size_of_rf;
		int _size_of_dm;
		
		void alu_operation(bool c1, bool c2, bool c3, bool c4, bool c5, bool c6,
						   sc_uint<alu_op_size> cop) {
			al_c1 = c1;
			al_c2 = c2;
			al_c3 = c3;
			al_c4 = c4;
			al_c5 = c5;
			al_c6 = c6;
			al_cop = cop;
		}
		
		void rf_config(bool en_r1, bool en_r2, bool en_rw,
					   sc_uint<reg_addr_size> r1, sc_uint<reg_addr_size> r2, sc_uint<reg_addr_size> rw) {
			rf_en_r1 = en_r1;
			rf_en_r2 = en_r2;
			rf_en_rw = en_rw;
			rf_addr_r1 = r1;
			rf_addr_r2 = r2;
			rf_addr_rw = rw;
		}
		
		void rf_read() {
			for (int i = 0; i < _size_of_rf; i++) {
				clock = 0;
				wait(2.5, SC_NS);
				
				alu_operation(0, 0, 0, 0, 0, 0, 0b010);		//NOP
				rf_en_r1 = 1;
				rf_en_r2 = 0;
				rf_en_rw = 0;
				rf_addr_r1 = i;
				wait(2.5, SC_NS);
				
				clock = 1;
				wait(5, SC_NS);
			}
		}
		
		void dm_config(bool en_write) {
			dm_en_write = en_write;
		}
		
		void dm_read() {
			alu_operation(0, 0, 0, 0, 0, 0, 0b010);		//NOP
			dm_en_write = 0;
			for (int i = 0; i < _size_of_dm; i++) {
				clock = 0;
				wait(2.5, SC_NS);
				
				wait(2.5, SC_NS);
				
				clock = 1;
				wait(5, SC_NS);
			}
		}
};