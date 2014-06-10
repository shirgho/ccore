#pragma once

#include "../core.h"

#ifdef LINUX
#include "../../linux/utils/timing.h"
#elif defined WIN32
#include "../../win32/utils/timing.h"
#endif

void ccDelay(int ms);
