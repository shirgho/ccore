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
		ccErrorPush(CC_ERROR_THREAD_CREATE);
		return CC_FAIL;
	}
	return CC_SUCCESS;
}

ccReturn ccThreadJoin(ccThread thread)
{
	if(WaitForSingleObject(_THREAD->threadHandle, INFINITE) == WAIT_OBJECT_0) {
		if(CloseHandle(_THREAD->threadHandle) == 0) {
			ccErrorPush(CC_ERROR_THREAD_CREATE);
			free(thread);
			return CC_FAIL;
		}
		free(thread);
		return CC_SUCCESS;
	}
	else{
		ccErrorPush(CC_ERROR_THREAD_CREATE);
		return CC_FAIL;
	}
}

bool ccThreadFinished(ccThread thread)
{
	if(WaitForSingleObject(_THREAD->threadHandle, 0) == WAIT_OBJECT_0) {
		if(CloseHandle(_THREAD->threadHandle) == 0) {
			ccErrorPush(CC_ERROR_THREAD_CREATE);
		}
		free(thread);
		return true;
	}

	return false;
}

ccReturn ccThreadMutexCreate(ccMutex *mutex)
{
	if(!InitializeCriticalSectionAndSpinCount(mutex, _CRITICAL_SECTION_SPIN_COUNT)) {
		ccErrorPush(CC_ERROR_THREAD_MUTEXCREATE);
		return CC_FAIL;
	}
	
	return CC_SUCCESS;
}

ccReturn ccThreadMutexJoin(ccMutex *mutex)
{
	EnterCriticalSection(mutex);

	return CC_SUCCESS;
}

ccReturn ccThreadMutexRelease(ccMutex *mutex)
{
	LeaveCriticalSection(mutex);

	return CC_SUCCESS;
}

ccReturn ccThreadMutexFree(ccMutex *mutex)
{
	DeleteCriticalSection(mutex);

	return CC_SUCCESS;
}