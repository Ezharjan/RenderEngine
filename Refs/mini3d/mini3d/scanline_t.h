#pragma once
#include "vertex_t.h"
#include "trapezoid_t.h"

class scanline_t
{
public:
	scanline_t();
	~scanline_t();
	// 根据左右两边的端点，初始化计算出扫描线的起点和步长。
	void trapezoid_init_scan_line(const trapezoid_t& trap, int y);

	vertex_t vertex_start, step; int x_vertex_start, y_vertex_start, width_scanline;//vertex_start是起点，x_vertex_start、y_vertex_start是起点vertex_start的坐标，width是扫描线的宽度
};

