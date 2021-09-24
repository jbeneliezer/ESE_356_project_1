#include "rf.cpp"

#define ADDR_SIZE 4
#define DATA_SIZE 16

template<int addr_size, int data_size> class stimulus:public sc_module {
	public:
		//PORTS
		sc_out<bool> clock;
		sc_out<sc_uint<addr_size> > addr_r1, addr_r2, addr_rw;
		sc_out<bool> en_r1, en_r2, en_rw;
		sc_out<sc_uint<data_size> > data_rw;

		//CONSTRUCTOR
		SC_HAS_PROCESS(stimulus);

		stimulus(sc_module_name name, int size):sc_module(name), _size_of_test_data(size) {
			SC_THREAD(main);
		}

		//PROCESS
		void main() {
			en_r1 = 1;
			en_r2 = 1;
			en_rw = 0;
			for (int i = 1; i < _size_of_test_data; i++) {
				clock = 0;
				wait(2.5, SC_NS);
				
				addr_r1 = i;
				addr_r2 = i-1;
				addr_rw = i;
				wait(2.5, SC_NS);
				
				clock = 1;
				wait(5, SC_NS);
			}
			en_r1 = 0;
			en_r2 = 0;
			en_rw = 1;
			for (int i = 1; i < _size_of_test_data; i++) {
				clock = 0;
				wait(2.5, SC_NS);
				
				addr_rw = i;
				data_rw = (i * 2) % 0xFFFF;
				wait(2.5, SC_NS);
				
				clock = 1;
				wait(5, SC_NS);
			}
			en_r1 = 1;
			en_r2 = 1;
			en_rw = 0;
			for (int i = 1; i < _size_of_test_data; i++) {
				clock = 0;
				wait(2.5, SC_NS);
				
				addr_r1 = i;
				addr_r2 = i-1;
				addr_rw = i;
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
		sc_in<sc_uint<data_size> > data_r1, data_r2;

		//CONSTRUCTOR
		SC_HAS_PROCESS(response);
		
		response(sc_module_name  name):sc_module(name) {
			SC_METHOD(main);
			sensitive << data_r1 << data_r2;
		}

		//PROCESS
		void main() {
			cout << "At time: " << sc_time_stamp()
				 << " | Output data_r1: " << data_r1.read()
				 << " | Output data_r2: " << data_r2.read()
				 << endl;
		}
};

int sc_main(int argc, char* argv[]) {
    //SIGNALS
    sc_signal<bool> clock;
	sc_signal<sc_uint<ADDR_SIZE> > addr_r1, addr_r2, addr_rw;
	sc_signal<bool> en_r1, en_r2, en_rw;
	sc_signal<sc_uint<DATA_SIZE> > data_r1, data_r2;
	sc_signal<sc_uint<DATA_SIZE> > data_rw;
	
	//LOCAL VAR
	const int data[] = {0, 12, 24, 24, 30, 0, 0, 0,
						12, 34, 56, 0, -24, -12, 0, 1};
	const int size = sizeof(data) / sizeof(data[0]);

    //MODULES
    reg_file<size, ADDR_SIZE, DATA_SIZE> reg_file("reg_file", data);
	reg_file.clock(clock);
	reg_file.addr_r1(addr_r1);
	reg_file.addr_r2(addr_r2);
	reg_file.addr_rw(addr_rw);
	reg_file.en_r1(en_r1);
	reg_file.en_r2(en_r2);
	reg_file.en_rw(en_rw);
	reg_file.data_r1(data_r1);
	reg_file.data_r2(data_r2);
	reg_file.data_rw(data_rw);

	stimulus<ADDR_SIZE, DATA_SIZE> stim("stim", size);
	stim.clock(clock);
	stim.addr_r1(addr_r1);
	stim.addr_r2(addr_r2);
	stim.addr_rw(addr_rw);
	stim.en_r1(en_r1);
	stim.en_r2(en_r2);
	stim.en_rw(en_rw);
	stim.data_rw(data_rw);

	response<DATA_SIZE> resp("resp");
	resp.data_r1(data_r1);
	resp.data_r2(data_r2);

    //TRACES
    sc_trace_file* tf = sc_create_vcd_trace_file("rf_trace");
    sc_trace(tf, clock, "clock");
	sc_trace(tf, en_r1, "en_r1");
    sc_trace(tf, addr_r1, "addr_r1");
    sc_trace(tf, data_r1, "data_r1");
    sc_trace(tf, en_r2, "en_r2");
    sc_trace(tf, addr_r2, "addr_r2");
    sc_trace(tf, data_r2, "data_r2");
    sc_trace(tf, en_rw, "en_rw");
    sc_trace(tf, addr_rw, "addr_rw");
    sc_trace(tf, data_rw, "data_rw");

    //START SIM
    sc_start(size * 3 * 10, SC_NS);
    
    sc_close_vcd_trace_file(tf);

    return 0;
}
