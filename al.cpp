#include "systemc.h"

template<typename T> class al: public sc_module
{
	public:
		// Port declarations
    	sc_in<bool> c1, c2, c3, c4, c5, c6;
    	sc_in<T<3> > cop;
    	sc_in<T<16> > input1, input2, fc_input;
    	sc_out<T<16> > fc_output, mdr_output, mar_output, output;
    	sc_out<T<3> > psr_output;
    	sc_out<bool> cout;

		SC_HAS_PROCESS(al);

		al(sc_module_name name) : sc_module(name)
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

	private:

		// Local signals
    	sc_signal<sc_int<16> > c1_out1, c1_out2, c2_out, c3_out, c4_out, alu_out;
    	sc_signal<bool> c5_out;

		void prc_c1()
		{
			c1_out1 = c1 ? input2: input1;
			c1_out2 = c1 ? input1: input2;
		}

		void prc_c2()
		{
			c2_out = c2 ? 0: c1_out2;
			mar_output = c2_out;
			fc_output = c2_out;
		}

		void prc_c3()
		{
			c3_out = c3 ? (c1_out1[7] ? c1_out1 | 0xFF00 : c1_out1 & 0x00FF) : c1_out1;
		}

		void prc_c4()
		{
			c4_out = c4 ? c3_out ^ 0xFFFF: c3_out;
			mdr_output = c4_out;
		}

		void prc_c5()
		{
			c5_out = c5 ? c4_out[15]: false;
		}

		void  prc_alu()
		{
			switch(cop)
			{
				case "100":
					alu_out = (c4_out + c2_out) | c5_out;
					cout = c4_out[15] & c2_out[15];
					psr_output[0] = (c4_out[15] == c2_out[15]) ^ alu_out[15];
					psr_output[1] = alu_out[15];
					psr_output[2] = (alu_out == 0);
					break;
				case "000":
					alu_out = c4_out & c2_out;
					psr_output[0] = '0';
					psr_output[1] = '0';
					psr_output[2] = (alu_out == 0);
					break;
				case "001":
					alu_out = c4_out | c2_out;
					psr_output[0] = '0';
					psr_output[1] = '0';
					psr_output[2] = (alu_out == 0);
					break;
				case "010":
					alu_out = c4_out ^ c2_out;
					psr_output[0] = '0';
					psr_output[1] = '0';
					psr_output[2] = (alu_out == 0);
					break;
				case "011":
				case "101":
				case "110":
				case "111":
					alu_out = c4_out;
					break;
				default:
					break;
			}
		}

		void prc_c6()
		{
			output = c6 ? fc_input : alu_out;
		}

		void prc_clear()
		{
			fc_output = mdr_output = mar_output = output = 0x0000;
			psr_output = "000";
		}

		void prc_reset()
		{
			fc_output = mdr_output = mar_output = output = 0x0000;
			psr_output = "000";
		}
}