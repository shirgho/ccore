#include <ccore/timing.h>

void ccDelay(int ms)
{
	usleep(ms * 1000);
}
