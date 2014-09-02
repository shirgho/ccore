#pragma once

#include <ccore/net.h>

#define CC_RETURN_ON_SOCKET_ERROR(x) if(x == INVALID_SOCKET){ \
		ccErrorPush(CC_ERROR_NET); \
		return CC_FAIL; \
	} \
	return CC_SUCCESS

#define CC_RETURN_ON_ERROR(x) if(x == -1){ \
		ccErrorPush(CC_ERROR_NET); \
		return CC_FAIL; \
	} \
	return CC_SUCCESS
