/*主要是针对于三角形的光栅化方法*/
#pragma once

#include "Math.h"
#include "DrawTriangle.h"

#ifndef __RasterTriangle__
#define __RasterTriangle__


namespace RenderEngine {

	enum RasterType
	{
		InGouraud,
		InTexture
	};

	class RasterTriangle
	{
	public:
		RasterTriangle(RasterType rasterizationType, CGVertex* point1, CGVertex* point2, CGVertex* point3);
		~RasterTriangle() {};
		void Rasterize();
		bool isValid();
		void ColorInterpolation(CGVertex& s1, CGVertex& s3, CGVertex& s4); // GouraudInterpolation
		void TextureInterpolation(CGVertex& s1, CGVertex& s3, CGVertex& s4);
		void ConsiderFlatOrDiagonal();
		void DecideTriangleState(CGVertex s1, CGVertex s2, CGVertex s3);

	private:
		RasterType rasterType;
		CGVertex* p1;
		CGVertex* p2;
		CGVertex* p3;

	};

	RasterTriangle::RasterTriangle(RasterType rasterizationType, CGVertex* p1, CGVertex* p2, CGVertex* p3) :
		rasterType(rasterizationType), p1(p1), p2(p2), p3(p3)
	{
		this->Rasterize();
	}


	inline void RasterTriangle::Rasterize()
	{
		if (isValid())
		{
			ConsiderFlatOrDiagonal();
		}
		else
		{
			return;
		}
	}

	inline bool RasterTriangle::isValid()
	{
		// 三个点在同一水平面,则绘制失败
		if (p1->y == p2->y && p1->y == p3->y)
		{
			return false;
		}

		// 三个点在同一垂直平面,则绘制失败
		if (p1->x == p2->x && p1->x == p3->x)
		{
			return false;
		}
		return true;
	}

	inline void RasterTriangle::ColorInterpolation(CGVertex& s1, CGVertex& s3, CGVertex& s4)
	{
		s4.r = (s4.y - s1.y) *(s3.r - s1.r) / (s3.y - s1.y) + s1.r;
		s4.g = (s4.y - s1.y) *(s3.g - s1.g) / (s3.y - s1.y) + s1.g;
		s4.b = (s4.y - s1.y) *(s3.b - s1.b) / (s3.y - s1.y) + s1.b;
	}

	inline void RasterTriangle::TextureInterpolation(CGVertex& s1, CGVertex& s3, CGVertex& s4)
	{
		// 求出S1和S3的Z在相机空间的值
		float A = FAR_CLIPPINFG_PLANE / (FAR_CLIPPINFG_PLANE - NEAR_CLIPPING_PLANE);
		float B = -(FAR_CLIPPINFG_PLANE* NEAR_CLIPPING_PLANE) / (FAR_CLIPPINFG_PLANE - NEAR_CLIPPING_PLANE);


		// 求出S1和S3的在相机空间的Z的倒数
		float ZCamera_S3_Rec = 1.0f / (B / (s3.z - A));
		float ZCamera_S1_Rec = 1.0f / (B / (s1.z - A));

		// 求出S1和S3的 U/Zcamera, V/Camera, 1/Zcamera, nx/Zcamera,ny/Zcamera, nz/Zcamera, 
		float ZU_S3 = s3.u * ZCamera_S3_Rec;
		float ZU_S1 = s1.u * ZCamera_S1_Rec;
		float ZV_S3 = s3.v * ZCamera_S3_Rec;
		float ZV_S1 = s1.v * ZCamera_S1_Rec;
		float ZNX_S3 = s3.nx * ZCamera_S3_Rec;
		float ZNX_S1 = s1.nx * ZCamera_S1_Rec;
		float ZNY_S3 = s3.ny * ZCamera_S3_Rec;
		float ZNY_S1 = s1.ny * ZCamera_S1_Rec;
		float ZNZ_S3 = s3.nz * ZCamera_S3_Rec;
		float ZNZ_S1 = s1.nz * ZCamera_S1_Rec;


		// 对(1/Zcamera)*U (1/Zcamera)*V  1/Zcamera进行线性插值
		float Z_S4 = (s4.y - s1.y) *(ZCamera_S3_Rec - ZCamera_S1_Rec) / (s3.y - s1.y) + ZCamera_S1_Rec;
		float ZU_S4 = (s4.y - s1.y) *(ZU_S3 - ZU_S1) / (s3.y - s1.y) + ZU_S1;
		float ZV_S4 = (s4.y - s1.y) *(ZV_S3 - ZV_S1) / (s3.y - s1.y) + ZV_S1;
		float ZNX_S4 = (s4.y - s1.y) *(ZNX_S3 - ZNX_S1) / (s3.y - s1.y) + ZNX_S1;
		float ZNY_S4 = (s4.y - s1.y) *(ZNY_S3 - ZNY_S1) / (s3.y - s1.y) + ZNY_S1;
		float ZNZ_S4 = (s4.y - s1.y) *(ZNZ_S3 - ZNZ_S1) / (s3.y - s1.y) + ZNZ_S1;

		// 求s4的U,V值
		s4.u = ZU_S4 / Z_S4;
		s4.v = ZV_S4 / Z_S4;
		s4.nx = ZNX_S4 / Z_S4;
		s4.ny = ZNY_S4 / Z_S4;
		s4.nz = ZNZ_S4 / Z_S4;


		// s4的Z值可以从Z相机的倒数计算出来 
		s4.z = Z_S4 * B + A;
	}

