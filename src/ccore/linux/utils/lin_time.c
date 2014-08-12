#include "lin_time.h"

void ccTimeDelay(int ms)
{
	usleep(ms * _TO_MICROSECONDS);
}

unsigned long ccTimeNanoseconds()
{
	struct timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
	return time.tv_nsec + time.tv_sec * _TO_SECONDS;
}