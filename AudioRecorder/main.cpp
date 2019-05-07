#include <iostream>

#include "Recorder.h"

int main()
{
	Recorder r;

	r.start();

	while (true)
	{
		Pa_Sleep(100);
	}
}