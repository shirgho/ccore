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

<<<<<<< HEAD
#ifdef __GNUC__
 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
=======
#define max(a,b) \
({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })
>>>>>>> a484898bf5c8e849eff34adaef764f6bfc52fce5

#define min(a,b) \
({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

int ccRectIntersectionArea(ccRect *rectA, ccRect *rectB);
