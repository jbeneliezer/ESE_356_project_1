#include "al.h"

void al::load()
{
    
}

void al::stor()
{

}

void al::prc_output()
{
  	data_out = data_in & third & (!second) & first;
}

void al::prc_clear()
{
	data_out = false;
}

void al::prc_reset()
{
	first = second = third = fourth = false;
}
