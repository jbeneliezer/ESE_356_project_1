#include "systemc.h"

SC_MODULE(tb)
{
	sc_in<bool> clk;
	sc_out<bool> rst, clr;
	sc_out<bool> data_in;
	sc_in<bool> data_out;

	void source();
	void sink();

	SC_CTOR(tb)
	{
		SC_CTHREAD(source, clk.pos());
		SC_CTHREAD(sink, clk.pos());
	}
};