	inline void RasterTriangle::DecideTriangleState(CGVertex s1, CGVertex s2, CGVertex s3)
	{
		// 为平底三角形
		if (s1.y < s2.y)
		{
			if (rasterType == InGouraud)
			{
				DrawGouroudTriangle(false, &s1, &s2, &s3);
			}
			else if (rasterType == InTexture)
			{
				DrawTextureTriangle(false, &s1, &s2, &s3);
			}
		}
		// 为平顶三角形
		else
		{
			if (rasterType == InGouraud)
			{
				DrawGouroudTriangle(true, &s1, &s2, &s3);
			}
			else if (rasterType == InTexture)
			{
				DrawTextureTriangle(true, &s1, &s2, &s3);
			}
		}
	}

	inline void RasterTriangle::ConsiderFlatOrDiagonal()
	{
		/*第一类情况，三角形为平顶或者平底三角形*/
		if (p1->y == p2->y || p1->y == p3->y || p2->y == p3->y)
		{
			if (p1->y == p2->y)
			{
				DecideTriangleState(*p3, *p1, *p2);

			}
			else  if (p1->y == p3->y)
			{
				DecideTriangleState(*p2, *p1, *p3);
			}
			else  if (p2->y == p3->y)
			{
				DecideTriangleState(*p1, *p2, *p3);
			}
		}


		/*第二类情况，三角形为斜三角形*/
		// 让最高(在屏幕空间的Y值最大)点为S3，最低点为S1，中间点为S2和S4,线段{S2,S4}为斜三角形分割线，即 s3y>s2y>s1y
		// 分六种情况
		else
		{
			CGVertex s1;
			CGVertex s2;
			CGVertex s3;

			// Y值大小顺序321
			if ((p3->y > p2->y) && (p2->y > p1->y))
			{
				s1 = *p1;
				s2 = *p2;
				s3 = *p3;

			}
			// Y值大小顺序312
			else if ((p3->y > p1->y) && (p1->y > p2->y))
			{
				s1 = *p2;
				s2 = *p1;
				s3 = *p3;
			}

			// Y值大小顺序231
			else if ((p2->y > p3->y) && (p3->y > p1->y))
			{
				s1 = *p1;
				s2 = *p3;
				s3 = *p2;
			}

			// Y值大小顺序213
			else if ((p2->y > p1->y) && (p1->y > p3->y))
			{
				s1 = *p3;
				s2 = *p1;
				s3 = *p2;
			}

			// Y值大小顺序123
			else if ((p1->y > p2->y) && (p2->y > p3->y))
			{
				s1 = *p3;
				s2 = *p2;
				s3 = *p1;

			}

			// Y值大小顺序132
			else if ((p1->y > p3->y) && (p3->y > p2->y))
			{
				s1 = *p2;
				s2 = *p3;
				s3 = *p1;
			}

			// 计算点s4的X,Y,Z,R,G,B值
			CGVertex s4;
			s4.y = s2.y;
			s4.x = (s4.y - s1.y) *(s3.x - s1.x) / (s3.y - s1.y) + s1.x;

			if (rasterType == InGouraud)
			{
				ColorInterpolation(s1, s3, s4);

				// s1s2s4为平底三角形
				DrawGouroudTriangle(false, &s1, &s2, &s4);
				// s3s2s4为平顶三角形
				DrawGouroudTriangle(true, &s3, &s2, &s4);
			}
			else if (rasterType == InTexture)
			{
				TextureInterpolation(s1, s3, s4);

				// s1s2s4为平底三角形
				DrawTextureTriangle(false, &s1, &s2, &s4);
				// s3s4s2为平顶三角形
				DrawTextureTriangle(true, &s3, &s2, &s4);
			}
		}
	}
}

#endif // !__RasterTriangle__
