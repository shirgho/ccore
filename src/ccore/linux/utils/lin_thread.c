#include "lin_thread.h"

ccError ccThreadCreate(ccThread *thread, void *function)
{
	ccMalloc(*thread, sizeof(ccThread_lin));
	((ccThread_lin*)*thread)->function = function;
	return CC_ERROR_NONE;
}

ccError ccThreadStart(ccThread thread, void *data)
{
	if(pthread_create(&_THREAD->id, NULL, _THREAD->function, data) != 0) {
		free(thread);
		return CC_ERROR_THREAD;
	}
	return CC_ERROR_NONE;
}

ccError ccThreadJoin(ccThread thread)
{
	return pthread_join(_THREAD->id, NULL) == 0 ? CC_ERROR_NONE : CC_ERROR_THREAD;
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
