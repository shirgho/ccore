#pragma once

#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>

#include <ccore/net.h>

#define CC_RETURN_ON_ERROR(x) if(CC_UNLIKELY(x == -1)){ \
		ccErrorPush(CC_ERROR_NET); \
		return CC_FAIL; \
	} \
	return CC_SUCCESS
