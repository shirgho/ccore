#pragma once

#include <pthread.h>
#include <signal.h>

#include <ccore/thread.h>

typedef struct {
	pthread_t id;
	void *function;
} ccThread_lin;

#define _THREAD ((ccThread_lin*)thread)
