#pragma once

#ifdef __linux__
#include "../linux/core.h"
#include "../x11/core.h"

#elif defined _WIN32
#include "../win32/core.h"
#else
#error "OS not supported!"
#endif