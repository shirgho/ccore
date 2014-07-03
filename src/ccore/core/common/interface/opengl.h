#pragma once

#include "../core.h"
#include "window.h"
#include "../utils/error.h"

//Load all extensions using GLEW
#define GLEW_STATIC
#include "../../../../../lib/glew/GL/glew.h"

#ifdef X11
#include "../../x11/interface/opengl.h"
#elif defined WIN32
#include "../../win32/interface/opengl.h"
#endif

//opengl
ccError ccGLBindContext(int glVersionMajor, int glVersionMinor); //bind the openGl context to window
ccError ccGLFreeContext(); //free context
ccError ccGLSwapBuffers(); //swap the buffers
