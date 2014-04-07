#pragma once

#ifdef __linux__
#define LINUX
#define OSLOC "core_linux/"

#ifdef gtk
#define GTK
#define GFXLOC "core_gtk/"
#else
#define X11
#define GFXLOC core_x11
#endif

#elif _WIN32
#define WIN32
#define GFXLOC "core_win32"

#else
#error "OS not supported!"
#endif

#define __LOCATION(x) #x
#define _LOCATION(x, loc) __LOCATION(../../loc/x.h)
#define LOCATION_GFX(x) _LOCATION(x, GFXLOC)
#define LOCATION_OS(x) _LOCATION(x, OSLOC)
