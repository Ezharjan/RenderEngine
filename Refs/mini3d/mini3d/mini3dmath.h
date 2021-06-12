#pragma once

#include"vector_t.h"

static class mini3dmath
{
public:
	mini3dmath();
	~mini3dmath();

	//保证x的值在min和max之间，小于min则输出min，大于max则输出max，中间值则输出x
	static int CMID(int x, int min, int max);
	// 计算插值：t 为 [0, 1] 之间的数值
	static float interp(float x1, float x2, float t);
	//矩阵设置为平移矩阵
	static void setTranslateMatrix(matrix_t& matrix, const float x, const float y, const  float z);
	//设置缩放矩阵
	static void setScaleMatrix(matrix_t& matrix, float x, float y, float z);
	//设置旋转矩阵
	static void setRotateMatrix(matrix_t& matrix, float x, float y, float z, float theta);
	//设置视图矩阵
	static void setLookatMatrix(matrix_t& matrix, const vector_t& eye, const vector_t& at, const vector_t& up);
	//设置透视投影矩阵 D3DXMatrixPerspectiveFovLH，D3D投影矩阵，计算机图形学P43
	static void setPerspectiveMatrix(matrix_t& matrix, float fovy, float aspect, float znear, float zfar);
	// 检查齐次坐标同 cvv 的边界用于视锥裁剪
	static int check_cvv(const vector_t& v);
};

