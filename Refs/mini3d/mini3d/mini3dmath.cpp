#include "mini3dmath.h"
#include "matrix_t.h"
#include <cmath>


mini3dmath::mini3dmath()
{
}


mini3dmath::~mini3dmath()
{
}

int mini3dmath::CMID(int x, int min, int max)
{
	return (x<min) ? min : ((x>max) ? max : x);
}

float mini3dmath::interp(float x1, float x2, float t)
{
	return x1 + (x2 - x1)*t;
}

void mini3dmath::setTranslateMatrix(matrix_t& matrix, const float x, const float y, const float z)
{
	matrix.identity();
	matrix.m[3][0] = x;
	matrix.m[3][1] = y;
	matrix.m[3][2] = z;
}

void mini3dmath::setScaleMatrix(matrix_t& matrix, float x, float y, float z)
{
	matrix.identity();
	matrix.m[0][0] = x;
	matrix.m[1][1] = y;
	matrix.m[2][2] = z;
}

void mini3dmath::setRotateMatrix(matrix_t& matrix, float x, float y, float z, float theta)
{
	float qsin = static_cast<float>(sin(theta*2));
	float qcos = static_cast<float>(cos(theta*2));
	vector_t vec(x, y, z, 1.0f);
	float w = qcos;
	vec.normalize();
	x = vec.x * qsin;
	y = vec.y * qsin;
	z = vec.z * qsin;

	matrix.m[0][0] = 1 - 2 * y * y - 2 * z * z;
	matrix.m[1][0] = 2 * x * y - 2 * w * z;
	matrix.m[2][0] = 2 * x * z + 2 * w * y;
	matrix.m[0][1] = 2 * x * y + 2 * w * z;
	matrix.m[1][1] = 1 - 2 * x * x - 2 * z * z;
	matrix.m[2][1] = 2 * y * z - 2 * w * x;
	matrix.m[0][2] = 2 * x * z - 2 * w * y;
	matrix.m[1][2] = 2 * y * z + 2 * w * x;
	matrix.m[2][2] = 1 - 2 * x * x - 2 * y * y;
	matrix.m[0][3] = matrix.m[1][3] = matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = matrix.m[3][1] = matrix.m[3][2] = 0.0f;
	matrix.m[3][3] = 1.0f;


}

void mini3dmath::setLookatMatrix(matrix_t& matrix, const vector_t& eye, const vector_t& at, const vector_t& up)
{
	vector_t xAxis, yAxis, zAxis;
	//求uvn坐标
	zAxis = at - eye;//左手坐标系，指向at
	zAxis.normalize();//得到单位向量n
	xAxis = up.crossProduct(zAxis);//得到单位向量u
	xAxis.normalize();
	yAxis = zAxis.crossProduct(xAxis);//得到单位向量v

	matrix.m[0][0] = xAxis.x;
	matrix.m[1][0] = xAxis.y;
	matrix.m[2][0] = xAxis.z;
	matrix.m[3][0] = -xAxis.dotProduct(eye);

	matrix.m[0][1] = yAxis.x;
	matrix.m[1][1] = yAxis.y;
	matrix.m[2][1] = yAxis.z;
	matrix.m[3][1] = -yAxis.dotProduct(eye);

	matrix.m[0][2] = zAxis.x;
	matrix.m[1][2] = zAxis.y;
	matrix.m[2][2] = zAxis.z;
	matrix.m[3][2] = -zAxis.dotProduct(eye);

	matrix.m[0][3] = matrix.m[1][3] = matrix.m[2][3] = 0.0f;
	matrix.m[3][3] = 1.0f;

}

void mini3dmath::setPerspectiveMatrix(matrix_t& matrix, float fovy, float aspect, float znear, float zfar)
{
	float fax = 1.0f / static_cast<float>(tan(fovy * 0.5f));
	matrix.zero();

	matrix.m[0][0] = static_cast<float>(fax / aspect);
	matrix.m[1][1] = static_cast<float>(fax);
	matrix.m[2][2] = zfar / (zfar - znear);
	matrix.m[3][2] = -znear * zfar / (zfar - znear);
	matrix.m[2][3] = 1;
}

int mini3dmath::check_cvv(const vector_t& v)
{
	float w = v.w;
	int check = 0;
	//只要以下其中一个条件满足，返回值check就不为0，即说明需要被裁剪
	//因为后面透视除法会除以w，进入标准设备坐标系，如果此时比w大或者比w小，透视除法之后会大于1或者小于-1，则会被剔除。
	if (v.z < 0.0f) check |= 1;
	if (v.z > w) check |= 2;
	if (v.x < -w) check |= 4;
	if (v.x > w) check |= 8;
	if (v.y < -w) check |= 16;
	if (v.y > w) check |= 32;
	return check;
}



