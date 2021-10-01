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
			SC_METHOD(prc_c1);
			sensitive << c1 << input_r1 << input_r2;
			
			SC_METHOD(prc_c2);
			sensitive << c2 << _c1_out2;

			SC_METHOD(prc_c3);
			sensitive << c3 << input_imm << _c1_out1;

			SC_METHOD(prc_c4);
			sensitive << c4 << _c3_out;

			SC_METHOD(prc_c5);
			sensitive << c5 << _c4_out;

        	SC_METHOD(prc_alu);
			sensitive << cop << _c2_out << _c4_out << _c5_out;

			SC_METHOD(prc_c6);
			sensitive << c6 << input_con << _alu_out;
    	}

	private:
		// Local signals
    	sc_signal<sc_uint<data_size> > _c1_out1, _c1_out2, _c2_out, _c3_out, _c4_out;
    	sc_signal<bool> _c5_out;
		sc_signal<sc_uint<data_size> > _alu_out;
		sc_uint<psr_size> psr;

		// Process
		void prc_c1()
		{
			_c1_out1 = c1.read() ? input_r2.read() : input_r1.read();
			_c1_out2 = c1.read() ? input_r1.read() : input_r2.read();
		}

		void prc_c2()
		{
			sc_uint<data_size> _out = c2.read() ? (sc_uint<data_size>)0 : _c1_out2.read();
			_c2_out = _out;
			output_mar = _out;
			output_con = _out;
		}

		void prc_c3()
		{
			_c3_out = c3.read() ? input_imm.read() : _c1_out1.read();
		}

		void prc_c4()
		{
			sc_uint<data_size> _out = c4.read() ? (sc_uint<data_size>)(_c3_out.read() ^ 0xFF)
								: _c3_out.read();
			_c4_out = _out;
			output_mdr = _out;
		}

		void prc_c5()
		{
			_c5_out = c5.read() ? (bool)_c4_out.read()[data_size-1] : true;
		}

		void prc_alu()
		{
			sc_uint<data_size> _alu_in1 = _c4_out.read(), _alu_in2 = _c2_out.read();
			sc_uint<data_size> _out;
			sc_uint<data_size+1> _carry_detect;
			sc_uint<4> _shift_amt = _alu_in1;

			switch(cop.read())
			{
				case 0b011:
					_carry_detect = _alu_in1 + _alu_in2;
					_out = _carry_detect.range(data_size-1, 0);
					psr[2] = _carry_detect[data_size];
					psr[1] = _out[data_size-1];
					psr[0] = (_out == 0);
					break;
				case 0b000:
					_out = _alu_in1 & _alu_in2;
					psr[2] = '0';
					psr[1] = '0';
					psr[0] = (_out == 0);
					break;
				case 0b001:
					_out = _alu_in1 | _alu_in2;
					psr[2] = '0';
					psr[1] = '0';
					psr[0] = (_out == 0);
					break;
				case 0b010:
					_out = _alu_in1 ^ _alu_in2;
					psr[2] = '0';
					psr[1] = '0';
					psr[0] = (_out == 0);
					break;
				case 0b100:
					if (_shift_amt != 0) {	//Do not attempt a shift if given 0 to avoid invalid index
						if (!_c5_out) {		//_c5_out == 0, arithmetic right shift
							psr[2] = _alu_in2[_shift_amt-1];
							_out = _alu_in2 >> _shift_amt;
						}
						else {				//_c5_out == 1, left shift or logical right shift
							if (_shift_amt[3] == 1) {		//if shift_amt is negative
								_shift_amt = ~_shift_amt;	//invert shift_amt back to a positive number
								psr[2] = _alu_in1[data_size - _shift_amt];
								_out = _alu_in2 << _shift_amt;
							}
							else {
								psr[2] = _alu_in2[_shift_amt];
								_out = _alu_in2 >> _shift_amt;
							}
						}
					}
					psr[1] = _out[data_size-1];
					psr[0] = (_out == 0);
					break;
				case 0b101:
					_out = _alu_in1;
					break;
				case 0b110:
					_out = _alu_in1;
					break;
				case 0b111:
					_out = _alu_in1;
					break;
				default:
					break;
			}

			_alu_out = _out;
			output_psr = psr;
		}

		void prc_c6()
		{
			output_rw = c6.read() ? input_con.read() : _alu_out;
		}
};