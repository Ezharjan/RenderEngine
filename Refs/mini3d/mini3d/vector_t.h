#pragma once

class matrix_t;

//D3D采用行向量
class vector_t
{
public:
	vector_t();
	vector_t(float x, float y, float z, float w) :x(x), y(y), z(z), w(w){};
	~vector_t();


	// | v |，计算向量的长度
	float get_vector_length();

	//向量相加
	vector_t operator+(const vector_t& other)const;
	//向量相减
	vector_t operator-(const vector_t& other)const;
	//向量右乘矩阵
	vector_t operator*(const matrix_t& matrix)const;

	//向量点乘
	float dotProduct(const vector_t& other)const;
	//向量叉乘,公式a×b=(xa,ya,za)×(xb,yb,zb)=(ya*zb-yb*za,xa*zb-xb*za,xa*yb-xb*ya)。
	vector_t crossProduct(const vector_t& other) const;
	// 向量插值，t取值 [0, 1]
	vector_t& vector_interp(const vector_t& x1,const vector_t& x2,const float t);
	//向量归一化
	vector_t& normalize();

	float x;
	float y;
	float z;
	float w;
};

