#pragma once
#include "edge_t.h"

//����
class trapezoid_t
{
public:
	trapezoid_t();
	~trapezoid_t();

	// �������������� 0-2 �����Σ����ҷ��غϷ����ε������������ζ�����ϵ���1��2��3����Ȼ���и���������������Σ��������α�ʾ��
	static int trapezoid_init_triangle(trapezoid_t* trap, vertex_t& p1, vertex_t& p2, vertex_t& p3);

	static void trapezoid_edge_interp(trapezoid_t& trap, float y);

	float top, bottom; //�����ϵ��µ�
	edge_t left, right;//��������ұ�

};

