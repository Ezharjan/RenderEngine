#pragma once

#include "Main.h"


#ifndef __PipelineCore__
#define __PipelineCore__


namespace RenderEngine {


	

	void ConstructMVP(const Vector3& lookAt, Vector3& cameraUp)
	{
		ViewMatrix = MatrixLookAtLH(&CameraPos, &lookAt, &cameraUp,cameraUp);
		ProjectionMatrix = MatrixPerspectiveFovLH(CAMERA_FOV, float(screenWidth) / float(screenHeight), NEAR_CLIPPING_PLANE, FAR_CLIPPINFG_PLANE);
		ViewPortMatrix = MatrixViewPort((float)screenWidth, (float)screenHeight, 1.0f, 0.0f, 0.0f, 0.0f);
	}

	
	void UpdateViewMatrix4Translation(const Vector3& objPos)
	{
		std::cout << "translated!!!" << std::endl;
		//ViewMatrix = MatrixLookAtLH(&CameraPos, &objPos, &cameraUp);
	}

	   	 
	void UpdateViewMatrix4Rotation(const Vector2& lastPointV2, const Vector2& curPointV2)
	{
		Vector4 quat = GetQuaternion(lastPointV2,curPointV2);
		Matrix3f currentRotation = GetMatrixFromQuat(quat);	
		CameraPos = currentRotation * CameraPos;
		ViewMatrix = MatrixLookAtLH(&CameraPos, &lookAt, &cameraUp,cameraUp);
	}



	void ReMVPAndUpdateCamPos()
	{
		ConstructMVP(lookAt, cameraUp);

		// 初始化光强和规格化光强
		VectorNormalize(DiffuseDir);
		DiffuseDirRevse.setR(DiffuseDir.getR());
		DiffuseDirRevse.setG(-DiffuseDir.getG());
		DiffuseDirRevse.setB(-DiffuseDir.getB());
		DiffuseDirRevse.setA(0.f);
	}


	void Initlize3D(const HWND& hwnd)
	{
		screenWidth = WINDOW_WIDTH;
		screenHeight = WINDOW_HEIGHT;

		// 初始化背部缓存数组的容量
		BackBuffer = new BYTE[WINDOW_WIDTH * WINDOW_HEIGHT * 3]();

		// 从屏幕的远点位置开始绘制矩形
		RECT rect = { 0, 0, screenWidth, screenHeight };

		// 由屏幕大小获取窗口的大小
		AdjustWindowRect(&rect, GetWindowLong(hwnd, GWL_STYLE), 0);//WinAPI
		windowWidth = rect.right - rect.left;
		windowHeight = rect.bottom - rect.top;

		// 构造位图，填充位图的结构体
		ZeroMemory(&bitmapInfo, sizeof(BITMAPINFO)); //WinAPI
		bitmapInfo.bmiHeader.biBitCount = 24;      //图像的位数，每个像素多少位，也可直接写24(RGB)或者32(RGBA)
		bitmapInfo.bmiHeader.biCompression = BI_RGB; //针对于位图所采用的压缩方式
		bitmapInfo.bmiHeader.biWidth = WINDOW_WIDTH; //位图宽和窗口宽度一致
		bitmapInfo.bmiHeader.biHeight = -WINDOW_HEIGHT; //位图高度取窗口高度相反数
		bitmapInfo.bmiHeader.biPlanes = 1; //目标设备包含的层数，必须设置为1
		bitmapInfo.bmiHeader.biSizeImage = 0; //图像数据的大小（=biWidth*biHeight*biBitCount）
		bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); //结构体的大小（含颜色表）

		// 获取屏幕HDC
		screen_hwnd = hwnd;
		screen_hdc = GetDC(screen_hwnd);

