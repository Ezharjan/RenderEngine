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

	// 构建MVP过程
	//void ConstructMVP(const Vector4& lookAt, const Vector4& cameraUp);
	void ConstructMVP(const Vector3& lookAt, const Vector3& cameraUp);

	// 初始化3D各种参数
	void Initlize3D(const HWND& hwnd);

	// 初始化载入模型顶点并获取世界矩阵
	void InitializeScene();

	// 需要在每帧调用中执行的用户变换函数的委托形式
	void TranslateOBJByUser(short xMove,short yMove,short zMove = 0);

	// 需要在每帧调用中执行的用户变换函数的委托形式
	void RotateOBJByUser(short xMove,short yMove,short zMove = 0);

	// 需要在每帧调用中执行的用户变换函数的委托形式
	void ScaleOBJByUser(short xMove,short yMove,short zMove = 0);

	// 二维观察流水线
	void RunObservationPipeline();

	// 更新场景(顶点位置,颜色，纹理等)数据，并接收用户方面对物体的变换
	void UpdateScene(void(*callback)(short x, short y,short z),short x,short y,short z = 0);

	// 渲染函数
	//void Render(RenderEngine::DisplayMode mode);

	// 以线框模式在渲染
	void RenderInWireFrameMode(byte r, byte g, byte b);

	// 以实体的模式在渲染
	void RenderInSolidMode();

	// 以纹理的模式在渲染
	void RenderInTextureMode();

	// 清除背部缓存
	void ClearBackBuffer(byte r, byte g, byte b);

	// 清除深度缓存
	void ClearDepthBuffer(float MaxDepth);

	// 释放资源
	void Release3D();
}



// Decalre the function related to the user interaction befor calling it in WinMain
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// Entry of all the functions
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);


#endif // !__Main__
