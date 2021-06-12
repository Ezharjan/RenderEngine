#pragma once
class texturecoord_t
{
public:
	texturecoord_t();
	texturecoord_t(float _u, float _v) :u(_u), v(_v) {};
	~texturecoord_t();


	float u, v;
};

