#pragma once
#include "edge_t.h"

//梯形
class trapezoid_t
{
public:
	trapezoid_t();
	~trapezoid_t();

	// 根据三角形生成 0-2 个梯形，并且返回合法梯形的数量。三角形顶点从上到下1，2，3排序，然后切割成上下两个三角形，并用梯形表示。
	static int trapezoid_init_triangle(trapezoid_t* trap, vertex_t& p1, vertex_t& p2, vertex_t& p3);

	static void trapezoid_edge_interp(trapezoid_t& trap, float y);

	float top, bottom; //梯形上底下底
	edge_t left, right;//梯形左边右边

};

