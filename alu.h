#include "systemc.h"

SC_MODULE(alu)
{
    sc_in<bool> c1, c2, c3, c4, c5, c6, cin, cop;
    sc_in<sc_uint<8> > imm;
    sc_in<sc_uint<16> > input1, input2, fc_input;
    sc_out<sc_uint<16> > fc_output, mdr_output, mar_output, output;
    sc_out<sc_uint<3> > psr_output;
    sc_out<bool> cout;

    void load();
    void store();
    void jeq();
    void jle();
    void breq();
    void bne();
    void jal();

    SC_CTOR(alu)
    {
        
    }

}