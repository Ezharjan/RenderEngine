#include <sstream>
#include <math.h>
#include "Math.h"
#include "Colour.h"
#include "CGVertex.h"

#include "Matrix.h"

namespace RenderEngine {

	float RadToDegree(const float& rad) {
		return (rad * 180.f / MATH_PI);
	}

	float DegreeToRad(const float& degree) {
		return (degree * MATH_PI / 180.f);
	}

	float LinearInerpolate(const float& x1, const float& x2, const float& t)
	{
		return x1 + (x2 - x1) * t; // formula of linear interpolation
	}


	float Lerp(const float a, const float b, const float t)
	{
		if (t <= 0.f)
		{
			return a;
		}
		else if (t >= 1.f)
		{
			return b;
		}
		else
		{
			return b * t + (1.f - t) * a;
		}
	}

	Vector4 Lerp(const Vector4& a, const Vector4& b, float t)
	{
		Vector4 result(
			Lerp(a.getX(), b.getX(), t),
			Lerp(a.getY(), b.getY(), t),
			Lerp(a.getZ(), b.getZ(), t),
			Lerp(a.getW(), b.getW(), t)
		);
		return result;
	}

	Colour Lerp(const Colour& a, const Colour& b, float t)
	{
		Colour result;
		result.r = Lerp(a.r, b.r, t);
		result.g = Lerp(a.g, b.g, t);
		result.b = Lerp(a.b, b.b, t);
		return result;
	}


	float Clamp(const float& param, const float& min, const float& max)
	{
		if (param <= min)
			return min;
		else if (param >= max)
			return max;

		return param;

		// optimized way below
		//return ((param < min) ? min : ((param > max) ? max : param));
	}

	void CLAMP(float& param, const float& min, const float& max)
	{
		if (param <= min)
			param = min;
		else if (param >= max)
			param = max;
	}

	void ColorInterpolation(CGVertex & s1, CGVertex & s3, CGVertex & s4)
	{
		s4.color.r = (s4.pos.getY() - s1.pos.getY()) *(s3.color.r - s1.color.r) / (s3.pos.getY() - s1.pos.getY()) + s1.color.r;
		s4.color.g = (s4.pos.getY() - s1.pos.getY()) *(s3.color.g - s1.color.g) / (s3.pos.getY() - s1.pos.getY()) + s1.color.g;
		s4.color.b = (s4.pos.getY() - s1.pos.getY()) *(s3.color.b - s1.color.b) / (s3.pos.getY() - s1.pos.getY()) + s1.color.b;
	}

	bool isPrimeNumber(const int& num)
	{
		for (size_t i = 2; i < num; i++)
		{
			if (num % i == 0)
			{
				return false;
			}
		}
		return true;
	}


	float StringToNum(const std::string & str)
	{
		std::istringstream iss(str);
		int num;
		iss >> num;
		return (float)num;
	}


	int CharToNum(const char& c)
	{
		return atoi(&c);
	}

	float Sigmoid(const float x)
	{
		return (1 / 1 + exp(-x));
	}

	unsigned char FloatToByte(const float x)
	{
		return unsigned char((int)(x * 255.f) % 256);
	}

	float MapTo0_255f(const float x)
	{
		float result = Clamp(x, 0.f, 0.999999f) * 255.f; // never dropping value ranging 0~1
		if (result - EPSILON >= 1.f)
			result = floorf(Clamp(x, 0.f, 0.999999f) * 256.f);
		return result;
	}

	float GetGrayScaleViaGamaCorrection(const Colour& valueToBeCorrected)
	{
		float numerator = powf(MapTo0_255f(valueToBeCorrected.r), 2.2f) + powf((1.5f*MapTo0_255f(valueToBeCorrected.g)), 2.2f) + powf((0.6f*MapTo0_255f(valueToBeCorrected.b)), 2.2f);
		float denominator = 1.f + powf(1.5f, 2.2f) + powf(0.6f, 2.2f);
		float result = powf((numerator / denominator), 1.f / 2.2f);
		return result;
	}

	Colour GetGrayScaleViaGamaCorrection(const float sameRGBValue)
	{
		float numerator = powf(MapTo0_255f(sameRGBValue), 2.2f) + powf((1.5f*MapTo0_255f(sameRGBValue)), 2.2f) + powf((0.6f*MapTo0_255f(sameRGBValue)), 2.2f);
		float denominator = 1.f + powf(1.5f, 2.2f) + powf(0.6f, 2.2f);
		float result = powf((numerator / denominator), 1.f / 2.2f);
		return  Colour(result, result, result);
	}

	float GetBrightnessViaGamaCorrection(const Colour & valueToBeCorrected)
	{
		float numerator = powf(valueToBeCorrected.r, 2.2f) + powf((1.5f*valueToBeCorrected.g), 2.2f) + powf((0.6f*valueToBeCorrected.b), 2.2f);
		float denominator = 1.f + powf(1.5f, 2.2f) + powf(0.6f, 2.2f);
		float result = powf((numerator / denominator), 1.f / 2.2f);
		return result;
	}

	Colour GetBrightnessViaGamaCorrection(const float sameRGBValue)
	{
		float numerator = powf(sameRGBValue, 2.2f) + powf((1.5f*sameRGBValue), 2.2f) + powf((0.6f*sameRGBValue), 2.2f);
		float denominator = 1.f + powf(1.5f, 2.2f) + powf(0.6f, 2.2f);
		float result = powf((numerator / denominator), 1.f / 2.2f);
		return  Colour(result, result, result);
	}


	Vector4 Vector4DotMatrix4f(const Vector4 & vec4, const Matrix4f & m4f)
	{
		Vector4 result;
		result.setX(vec4.getX() * m4f.matrix[0][0] + vec4.getY() * m4f.matrix[1][0] + vec4.getZ() * m4f.matrix[2][0] + vec4.getW() * m4f.matrix[3][0]);
		result.setY(vec4.getX() * m4f.matrix[0][1] + vec4.getY() * m4f.matrix[1][1] + vec4.getZ() * m4f.matrix[2][1] + vec4.getW() * m4f.matrix[3][1]);
		result.setZ(vec4.getX() * m4f.matrix[0][2] + vec4.getY() * m4f.matrix[1][2] + vec4.getZ() * m4f.matrix[2][2] + vec4.getW() * m4f.matrix[3][2]);
		result.setW(vec4.getX() * m4f.matrix[0][3] + vec4.getY() * m4f.matrix[1][3] + vec4.getZ() * m4f.matrix[2][3] + vec4.getW() * m4f.matrix[3][3]);
		return result;
	}
}