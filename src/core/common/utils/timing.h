#pragma once

#include "../core.h"

#ifdef LINUX
#include "../../core_linux/utils/timing.h"
#elif defined WIN32
#include "../../core_win32/utils/timing.h"
#endif

void ccDelay(int ms);
