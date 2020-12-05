#pragma once

#include "Math.h"

#ifndef __Config__
#define __Config__

namespace RenderEngine {


#define WINDOW_TITLE   "RenderEngine"

// 窗口宽度和高度 
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

// 窗口相对于屏幕的位置
#define AT_SCREEN_POS_X 400
#define AT_SCREEN_POS_Y 200

// 近截面和远截面
#define NEAR_CLIPPING_PLANE 1.0f
#define FAR_CLIPPINFG_PLANE 100.0f

//#define MODEL_PATH "ModelData/cube.txt"
//#define MODEL_PATH "ModelData/MyCude.obj"
#define MODEL_PATH "ModelData/OfficialCube.obj"
//#define MODEL_PATH "ModelData/result.cs"

#define TEXTURE_PATH "Texture/1.bmp"

#define BGColor_R 0
#define BGColor_G 1
#define BGColor_B 1

#define CAMERA_FOV 90.0f


	// 构建相机变换矩阵的三个变量(cameraPos+lokAt+cameraUp)
	extern Vector3 CameraPos(0, 0, -3); // 相机在世界坐标系下的位置
	extern Vector3 lookAt(0.0f, 0.0f, 0.0f);
	extern Vector3 cameraUp(0.0f, 1.0f, 0.0f); // 初始值

	// 漫反射光光强，环境光颜色, 漫反射光方向(这里直接指出光的反向便于和单位法向量的计算)
	extern Vector4 DiffuseLight(255, 255, 255);
	extern Vector4 AmbientLight(255, 255, 255);
	extern Vector4 DiffuseDir(0, 0, 1);
	extern Vector4 DiffuseDirRevse(0,0,1);
}

#endif // !__Config__