		// 获取兼容HDC和兼容Bitmap，兼容Bitmap选入兼容HDC
		hCompatibleDC = CreateCompatibleDC(screen_hdc); //创建兼容窗口句柄
		hCompatibleBitmap = CreateCompatibleBitmap(screen_hdc, WINDOW_WIDTH, WINDOW_HEIGHT); //创建兼容位图句柄
		hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hCompatibleBitmap); //选入原始的位图对象


		LoadModelData(PointList, std::string(MODEL_PATH));
		LoadTextureData(TEXTURE_PATH);


		ReMVPAndUpdateCamPos();
	}



	// 初始化载入模型顶点并获取世界矩阵
	void InitializeScene() {

		// 赋予顶点缓存数据
		PointListCache = PointList;

		// 重新获取世界矩阵
		worldMatrix = Matrix4f::getIdentityMatrix();

		// 移动世界矩阵至坐标系的原点
		Matrix4f translateMatrix = Matrix4f::getTranslateMatrix(0.0, 0.0f, 0.0f);
		worldMatrix = translateMatrix * worldMatrix;
	}



	static float xMovtion = 0;
	static float yMovtion = 0;

	// 需要在每帧调用中执行的用户变换函数的委托形式
	void TranslateOBJByUser(short xMove, short yMove, short zMove) {

		// 当且仅当用户要改变参数的时候才传入值并更新矩阵
		if (xMove != 0 && yMove != 0)
		{
			Matrix4f TranslateMa = Matrix4f::getTranslateMatrix(xMovtion, yMovtion, 0); //绕着Y轴旋转
			worldMatrix = TranslateMa * worldMatrix;
		}
	}


	static short xRotation = 0;
	static short yRotation = 0;

	// 需要在每帧调用中执行的用户变换函数的委托形式
	void RotateOBJByUser(short xMove, short yMove, short zMove) {

		xRotation += xMove;
		yRotation += yMove;

		// 获取X和Y旋转矩阵
		Matrix4f RotateYMa = Matrix4f::getRotateYMatrix(xRotation); //绕着Y轴旋转
		Matrix4f RotateXMa = Matrix4f::getRotateXMatrix(yRotation); //绕着X轴旋转

		// 更新世界矩阵
		worldMatrix = RotateYMa * worldMatrix;
		worldMatrix = RotateXMa * worldMatrix;
	}




	static short zScaler = 0;

	// 需要在每帧调用中执行的用户变换函数的委托形式
	void ScaleOBJByUser(short xMove, short yMove, short zMove) {

		zScaler += zMove;

		// 当且仅当用户要改变参数的时候才传入值并更新矩阵
		if (zMove != 0) {

			std::cout << "Z Scaler is : " << zScaler << std::endl;

			Matrix4f ZoomMa = Matrix4f::getTranslateMatrix(0, 0, zScaler* 0.01); //Z轴上进行Zoom
			worldMatrix = ZoomMa * worldMatrix;
		}
	}


	// 二维观察流水线
	void RunObservationPipeline() {

		// 将所有顶点的法向量变换到世界空间
		TransormObjectNormalToWorld(PointListCache, &worldMatrix);

		// 对所有顶点进行世界变换
		ObjectTransform(PointListCache, &worldMatrix);

		// 对所有顶点进行相机变换
		ObjectTransform(PointListCache, &ViewMatrix);

		// 对所有顶点进行透视投影变换
		ObjectTransform(PointListCache, &ProjectionMatrix);

		// 进行CVV裁剪算法
		CVVClip(PointListCache);

		// 对所有顶点进行透视除法
		PerspectiveDivede(PointListCache);

		// 对所有顶点进行视口变换
		ObjectTransform(PointListCache, &ViewPortMatrix);

		// 进行背面剔除
		BackCulling(PointListCache);
	}



	void UpdateScene(void(*TransformCallback)(short x, short y, short z), short x, short y, short z)
	{
		// 初始化基本设置
		InitializeScene();

		// 调用用户传入的变换函数
		TransformCallback(x, y, z);

		// 开启二维观察流水线
		RunObservationPipeline();
	}


	void Render(DisplayMode mode)
	{
		// 以线框模式渲染
		if (mode == WIRE_FRAME_MODE)
		{
			RenderInWireFrameMode(255, 255, 255);
		}

		// 以实心模式渲染
		else if (mode == SOLID_MODE)
		{
			RenderInSolidMode();
		}
		// 以纹理模式渲染
		else
		{
			RenderInTextureMode();
		}

		// Set the pixels in a compatible bitmap using the color data found in the specified DIB(DIB: 设备无关位图文件)
		SetDIBits(screen_hdc, hCompatibleBitmap, 0, WINDOW_HEIGHT, BackBuffer, (BITMAPINFO*)&bitmapInfo, DIB_RGB_COLORS);
		// 将源设备环境区域中的像素进行位块转换以传送到目标设备环境
		BitBlt(screen_hdc, -1, -1, WINDOW_WIDTH, WINDOW_HEIGHT, hCompatibleDC, 0, 0, SRCCOPY);
	}




	void RenderInWireFrameMode(byte r, byte g, byte b)
	{
		for (size_t i = 0; i != PointListCache.size(); ++i)
		{
			if ((i % 3 == 0) && (PointListCache[i].IsCull == false))
			{
				// 点，在表示时应为(x, y, z, 1)，w为1。
				Vector4 p1(PointListCache[i].x, PointListCache[i].y, PointListCache[i].z, 1.f);
				Vector4 p2(PointListCache[i + 1].x, PointListCache[i + 1].y, PointListCache[i + 1].z, 1.f);
				Vector4 p3(PointListCache[i + 2].x, PointListCache[i + 2].y, PointListCache[i + 2].z, 1.f);
				
				DrawLine(&p1, &p2, r, g, b);
				DrawLine(&p1, &p3, r, g, b);
				DrawLine(&p2, &p3, r, g, b);
			}
		}
	}


	void RenderInSolidMode()
	{
		for (size_t i = 0; i != PointListCache.size(); ++i)
		{
			if ((i % 3 == 0) && (PointListCache[i].IsCull == false))
			{
				RasterTriangle(InGouraud, &PointListCache[i], &PointListCache[i + 1], &PointListCache[i + 2]);
			}
		}
	}


	void RenderInTextureMode()
	{
		for (size_t i = 0; i != PointListCache.size(); ++i)
		{
			if ((i % 3 == 0) && (PointListCache[i].IsCull == false))
			{
				RasterTriangle(InTexture, &PointListCache[i], &PointListCache[i + 1], &PointListCache[i + 2]);
			}
		}
	}




	void ClearBackBuffer(byte r, byte g, byte b)
	{
		for (int i = 0; i < screenHeight*screenWidth; ++i)
		{
			BackBuffer[i * 3 + 0] = b;
			BackBuffer[i * 3 + 1] = g;
			BackBuffer[i * 3 + 2] = r;
		}
	}



	void Release3D()
	{
		// 释放屏幕HDC,兼容HDC，兼容Bitmap
		SelectObject(hCompatibleDC, hOldBitmap);
		DeleteObject(hCompatibleDC);
		DeleteObject(hCompatibleDC);

		// 释放旧的HDC
		ReleaseDC(screen_hwnd, screen_hdc);

		// 删除背部缓存
		delete[] BackBuffer;

		// 删除纹理缓存
		delete[] TextureBuffer;
	}



	void ClearDepthBuffer(float MaxDepth)
	{
		for (int i = 0; i < screenHeight; ++i)
		{
			for (int j = 0; j < screenWidth; ++j)
			{
				//DepthBuffer[i][j] = MaxDepth;
			}
		}
	
	}
	
	
	void MoveCameraPosition(const CameraMovDirecton direction)
	{
		switch (direction)
		{
		case goUp:
			std::cout << "cameraPosY: " << CameraPos.getY() << std::endl;
			CameraPos.setZ(CameraPos.getY() - 1);
			break;
		case goDown:
			std::cout << "cameraPosY: " << CameraPos.getY() << std::endl;
			CameraPos.setZ(CameraPos.getY() + 1);
			break;
		case goRight:
			std::cout << "cameraPosX: " << CameraPos.getX() << std::endl;
			CameraPos.setZ(CameraPos.getX() - 1);
			break;
		case goLeft:
			std::cout << "cameraPosX: " << CameraPos.getX() << std::endl;
			CameraPos.setZ(CameraPos.getX() + 1);
			break;
		case goIn:
			std::cout << "cameraPosZ: " << CameraPos.getZ() << std::endl;
			CameraPos.setZ(CameraPos.getZ() + 1);
			break;
		case goOut:
			std::cout << "cameraPosZ: " << CameraPos.getZ() << std::endl;
			CameraPos.setZ(CameraPos.getZ() - 1);
			break;
		default:
			break;
		}


		// Camera and the object should have a relevant position
		Vector3 objPos(CameraPos.getX(), CameraPos.getY(), 0);
		UpdateViewMatrix4Translation(objPos);
	}
}




#endif // !__PipelineCore__
