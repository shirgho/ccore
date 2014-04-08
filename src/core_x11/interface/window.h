#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "../../core/types.h"
#include "../../core/interface/event.h"
#include "../../core/utils/error.h"

typedef struct {
	unsigned short width;
	unsigned short height;
	ccEvent event;

	bool(*pollEvent)();
} ccWindow;
