#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

//TODO: fix GL include fuckup
#include "../../common/interface/opengl.h"
#include "../../../../../lib/glew/GL/glew.h"
#include <GL/gl.h>
#include <GL/glx.h>

#include "../../common/types.h"
#include "../../common/interface/event.h"
#include "../../common/utils/error.h"
#include "../../common/debugutils/assert.h"
#include "../../common/debugutils/print.h"

#ifdef LINUX
#include "../../linux/io/key.h"
#endif
