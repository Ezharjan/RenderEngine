#pragma once

#include <iostream>
#include <iostream>
#include "Vector.h"
#include "Textcoord.h"
#include "Config.h"


namespace RenderEngine {

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

	struct Colour;
	struct  CGVertex;

	float DegreeToRad(const float& degree);

	float RadToDegree(const float& rad);

	float LinearInerpolate(const float& x1, const float& x2, const float& t);

	float Lerp(float a, float b, float t);

	Vector4 Lerp(const Vector4& a, const Vector4& b, float t);

	Colour Lerp(const Colour& a, const Colour& b, float t);

	float Clamp(const float& param, const float& min, const float& max);

	void CLAMP(float& param, const float& min, const float& max);

	void ColorInterpolation(CGVertex& s1, CGVertex& s3, CGVertex& s4);

	bool isPrimeNumber(const int& num);

	float StringToNum(const std::string& str);

	int CharToNum(const char& c);

	float Sigmoid(const float x);

	unsigned char FloatToByte(const float x);

	float MapTo0_255f(const float x);

	float GetGrayScaleViaGamaCorrection(const Colour& valueToBeCorrected);

	Colour GetGrayScaleViaGamaCorrection(const float sameRGBValue);

	float GetBrightnessViaGamaCorrection(const Colour& valueToBeCorrected);

	Colour GetBrightnessViaGamaCorrection(const float sameRGBValue);

	// Greatest double predecessor of 256:
	constexpr double MAXCOLOR = 256.0 - std::numeric_limits<double>::epsilon() * 128;
	inline uint32_t float_to_int_color(const double color) {
		return static_cast<uint32_t>(color * MAXCOLOR);
	}


	class Matrix4f;
	Vector4 Vector4DotMatrix4f(const Vector4& vec4, const Matrix4f& m4f);

}