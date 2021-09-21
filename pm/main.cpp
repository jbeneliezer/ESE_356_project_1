#include "pm.cpp"

#define ADDR_SIZE 16
#define DATA_SIZE 16

template<int addr_size, int data_size> class stimulus:public sc_module {
	public:
		//PORTS
		sc_out<bool> clock;
		sc_out<sc_uint<addr_size> > addr;
		sc_out<bool> en_read;

		//CONSTRUCTOR
		SC_HAS_PROCESS(stimulus);

		stimulus(sc_module_name name, int size):sc_module(name), _size_of_test_data(size) {
			SC_THREAD(main);
		}

		//PROCESS
		void main() {
			en_read = 1;
			for (int i = 0; i < _size_of_test_data; i++) {
				clock = 0;
				wait(2.5, SC_NS);
				
				addr = i;
				wait(2.5, SC_NS);
				
				clock = 1;
				wait(5, SC_NS);
			}
		}

		private:
			int _size_of_test_data;
};

template<int data_size> class response:public sc_module {
	public:
		//PORTS
		sc_in<sc_uint<data_size> > data_out;

		//CONSTRUCTOR
		SC_HAS_PROCESS(response);
		
		response(sc_module_name  name):sc_module(name) {
			SC_METHOD(main);
			sensitive << data_out;
		}

		//PROCESS
		void main() {
			cout << "At time: " << sc_time_stamp()
				 << " | Output data: " << data_out.read()
				 << endl;
		}
};

int sc_main(int argc, char* argv[]) {
    //SIGNALS
	sc_signal<bool> clock;
	sc_signal<sc_uint<ADDR_SIZE> > addr;
	sc_signal<bool> en_read;
	sc_signal<sc_uint<DATA_SIZE> > data_out;
	
	//LOCAL VAR
	const int data[] = {0, 12, 24, 24, 30, 0, 0, 0,
						12, 34, 56, 0, -24, -12, 0, 1};
	const int size = sizeof(data) / sizeof(data[0]);

    //MODULES
    prgm_mem<size, ADDR_SIZE, DATA_SIZE> prgm_mem("prgm_mem", &data[0]);
	prgm_mem.clock(clock);
	prgm_mem.addr(addr);
	prgm_mem.en_read(en_read);
	prgm_mem.data_out(data_out);

	stimulus<ADDR_SIZE, DATA_SIZE> stim("stim", size);
	stim.clock(clock);
	stim.addr(addr);
	stim.en_read(en_read);

	response<DATA_SIZE> resp("resp");
	resp.data_out(data_out);

    //TRACES
    sc_trace_file* tf = sc_create_vcd_trace_file("pm_trace");
    sc_trace(tf, clock, "clock");
    sc_trace(tf, addr, "addr");
    sc_trace(tf, en_read, "en_read");
	sc_trace(tf, data_out, "data_out");

    //START SIM
    sc_start(size * 10, SC_NS);
    
    sc_close_vcd_trace_file(tf);

    return 0;
}
