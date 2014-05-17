#include "matrix.h"

ccMat22 ccMat22Zero()
{
	ccMat22 m;

	m._m11 = m._m12 = m._m21 = m._m22 = 0;
}

ccMat22 ccMat22Identity()
{
	ccMat22 m;

	m._m11 = m._m22 = 1;
	m._m12 = m._m21 = 0;

	return m;
}

// Unary functions

float ccMat22Determinant(ccMat22 a)
{
	return a._m11 * a._m22 - a._m12 * a._m21;
}

ccMat22 ccMat22Cofactor(ccMat22 a)
{
	ccMat22 m;

	m._m11 = a._m22;
	m._m12 = -a._m21;
	m._m21 = -a._m12;
	m._m22 = a._m11;

	return m;
}

ccMat22 ccMat22Inverse(ccMat22 a)
{
	ccMat22 m;
	float determinantInv;

	determinantInv = 1.0f / ccMat22Determinant(a);
	m._m11 = a._m11 * determinantInv;
	m._m12 = a._m12 * determinantInv;
	m._m21 = a._m21 * determinantInv;
	m._m22 = a._m22 * determinantInv;

	return m;
}

// Binary functions

ccMat22 ccMat22AddM(ccMat22 a, ccMat22 b)
{
	ccMat22 m;

	m._m11 = a._m11 + b._m11;
	m._m12 = a._m12 + b._m12;
	m._m22 = a._m22 + b._m22;
	m._m21 = a._m21 + b._m21;

	return m;
}

ccMat22 ccMat22MulM(ccMat22 a, ccMat22 b)
{
	ccMat22 m;
	int i;

	m = ccMat22Zero();
	for(i = 0; i < 2; i++){
		m._m11 += a[0][i] * b[i][0];
		m._m12 += a[0][i] * b[i][1];
		m._m21 += a[1][i] * b[i][0];
		m._m22 += a[1][i] * b[i][1];
	}

	return m;
}
