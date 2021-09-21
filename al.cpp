#include "al.h"

void al::prc_c1()
{
	c1_out1 = c1 ? input2: input1;
	c1_out2 = c1 ? input1: input2;
}

void al::prc_c2()
{
	c2_out = c2 ? 0: c1_out2;
	mar_output = c2_out;
	fc_output = c2_out;
}

void al::prc_c3()
{
	c3_out = c3 ? (c1_out1[7] ? c1_out1 | 0xFF00 : c1_out1 & 0x00FF) : c1_out1;
}

void al::prc_c4()
{
	c4_out = c4 ? c3_out ^ 0xFFFF: c3_out;
	mdr_output = c4_out;
}

void al::prc_c5()
{
	c5_out = c5 ? c4_out[15]: false;
}

void al:: prc_alu()
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

void al::prc_clear()
{
	fc_output = mdr_output = mar_output = output = 0x0000;
	psr_output = "000";
}

void al::prc_reset()
{
	fc_output = mdr_output = mar_output = output = 0x0000;
	psr_output = "000";
}