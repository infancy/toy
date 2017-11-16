#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef TOY_MATH_MATRIX_H
#define TOY_MATH_MATRIX_H

#include"toy.h"

namespace toy
{

// Row Major

class Matrix
{
public:
	using T = int;

	Matrix(){}

	Matrix operator*(const Matrix& rhs)
	{
		Matrix ret;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				//ret.m[i][j] = m.row(i) * rhs.col(j)
				ret.m[i][j] = m[i][0] * rhs.m[0][j] + 
							  m[i][1] * rhs.m[1][j] +
							  m[i][2] * rhs.m[2][j] +
							  m[i][3] * rhs.m[3][j];
		return ret;
	}

private:
	T& operator()(int row, int col)
	{
		DCHECK(0 <= row && row < 4 && 0 <= col && col < 4);
		return m[row][col];
	}

private:
	T m[4][4];
};

}	//namespace toy

#endif //TOY_MATH_MATRIX_H
