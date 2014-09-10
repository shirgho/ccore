#include "lin_thread.h"

ccReturn ccThreadCreate(ccThread *thread, void *function)
{
	ccMalloc(*thread, sizeof(ccThread_lin));
	((ccThread_lin*)*thread)->function = function;

	return CC_SUCCESS;
}

ccReturn ccThreadStart(ccThread thread, void *data)
{
	if(pthread_create(&_THREAD->id, NULL, _THREAD->function, data) != 0) {
		free(thread);
		ccErrorPush(CC_ERROR_THREAD_CREATE);
		return CC_FAIL;
	}

	return CC_SUCCESS;
}

ccReturn ccThreadJoin(ccThread thread)
{
	if(pthread_join(_THREAD->id, NULL) == 0){
		return CC_SUCCESS;  
	}else{
		ccErrorPush(CC_ERROR_THREAD_CREATE);
		return CC_FAIL;
	}
}

bool ccThreadFinished(ccThread thread)
{
	if(pthread_kill(_THREAD->id, 0) == 0){
		return false;
	}else{
		free(thread);
		return true;
	}
}

ccReturn ccThreadMutexCreate(ccMutex *mutex)
{
	if(pthread_mutex_init(mutex, NULL) != 0) {
		ccErrorPush(CC_ERROR_THREAD_MUTEXCREATE);
		return CC_FAIL;
	}

	return CC_SUCCESS;
}

ccReturn ccThreadMutexJoin(ccMutex mutex)
{
	if(pthread_mutex_lock(&mutex) != 0) {
		ccErrorPush(CC_ERROR_THREAD_MUTEX);
		return CC_FAIL;
	}

	return CC_SUCCESS;
}

bool ccThreadMutexFinished(ccMutex mutex)
{
	int result = pthread_mutex_trylock(&mutex);

	if(result == 0) {
		return true;
	}
	else{
		if(result == EBUSY) ccErrorPush(CC_ERROR_THREAD_MUTEX);
		return false;
	}
}

ccReturn ccThreadMutexRelease(ccMutex mutex)
{
	if(pthread_mutex_unlock(&mutex) != 0) {
		ccErrorPush(CC_ERROR_THREAD_MUTEX);
		return CC_FAIL;
	}

	return CC_SUCCESS;
}

ccReturn ccThreadMutexFree(ccMutex mutex)
{
	if(pthread_mutex_destroy(&mutex) != 0) {
		ccErrorPush(CC_ERROR_THREAD_MUTEX);
		return CC_FAIL;
	}

	return CC_SUCCESS;
}