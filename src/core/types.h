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

#ifdef __GNUC__
 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

#endif

int ccRectIntersectionArea(ccRect *rectA, ccRect *rectB);
