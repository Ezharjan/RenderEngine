#include "transform_t.h"
#include "mini3dmath.h"


transform_t::transform_t()
{
}


transform_t::~transform_t()
{
}

void transform_t::transform_update()
{
	transform = world * view * projection;
}

void transform_t::transform_init(const int width, const int height)
{
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	world.identity();
	view.identity();
	mini3dmath::setPerspectiveMatrix(projection, 3.1415926f * 0.5f, aspect, 1.0f, 500.0f);
	this->width = static_cast<float>(width);
	this->height = static_cast<float>(height);
	transform_update();
}

vector_t transform_t::transform_apply(const vector_t& point_untransformed) const
{
	vector_t point_transformed = static_cast<vector_t>(point_untransformed)*transform;
	return point_transformed;
}

vector_t transform_t::tranform_homogenize(const vector_t& v)const
{
	vector_t result;
	float w_reciprocal = 1.0f / v.w;
	result.x = (v.x * w_reciprocal + 1.0f)*0.5f*width;
	result.y = (1.0f - v.y *w_reciprocal)*0.5f*height;
	result.z = v.z * w_reciprocal;
	result.w = 1.0f;

	return result;
}




