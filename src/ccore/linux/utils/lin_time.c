#include "lin_time.h"

void ccTimeDelay(int ms)
{
	usleep(ms * _TO_MICROSECONDS);
}

uint64_t ccTimeNanoseconds()
{
	struct timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
	return (uint64_t)(time.tv_nsec + time.tv_sec * _TO_SECONDS);
}