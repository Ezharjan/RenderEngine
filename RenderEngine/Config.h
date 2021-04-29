#pragma once

#include "Math.h"

namespace RenderEngine {


#ifndef WINDOW_TITLE 
#define WINDOW_TITLE "RenderEngine"
#endif // !WINDOW_TITLE 

#ifndef WINDOW_WIDTH  
#define WINDOW_WIDTH  720
#endif // !WINDOW_WIDTH  
#ifndef WINDOW_HEIGHT
#define WINDOW_HEIGHT 576
#endif // !WINDOW_HEIGHT

// Relevant position of window on screen
#ifndef AT_SCREEN_POS_X
#define AT_SCREEN_POS_X 400
#endif // !AT_SCREEN_POS_X
#ifndef AT_SCREEN_POS_Y 
#define AT_SCREEN_POS_Y 200
#endif // !AT_SCREEN_POS_Y 

#ifndef NEAR_CLIPPING_PLANE
#define NEAR_CLIPPING_PLANE 1.0f
#endif // !NEAR_CLIPPING_PLANE
#ifndef FAR_CLIPPINFG_PLANE
#define FAR_CLIPPINFG_PLANE 100.0f
#endif // !FAR_CLIPPINFG_PLANE

#ifndef MODEL_PATH 
#define MODEL_PATH "assets/Tonny.obj"
//#define MODEL_PATH "assets/Ball.obj"
//#define MODEL_PATH "assets/Cerberus.obj"
//#define MODEL_PATH "assets/Teapot.obj"
#endif // !MODEL_PATH 
#ifndef TEXTURE_PATH
#define TEXTURE_PATH_4_SHADOWPLANE "assets/Ground.bmp"
//#define TEXTURE_PATH "assets/Bricks.bmp"
//#define TEXTURE_PATH "assets/Green.bmp"
#define TEXTURE_PATH "assets/Tonny.bmp"
//#define TEXTURE_PATH "assets/Cerberus.bmp"
#endif // !TEXTURE_PATH


#ifndef CAMERA_FOV
#define CAMERA_FOV 90.0f
#endif // !CAMERA_FOV



#ifndef STATE_DRAW_TEX 
#define STATE_DRAW_TEX 1
#endif // !STATE_DRAW_TEX 

#ifndef STATE_DRAW_LINE 
#define STATE_DRAW_LINE 2
#endif // !STATE_DRAW_LINE 

#ifndef STATE_DRAW_COLOR 
#define STATE_DRAW_COLOR 4
#endif // !STATE_DRAW_COLOR 

#ifndef DRAW_MODEL
#define DRAW_MODEL 8
#endif // !DRAW_MODEL

#ifndef DRAW_CREATED_TEX
#define DRAW_CREATED_TEX 16
#endif // !DRAW_CREATED_TEX 

#ifndef DRAW_WALT_HEAD
#define DRAW_WALT_HEAD 32
#endif // !DRAW_WALT_HEAD

#ifndef INTERP_NONE 
#define INTERP_NONE 1
#endif // !INTERP_NONE 

#ifndef INTERP_BILINEAR 
#define INTERP_BILINEAR 0
#endif // !INTERP_BILINEAR 

#ifndef CULL_MODE_NONE 
#define CULL_MODE_NONE 1
#endif // !CULL_MODE_NONE 

#ifndef CULL_MODE_BACK 
#define CULL_MODE_BACK 0
#endif // !CULL_MODE_BACK 


#pragma region ASCII of A-Z in HEX
#ifndef VK_A
#define VK_A 0x41
#endif // !VK_A
#ifndef VK_B
#define VK_B 0x42
#endif // !VK_B
#ifndef VK_C
#define VK_C 0x43
#endif // !VK_C
#ifndef VK_D
#define VK_D 0x44
#endif // !VK_D
#ifndef VK_E
#define VK_E 0x45
#endif // !VK_E
#ifndef VK_F
#define VK_F 0x46
#endif // !VK_F
#ifndef VK_G
#define VK_G 0x47
#endif // !VK_G
#ifndef VK_H
#define VK_H 0x48
#endif // !VK_H
#ifndef VK_I
#define VK_I 0x49
#endif // !VK_I
#ifndef VK_J
#define VK_J 0x4A
#endif // !VK_J
#ifndef VK_K 
#define VK_K 0x4B
#endif // !VK_K 
#ifndef VK_L 
#define VK_L 0x4C
#endif // !VK_L
#ifndef VK_M 
#define VK_M 0x4D
#endif // !VK_M
#ifndef VK_N
#define VK_N 0x4E
#endif // !VK_N
#ifndef VK_O
#define VK_O 0x4F
#endif // !VK_O
#ifndef VK_P
#define VK_P 0x50
#endif // !VK_P
#ifndef VK_Q
#define VK_Q 0x51
#endif // !VK_Q
#ifndef VK_R
#define VK_R 0x52
#endif // !VK_R
#ifndef VK_S
#define VK_S 0x53
#endif // !VK_S
#ifndef VK_T
#define VK_T 0x54
#endif // !VK_T
#ifndef VK_U
#define VK_U 0x55
#endif // !VK_U
#ifndef VK_V
#define VK_V 0x56
#endif // !VK_V
#ifndef VK_W
#define VK_W 0x57
#endif // !VK_W
#ifndef VK_X
#define VK_X 0x58
#endif // !VK_X
#ifndef VK_Y
#define VK_Y 0x59
#endif // !VK_Y
#ifndef VK_Z
#define VK_Z 0x5A
#endif // !VK_Z
#pragma endregion

#ifndef MATH_PI
#define MATH_PI 3.141592657519f
#endif // !MATH_PI

#ifndef EPSILON 
#define EPSILON 1.0e-5
#endif // !EPSILON 
#ifndef INFINIT
#define INFINIT 1.0e9
#endif // !INFINIT 


#ifndef AMBIENT_INTENSITY
#define AMBIENT_INTENSITY 0.3f
#endif // !AMBIENT_INTENSITY

#ifndef WHITE
#define WHITE Colour(1.f, 1.f, 1.f)
#endif


//#define MVP_DONE_IN_ONE_TIME


#ifndef LOOK_AT_POS_X
#define LOOK_AT_POS_X 0
#endif // !LOOK_AT_POS_X
#ifndef LOOK_AT_POS_Y
#define LOOK_AT_POS_Y 0
#endif // !LOOK_AT_POS_Y
#ifndef LOOK_AT_POS_Z
#define LOOK_AT_POS_Z 0
#endif // !CAMERA_POS_Y


#define $log(msg) std::cout<<msg<<std::endl;

}