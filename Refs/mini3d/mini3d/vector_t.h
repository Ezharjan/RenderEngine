#pragma once

class matrix_t;

//D3D����������
class vector_t
{
public:
	vector_t();
	vector_t(float x, float y, float z, float w) :x(x), y(y), z(z), w(w){};
	~vector_t();


	// | v |�����������ĳ���
	float get_vector_length();

	//�������
	vector_t operator+(const vector_t& other)const;
	//�������
	vector_t operator-(const vector_t& other)const;
	//�����ҳ˾���
	vector_t operator*(const matrix_t& matrix)const;

	//�������
	float dotProduct(const vector_t& other)const;
	//�������,��ʽa��b=(xa,ya,za)��(xb,yb,zb)=(ya*zb-yb*za,xa*zb-xb*za,xa*yb-xb*ya)��
	vector_t crossProduct(const vector_t& other) const;
	// ������ֵ��tȡֵ [0, 1]
	vector_t& vector_interp(const vector_t& x1,const vector_t& x2,const float t);
	//������һ��
	vector_t& normalize();

	float x;
	float y;
	float z;
	float w;
};

