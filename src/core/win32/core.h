#pragma once

#define WIN32

//Includes openGL libraries when using visual studio:
#ifdef _MSC_VER
#pragma comment(lib, "opengl32")
#pragma comment(lib, "glu32")
#endif