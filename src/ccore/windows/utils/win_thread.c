#include "win_thread.h"

ccReturn ccThreadCreate(ccThread *thread, void *function)
{
	ccMalloc(*thread, sizeof(ccThread_win));
	((ccThread_win*)*thread)->function = (LPTHREAD_START_ROUTINE)function;
	return CC_SUCCESS;
}

ccReturn ccThreadStart(ccThread thread, void *data)
{
	_THREAD->threadHandle = CreateThread(NULL, 0, _THREAD->function, (LPVOID)data, 0, NULL);
	if(_THREAD->threadHandle == NULL) {
		free(thread);
		ccErrorPush(CC_ERROR_THREAD);
		return CC_FAIL;
	}
	return CC_SUCCESS;
}

ccReturn ccThreadJoin(ccThread thread)
{
	if(WaitForSingleObject(_THREAD->threadHandle, INFINITE) == WAIT_OBJECT_0) {
		if(CloseHandle(_THREAD->threadHandle) == 0) {
			ccErrorPush(CC_ERROR_THREAD);
			free(thread);
			return CC_FAIL;
		}
		free(thread);
		return CC_SUCCESS;
	}
	else{
		ccErrorPush(CC_ERROR_THREAD);
		return CC_FAIL;
	}
}

bool ccThreadFinished(ccThread thread)
{
	if(WaitForSingleObject(_THREAD->threadHandle, 0) == WAIT_OBJECT_0) {
		if(CloseHandle(_THREAD->threadHandle) == 0) {
			ccErrorPush(CC_ERROR_THREAD);
		}
		free(thread);
		return true;
	}
	return false;
}

ccMutex ccThreadMutexCreate(void)
{
	ccMutex m = CreateMutex(NULL, FALSE, NULL);
	if(m == NULL) {
		ccErrorPush(CC_ERROR_MUTEX_CREATION);
	}
	return m;
}

ccReturn ccThreadMutexJoin(ccMutex mutex)
{
	if(WaitForSingleObject(mutex, INFINITE) != WAIT_OBJECT_0) {
		ccErrorPush(CC_ERROR_MUTEX);
		return CC_FAIL;
	}

	return CC_SUCCESS;
}

bool ccThreadMutexFinished(ccMutex mutex)
{
	if(WaitForSingleObject(mutex, 0) == WAIT_OBJECT_0) {
		return true;
	}
	return false;
}

ccReturn ccThreadMutexRelease(ccMutex mutex)
{
	if(ReleaseMutex(mutex) == 0) {
		ccErrorPush(CC_ERROR_MUTEX);
		return CC_FAIL;
	}
	
	return CC_SUCCESS;
}

ccReturn ccThreadMutexFree(ccMutex mutex)
{
	if(CloseHandle(mutex) == 0) {
		ccErrorPush(CC_ERROR_MUTEX);
		return CC_FAIL;
	}

	return CC_SUCCESS;
}