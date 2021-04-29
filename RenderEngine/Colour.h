#pragma once

#include <Windows.h>
#include "Math.h"

namespace RenderEngine {

	struct Colour {
		Colour() = default;
		Colour(const float red, const float green, const float blue) :
			r(red), g(green), b(blue){}
		Colour(const Colour& right)
		{
			this->r = right.r;
			this->g = right.g;
			this->b = right.b;
		}

		bool operator ==(const Colour& right)
		{
			if (this->r - right.r <= EPSILON
				&& this->g - right.g <= EPSILON
				&& this->b - right.b <= EPSILON)
			{
				return true;
			}
			else {
				return false;
			}
		}

		Colour& operator =(const Colour& right)
		{
			if (this == &right) 
			{
				return *this;
			}
			this->r = right.r;
			this->g = right.g;
			this->b = right.b;
			return *this;
		}
		Colour ClampColor(Colour& color)
		{
			Clamp(color.r, 0.f, 1.f);
			Clamp(color.g, 0.f, 1.f);
			Clamp(color.b, 0.f, 1.f);
			return color;
		}
		Colour operator /(const float right) {
			Colour result;
			result.r = this->r / right;
			result.g = this->g / right;
			result.b = this->b / right;
			return result;
		}
		Colour operator *(const Colour& right) const
		{
			Colour result;
			result.r = this->r * right.r;
			result.g = this->g * right.g;
			result.b = this->b * right.b;
			return result;
		}
		Colour operator *(const float& right) const
		{
			Colour result;
			result.r = this->r * right;
			result.g = this->g * right;
			result.b = this->b * right;
			return result;
		}
		Colour operator +(const Colour& right)
		{
			Colour result;
			result.r = this->r + right.r;
			result.g = this->g + right.g;
			result.b = this->b + right.b;
			return result;
		}
		Colour operator -(const Colour& right)
		{
			Colour result;
			result.r = this->r - right.r;
			result.g = this->g - right.g;
			result.b = this->b - right.b;
			return result;
		}
		Colour& operator *=(const Colour& right) 
		{
			return (*this) = (*this * right);
		}

		float r, g, b;
	};
	
	inline std::ostream& operator <<(std::ostream& os, const Colour& right) {
		os << "r: " << right.r << ";  g: " << right.g << ";  b: " << right.b;
		return os;
	}
}

