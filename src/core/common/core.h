#pragma once

#ifdef __linux__
#include "../core_linux/core.h"
#ifdef GTK
#include "../core_gtk/core.h"
#else
#define X11
#include "../core_x11/core.h"
#endif

#elif defined _WIN32
#include "../core_win32/core.h"
#else
#error "OS not supported!"
#endif
