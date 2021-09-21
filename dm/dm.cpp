#include "systemc.h"

template<int size, int addr_size, int data_size> class data_mem:public sc_module {
	public:
		//PORTS
		sc_in<bool> clock;
		sc_in<sc_uint<addr_size> > addr;
		sc_in<bool> en_read, en_write;
		sc_in<sc_uint<data_size> > data_in;
		sc_out<sc_uint<data_size> > data_out;
		
		//CONSTRUCTOR
		SC_HAS_PROCESS(data_mem);
		
		data_mem(sc_module_name name, const int* data_ptr):sc_module(name), _data_ptr(data_ptr) {
			SC_METHOD(mem_read);
			sensitive << clock.pos();

			SC_METHOD(mem_write);
			sensitive << clock.neg();
			
			for (int i = 0; i < size; i++) {
				mem_data[i] = *(_data_ptr + i);	//tranfer data from main program to data memory
			}
		}
	private:
		//LOCAL VAR
		const int* _data_ptr;					//pointer to received data
		sc_uint<data_size> mem_data[size];		//memory data
		
		//PROCESS
		void mem_read() {
			if (en_read.read()) {
				data_out = mem_data[addr.read()];
			}
		}
		
		void mem_write() {
			if (en_write.read()) {
				mem_data[addr.read()] = data_in.read();
			}
		}
};
