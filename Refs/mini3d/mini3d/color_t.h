#pragma once
class color_t
{
public:
	color_t();
	color_t(float _r, float _g, float _b) :r(_r), g(_g), b(_b) {};
	~color_t();

	color_t& operator*=(float x);

	float r, g, b;//颜色用0-1之间的数表示
};

