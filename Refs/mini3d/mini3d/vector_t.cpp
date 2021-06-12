#include "vector_t.h"
#include <cmath>
#include "mini3dmath.h"
#include "matrix_t.h"


vector_t::vector_t()
{
}


vector_t::~vector_t()
{
}

float vector_t::get_vector_length()
{
	float sq = x*x + y*y + z*z;
	return static_cast<float>(sqrt(sq));
}

vector_t vector_t::operator+(const vector_t& other) const
{
	vector_t temp;
	temp.x = x + other.x;
	temp.y = y + other.y;
	temp.z = z + other.z;
	temp.w = 1.0f;
	return temp;
}

vector_t vector_t::operator-(const vector_t& other) const
{
	vector_t temp;
	temp.x = x - other.x;
	temp.y = y - other.y;
	temp.z = z - other.z;
	temp.w = 1.0f;
	return temp;
}

vector_t vector_t::operator*(const matrix_t& matrix) const
{
	vector_t result;
	auto m = matrix.m;
	result.x = x* m[0][0] + y*m[1][0] + z * m[2][0] + w*m[3][0];
	result.y = x* m[0][1] + y*m[1][1] + z * m[2][1] + w*m[3][1];
	result.z = x* m[0][2] + y*m[1][2] + z * m[2][2] + w*m[3][2];
	result.w = x* m[0][3] + y*m[1][3] + z * m[2][3] + w*m[3][3];
	return result;
}

//void vector_t::operator=(const vector_t& other)
//{
//	x = other.x;
//	y = other.y;
//	z = other.z;
//	w = other.w;
//}

float vector_t::dotProduct(const vector_t& other) const
{
	return x*other.x + y*other.y + z * other.z;
}

vector_t vector_t::crossProduct(const vector_t& other) const
{
	float m1, m2, m3;
	m1 = y*other.z - z*other.y;
	m2 = z*other.x - x*other.z;
	m3 = x*other.y - y*other.x;
	return vector_t(m1, m2, m3, 1.0f);
}

vector_t& vector_t::vector_interp(const vector_t& x1, const vector_t& x2, const float t)
{
	x = mini3dmath::interp(x1.x, x2.x, t);
	y = mini3dmath::interp(x1.y, x2.y, t);
	z = mini3dmath::interp(x1.z, x2.z, t);
	w = 1.0f;
	return *this;
}

vector_t& vector_t::normalize()
{
	float length = get_vector_length();
	if (length != 0.0f)
	{
		float inv = 1.0f / length;
		x *= inv;
		y *= inv;
		z *= inv;
	}
	return *this;
}


