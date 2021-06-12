#pragma once


class matrix_t
{
public:
	matrix_t();
	~matrix_t();

	//�������
	matrix_t operator+(const matrix_t& other)const;
	//�������
	matrix_t operator-(const matrix_t& other)const;
	//�������
	matrix_t operator*(const matrix_t& other)const;
	//��������
	void scale(const float f);
	//��������Ϊ��λ����
	void identity();
	//��������Ϊ0����
	void zero();



	float m[4][4];//4*4����

};

