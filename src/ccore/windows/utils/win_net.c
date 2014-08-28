#include "win_net.h"

ccError ccNetInitialize(void)
{
	WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
		return CC_ERROR_NET;
    }

	return CC_ERROR_NONE;
}

ccError ccNetFree(void)
{
	WSACleanup();

	return CC_ERROR_NONE;
}
