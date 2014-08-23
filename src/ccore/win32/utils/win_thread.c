#include "win_thread.h"

ccError ccThreadCreate(ccThread *thread, void *function)
{
	ccMalloc(*thread, sizeof(ccThread_win));
	((ccThread_win*)*thread)->function = (LPTHREAD_START_ROUTINE)function;
	return CC_ERROR_NONE;
}

ccError ccThreadStart(ccThread thread, void *data)
{
	_THREAD->threadHandle = CreateThread(NULL, 0, _THREAD->function, (LPVOID)data, 0, NULL);
	if(_THREAD->threadHandle == NULL) {
		free(thread);
		return CC_ERROR_THREAD;
	}
	return CC_ERROR_NONE;
}

ccError ccThreadJoin(ccThread thread)
{
	return WaitForSingleObject(_THREAD->threadHandle, INFINITE) == WAIT_OBJECT_0?CC_ERROR_NONE:CC_ERROR_THREAD;
}

bool ccThreadFinished(ccThread thread)
{
	if(WaitForSingleObject(_THREAD->threadHandle, 0) == WAIT_OBJECT_0) {
		CloseHandle(_THREAD->threadHandle);
		free(thread);
		return true;
	}
	return false;
}