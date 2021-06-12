#pragma once
#include "vector_t.h"
#include "texturecoord_t.h"
#include "color_t.h"

class vertex_t
{
public:
	vertex_t();
	vertex_t(float positionX, float positionY, float positionZ, float positionW,
		float texturecoordinateU, float texturecoordinateV,
		float colorR, float colorG, float colorB,
		float W_reciprocal);
	vertex_t(vector_t _position, texturecoord_t _texturecoordinate, color_t _color, float _w_reciprocal);
	~vertex_t();

	//����rhw�����ĳ�ʼ�� https://www.zhihu.com/question/40624282
	void vertex_w_reciprocal_init();
	//�����ֵ�������������㣩
	void vertex_interp(const vertex_t& x1, const vertex_t& x2, float t);
	//������������ڲ�ֵ��
	void vertex_division(const vertex_t& x1, const vertex_t& x2, float w);
	//����ӷ�
	vertex_t& operator+=(const vertex_t& other);
	void operator=(const vertex_t& other);

	vector_t position;
	texturecoord_t texturecoordinate;
	color_t color;
	float w_reciprocal;//w_reciprocal��w�ĵ�����w���壩
};

