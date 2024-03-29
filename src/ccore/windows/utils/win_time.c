#include "win_time.h"

#ifdef CC_USE_TIME

static void calculateConversionFactor(void)
{
	LARGE_INTEGER ticksPerSecond;
	QueryPerformanceFrequency(&ticksPerSecond);
	_ticksToNanoSeconds = (double)(_TO_SECONDS / ticksPerSecond.QuadPart);
}

ccReturn ccTimeDelay(int ms)
{
	Sleep(ms);

	return CC_SUCCESS;
}

uint64_t ccTimeNanoseconds(void)
{
	LARGE_INTEGER time;

	if(_ticksToNanoSeconds == -1.0) {
		calculateConversionFactor();
	}

	QueryPerformanceCounter(&time);
	return (uint64_t)(time.QuadPart * _ticksToNanoSeconds);
}

#endif