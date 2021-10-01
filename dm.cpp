#include "systemc.h"

template<int size, int addr_size, int data_size> class data_mem:public sc_module {
	public:
		//PORTS
		sc_in<bool> clock;
		sc_in<sc_uint<addr_size> > addr;
		sc_in<bool> en_write;					//0 = read mode, 1 = write mode
		sc_in<sc_uint<data_size> > data_in;
		sc_out<sc_uint<data_size> > data_out;
		
		//CONSTRUCTOR
		SC_HAS_PROCESS(data_mem);
		
		data_mem(sc_module_name name, const int* data_ptr):sc_module(name), _data_ptr(data_ptr) {
			SC_METHOD(mem_access);
			sensitive << clock.pos();
			
			for (int i = 0; i < size; i++) {
				mem_data[i] = *(_data_ptr + i);	//tranfer data from main program to data memory
			}
		}
	private:
		//LOCAL VAR
		const int* _data_ptr;					//pointer to received data
		sc_uint<data_size> mem_data[size];		//memory data
		
		//PROCESS
		void mem_access() {
			if (en_write.read()) {
				if (addr.read() < size)
					mem_data[addr.read()] = data_in.read();
			}
			else {
				if (addr.read() < size)
					data_out = mem_data[addr.read()];
			}
		}
};
