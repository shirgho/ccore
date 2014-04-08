#pragma once

#ifdef __linux__
#include "../core_x11/core.h"
#elif defined _WIN32
#include "../core_win32/core.h."
#else
#error "OS not supported!"
#endif