/*��Ҫ������������εĹ�դ������*/
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
		// ��������ͬһˮƽ��,�����ʧ��
		if (p1->y == p2->y && p1->y == p3->y)
		{
			return false;
		}

		// ��������ͬһ��ֱƽ��,�����ʧ��
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
		// ���S1��S3��Z������ռ��ֵ
		float A = FAR_CLIPPINFG_PLANE / (FAR_CLIPPINFG_PLANE - NEAR_CLIPPING_PLANE);
		float B = -(FAR_CLIPPINFG_PLANE* NEAR_CLIPPING_PLANE) / (FAR_CLIPPINFG_PLANE - NEAR_CLIPPING_PLANE);


		// ���S1��S3��������ռ��Z�ĵ���
		float ZCamera_S3_Rec = 1.0f / (B / (s3.z - A));
		float ZCamera_S1_Rec = 1.0f / (B / (s1.z - A));

		// ���S1��S3�� U/Zcamera, V/Camera, 1/Zcamera, nx/Zcamera,ny/Zcamera, nz/Zcamera, 
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


		// ��(1/Zcamera)*U (1/Zcamera)*V  1/Zcamera�������Բ�ֵ
		float Z_S4 = (s4.y - s1.y) *(ZCamera_S3_Rec - ZCamera_S1_Rec) / (s3.y - s1.y) + ZCamera_S1_Rec;
		float ZU_S4 = (s4.y - s1.y) *(ZU_S3 - ZU_S1) / (s3.y - s1.y) + ZU_S1;
		float ZV_S4 = (s4.y - s1.y) *(ZV_S3 - ZV_S1) / (s3.y - s1.y) + ZV_S1;
		float ZNX_S4 = (s4.y - s1.y) *(ZNX_S3 - ZNX_S1) / (s3.y - s1.y) + ZNX_S1;
		float ZNY_S4 = (s4.y - s1.y) *(ZNY_S3 - ZNY_S1) / (s3.y - s1.y) + ZNY_S1;
		float ZNZ_S4 = (s4.y - s1.y) *(ZNZ_S3 - ZNZ_S1) / (s3.y - s1.y) + ZNZ_S1;

		// ��s4��U,Vֵ
		s4.u = ZU_S4 / Z_S4;
		s4.v = ZV_S4 / Z_S4;
		s4.nx = ZNX_S4 / Z_S4;
		s4.ny = ZNY_S4 / Z_S4;
		s4.nz = ZNZ_S4 / Z_S4;


		// s4��Zֵ���Դ�Z����ĵ���������� 
		s4.z = Z_S4 * B + A;
	}

	inline void RasterTriangle::DecideTriangleState(CGVertex s1, CGVertex s2, CGVertex s3)
	{
		// Ϊƽ��������
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
		// Ϊƽ��������
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
		/*��һ�������������Ϊƽ������ƽ��������*/
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


		/*�ڶ��������������Ϊб������*/
		// �����(����Ļ�ռ��Yֵ���)��ΪS3����͵�ΪS1���м��ΪS2��S4,�߶�{S2,S4}Ϊб�����ηָ��ߣ��� s3y>s2y>s1y
		// ���������
		else
		{
			CGVertex s1;
			CGVertex s2;
			CGVertex s3;

			// Yֵ��С˳��321
			if ((p3->y > p2->y) && (p2->y > p1->y))
			{
				s1 = *p1;
				s2 = *p2;
				s3 = *p3;

			}
			// Yֵ��С˳��312
			else if ((p3->y > p1->y) && (p1->y > p2->y))
			{
				s1 = *p2;
				s2 = *p1;
				s3 = *p3;
			}

			// Yֵ��С˳��231
			else if ((p2->y > p3->y) && (p3->y > p1->y))
			{
				s1 = *p1;
				s2 = *p3;
				s3 = *p2;
			}

			// Yֵ��С˳��213
			else if ((p2->y > p1->y) && (p1->y > p3->y))
			{
				s1 = *p3;
				s2 = *p1;
				s3 = *p2;
			}

			// Yֵ��С˳��123
			else if ((p1->y > p2->y) && (p2->y > p3->y))
			{
				s1 = *p3;
				s2 = *p2;
				s3 = *p1;

			}

			// Yֵ��С˳��132
			else if ((p1->y > p3->y) && (p3->y > p2->y))
			{
				s1 = *p2;
				s2 = *p3;
				s3 = *p1;
			}

			// �����s4��X,Y,Z,R,G,Bֵ
			CGVertex s4;
			s4.y = s2.y;
			s4.x = (s4.y - s1.y) *(s3.x - s1.x) / (s3.y - s1.y) + s1.x;

			if (rasterType == InGouraud)
			{
				ColorInterpolation(s1, s3, s4);

				// s1s2s4Ϊƽ��������
				DrawGouroudTriangle(false, &s1, &s2, &s4);
				// s3s2s4Ϊƽ��������
				DrawGouroudTriangle(true, &s3, &s2, &s4);
			}
			else if (rasterType == InTexture)
			{
				TextureInterpolation(s1, s3, s4);

				// s1s2s4Ϊƽ��������
				DrawTextureTriangle(false, &s1, &s2, &s4);
				// s3s4s2Ϊƽ��������
				DrawTextureTriangle(true, &s3, &s2, &s4);
			}
		}
	}
}

#endif // !__RasterTriangle__
