#include "win_net.h"

ccReturn ccNetInitialize(void)
{
	WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
		ccErrorPush(CC_ERROR_NET);
		return CC_FAIL;
    }

	_ccNet = true;
	return CC_SUCCESS;
}

ccReturn ccNetFree(void)
{
	WSACleanup();

	_ccNet = false;
	return CC_SUCCESS;
}
