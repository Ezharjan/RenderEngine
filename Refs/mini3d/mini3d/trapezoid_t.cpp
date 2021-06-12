#include "trapezoid_t.h"
#include <future>


trapezoid_t::trapezoid_t()
{
}


trapezoid_t::~trapezoid_t()
{
}

int trapezoid_t::trapezoid_init_triangle(trapezoid_t* trap, vertex_t& p1, vertex_t& p2, vertex_t& p3)
{
	//const vertex_t *p;
	float k, x;
	//ȷ������p1�������棬p2���м䣬p3�������棨��Ļ����ԭ�������Ͻǣ�
	if (p1.position.y > p2.position.y)
	{
		const auto p = p1;
		p1 = p2;
		p2 = p;
	}
	if (p1.position.y > p3.position.y) {
		const auto p = p1;
		p1 = p3;
		p3 = p;
	}
	if (p2.position.y > p3.position.y) {
		const auto p = p2;
		p2 = p3;
		p3 = p;
	}
	if (p1.position.y == p2.position.y && p1.position.y == p3.position.y)return 0;
	if (p1.position.x == p2.position.x && p1.position.x == p3.position.x)return 0;

	if (p1.position.y == p2.position.y)
	{
		if (p1.position.x > p2.position.x) {
			const auto& p = p1;
			p2 = p;
			p1 = p2;
		}
		trap[0].top = p1.position.y;
		trap[0].bottom = p3.position.y;
		trap[0].left.v1 = p1;
		trap[0].left.v2 = p3;
		trap[0].right.v1 = p2;
		trap[0].right.v2 = p3;
		return (trap[0].top < trap[0].bottom) ? 1 : 0;
	}

	if (p2.position.y == p3.position.y)// triangle up
	{
		if (p2.position.x > p3.position.x) {
			const auto& p = p1;
			p2 = p;
			p1 = p2;
		}//ȷ��p2��p3�����
		trap[0].top = p1.position.y;
		trap[0].bottom = p3.position.y;
		trap[0].top = p1.position.y;
		trap[0].bottom = p3.position.y;
		trap[0].left.v1 = p1;
		trap[0].left.v2 = p2;
		trap[0].right.v1 = p1;
		trap[0].right.v2 = p3;
		return (trap[0].top < trap[0].bottom) ? 1 : 0;
	}
	//�����ε������߶�û����X��ƽ��
	trap[0].top = p1.position.y;
	trap[0].bottom = p2.position.y;
	trap[1].top = p2.position.y;
	trap[1].bottom = p3.position.y;

	//�����Բ�ֵ
	k = (p3.position.y - p1.position.y) / (p2.position.y - p1.position.y);//P3-P1�Ĵ�ֱ������P2-P1�Ĵ�ֱ�����ֵ
	x = p1.position.x + (p2.position.x - p1.position.x)*k;//����ͬ����k��P3��xֵӦ���Ƕ���

	if(x <= p3.position.x)
	{
		trap[0].left.v1 = p1;
		trap[0].left.v2 = p2;
		trap[0].right.v1 = p1;
		trap[0].right.v2 = p3;

		trap[1].left.v1 = p2;
		trap[1].left.v2 = p3;
		trap[1].right.v1 = p1;
		trap[1].right.v2 = p3;
	}
	else
	{
		trap[0].left.v1 = p1;
		trap[0].left.v2 = p3;
		trap[0].right.v1 = p1;
		trap[0].right.v2 = p2;

		trap[1].left.v1 = p1;
		trap[1].left.v2 = p3;
		trap[1].right.v1 = p2;
		trap[1].right.v2 = p3;
	}

	return 2;
}

void trapezoid_t::trapezoid_edge_interp(trapezoid_t& trap, float y)
{
	float s1 = trap.left.v2.position.y - trap.left.v1.position.y;
	float s2 = trap.right.v2.position.y - trap.right.v1.position.y;
	float t1 = (y - trap.left.v1.position.y) / s1;//Ϊ�����ֵ���������ϵ
	float t2 = (y - trap.right.v1.position.y) / s2;

	trap.left.v.vertex_interp(trap.left.v1, trap.left.v2, t1);
	trap.right.v.vertex_interp(trap.right.v1, trap.right.v2, t2);
}

