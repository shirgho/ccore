#pragma once

#include "vector.h"

#define _m11 a[0][0]
#define _m12 a[0][1]
#define _m21 a[1][0]
#define _m22 a[1][1]

typedef struct {
	float a[2][2];
} ccMat22;
