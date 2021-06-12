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

	//顶点rhw参数的初始化 https://www.zhihu.com/question/40624282
	void vertex_w_reciprocal_init();
	//顶点插值（根据两个顶点）
	void vertex_interp(const vertex_t& x1, const vertex_t& x2, float t);
	//顶点除法（用于插值）
	void vertex_division(const vertex_t& x1, const vertex_t& x2, float w);
	//顶点加法
	vertex_t& operator+=(const vertex_t& other);
	void operator=(const vertex_t& other);

	vector_t position;
	texturecoord_t texturecoordinate;
	color_t color;
	float w_reciprocal;//w_reciprocal是w的倒数（w缓冲）
};

