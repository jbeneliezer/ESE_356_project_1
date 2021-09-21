#include "systemc.h"

SC_MODULE(al)
{
    sc_in<bool> c1, c2, c3, c4, c5, c6;
    sc_in<sc_lv<3> > cop;
    sc_in<sc_lv<16> > input1, input2, fc_input;
    sc_out<sc_lv<16> > fc_output, mdr_output, mar_output, output;
    sc_out<sc_lv<3> > psr_output;
    sc_out<bool> cout;

    void prc_c1();
    void prc_c2();
    void prc_c3();
    void prc_c4();
    void prc_c5();
    void prc_alu();
    void prc_c6();
    void prc_clear();
    void prc_reset();

    sc_signal<sc_int<16> > c1_out1, c1_out2, c2_out, c3_out, c4_out, alu_out;
    sc_signal<bool> c5_out;

    SC_CTOR(al)
    {
        SC_METHOD(prc_c1)
        {
            sensitive << c1;
        }
        SC_METHOD(prc_c2)
        {
            sensitive << c2;
        }
        SC_METHOD(prc_c3)
        {
            sensitive << c3
        }
        SC_METHOD(prc_c4)
        {
            sensitive << c4;
        }
        SC_METHOD(prc_c5)
        {
            sensitive << c5
        }
        SC_METHOD(prc_alu)
        {
            sensitive << cin << cop;
        }
        SC_METHOD(prc_c6)
        {
            sensitive << c6
        }

        SC_CTHREAD(prc_clear, clr.pos());
		SC_CTHREAD(prc_reset, rst.neg());
    }
}
