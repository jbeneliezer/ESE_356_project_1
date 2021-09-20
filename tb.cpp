#include "tb.h"
#include <stdlib.h>
#include <time.h>

void tb::source()
{
	data_in.write(0);
	rst.write(0);
	wait();
	rst.write(1);
	wait();

	bool tmp;
	srand(time(0));

	for(int i = 0; i < 64; ++i)
	{
		tmp = rand() % 2;
		data_in.write(tmp);
		wait();
	}
}

void tb::sink()
{
	bool tmp;

	for (int i = 0; i < 64; ++i)
	{
		tmp = data_out;
		wait();
	}

	sc_stop;
}	
