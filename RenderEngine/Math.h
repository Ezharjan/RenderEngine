#pragma once

#ifndef __Math__
#define __Math__

#include "CustomizedException.h"
#include "Vector.h"
#include "Matrix.h"

namespace RenderEngine {

#pragma pack(push)
	struct CGVertex
	{
		float x, y, z, w;
		float u, v;  //纹理坐标
		float nx, ny, nz; //法线向量
		byte r, g, b; //这主要是用于Gouraud着色
		bool IsCull = false;  //是否被剔除了，默认不被剔除

		float getX() const { return x; }
		float getY() const { return y; }
		float getZ() const { return z; }
		float getW() const { return w; }
		void setX(const float inX) { x = inX; }
		void setY(const float inY) { y = inY; }
		void setZ(const float inZ) { z = inZ; }
		void setW(const float inW) { w = inW; }
	};
#pragma pack(pop)


	float LinearInerpolate(const float& x1,const float& x2,const float& t)
	{
		return x1 + (x2 - x1) * t; //线性插值运算公式
	}


	// 夹挤运算 --- 拉格朗日夹挤准则
	float Clamp(const float& param,const float& min,const float& max)
	{
		if (param <= min)
			return min;
		else if (param >= max)
			return max;

		return param;
		
		// 下方优化上述算法的结构
		//return ((param < min) ? min : ((param > max) ? max : param));
	}

	
	template<typename T>
	void VectorNormalize(T& v4)
	{
		float length = (float)sqrt(pow(v4.getX(), 2) + pow(v4.getY(), 2) + pow(v4.getZ(), 2));
		v4.setX(v4.getX() / length);
		v4.setY(v4.getY() / length);
		v4.setZ(v4.getZ() / length);
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
}

#endif // !__Math__
