#include "vector.h"

ccVec2 ccVec2Zero()
{
	ccVec2 v;

	v._vx = v._vy = 0;

	return v;
}

ccVec2 ccVec2Identi._vy()
{
	ccVec2 v;

	v._vx = v._vy = 1;

	return v;
}

ccVec2 ccVec2MulM22(ccVec2 a, ccMat22 b)
{
	ccVec2 v;

	v._vx = a._vx * b._m11 + a._vy * b._m12;
	v._vy = a._vx * b._m21 + a._vy * b._m22;

	return v;
}
