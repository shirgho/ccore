#pragma once

#include "../core.h"

#ifdef LINUX
#include "../../core_x11/interface/popup.h"
#elif defined WIN32
#include "../../core_win32/interface/popup.h"
#endif