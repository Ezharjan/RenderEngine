#pragma once

#include"Math.h"

#ifndef __DrawTriangle__
#define __DrawTriangle__


namespace RenderEngine {

	class DrawGouroudTriangle
	{
	public:
		DrawGouroudTriangle(bool drawTopTriangle, CGVertex* p1, CGVertex* p2, CGVertex* p3) :
			shouldDrawTopTriangle(drawTopTriangle), x1(p1->x), y1(p1->y), r1(p1->r), g1(p1->g), b1(p1->b)
		{
			// ����P2�Ĳ�������ߣ���P3�Ĳ������ұ�
			if (p2->x > p3->x)
			{
				x2 = p3->x;
				y2 = p3->y;
				r2 = p3->r;
				g2 = p3->g;
				b2 = p3->b;

				x3 = p2->x;
				y3 = p2->y;
				r3 = p2->r;
				g3 = p2->g;
				b3 = p2->b;
			}
			else
			{
				x2 = p2->x;
				y2 = p2->y;
				r2 = p2->r;
				g2 = p2->g;
				b2 = p2->b;

				x3 = p3->x;
				y3 = p3->y;
				r3 = p3->r;
				g3 = p3->g;
				b3 = p3->b;
			}

			// �����ʱ���Ĭ�ϵ���
			this->Draw();
		}
		~DrawGouroudTriangle() {}
		void Draw();
		void InsideLoop(int y);

	private:

		bool shouldDrawTopTriangle;

		int x1;
		int y1;
		byte r1;
		byte g1;
		byte b1;

		int x2;
		int y2;
		byte r2;
		byte g2;
		byte b2;

		int x3;
		int y3;
		byte r3;
		byte g3;
		byte b3;

		int leftX;
		int rightX;
		int leftR;
		int rightR;
		int leftG;
		int rightG;
		int leftB;
		int rightB;
	};



	void DrawGouroudTriangle::Draw() {
		if (y2 == y1 || y3 == y1)
		{
			return;
		}
		if (shouldDrawTopTriangle)
		{
			for (int y = y1; y >= y2 && y >= 0 && y <= screenHeight; --y) {
				InsideLoop(y);
			}
		}
		else
		{
			for (int y = y1; y <= y2 && y >= 0 && y <= screenHeight; ++y) {
				InsideLoop(y);
			}
		}
	}

	void DrawGouroudTriangle::InsideLoop(int y) {
		leftX = (y - y1) *(x2 - x1) / (y2 - y1) + x1;
		rightX = (y - y1) *(x3 - x1) / (y3 - y1) + x1 + 1;

		// ���������ұߵĵ�Rֵ
		leftR = (y - y1) *(r2 - r1) / (y2 - y1) + r1;
		rightR = (y - y1) *(r3 - r1) / (y3 - y1) + r1;

		// ���������ұߵĵ�Gֵ
		leftG = (y - y1) *(g2 - g1) / (y2 - y1) + g1;
		rightG = (y - y1) *(g3 - g1) / (y3 - y1) + g1;

		// ���������ұߵĵ�Bֵ
		leftB = (y - y1) *(b2 - b1) / (y2 - y1) + b1;
		rightB = (y - y1) *(b3 - b1) / (y3 - y1) + b1;

		float XLength = rightX - leftX;
		if (XLength != 0.0f)
		{
			for (int x = leftX; x <= rightX && x >= 0 && x <= screenWidth; ++x)
			{
				// ͨ�����Բ�ֵ����ʵ�������ε���ɫ����
				float t = (float)(x - leftX) / XLength;
				byte r = (byte)LinearInerpolate(leftR, rightR, t);
				byte g = (byte)LinearInerpolate(leftG, rightG, t);
				byte b = (byte)LinearInerpolate(leftB, rightB, t);

				if ((int(y) *  screenWidth * 3 + (int(x) + 1) * 3 - 1) > WINDOW_WIDTH*WINDOW_HEIGHT*3) return;
				BackBuffer[int(y) *  screenWidth * 3 + (int(x) + 1) * 3 - 1] = r;
				BackBuffer[int(y) *  screenWidth * 3 + (int(x) + 1) * 3 - 2] = g;
				BackBuffer[int(y) *  screenWidth * 3 + (int(x) + 1) * 3 - 3] = b;
			}
		}
	}






