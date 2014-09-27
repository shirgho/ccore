#pragma once

#include <Windows.h>

#include <ccore/thread.h>

typedef struct {
	HANDLE threadHandle;
	LPTHREAD_START_ROUTINE function;
} ccThread_win;

#define _THREAD ((ccThread_win*)thread)

#define _CRITICAL_SECTION_SPIN_COUNT 400