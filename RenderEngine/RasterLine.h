#pragma once


#ifndef __RasterLine__
#define __RasterLine__

namespace RenderEngine {

	// 未改进的Bresenham快速画直线算法
	void DrawLine(Vector4* startPoint, Vector4* endPoint, byte r, byte g, byte b)
	{
		if (startPoint == endPoint)
			return;


		if (startPoint->getX() == endPoint->getX() && startPoint->getY() == endPoint->getY())
			return;
		/*	float x1 = int(startPoint->getX() + .5f);
			float y1 = int(startPoint->getY() + .5f);
			float x2 = int(endPoint->getX() + .5f);
			float y2 = int(endPoint->getY() + .5f);*/
		float x1 = startPoint->getX();
		float y1 = startPoint->getY();
		float x2 = endPoint->getX();
		float y2 = endPoint->getY();

		float xTemp, yTemp;



		/*1：绘制不存在斜率的线段*/
		if (x1 == x2)
		{
			// 防止线越过视平面
			if (x1 > screenWidth || x1 < 0)
			{
				return;
			}

			if (y1 > y2)
			{
				yTemp = y1;
				y1 = y2;
				y2 = yTemp;
			}
			for (float y = y1; y < y2&&y < screenHeight && y >= 0; ++y)
			{
				if ((int(y + .5f) *  screenWidth * 3 + (int(x1 + .5f) + 1) * 3 - 1) > WINDOW_WIDTH*WINDOW_HEIGHT * 3) return;
				BackBuffer[int(y + .5f) *  screenWidth * 3 + (int(x1 + .5f) + 1) * 3 - 1] = r;
				BackBuffer[int(y + .5f) *  screenWidth * 3 + (int(x1 + .5f) + 1) * 3 - 2] = g;
				BackBuffer[int(y + .5f) *  screenWidth * 3 + (int(x1 + .5f) + 1) * 3 - 3] = b;
			}
			return;
		}



		/*2：绘制拥有斜率的线段*/

		// 规定X1<X2，否则交换
		if (x1 > x2)
		{
			xTemp = x1;
			x1 = x2;
			x2 = xTemp;

			yTemp = y1;
			y1 = y2;
			y2 = yTemp;
		}

		float k = (y2 - y1) / (x2 - x1);

		// 1：as the gradient factor is less than 1 or bigger than 0
		if (k >= 0.0f&&k <= 1.0f)
		{
			// 横向针对每个像素进行着色，一直从x1着色到x2并且不超出屏幕宽度
			for (float x = x1, y = y1; x <= x2 && x >= 0 && x < screenWidth; ++x)
			{
				float dis = (x - x1) * k + y1 - y;
				if (dis >= 0.5)
				{
					++y;
				}
				if ((int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 1) > WINDOW_WIDTH*WINDOW_HEIGHT * 3) return;
				BackBuffer[int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 1] = r;
				BackBuffer[int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 2] = g;
				BackBuffer[int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 3] = b;
			}
		}

		// 2：as the gradient factor is less than 0 or bigger than -1
		else if (k < 0.0f&&k >= -1.0f)
		{
			for (float x = x1, y = y1; x <= x2 && x >= 0 && x < screenWidth; ++x)
			{
				float dis = (x - x1)*k + y1 - y;
				if (dis < -0.5)
				{
					--y;
				}
				if ((int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 1) > WINDOW_WIDTH*WINDOW_HEIGHT * 3) return;
				BackBuffer[int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 1] = r;
				BackBuffer[int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 2] = g;
				BackBuffer[int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 3] = b;
			}
		}

		// 3：as the gradient factor is bigger than -1
		else if (k > 1.0f)
		{
			float k1 = 1.0f / k;
			for (float y = y1, x = x1; y <= y2 && y >= 0 && y < screenHeight; ++y)
			{
				float dis = (y - y1)*k1 + x1 - x;
				if (dis >= 0.5)
				{
					++x;
				}
				if ((int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 1) > WINDOW_WIDTH*WINDOW_HEIGHT * 3) return;
				BackBuffer[int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 1] = r;
				BackBuffer[int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 2] = g;
				BackBuffer[int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 3] = b;
			}
		}

		// 4：as the gradient factor is less than -1
		else if (k < -1.0f)
		{
			float k1 = 1.0f / k;
			for (float y = y2, x = x2; y <= y1 && y < screenHeight && y >= 0; ++y)
			{
				float dis = (y - y2)*k1 + x2 - x;
				if (dis <= -0.5)
				{
					--x;
				}
				if ((int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 1) > WINDOW_WIDTH*WINDOW_HEIGHT * 3) return;
				BackBuffer[int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 1] = r;
				BackBuffer[int(y + .5f) *  screenWidth * 3 + (int(x + .5f) + 1) * 3 - 2] = g;
				BackBuffer[int(y + .5f) * screenWidth * 3 + (int(x + .5f) + 1) * 3 - 3] = b;
			}
		}

	}

	typedef struct Colour {
		byte r, g, b;
	};

}

#endif // !__RasterLine__
