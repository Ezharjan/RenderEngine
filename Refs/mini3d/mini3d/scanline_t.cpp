#include "scanline_t.h"


scanline_t::scanline_t()
{

}

scanline_t::~scanline_t()
{
}

void scanline_t::trapezoid_init_scan_line(const trapezoid_t& trap, int y)
{
	float width = trap.right.v.position.x - trap.left.v.position.x;
	x_vertex_start = static_cast<int>(trap.left.v.position.x + 0.5f);
	width_scanline = static_cast<int>(trap.right.v.position.x + 0.5f) - x_vertex_start;
	y_vertex_start = y;
	vertex_start = trap.left.v;//¼ÆËãÉ¨ÃèÏßÆðµã£¨´Ó×óÍùÓÒÉ¨Ãè£©

	if (trap.left.v.position.x >= trap.right.v.position.x)width_scanline = 0;

	step.vertex_division(trap.left.v, trap.right.v, width);



}
