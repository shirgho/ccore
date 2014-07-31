#pragma once

#ifdef __linux__
#include "../src/linux/core.h"
#include "../src/x11/core.h"

#elif defined _WIN32
#include "../../src/ccore/win32/core.h"
#else
#error "OS not supported!"
#endif