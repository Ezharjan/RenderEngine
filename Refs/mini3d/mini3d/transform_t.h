#pragma once
#include "matrix_t.h"
#include "vector_t.h"
#include "point_t.h"

//坐标变换
class transform_t
{
public:
	transform_t();
	~transform_t();

	// mvp矩阵更新，计算 transform = world * view * projection
	void transform_update();
	// mvp矩阵初始化，设置屏幕长宽
	void transform_init(const int width, const int height);
	//mvp矩阵变换
	vector_t transform_apply(const vector_t& point_untransformed)const;
	// 归一化，得到屏幕坐标。（包含透视除法、变到0-1之间、根据长宽进行屏幕映射）
	vector_t tranform_homogenize(const vector_t& x)const;


	matrix_t world;			//世界坐标变换
	matrix_t view;			//视图坐标变换
	matrix_t projection;	//投影坐标变换
	matrix_t transform;		//transform = world * view * projection
	float width, height;	//屏幕大小

};

