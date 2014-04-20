#pragma once

#include <stdlib.h>
#include <Windows.h>
#include <stdio.h>

#include "../../core/types.h"
#include "../../core/interface/event.h"
#include "../../core/utils/error.h"

typedef struct _ccWindow ccWindow;

typedef struct _ccWindow{
	//Note: dimensions subtract border size
	unsigned short width;
	unsigned short height;
	ccEvent event;

	//OS specific values
	HDC hdc;
	MSG msg;
	HWND winHandle;
	HGLRC renderContext;
} ccWindow;
