#pragma once

#include<Windows.h>
#include <windowsx.h> // GET_X_LPARAM => LOWORD; GET_Y_LPARAM => HIWORD
#include<iostream>
#include"Preperation.h"
#ifdef _DEBUG
#include "DebugHelper.h"
#endif //_DEBUG

#ifndef __Main__
#define  __Main__


namespace RenderEngine {

	// ����MVP����
	//void ConstructMVP(const Vector4& lookAt, const Vector4& cameraUp);
	void ConstructMVP(const Vector3& lookAt, const Vector3& cameraUp);

	// ��ʼ��3D���ֲ���
	void Initlize3D(const HWND& hwnd);

	// ��ʼ������ģ�Ͷ��㲢��ȡ�������
	void InitializeScene();

	// ��Ҫ��ÿ֡������ִ�е��û��任������ί����ʽ
	void TranslateOBJByUser(short xMove,short yMove,short zMove = 0);

	// ��Ҫ��ÿ֡������ִ�е��û��任������ί����ʽ
	void RotateOBJByUser(short xMove,short yMove,short zMove = 0);

	// ��Ҫ��ÿ֡������ִ�е��û��任������ί����ʽ
	void ScaleOBJByUser(short xMove,short yMove,short zMove = 0);

	// ��ά�۲���ˮ��
	void RunObservationPipeline();

	// ���³���(����λ��,��ɫ�������)���ݣ��������û����������ı任
	void UpdateScene(void(*callback)(short x, short y,short z),short x,short y,short z = 0);

	// ��Ⱦ����
	void Render(RenderEngine::DisplayMode mode);

	// ���߿�ģʽ����Ⱦ
	void RenderInWireFrameMode(byte r, byte g, byte b);

	// ��ʵ���ģʽ����Ⱦ
	void RenderInSolidMode();

	// �������ģʽ����Ⱦ
	void RenderInTextureMode();

	// �����������
	void ClearBackBuffer(byte r, byte g, byte b);

	// �����Ȼ���
	void ClearDepthBuffer(float MaxDepth);

	// �ͷ���Դ
	void Release3D();
}



// Decalre the function related to the user interaction befor calling it in WinMain
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// Entry of all the functions
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);


#endif // !__Main__
