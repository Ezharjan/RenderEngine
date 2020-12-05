/*主要是三维结构体数据以及基本全局变量的声明*/
#pragma once

#include<vector>
#include "Math.h"
#include "Config.h"


#ifndef __3DStruct__
#define __3DStruct__

namespace RenderEngine {

	int screenWidth;
	int screenHeight;
	int windowWidth;
	int windowHeight;
	int textureWidth;
	int textureHeight;


	// 背部缓存，深度缓存，纹理缓存(纹理缓存的颜色也是以BGR的顺序存储纹理颜色的)
	BYTE *BackBuffer = new BYTE[WINDOW_WIDTH * WINDOW_HEIGHT * 3]();  //每三个byte为一个像素，总共有800*600个像素，800*600*3个byte, BGR的颜色存储顺序
	//float DepthBuffer[WINDOW_HEIGHT][WINDOW_WIDTH]; //深度缓存
	BYTE *TextureBuffer = new BYTE[textureHeight*textureWidth * 3];

	// 主窗口的设备描述表/上下文HDC和Handle
	static HDC screen_hdc; //Handle to Device Context：上下文句柄
	static HWND screen_hwnd; //Handle to A Window：窗口句柄 --- 窗口的唯一标识码
	static HDC hCompatibleDC; //兼容HDC
	static HBITMAP hCompatibleBitmap; //兼容BITMAP
	static HBITMAP hOldBitmap; //旧的BITMAP				  
	static BITMAPINFO bitmapInfo; //BITMAPINFO结构体


	// 四个变换矩阵
	Matrix4f worldMatrix;
	Matrix4f ViewMatrix;
	Matrix4f ProjectionMatrix;
	Matrix4f ViewPortMatrix;

	enum CameraMovDirecton
	{
		goUp,
		goDown,
		goRight,
		goLeft,
		goIn,
		goOut
	};

	// 顶点列表，三个点为一个三角形
	std::vector<CGVertex> PointList;  //初始顶点数据
	std::vector<CGVertex> PointListCache;  //用于几大变换的顶点缓存数据



	// 枚举型显示模式
	enum  DisplayMode
	{
		WIRE_FRAME_MODE,
		SOLID_MODE,
		TEXTURE_MODE,
	};


	DisplayMode MyDisplayMode;
}



#endif // !__3DStruct__