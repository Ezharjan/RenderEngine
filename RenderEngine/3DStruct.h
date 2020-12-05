/*��Ҫ����ά�ṹ�������Լ�����ȫ�ֱ���������*/
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


	// �������棬��Ȼ��棬������(���������ɫҲ����BGR��˳��洢������ɫ��)
	BYTE *BackBuffer = new BYTE[WINDOW_WIDTH * WINDOW_HEIGHT * 3]();  //ÿ����byteΪһ�����أ��ܹ���800*600�����أ�800*600*3��byte, BGR����ɫ�洢˳��
	//float DepthBuffer[WINDOW_HEIGHT][WINDOW_WIDTH]; //��Ȼ���
	BYTE *TextureBuffer = new BYTE[textureHeight*textureWidth * 3];

	// �����ڵ��豸������/������HDC��Handle
	static HDC screen_hdc; //Handle to Device Context�������ľ��
	static HWND screen_hwnd; //Handle to A Window�����ھ�� --- ���ڵ�Ψһ��ʶ��
	static HDC hCompatibleDC; //����HDC
	static HBITMAP hCompatibleBitmap; //����BITMAP
	static HBITMAP hOldBitmap; //�ɵ�BITMAP				  
	static BITMAPINFO bitmapInfo; //BITMAPINFO�ṹ��


	// �ĸ��任����
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

	// �����б�������Ϊһ��������
	std::vector<CGVertex> PointList;  //��ʼ��������
	std::vector<CGVertex> PointListCache;  //���ڼ���任�Ķ��㻺������



	// ö������ʾģʽ
	enum  DisplayMode
	{
		WIRE_FRAME_MODE,
		SOLID_MODE,
		TEXTURE_MODE,
	};


	DisplayMode MyDisplayMode;
}



#endif // !__3DStruct__