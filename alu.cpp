#include "alu.h"

void alu::load()
{
    
}

void alu::stor()
{

}

void alu::prc_output()
{
  	data_out = data_in & third & (!second) & first;
}

void alu::prc_clear()
{
	data_out = false;
}

void alu::prc_reset()
{
	first = second = third = fourth = false;
}
