#include "systemc.h"

template<int opcode_size, int psr_size, int data_size> class al: public sc_module
{
	public:
		// Port declarations
    	sc_in<bool> clock, c1, c2, c3, c4, c5, c6;
    	sc_in<sc_uint<opcode_size> > cop;
    	sc_in<T<sc_uint<data_size> > input_r1, input_r2, input_con;
    	sc_out<sc_uint<data_size> > output_con, output_mdr, output_mar, output_rw;
    	sc_out<T<sc_uint<psr_size> > output_psr;

		// Constructor
		SC_HAS_PROCESS(alu);

		alu(sc_module_name name) : sc_module(name)
		{
        	SC_METHOD(prc_c1)
        	{
            	sensitive << c1 << input_r1 << input_r2;
        	}
        	SC_METHOD(prc_c2)
        	{
            	sensitive << c2 << _c1_out2;
        	}
        	SC_METHOD(prc_c3)
        	{
        	    sensitive << c3 << _c1_out1;
        	}
        	SC_METHOD(prc_c4)
        	{
        	    sensitive << c4 << _c3_out;
        	}
        	SC_METHOD(prc_c5)
        	{
        	    sensitive << c5 << _c4_out;
        	}
        	SC_METHOD(prc_alu)
        	{
        	    sensitive << clock.pos();
        	}
        	SC_METHOD(prc_c6)
        	{
        	    sensitive << c6 << _alu_out;
        	}
    	}

	private:
		// Local signals
    	sc_signal<sc_int<16> > _c1_out1, _c1_out2, _c2_out, _c3_out, _c4_out, _alu_out;
    	sc_signal<bool> _c5_out;

		// Process
		void prc_c1()
		{
			_c1_out1 = c1 ? input_r2: input_r1;
			_c1_out2 = c1 ? input_r1: input_r2;
		}

		void prc_c2()
		{
			_c2_out = c2 ? 0: _c1_out2;
			output_mar = _c2_out;
			output_con = _c2_out;
		}

		void prc_c3()
		{
			_c3_out = c3 ? (_c1_out1[7] ? _c1_out1 | 0xFF00 : _c1_out1 & 0x00FF) : _c1_out1;
		}

		void prc_c4()
		{
			_c4_out = c4 ? _c3_out ^ 0xFFFF: _c3_out;
			output_mdr = _c4_out;
		}

		void prc_c5()
		{
			_c5_out = c5 ? _c4_out[15]: false;
		}

		void prc_alu()
		{
			switch(cop)
			{
				case "100":
					_alu_out = (_c4_out + _c2_out) | _c5_out;
					cout = _c4_out[15] & _c2_out[15];
					output_psr[0] = (_c4_out[15] == _c2_out[15]) ^ _alu_out[15];
					output_psr[1] = _alu_out[15];
					output_psr[2] = (_alu_out == 0);
					break;
				case "000":
					_alu_out = _c4_out & _c2_out;
					output_psr[0] = '0';
					output_psr[1] = '0';
					output_psr[2] = (_alu_out == 0);
					break;
				case "001":
					_alu_out = _c4_out | _c2_out;
					output_psr[0] = '0';
					output_psr[1] = '0';
					output_psr[2] = (_alu_out == 0);
					break;
				case "010":
					_alu_out = _c4_out ^ _c2_out;
					output_psr[0] = '0';
					output_psr[1] = '0';
					output_psr[2] = (_alu_out == 0);
					break;
				case "011":
				case "101":
				case "110":
				case "111":
					_alu_out = __c4_out;
					break;
				default:
					break;
			}
		}

		void prc_c6()
		{
			output = c6 ? input_con : _alu_out;
		}
}
