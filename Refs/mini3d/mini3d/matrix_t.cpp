#include "matrix_t.h"


matrix_t::matrix_t()
{
}


matrix_t::~matrix_t()
{
}

matrix_t matrix_t::operator+(const matrix_t& other) const
{
	matrix_t result;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			result.m[i][j] = m[i][j] + other.m[i][j];
	return result;
}

matrix_t matrix_t::operator-(const matrix_t& other) const
{
	matrix_t result;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			result.m[i][j] = m[i][j] - other.m[i][j];
	return result;
}

matrix_t matrix_t::operator*(const matrix_t& other) const
{
	matrix_t result;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			result.m[j][i] = (m[j][0] * other.m[0][i]) +
			(m[j][1] * other.m[1][i]) +
			(m[j][2] * other.m[2][i]) +
			(m[j][3] * other.m[3][i]);
	return result;
}

void matrix_t::scale(const float f)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m[i][j] = m[i][j] * f;
}

void matrix_t::identity()
{
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
	m[0][1] = m[0][2] = m[0][3] = 0.0f;
	m[1][0] = m[1][2] = m[1][3] = 0.0f;
	m[2][0] = m[2][1] = m[2][3] = 0.0f;
	m[3][0] = m[3][1] = m[3][2] = 0.0f;
}

void matrix_t::zero()
{
	m[0][0] = m[0][1] = m[0][2] = m[0][3] = 0.0f;
	m[1][0] = m[1][1] = m[1][2] = m[1][3] = 0.0f;
	m[2][0] = m[2][1] = m[2][2] = m[2][3] = 0.0f;
	m[3][0] = m[3][1] = m[3][2] = m[3][3] = 0.0f;
}


