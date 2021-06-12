#include "color_t.h"


color_t::color_t()
{
}

color_t::~color_t()
{
}

color_t& color_t::operator*=(float x)
{
	r *= x;
	g *= x;
	b *= x;
	return *this;
}
