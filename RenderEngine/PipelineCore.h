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

		// ��ʼ����ǿ�͹�񻯹�ǿ
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

		// ��ʼ�������������������
		BackBuffer = new BYTE[WINDOW_WIDTH * WINDOW_HEIGHT * 3]();

		// ����Ļ��Զ��λ�ÿ�ʼ���ƾ���
		RECT rect = { 0, 0, screenWidth, screenHeight };

		// ����Ļ��С��ȡ���ڵĴ�С
		AdjustWindowRect(&rect, GetWindowLong(hwnd, GWL_STYLE), 0);//WinAPI
		windowWidth = rect.right - rect.left;
		windowHeight = rect.bottom - rect.top;

		// ����λͼ�����λͼ�Ľṹ��
		ZeroMemory(&bitmapInfo, sizeof(BITMAPINFO)); //WinAPI
		bitmapInfo.bmiHeader.biBitCount = 24;      //ͼ���λ����ÿ�����ض���λ��Ҳ��ֱ��д24(RGB)����32(RGBA)
		bitmapInfo.bmiHeader.biCompression = BI_RGB; //�����λͼ�����õ�ѹ����ʽ
		bitmapInfo.bmiHeader.biWidth = WINDOW_WIDTH; //λͼ��ʹ��ڿ��һ��
		bitmapInfo.bmiHeader.biHeight = -WINDOW_HEIGHT; //λͼ�߶�ȡ���ڸ߶��෴��
		bitmapInfo.bmiHeader.biPlanes = 1; //Ŀ���豸�����Ĳ�������������Ϊ1
		bitmapInfo.bmiHeader.biSizeImage = 0; //ͼ�����ݵĴ�С��=biWidth*biHeight*biBitCount��
		bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); //�ṹ��Ĵ�С������ɫ��

		// ��ȡ��ĻHDC
		screen_hwnd = hwnd;
		screen_hdc = GetDC(screen_hwnd);

		// ��ȡ����HDC�ͼ���Bitmap������Bitmapѡ�����HDC
		hCompatibleDC = CreateCompatibleDC(screen_hdc); //�������ݴ��ھ��
		hCompatibleBitmap = CreateCompatibleBitmap(screen_hdc, WINDOW_WIDTH, WINDOW_HEIGHT); //��������λͼ���
		hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hCompatibleBitmap); //ѡ��ԭʼ��λͼ����


		LoadModelData(PointList, std::string(MODEL_PATH));
		LoadTextureData(TEXTURE_PATH);


		ReMVPAndUpdateCamPos();
	}



	// ��ʼ������ģ�Ͷ��㲢��ȡ�������
	void InitializeScene() {

		// ���趥�㻺������
		PointListCache = PointList;

		// ���»�ȡ�������
		worldMatrix = Matrix4f::getIdentityMatrix();

		// �ƶ��������������ϵ��ԭ��
		Matrix4f translateMatrix = Matrix4f::getTranslateMatrix(0.0, 0.0f, 0.0f);
		worldMatrix = translateMatrix * worldMatrix;
	}



	static float xMovtion = 0;
	static float yMovtion = 0;

	// ��Ҫ��ÿ֡������ִ�е��û��任������ί����ʽ
	void TranslateOBJByUser(short xMove, short yMove, short zMove) {

		// ���ҽ����û�Ҫ�ı������ʱ��Ŵ���ֵ�����¾���
		if (xMove != 0 && yMove != 0)
		{
			Matrix4f TranslateMa = Matrix4f::getTranslateMatrix(xMovtion, yMovtion, 0); //����Y����ת
			worldMatrix = TranslateMa * worldMatrix;
		}
	}


	static short xRotation = 0;
	static short yRotation = 0;

	// ��Ҫ��ÿ֡������ִ�е��û��任������ί����ʽ
	void RotateOBJByUser(short xMove, short yMove, short zMove) {

		xRotation += xMove;
		yRotation += yMove;

		// ��ȡX��Y��ת����
		Matrix4f RotateYMa = Matrix4f::getRotateYMatrix(xRotation); //����Y����ת
		Matrix4f RotateXMa = Matrix4f::getRotateXMatrix(yRotation); //����X����ת

		// �����������
		worldMatrix = RotateYMa * worldMatrix;
		worldMatrix = RotateXMa * worldMatrix;
	}




	static short zScaler = 0;

	// ��Ҫ��ÿ֡������ִ�е��û��任������ί����ʽ
	void ScaleOBJByUser(short xMove, short yMove, short zMove) {

		zScaler += zMove;

		// ���ҽ����û�Ҫ�ı������ʱ��Ŵ���ֵ�����¾���
		if (zMove != 0) {

			std::cout << "Z Scaler is : " << zScaler << std::endl;

			Matrix4f ZoomMa = Matrix4f::getTranslateMatrix(0, 0, zScaler* 0.01); //Z���Ͻ���Zoom
			worldMatrix = ZoomMa * worldMatrix;
		}
	}


	// ��ά�۲���ˮ��
	void RunObservationPipeline() {

		// �����ж���ķ������任������ռ�
		TransormObjectNormalToWorld(PointListCache, &worldMatrix);

		// �����ж����������任
		ObjectTransform(PointListCache, &worldMatrix);

		// �����ж����������任
		ObjectTransform(PointListCache, &ViewMatrix);

		// �����ж������͸��ͶӰ�任
		ObjectTransform(PointListCache, &ProjectionMatrix);

		// ����CVV�ü��㷨
		CVVClip(PointListCache);

		// �����ж������͸�ӳ���
		PerspectiveDivede(PointListCache);

		// �����ж�������ӿڱ任
		ObjectTransform(PointListCache, &ViewPortMatrix);

		// ���б����޳�
		BackCulling(PointListCache);
	}



	void UpdateScene(void(*TransformCallback)(short x, short y, short z), short x, short y, short z)
	{
		// ��ʼ����������
		InitializeScene();

		// �����û�����ı任����
		TransformCallback(x, y, z);

		// ������ά�۲���ˮ��
		RunObservationPipeline();
	}


	void Render(DisplayMode mode)
	{
		// ���߿�ģʽ��Ⱦ
		if (mode == WIRE_FRAME_MODE)
		{
			RenderInWireFrameMode(255, 255, 255);
		}

		// ��ʵ��ģʽ��Ⱦ
		else if (mode == SOLID_MODE)
		{
			RenderInSolidMode();
		}
		// ������ģʽ��Ⱦ
		else
		{
			RenderInTextureMode();
		}

		// Set the pixels in a compatible bitmap using the color data found in the specified DIB(DIB: �豸�޹�λͼ�ļ�)
		SetDIBits(screen_hdc, hCompatibleBitmap, 0, WINDOW_HEIGHT, BackBuffer, (BITMAPINFO*)&bitmapInfo, DIB_RGB_COLORS);
		// ��Դ�豸���������е����ؽ���λ��ת���Դ��͵�Ŀ���豸����
		BitBlt(screen_hdc, -1, -1, WINDOW_WIDTH, WINDOW_HEIGHT, hCompatibleDC, 0, 0, SRCCOPY);
	}




	void RenderInWireFrameMode(byte r, byte g, byte b)
	{
		for (size_t i = 0; i != PointListCache.size(); ++i)
		{
			if ((i % 3 == 0) && (PointListCache[i].IsCull == false))
			{
				// �㣬�ڱ�ʾʱӦΪ(x, y, z, 1)��wΪ1��
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
		// �ͷ���ĻHDC,����HDC������Bitmap
		SelectObject(hCompatibleDC, hOldBitmap);
		DeleteObject(hCompatibleDC);
		DeleteObject(hCompatibleDC);

		// �ͷžɵ�HDC
		ReleaseDC(screen_hwnd, screen_hdc);

		// ɾ����������
		delete[] BackBuffer;

		// ɾ��������
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