	class DrawTextureTriangle
	{
	public:
		DrawTextureTriangle(bool drawTopTriangle, CGVertex* p1, CGVertex* p2, CGVertex* p3) :
			shouldDrawTopTriangle(drawTopTriangle),
			x1(p1->x), y1(p1->y),
			u1(p1->u), v1(p1->v), z1(p1->z),
			nx1(p1->nx), ny1(p1->ny), nz1(p1->nz)
		{
			// ����P2�Ĳ�������ߣ���P3�Ĳ������ұ�
			if (p2->x > p3->x)
			{
				x2 = p3->x;
				y2 = p3->y;
				z2 = p3->z;
				u2 = p3->u;
				v2 = p3->v;
				nx2 = p3->nx;
				ny2 = p3->ny;
				nz2 = p3->nz;

				x3 = p2->x;
				y3 = p2->y;
				z3 = p2->z;
				u3 = p2->u;
				v3 = p2->v;
				nx3 = p2->nx;
				ny3 = p2->ny;
				nz3 = p2->nz;
			}
			else
			{
				x2 = p2->x;
				y2 = p2->y;
				u2 = p2->u;
				v2 = p2->v;
				nx2 = p2->nx;
				ny2 = p2->ny;
				nz2 = p2->nz;

				z2 = p2->z;
				x3 = p3->x;
				y3 = p3->y;
				u3 = p3->u;
				v3 = p3->v;
				z3 = p3->z;
				nx3 = p3->nx;
				ny3 = p3->ny;
				nz3 = p3->nz;
			}

			// �����������������ռ��Zֵ�ĵ���
			Z1_Camera_Rec = 1.0 / (B / (z1 - A));
			Z2_Camera_Rec = 1.0 / (B / (z2 - A));
			Z3_Camera_Rec = 1.0 / (B / (z3 - A));

			// �����ʱ���Ĭ�ϵ���
			this->Draw();
		}
		~DrawTextureTriangle() {}
		void Draw();
		void InsideLoop(int y);

	private:

		bool shouldDrawTopTriangle;

		float A = FAR_CLIPPINFG_PLANE / (FAR_CLIPPINFG_PLANE - NEAR_CLIPPING_PLANE);
		float B = -(FAR_CLIPPINFG_PLANE* NEAR_CLIPPING_PLANE) / (FAR_CLIPPINFG_PLANE - NEAR_CLIPPING_PLANE);


		int x1;
		int y1;

		float u1;
		float v1;
		float z1;

		float nx1;
		float ny1;
		float nz1;

		int  x2;
		int  y2;

		float u2;
		float v2;
		float z2;

		float nx2;
		float ny2;
		float nz2;

		int  x3;
		int  y3;

		float u3;
		float v3;
		float z3;

		float nx3;
		float ny3;
		float nz3;


		// ɨ�������Ҷ˵��ֵ
		int  leftX;
		int  rightX;

		float leftZU;
		float rightZU;

		float leftZV;
		float rightZV;

		float leftZ;
		float rightZ;

		float leftZNX;
		float rightZNX;

		float leftZNY;
		float rightZNY;

		float leftZNZ;
		float rightZNZ;

		// ÿ��������ɨ����ÿ�����ľ���
		float ZVOneStep;
		float ZUOneStep;
		float ZOneStep;
		float ZNXOneStep;
		float ZNYOneStep;
		float ZNZOneStep;



		float ZU;
		float ZV;

		float Z;

		int x, y;

