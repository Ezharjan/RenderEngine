#pragma once


class matrix_t
{
public:
	matrix_t();
	~matrix_t();

	//矩阵相加
	matrix_t operator+(const matrix_t& other)const;
	//矩阵相减
	matrix_t operator-(const matrix_t& other)const;
	//矩阵相乘
	matrix_t operator*(const matrix_t& other)const;
	//矩阵缩放
	void scale(const float f);
	//矩阵设置为单位矩阵
	void identity();
	//矩阵设置为0矩阵
	void zero();



	float m[4][4];//4*4矩阵

};

