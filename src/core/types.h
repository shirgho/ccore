#pragma once

#define true 1
#define false 0

typedef char bool;

typedef struct {
	int x, y;
} ccPoint;

typedef struct {
	int x, y;
	int width, height;
} ccRect;

int ccRectIntersectionArea(ccRect *rectA, ccRect *rectB);