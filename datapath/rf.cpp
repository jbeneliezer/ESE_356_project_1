#include "systemc.h"

template<int size, int addr_size, int data_size> class reg_file:public sc_module {
	public:
		//PORTS
		sc_in<bool> clock;
		sc_in<sc_uint<addr_size> > addr_r1, addr_r2, addr_rw;
		sc_in<bool> en_r1, en_r2, en_rw;
		sc_out<sc_uint<data_size> > data_r1, data_r2;
		sc_in<sc_uint<data_size> > data_rw;
		
		//CONSTRUCTOR
		SC_HAS_PROCESS(reg_file);
		
		reg_file(sc_module_name name, const int* data_ptr):sc_module(name), _data_ptr(data_ptr) {
			SC_METHOD(r1_read);
			sensitive << clock.pos();
			
			SC_METHOD(r2_read);
			sensitive << clock.pos();
			
			SC_METHOD(rw_write);
			sensitive << clock.neg();
			
			for (int i = 0; i < size; i++) {
				reg_data[i] = *(_data_ptr + i);	//tranfer data from main program to registers
			}
		}
	private:
		//LOCAL VAR
		const int* _data_ptr;				//pointer to received data
		sc_uint<data_size> reg_data[size];			//16-bit data registers
		
		//PROCESS
		void r1_read() {
			if (en_r1.read()) {
				data_r1.write(reg_data[addr_r1.read()]);
			}
		}
		
		void r2_read() {
			if (en_r2.read()) {
				data_r2.write(reg_data[addr_r2.read()]);
			}
		}
		
		void rw_write() {
			if (en_rw.read()) {
				reg_data[addr_rw.read()] = data_rw.read();
			}
		}
};