		float ZNX;
		float ZNY;
		float ZNZ;


		// �����������������ռ��Zֵ�ĵ���
		float Z1_Camera_Rec;
		float Z2_Camera_Rec;
		float Z3_Camera_Rec;

	};


	void DrawTextureTriangle::Draw() {
		// ѭ��������ʽ��ĸ����Ϊ0
		if (y2 == y1 || y3 == y1)
		{
			return;
		}
		if (shouldDrawTopTriangle)
		{
			// ע�� y>=0&& y<=screen_h������û������ƽ��Ĳü�
			for (y = y1; y >= 0 && y <= screenHeight && y >= y2; --y) {
				InsideLoop(y);
			}
		}
		else {
			// �ж�ѭ����������ȱ�١�=��
			// ע�� y>=0 && y<=screenHeight
			for (y = y1; y >= 0 && y <= screenHeight && y <= y2; ++y) {
				InsideLoop(y);
			}
		}
	}

	void DrawTextureTriangle::InsideLoop(int y) {


		// ���������ұߵĵ�X
		leftX = (y - y1) *(x2 - x1) / (y2 - y1) + x1;
		rightX = (y - y1) *(x3 - x1) / (y3 - y1) + x1 + 2;

		leftZU = (float)(y - y1) *(Z2_Camera_Rec*u2 - Z1_Camera_Rec * u1) / (float)(y2 - y1) + Z1_Camera_Rec * u1;
		rightZU = (float)(y - y1) *(Z3_Camera_Rec*u3 - Z1_Camera_Rec * u1) / (float)(y3 - y1) + Z1_Camera_Rec * u1;

		// ���������ұߵĵ��(1/Z)Vֵ
		leftZV = (float)(y - y1) *(Z2_Camera_Rec*v2 - Z1_Camera_Rec * v1) / (float)(y2 - y1) + Z1_Camera_Rec * v1;
		rightZV = (float)(y - y1) *(Z3_Camera_Rec*v3 - Z1_Camera_Rec * v1) / (float)(y3 - y1) + Z1_Camera_Rec * v1;


		// ���������ұߵĵ��(1/Z)ֵ,
		leftZ = (float)(y - y1) *(Z2_Camera_Rec - Z1_Camera_Rec) / (float)(y2 - y1) + Z1_Camera_Rec;
		rightZ = (float)(y - y1) *(Z3_Camera_Rec - Z1_Camera_Rec) / (float)(y3 - y1) + Z1_Camera_Rec;

		// ���������ұߵĵ��(1/Z)*nxֵ,
		leftZNX = (float)(y - y1) *(Z2_Camera_Rec*nx2 - Z1_Camera_Rec * nx1) / (float)(y2 - y1) + Z1_Camera_Rec * nx1;
		rightZNX = (float)(y - y1) *(Z3_Camera_Rec*nx3 - Z1_Camera_Rec * nx1) / (float)(y3 - y1) + Z1_Camera_Rec * nx1;

		// ���������ұߵĵ��(1/Z)*nyֵ,
		leftZNY = (float)(y - y1) *(Z2_Camera_Rec*ny2 - Z1_Camera_Rec * ny1) / (float)(y2 - y1) + Z1_Camera_Rec * ny1;
		rightZNY = (float)(y - y1) *(Z3_Camera_Rec*ny3 - Z1_Camera_Rec * ny1) / (float)(y3 - y1) + Z1_Camera_Rec * ny1;

		// ���������ұߵĵ��(1/Z)*nzֵ,
		leftZNZ = (float)(y - y1) *(Z2_Camera_Rec*nz2 - Z1_Camera_Rec * nz1) / (float)(y2 - y1) + Z1_Camera_Rec * nz1;
		rightZNZ = (float)(y - y1) *(Z3_Camera_Rec*nz3 - Z1_Camera_Rec * nz1) / (float)(y3 - y1) + Z1_Camera_Rec * nz1;

		// ���������ߵ�ɨ�賤�����������ע��
		float XLength = (rightX - leftX);


		if (XLength != 0.0f)
		{
			// ����ZU,ZV,Z��ÿ��λX����仯���仯�ĳ���
			ZUOneStep = (rightZU - leftZU) / XLength;
			ZVOneStep = (rightZV - leftZV) / XLength;
			ZOneStep = (rightZ - leftZ) / XLength;
			ZNXOneStep = (rightZNX - leftZNX) / XLength;
			ZNYOneStep = (rightZNY - leftZNY) / XLength;
			ZNZOneStep = (rightZNZ - leftZNZ) / XLength;

			// ͨ����ֵ�õ�ɨ������ÿ�����ص�ZU,ZV,Z,ZNX,ZNY,ZNZֵ
			// ע��x>=0&&x<=screen_w
			for (x = leftX + 1, ZU = leftZU, ZV = leftZV, Z = leftZ, ZNX = leftZNX, ZNY = leftZNY, ZNZ = leftZNZ; x <= rightX && x >= 0 && x <= screenWidth; ++x, ZU += ZUOneStep, ZV += ZVOneStep, Z += ZOneStep, ZNX += ZNXOneStep, ZNY += ZNYOneStep, ZNZ += ZNZOneStep)
			{
				// ������ص�Z����, 0.0=<zBuffer<=1.0
				float zBuffer = Z * B + A;

				// Z�������
				/*if (zBuffer >= DepthBuffer[x][y])
				{
					continue;
				}*/
				// ���������ص��������������
				float u = ZU / Z;
				float v = ZV / Z;

				float nx = ZNX / Z;
				float ny = ZNY / Z;
				float nz = ZNZ / Z;

				Vector4 NormalVec(nx, ny, nz);

				float texPosX = u * textureWidth;
				float texPosY = v * textureHeight;

				int texPosX_Int = int(texPosX);
				int texPosY_Int = int(texPosY);

				float texPosX_Frac = texPosX - texPosX_Int;
				float texPosY_Frac = texPosY - texPosY_Int;

				byte r0, g0, b0;
				byte r1, g1, b1;
				byte r2, g2, b2;
				byte r3, g3, b3;
				byte r, g, b;

				float LightR, LightG, LightB;

				Vector4* DiffuseColor = new Vector4();



				// ��ֹ���������泬������
				if (texPosX_Int == textureWidth || texPosY_Int == textureHeight)
					continue;
				// ������ͼ�ķǱ�Ե���ֿɽ���˫���Բ�ֵ
				else if ((texPosX_Int + 1) < textureWidth && (texPosY_Int + 1) < textureHeight)
				{
					// p0����
					b0 = TextureBuffer[texPosY_Int * textureWidth * 3 + (texPosX_Int + 1) * 3 - 1];
					g0 = TextureBuffer[texPosY_Int* textureWidth * 3 + (texPosX_Int + 1) * 3 - 2];
					r0 = TextureBuffer[texPosY_Int* textureWidth * 3 + (texPosX_Int + 1) * 3 - 3];

					// p1����
					b1 = TextureBuffer[texPosY_Int * textureWidth * 3 + (texPosX_Int + 2) * 3 - 1];
					g1 = TextureBuffer[texPosY_Int * textureWidth * 3 + (texPosX_Int + 2) * 3 - 2];
					r1 = TextureBuffer[texPosY_Int * textureWidth * 3 + (texPosX_Int + 2) * 3 - 3];

					// p3����
					b2 = TextureBuffer[(texPosY_Int + 1)* textureWidth * 3 + (texPosX_Int + 2) * 3 - 1];
					g2 = TextureBuffer[(texPosY_Int + 1)* textureWidth * 3 + (texPosX_Int + 2) * 3 - 2];
					r2 = TextureBuffer[(texPosY_Int + 1)* textureWidth * 3 + (texPosX_Int + 2) * 3 - 3];

					// p4����
					b3 = TextureBuffer[(texPosY_Int + 1) * textureWidth * 3 + (texPosX_Int + 1) * 3 - 1];
					g3 = TextureBuffer[(texPosY_Int + 1)* textureWidth * 3 + (texPosX_Int + 1) * 3 - 2];
					r3 = TextureBuffer[(texPosY_Int + 1)* textureWidth * 3 + (texPosX_Int + 1) * 3 - 3];

					b = b0 * (1.0f - texPosX_Frac)*(1 - texPosY_Frac) + b1 * texPosX_Frac*(1 - texPosY_Frac) + b2 * texPosX_Frac*texPosY_Frac + b3 * (1.0f - texPosX_Frac)*texPosY_Frac;
					g = g0 * (1.0f - texPosX_Frac)*(1 - texPosY_Frac) + g1 * texPosX_Frac*(1 - texPosY_Frac) + g2 * texPosX_Frac*texPosY_Frac + g3 * (1.0f - texPosX_Frac)*texPosY_Frac;
					r = r0 * (1.0f - texPosX_Frac)*(1 - texPosY_Frac) + r1 * texPosX_Frac*(1 - texPosY_Frac) + r2 * texPosX_Frac*texPosY_Frac + r3 * (1.0f - texPosX_Frac)*texPosY_Frac;
				}

				// ������ͼ�ı�Ե���ֿɽ��е��ֵ
				else
				{
					b = TextureBuffer[texPosY_Int * textureWidth * 3 + (texPosX_Int + 1) * 3 - 1];
					g = TextureBuffer[texPosY_Int* textureWidth * 3 + (texPosX_Int + 1) * 3 - 2];
					r = TextureBuffer[texPosY_Int* textureWidth * 3 + (texPosX_Int + 1) * 3 - 3];
				}

				// ��񻯷�����
				VectorNormalize(NormalVec);

				// �Ե�λ���������������ķ�������е��
				float DiffuseFactor = NormalVec * DiffuseDirRevse;

				// ���軷����
				LightR = AmbientLight.getR();
				LightG = AmbientLight.getG();
				LightB = AmbientLight.getB();


				// ���ÿ�����ص��������
				DiffuseColor->setR(Clamp(DiffuseFactor*DiffuseLight.getR(), 0, 255.0f));
				DiffuseColor->setG(Clamp(DiffuseFactor*DiffuseLight.getG(), 0, 255.0f));
				DiffuseColor->setB(Clamp(DiffuseFactor*DiffuseLight.getB(), 0, 255.0f));

				// ���ÿ�������ܵ����ܵĹ�ǿ(�������ǿ���ϻ�����ǿ)
				LightR = Clamp(DiffuseColor->getR()+ LightR, 0, 255.0f);
				LightG = Clamp(DiffuseColor->getG() + LightG, 0, 255.0f);
				LightB = Clamp(DiffuseColor->getB() + LightB, 0, 255.0f);

				// ����ǿ������С255
				LightR /= 255.0f;
				LightG /= 255.0f;
				LightB /= 255.0f;

				// ����ù�ǿ����������ɫ
				b = b * LightB;
				g = g * LightG;
				r = r * LightR;


				// Check wheher the buffer index is outside the bounds of its size
				if ((int(y) *  screenWidth * 3 + (int(x) + 1) * 3 - 1) > WINDOW_WIDTH*WINDOW_HEIGHT * 3)
				{
					// if it's outside the bounds, then return and do nothing
					return;
				}
				BackBuffer[int(y) *  screenWidth * 3 + (int(x) + 1) * 3 - 1] = b;
				BackBuffer[int(y) *  screenWidth * 3 + (int(x) + 1) * 3 - 2] = g;
				BackBuffer[int(y) *  screenWidth * 3 + (int(x) + 1) * 3 - 3] = r;
			}
		}
	}
}

#endif // !__DrawTriangle__
