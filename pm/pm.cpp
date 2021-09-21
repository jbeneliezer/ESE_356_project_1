#include "systemc.h"

template<int size, int addr_size, int data_size> class prgm_mem:public sc_module {
	public:
		//PORTS
		sc_in<bool> clock;
		sc_in<sc_uint<addr_size> > addr;
		sc_in<bool> en_read;
		sc_out<sc_uint<data_size> > data_out;
		
		//CONSTRUCTOR
		SC_HAS_PROCESS(prgm_mem);
		
		prgm_mem(sc_module_name name, const int* data_ptr):sc_module(name), _data_ptr(data_ptr) {
			SC_METHOD(mem_read);
			sensitive << clock.pos();
			
			for (int i = 0; i < size; i++) {
				mem_data[i] = *(_data_ptr + i);	//tranfer data from main program to program memory
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
};
