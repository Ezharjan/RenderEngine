#pragma once

#include "Math.h"

#ifndef __Config__
#define __Config__

namespace RenderEngine {


#define WINDOW_TITLE   "RenderEngine"

// ���ڿ�Ⱥ͸߶� 
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

// �����������Ļ��λ��
#define AT_SCREEN_POS_X 400
#define AT_SCREEN_POS_Y 200

// �������Զ����
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


	// ��������任�������������(cameraPos+lokAt+cameraUp)
	extern Vector3 CameraPos(0, 0, -3); // �������������ϵ�µ�λ��
	extern Vector3 lookAt(0.0f, 0.0f, 0.0f);
	extern Vector3 cameraUp(0.0f, 1.0f, 0.0f); // ��ʼֵ

	// ��������ǿ����������ɫ, ������ⷽ��(����ֱ��ָ����ķ�����ں͵�λ�������ļ���)
	extern Vector4 DiffuseLight(255, 255, 255);
	extern Vector4 AmbientLight(255, 255, 255);
	extern Vector4 DiffuseDir(0, 0, 1);
	extern Vector4 DiffuseDirRevse(0,0,1);
}

#endif // !__Config__