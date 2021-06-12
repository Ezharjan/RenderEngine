#include "vertex_t.h"
#include "mini3dmath.h"

vertex_t::vertex_t()
{

}

vertex_t::vertex_t(float positionX, float positionY, float positionZ, float positionW,
	float texturecoordinateU, float texturecoordinateV,
	float colorR, float colorG, float colorB,
	float W_reciprocal) :position(positionX, positionY, positionZ, positionW), texturecoordinate(texturecoordinateU, texturecoordinateV), color(colorR, colorG, colorB), w_reciprocal(W_reciprocal) {}

vertex_t::vertex_t(vector_t _position, texturecoord_t _texturecoordinate, color_t _color, float _w_reciprocal) : position(_position), texturecoordinate(_texturecoordinate), color(_color), w_reciprocal(_w_reciprocal)
{
};

vertex_t::~vertex_t()
{
}

void vertex_t::vertex_w_reciprocal_init()
{
	float w_reciprocal = 1.0f / position.w;
	this->w_reciprocal = w_reciprocal;
	this->texturecoordinate.u *= w_reciprocal;//透视纹理校正
	this->texturecoordinate.v *= w_reciprocal;
	this->color *= w_reciprocal;
}

void vertex_t::vertex_interp(const vertex_t& x1, const vertex_t& x2, float t)
{
	position.vector_interp(x1.position, x2.position, t);
	texturecoordinate.u = mini3dmath::interp(x1.texturecoordinate.u, x2.texturecoordinate.u, t);//此时的UV坐标都是经过透视校正（除以w）之后的坐标，因此是线性的，插值结果正确
	texturecoordinate.v = mini3dmath::interp(x1.texturecoordinate.v, x2.texturecoordinate.v, t);
	color.r = mini3dmath::interp(x1.color.r, x2.color.r, t);//同上，线性的
	color.g = mini3dmath::interp(x1.color.g, x2.color.g, t);
	color.b = mini3dmath::interp(x1.color.b, x2.color.b, t);
	w_reciprocal = mini3dmath::interp(x1.w_reciprocal, x2.w_reciprocal, t);//同上，线性的
}

void vertex_t::vertex_division(const vertex_t& x1, const vertex_t& x2, float w)
{
	float inv = 1.0f / w;
	position.x = (x2.position.x - x1.position.x)*inv;
	position.y = (x2.position.y - x1.position.y)*inv;
	position.z = (x2.position.z - x1.position.z)*inv;
	position.w = (x2.position.w - x1.position.w)*inv;

	texturecoordinate.u = (x2.texturecoordinate.u - x1.texturecoordinate.u)*inv;
	texturecoordinate.v = (x2.texturecoordinate.v - x1.texturecoordinate.v)*inv;

	color.r = (x2.color.r - x1.color.r) * inv;
	color.g = (x2.color.g - x1.color.g)*inv;
	color.b = (x2.color.b - x1.color.b)*inv;

	w_reciprocal = (x2.w_reciprocal - x1.w_reciprocal)*inv;
}

vertex_t& vertex_t::operator+=(const vertex_t& other)
{
	position.x += other.position.x;
	position.y += other.position.y;
	position.z += other.position.z;

	w_reciprocal += other.w_reciprocal;

	texturecoordinate.u += other.texturecoordinate.u;
	texturecoordinate.v += other.texturecoordinate.v;

	color.r += other.color.r;
	color.g += other.color.g;
	color.b += other.color.b;

	return *this;
}

void vertex_t::operator=(const vertex_t& other)
{
	position = other.position;
	texturecoordinate = other.texturecoordinate;
	color = other.color;
	w_reciprocal = other.w_reciprocal;
}
