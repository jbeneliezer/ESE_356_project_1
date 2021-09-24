#include "systemc.h"

template<int op_size, int psr_size, int mem_addr_size, int data_size> class alu: public sc_module
{
	public:
		// Port declarations
    	sc_in<bool> clock, c1, c2, c3, c4, c5, c6;
    	sc_in<sc_uint<op_size> > cop;
    	sc_in<sc_uint<data_size> > input_r1, input_r2, input_imm, input_con;
    	sc_out<sc_uint<data_size> > output_con, output_rw;
    	sc_out<sc_uint<mem_addr_size> > output_mar, output_mdr;
    	sc_out<sc_uint<psr_size> > output_psr;		// [2]Carry : [1]Negative : [0]Zero

		// Constructor
		SC_HAS_PROCESS(alu);

		alu(sc_module_name name) : sc_module(name)
		{
        	SC_METHOD(prc_alu)
        	{
        	    sensitive << clock.pos();
        	}
    	}

	private:
		// Local signals
    	uint _c1_out1, _c1_out2, _c2_out, _c3_out, _c4_out;
    	bool _c5_out;
		uint _alu_out;
		uint _alu_in1, _alu_in2;
		sc_uint<psr_size> psr;

		// Process
		void prc_c1()
		{
			_c1_out1 = c1.read() ? input_r2.read() : input_r1.read();
			_c1_out2 = c1.read() ? input_r1.read() : input_r2.read();
		}

		void prc_c2()
		{
			_c2_out = c2.read() ? 0 : _c1_out2;
			output_mar = _c2_out;
			output_con = _c2_out;
			_alu_in2 = _c2_out;
		}

		void prc_c3()
		{
			if (c3.read()) {
				_c3_out = input_imm.read();
			}
			else {
				_c3_out = _c1_out1;
			}
		}

		void prc_c4()
		{
			_c4_out = c4.read() ? ~_c3_out : _c3_out;
			output_mdr = _c4_out;
			_alu_in1 = _c4_out;
		}

		void prc_c5()
		{
			_c5_out = c5.read() ? _c4_out >> (data_size-1) : false;
		}

		void prc_alu()
		{
			prc_c1();
			prc_c2();
			prc_c3();
			prc_c4();
			prc_c5();

			switch(cop.read())
			{
				case 0b011:
					_alu_out = _alu_in1 + _alu_in2;
					psr[2] = _alu_out >> data_size;
					psr[1] = _alu_out >> (data_size-1);
					psr[0] = (_alu_out == 0);
					break;
				case 0b000:
					_alu_out = _alu_in1 & _alu_in2;
					psr[2] = '0';
					psr[1] = '0';
					psr[0] = (_alu_out == 0);
					break;
				case 0b001:
					_alu_out = _alu_in1 | _alu_in2;
					psr[2] = '0';
					psr[1] = '0';
					psr[0] = (_alu_out == 0);
					break;
				case 0b010:
					_alu_out = _alu_in1 ^ _alu_in2;
					psr[2] = '0';
					psr[1] = '0';
					psr[0] = (_alu_out == 0);
					break;
				case 0b100:
					if (_c5_out) {		//_c5_out == 1, arithmetic right shift
						psr[2] = (_alu_in2 >> (~_alu_in1-1)) & 0b1;			//negative for right shift
						_alu_out = _alu_in2 >> (~_alu_in1);
					}
					else {				//_c5_out == 0, left shift or logical right shift
						if (_alu_in1 < 0) {									//negative for left shift
							psr[2] = (_alu_in1 >> (data_size - ~_alu_in1)) & 0b1;
							_alu_out = _alu_in2 << ~_alu_in1;
						}
						else {												//positive for right shift
							psr[2] = (_alu_in2 >> (_alu_in1-1)) & 0b1;
							_alu_out = _alu_in2 >> _alu_in1;
						}
					}
					psr[1] = _alu_out >> (data_size-1) & 0b1;
					psr[0] = (_alu_out == 0);
					break;
				case 0b101:
					_alu_out = _alu_in1;
					break;
				case 0b110:
					_alu_out = _alu_in1;
					break;
				case 0b111:
					_alu_out = _alu_in1;
					break;
				default:
					break;
			}

			output_psr = psr;
			prc_c6();
		}

		void prc_c6()
		{
			if (c6.read()) {
				output_rw = input_con.read();
			}
			else {
				output_rw = _alu_out;
			}
		}
};