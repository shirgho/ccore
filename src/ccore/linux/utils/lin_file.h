#pragma once

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <sys/stat.h>

#include <ccore/file.h>
#include <ccore/string.h>

#ifndef CC_USER_LOCATION
#define CC_USER_LOCATION "~/.config/"
#endif

#ifndef CC_TEMP_LOCATION
#define CC_TEMP_LOCATION "/tmp/"
#endif
