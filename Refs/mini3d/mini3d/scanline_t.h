#pragma once
#include "vertex_t.h"
#include "trapezoid_t.h"

class scanline_t
{
public:
	scanline_t();
	~scanline_t();
	// �����������ߵĶ˵㣬��ʼ�������ɨ���ߵ����Ͳ�����
	void trapezoid_init_scan_line(const trapezoid_t& trap, int y);

	vertex_t vertex_start, step; int x_vertex_start, y_vertex_start, width_scanline;//vertex_start����㣬x_vertex_start��y_vertex_start�����vertex_start�����꣬width��ɨ���ߵĿ��
};

