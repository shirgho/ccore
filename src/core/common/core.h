#pragma once

#ifdef __linux__
#include "../linux/core.h"
#ifdef GTK
#include "../gtk/core.h"
#else
#define X11
#include "../x11/core.h"
#endif

#elif defined _WIN32
#include "../win32/core.h"
#else
#error "OS not supported!"
#endif
