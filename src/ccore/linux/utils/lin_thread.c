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
		ccErrorPush(CC_ERROR_THREAD);
		return CC_FAIL;
	}

	return CC_SUCCESS;
}

ccReturn ccThreadJoin(ccThread thread)
{
	if(pthread_join(_THREAD->id, NULL) == 0){
		return CC_SUCCESS;  
	}else{
		ccErrorPush(CC_ERROR_THREAD);
		return CC_FAIL;
	}
}

bool ccThreadFinished(ccThread thread)s
{
	if(pthread_kill(_THREAD->id, 0) == 0){
		return false;
	}else{
		free(thread);
		return true;
	}
}

ccMutex ccThreadMutexCreate(void)
{

}