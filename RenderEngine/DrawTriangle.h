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
			// 调整P2的参数在左边，而P3的参数在右边
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

			// 构造的时候就默认调用
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

		// 三角形左右边的点R值
		leftR = (y - y1) *(r2 - r1) / (y2 - y1) + r1;
		rightR = (y - y1) *(r3 - r1) / (y3 - y1) + r1;

		// 三角形左右边的点G值
		leftG = (y - y1) *(g2 - g1) / (y2 - y1) + g1;
		rightG = (y - y1) *(g3 - g1) / (y3 - y1) + g1;

		// 三角形左右边的点B值
		leftB = (y - y1) *(b2 - b1) / (y2 - y1) + b1;
		rightB = (y - y1) *(b3 - b1) / (y3 - y1) + b1;

		float XLength = rightX - leftX;
		if (XLength != 0.0f)
		{
			for (int x = leftX; x <= rightX && x >= 0 && x <= screenWidth; ++x)
			{
				// 通过线性插值运算实现三角形的颜色渐变
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
			// 调整P2的参数在左边，而P3的参数在右边
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

			// 求出各个顶点在相机空间的Z值的倒数
			Z1_Camera_Rec = 1.0 / (B / (z1 - A));
			Z2_Camera_Rec = 1.0 / (B / (z2 - A));
			Z3_Camera_Rec = 1.0 / (B / (z3 - A));

			// 构造的时候就默认调用
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


		// 扫描线左右端点的值
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

		// 每个变量在扫描线每步横跨的距离
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


		// 求出各个顶点在相机空间的Z值的倒数
		float Z1_Camera_Rec;
		float Z2_Camera_Rec;
		float Z3_Camera_Rec;

	};


	void DrawTextureTriangle::Draw() {
		// 循环里两点式分母不能为0
		if (y2 == y1 || y3 == y1)
		{
			return;
		}
		if (shouldDrawTopTriangle)
		{
			// 注意 y>=0&& y<=screen_h，这里没进行视平面的裁剪
			for (y = y1; y >= 0 && y <= screenHeight && y >= y2; --y) {
				InsideLoop(y);
			}
		}
		else {
			// 判断循环条件不能缺少“=”
			// 注意 y>=0 && y<=screenHeight
			for (y = y1; y >= 0 && y <= screenHeight && y <= y2; ++y) {
				InsideLoop(y);
			}
		}
	}

	void DrawTextureTriangle::InsideLoop(int y) {


		// 三角形左右边的点X
		leftX = (y - y1) *(x2 - x1) / (y2 - y1) + x1;
		rightX = (y - y1) *(x3 - x1) / (y3 - y1) + x1 + 2;

		leftZU = (float)(y - y1) *(Z2_Camera_Rec*u2 - Z1_Camera_Rec * u1) / (float)(y2 - y1) + Z1_Camera_Rec * u1;
		rightZU = (float)(y - y1) *(Z3_Camera_Rec*u3 - Z1_Camera_Rec * u1) / (float)(y3 - y1) + Z1_Camera_Rec * u1;

		// 三角形左右边的点的(1/Z)V值
		leftZV = (float)(y - y1) *(Z2_Camera_Rec*v2 - Z1_Camera_Rec * v1) / (float)(y2 - y1) + Z1_Camera_Rec * v1;
		rightZV = (float)(y - y1) *(Z3_Camera_Rec*v3 - Z1_Camera_Rec * v1) / (float)(y3 - y1) + Z1_Camera_Rec * v1;


		// 三角形左右边的点的(1/Z)值,
		leftZ = (float)(y - y1) *(Z2_Camera_Rec - Z1_Camera_Rec) / (float)(y2 - y1) + Z1_Camera_Rec;
		rightZ = (float)(y - y1) *(Z3_Camera_Rec - Z1_Camera_Rec) / (float)(y3 - y1) + Z1_Camera_Rec;

		// 三角形左右边的点的(1/Z)*nx值,
		leftZNX = (float)(y - y1) *(Z2_Camera_Rec*nx2 - Z1_Camera_Rec * nx1) / (float)(y2 - y1) + Z1_Camera_Rec * nx1;
		rightZNX = (float)(y - y1) *(Z3_Camera_Rec*nx3 - Z1_Camera_Rec * nx1) / (float)(y3 - y1) + Z1_Camera_Rec * nx1;

		// 三角形左右边的点的(1/Z)*ny值,
		leftZNY = (float)(y - y1) *(Z2_Camera_Rec*ny2 - Z1_Camera_Rec * ny1) / (float)(y2 - y1) + Z1_Camera_Rec * ny1;
		rightZNY = (float)(y - y1) *(Z3_Camera_Rec*ny3 - Z1_Camera_Rec * ny1) / (float)(y3 - y1) + Z1_Camera_Rec * ny1;

		// 三角形左右边的点的(1/Z)*nz值,
		leftZNZ = (float)(y - y1) *(Z2_Camera_Rec*nz2 - Z1_Camera_Rec * nz1) / (float)(y2 - y1) + Z1_Camera_Rec * nz1;
		rightZNZ = (float)(y - y1) *(Z3_Camera_Rec*nz3 - Z1_Camera_Rec * nz1) / (float)(y3 - y1) + Z1_Camera_Rec * nz1;

		// 求左右两边的扫描长度因此下面请注意
		float XLength = (rightX - leftX);


		if (XLength != 0.0f)
		{
			// 计算ZU,ZV,Z随每单位X距离变化而变化的长度
			ZUOneStep = (rightZU - leftZU) / XLength;
			ZVOneStep = (rightZV - leftZV) / XLength;
			ZOneStep = (rightZ - leftZ) / XLength;
			ZNXOneStep = (rightZNX - leftZNX) / XLength;
			ZNYOneStep = (rightZNY - leftZNY) / XLength;
			ZNZOneStep = (rightZNZ - leftZNZ) / XLength;

			// 通过插值得到扫描线上每个像素的ZU,ZV,Z,ZNX,ZNY,ZNZ值
			// 注意x>=0&&x<=screen_w
			for (x = leftX + 1, ZU = leftZU, ZV = leftZV, Z = leftZ, ZNX = leftZNX, ZNY = leftZNY, ZNZ = leftZNZ; x <= rightX && x >= 0 && x <= screenWidth; ++x, ZU += ZUOneStep, ZV += ZVOneStep, Z += ZOneStep, ZNX += ZNXOneStep, ZNY += ZNYOneStep, ZNZ += ZNZOneStep)
			{
				// 求出像素的Z缓冲, 0.0=<zBuffer<=1.0
				float zBuffer = Z * B + A;

				// Z缓存测试
				/*if (zBuffer >= DepthBuffer[x][y])
				{
					continue;
				}*/
				// 相除求出像素的纹理坐标和向量
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



				// 防止访问纹理缓存超出界限
				if (texPosX_Int == textureWidth || texPosY_Int == textureHeight)
					continue;
				// 纹理贴图的非边缘部分可进行双线性插值
				else if ((texPosX_Int + 1) < textureWidth && (texPosY_Int + 1) < textureHeight)
				{
					// p0像素
					b0 = TextureBuffer[texPosY_Int * textureWidth * 3 + (texPosX_Int + 1) * 3 - 1];
					g0 = TextureBuffer[texPosY_Int* textureWidth * 3 + (texPosX_Int + 1) * 3 - 2];
					r0 = TextureBuffer[texPosY_Int* textureWidth * 3 + (texPosX_Int + 1) * 3 - 3];

					// p1像素
					b1 = TextureBuffer[texPosY_Int * textureWidth * 3 + (texPosX_Int + 2) * 3 - 1];
					g1 = TextureBuffer[texPosY_Int * textureWidth * 3 + (texPosX_Int + 2) * 3 - 2];
					r1 = TextureBuffer[texPosY_Int * textureWidth * 3 + (texPosX_Int + 2) * 3 - 3];

					// p3像素
					b2 = TextureBuffer[(texPosY_Int + 1)* textureWidth * 3 + (texPosX_Int + 2) * 3 - 1];
					g2 = TextureBuffer[(texPosY_Int + 1)* textureWidth * 3 + (texPosX_Int + 2) * 3 - 2];
					r2 = TextureBuffer[(texPosY_Int + 1)* textureWidth * 3 + (texPosX_Int + 2) * 3 - 3];

					// p4像素
					b3 = TextureBuffer[(texPosY_Int + 1) * textureWidth * 3 + (texPosX_Int + 1) * 3 - 1];
					g3 = TextureBuffer[(texPosY_Int + 1)* textureWidth * 3 + (texPosX_Int + 1) * 3 - 2];
					r3 = TextureBuffer[(texPosY_Int + 1)* textureWidth * 3 + (texPosX_Int + 1) * 3 - 3];

					b = b0 * (1.0f - texPosX_Frac)*(1 - texPosY_Frac) + b1 * texPosX_Frac*(1 - texPosY_Frac) + b2 * texPosX_Frac*texPosY_Frac + b3 * (1.0f - texPosX_Frac)*texPosY_Frac;
					g = g0 * (1.0f - texPosX_Frac)*(1 - texPosY_Frac) + g1 * texPosX_Frac*(1 - texPosY_Frac) + g2 * texPosX_Frac*texPosY_Frac + g3 * (1.0f - texPosX_Frac)*texPosY_Frac;
					r = r0 * (1.0f - texPosX_Frac)*(1 - texPosY_Frac) + r1 * texPosX_Frac*(1 - texPosY_Frac) + r2 * texPosX_Frac*texPosY_Frac + r3 * (1.0f - texPosX_Frac)*texPosY_Frac;
				}

				// 纹理贴图的边缘部分可进行点插值
				else
				{
					b = TextureBuffer[texPosY_Int * textureWidth * 3 + (texPosX_Int + 1) * 3 - 1];
					g = TextureBuffer[texPosY_Int* textureWidth * 3 + (texPosX_Int + 1) * 3 - 2];
					r = TextureBuffer[texPosY_Int* textureWidth * 3 + (texPosX_Int + 1) * 3 - 3];
				}

				// 规格化法向量
				VectorNormalize(NormalVec);

				// 对单位法向量和漫反射光的反方向进行点乘
				float DiffuseFactor = NormalVec * DiffuseDirRevse;

				// 赋予环境光
				LightR = AmbientLight.getR();
				LightG = AmbientLight.getG();
				LightB = AmbientLight.getB();


				// 求出每个像素的漫反射光
				DiffuseColor->setR(Clamp(DiffuseFactor*DiffuseLight.getR(), 0, 255.0f));
				DiffuseColor->setG(Clamp(DiffuseFactor*DiffuseLight.getG(), 0, 255.0f));
				DiffuseColor->setB(Clamp(DiffuseFactor*DiffuseLight.getB(), 0, 255.0f));

				// 求出每个像素受到的总的光强(漫反射光强加上环境光强)
				LightR = Clamp(DiffuseColor->getR()+ LightR, 0, 255.0f);
				LightG = Clamp(DiffuseColor->getG() + LightG, 0, 255.0f);
				LightB = Clamp(DiffuseColor->getB() + LightB, 0, 255.0f);

				// 将光强因子缩小255
				LightR /= 255.0f;
				LightG /= 255.0f;
				LightB /= 255.0f;

				// 最后用光强调节纹理颜色
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
