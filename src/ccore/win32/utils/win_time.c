#include "win_time.h"

static calculateConversionFactor()
{
	LARGE_INTEGER ticksPerSecond;
	QueryPerformanceFrequency(&ticksPerSecond);
	ticksToNanoSeconds = (double)(_TO_SECONDS / ticksPerSecond.QuadPart);
}

void ccTimeDelay(int ms)
{
	Sleep(ms);
}

uint64_t ccTimeNanoseconds()
{
	LARGE_INTEGER time;

	if(ticksToNanoSeconds == -1.0) {
		calculateConversionFactor();
	}

	QueryPerformanceCounter(&time);
	return (uint64_t)(time.QuadPart * ticksToNanoSeconds);
}