#include "systemc.h"
#include "alu.h"
#include "tb.h"

SC_MODULE (SYSTEM)
{
	tb* tb0;	
	alu* alu0;
  	sc_signal<bool> reset_sig, clear_sig, data_in_sig, data_out_sig;
	sc_clock clk_sig;

	SC_CTOR(SYSTEM)
		: clk_sig("clk_sig", 20, SC_NS)
	{
		tb0 = new tb("tb0");
		alu0 = new alu("alu0");

		tb0->clk(clk_sig);
		tb0->rst(reset_sig);
		tb0->clr(clear_sig);
		tb0->data_in(data_in_sig);
		tb0->data_out(data_out_sig);

		alu0->clk(clk_sig);
		alu0->rst(reset_sig);
		alu0->clr(clear_sig);
		alu0->data_in(data_in_sig);
		alu0->data_out(data_out_sig);

	}

	~SYSTEM()
	{
		delete tb0;
		delete alu0;
	}
};

SYSTEM* top = NULL;	

int sc_main(int argc, char* argv[])
{
	top = new SYSTEM("top");

 	sc_trace_file *tfile = sc_create_vcd_trace_file("myvcdfile");

  	sc_trace(tfile, top->data_in_sig, "data_in");
  	sc_trace(tfile, top->data_out_sig, "data_out");
  	sc_trace(tfile, top->clk_sig, "clk");
  	sc_trace(tfile, top->reset_sig, "rst");
  	sc_trace(tfile, top->clear_sig, "clr");

  	sc_start(1280, SC_NS);

   	sc_close_vcd_trace_file(tfile);

	sc_stop();

  	return(0);
}
