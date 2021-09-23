#include "systemc.h"

template<int op_size, int psr_size, int mem_addr_size, int data_size> class alu: public sc_module
{
	public:
		// Port declarations
    	sc_in<bool> clock, c1, c2, c3, c4, c5, c6;
    	sc_in<sc_uint<op_size> > cop;
    	sc_in<sc_uint<data_size> > input_r1, input_r2, input_imm, input_con;
    	sc_out<sc_uint<data_size> > output_con, output_rw;
    	sc_out<sc_uint<mem_addr_size> > output_mar;
    	sc_out<sc_uint<psr_size> > output_psr;		// [2]Carry : [1]Negative : [0]Zero
    	sc_inout<sc_uint<data_size> > inout_mdr;

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
    	sc_signal<sc_uint<data_size> > _c1_out1, _c1_out2, _c2_out, _c3_out, _c4_out,
									   _alu_in1, _alu_in2, _alu_out;
    	sc_signal<bool> _c5_out;

		// Process
		void prc_c1()
		{
			_c1_out1 = c1 ? input_r2 : input_r1;
			_c1_out2 = c1 ? input_r1 : input_r2;
		}

		void prc_c2()
		{
			_c2_out = c2 ? 0 : _c1_out2;
			output_mar = _c2_out;
			output_con = _c2_out;
			_alu_in2 = _c2_out;
		}

		void prc_c3()
		{
			_c3_out = c3 ? input_imm : _c1_out1;
		}

		void prc_c4()
		{
			_c4_out = c4 ? _c3_out ^ 0xFFFF : _c3_out;
			inout_mdr = _c4_out;
			_alu_in1 = _c4_out;
		}

		void prc_c5()
		{
			_c5_out = c5 ? (bool)_c4_out[data_size-1] : false;
		}

		void prc_alu()
		{
			switch(cop)
			{
				case "011":
					sc_uint<data_size+1> _carry_detect = _alu_in1 + _alu_in2;
					_alu_out = _carry_detect.range(data_size-1, 0);
					output_psr[2] = carry_detect[data_size];
					output_psr[1] = _alu_out[data_size-1];
					output_psr[0] = (_alu_out == 0);
					break;
				case "000":
					_alu_out = _alu_in1 & _alu_in2;
					output_psr[2] = '0';
					output_psr[1] = '0';
					output_psr[0] = (_alu_out == 0);
					break;
				case "001":
					_alu_out = _alu_in1 | _alu_in2;
					output_psr[2] = '0';
					output_psr[1] = '0';
					output_psr[0] = (_alu_out == 0);
					break;
				case "010":
					_alu_out = _alu_in1 ^ _alu_in2;
					output_psr[2] = '0';
					output_psr[1] = '0';
					output_psr[0] = (_alu_out == 0);
					break;
				case "100":
					if (_c5_out) {		//_c5_out == 1, arithmetic right shift
						output_psr[2] = _alu_in2[(_alu_in1 ^ 0xFFFF)-1];	//negative for right shift
						_alu_out = (sc_int<data_size>)_alu_in2 >> (_alu_in1 ^ 0xFFFF);
					}
					else {				//_c5_out == 0, left shift or logical right shift
						if ((sc_int<data_size>)_alu_in1 < 0) {				//negative for left shift
							output_psr[2] = _alu_in2[data_size - (_alu_in1 ^ 0xFFFF)];
							_alu_out = _alu_in2 << (_alu_in1 ^ 0xFFFF);
						}
						else {												//positive for right shift
							output_psr[2] = _alu_in2[_alu_in1-1];
							_alu_out = _alu_in2 >> _alu_in1;
						}
					}
					output_psr[1] = _alu_out[data_size-1];
					output_psr[0] = (_alu_out == 0);
					break;
				case "101":
					_alu_out = _alu_in1;
					break;
				case "110":
					_alu_out = _alu_in1;
					break;
				case "111":
					_alu_out = _alu_in1;
					break;
				default:
					break;
			}
		}

		void prc_c6()
		{
			output_rw = c6 ? input_con : _alu_out;
		}
}
