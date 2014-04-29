#pragma once

#define true 1
#define false 0

typedef char bool;

typedef struct {
	union {
		int x, width;
	};
	union {
		int y, height;
	}
} ccPoint;
