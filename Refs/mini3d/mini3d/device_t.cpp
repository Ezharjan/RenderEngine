#include "device_t.h"
#include <cassert>
#include"stdlib.h"
#include"string.h"
#include "mini3dmath.h"


device_t::device_t()
{
}


device_t::~device_t()
{
}

void device_t::device_init(int width, int height, void* fb)
{
	int need = sizeof(void*) * (height * 2 + 1024) + width * height * 8;
	char *ptr = (char*)malloc(need + 64);
	char *framebuf, *zbuf;
	int j;
	assert(ptr);
	framebuffer = (unsigned int**)ptr;
	zbuffer = (float**)(ptr + sizeof(void*) * height);
	ptr += sizeof(void*) * height * 2;
	texture = (unsigned int**)ptr;
	ptr += sizeof(void*) * 1024;
	//framebuf = (char*)ptr;
	zbuf = (char*)ptr + width * height * 4;
	ptr += width * height * 8;
	if (fb != nullptr)
	{
		framebuf = (char*)fb;
	}
	for (j = 0; j < height; j++) {
		framebuffer[j] = (unsigned int*)(framebuf + width * 4 * j);
		zbuffer[j] = (float*)(zbuf + width * 4 * j);
	}
	texture[0] = (unsigned int*)ptr;
	texture[1] = (unsigned int*)(ptr + 16);
	memset(texture[0], 0, 64);
	tex_width = 2;
	tex_height = 2;
	max_u = 1.0f;
	max_v = 1.0f;
	width_window = width;
	height_window = height;
	background = 0xc0c0c0;//银色，0xc0c0c0
	foreground = 0;
	transform.transform_init(width, height);
	render_state = RENDER_STATE_WIREFRAME;
}

void device_t::device_destroy()
{
	if (framebuffer)free(framebuffer);
	framebuffer = nullptr;
	zbuffer = nullptr;
	texture = nullptr;
}

void device_t::device_set_texture(void* bits, long pitch, int w, int h)
{
	char* ptr = (char*)bits;
	int j;
	assert(w <= 1024 && h <= 1024);
	for (j = 0; j < h; ptr += pitch, j++)
	{
		texture[j] = (unsigned int *)ptr;
	}
	tex_width = w;
	tex_height = h;
	max_u = static_cast<float>(w - 1);
	max_v = static_cast<float>(h - 1);
}

void device_t::device_clear(int mode)
{
	int y, x, height = height_window;
	for (y = 0; y < height; y++)//一行行的清空framebuffer
	{
		unsigned int *dst = framebuffer[y];
		unsigned int  cc = (height - 1 - y) * 230 / (height - 1);//这里是用渐变色（0-230）来清空
		cc = (cc << 16) | (cc << 8) | cc;
		if (mode == 0)cc = background;
		for (x = width_window; x > 0; dst++, x--)dst[0] = cc;
	}
	for (y = 0; y < height_window; y++)//一行行的清空zbuffer
	{
		float *dst = zbuffer[y];
		for (x = width_window; x > 0; dst++, x--)dst[0] = 0.0f;//zbuffer初始值全为0，代表无限远
	}

}

void device_t::device_draw_pixel(int x, int y, unsigned color)
{
	if (((unsigned int)x) < (unsigned int)width_window && ((unsigned int)y) < (unsigned int)height_window) {//判断点是否在屏幕内
		framebuffer[y][x] = color;
	}
}

void device_t::device_draw_line(int x1, int y1, int x2, int y2, unsigned color)
{
	int x, y, rem = 0;
	if (x1 == x2&&y1 == y2)//两点重合的情况
	{
		device_draw_pixel(x1, y1, color);
	}
	else if (x1 == x2)//与y轴平行的线段
	{
		int inc = (y1 <= y2) ? 1 : -1;
		for (y = y1; y != y2; y += inc)device_draw_pixel(x1, y, color);
		device_draw_pixel(x2, y2, color);
	}
	else if (y1 == y2)//与x轴平行的线段
	{
		int inc = (x1 <= x2) ? 1 : -1;
		for (x = x1; x != x2; x += inc) device_draw_pixel(x, y1, color);
		device_draw_pixel(x2, y2, color);
	}
	else//不与xy轴平行的线段
	{
		int dx = (x1 < x2) ? (x2 - x1) : (x1 - x2);//线段两端点在x轴的距离
		int dy = (y1 < y2) ? (y2 - y1) : (y1 - y2);//线段两端点在y轴的距离
		if (dx >= dy)
		{
			for (x = x1, y = y1; x <= x2; x++)
			{
				device_draw_pixel(x, y, color);
				rem += dy;//画个图就很清晰了，看手机里面的图片
				if (rem >= dx)
				{
					rem -= dx;
					y += (y2 >= y1) ? 1 : -1;
					device_draw_pixel(x, y, color);
				}
			}
			device_draw_pixel(x2, y2, color);
		}
		else
		{
			if (y2 < y1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
		}

	}
}

unsigned int device_t::device_texture_read(float u, float v)
{
	int x, y;
	u = u*max_u;
	v = v*max_v;
	x = (int)(u + 0.5f);//纹理采样采用最邻近法，以后可以考虑用线性插值来优化 
	y = (int)(v + 0.5f);
	x = mini3dmath::CMID(x, 0, tex_width - 1);//保证纹理坐标都在范围内
	y = mini3dmath::CMID(y, 0, tex_height - 1);
	return texture[y][x];
}

void device_t::device_draw_scanline(scanline_t& scanline)
{
	unsigned int *_framebuffer = framebuffer[scanline.y_vertex_start];
	float *_zbuffer = zbuffer[scanline.y_vertex_start];
	int x = scanline.x_vertex_start;//绘制的起点x
	int width_scanline = scanline.width_scanline;//绘制宽度
	int _width_window = width_window;//设备宽度
	int renderstate = render_state;//绘制模式
	for (; width_scanline > 0; x++, width_scanline--)
	{
		if (x >= 0 && x < _width_window)//判断x是否在屏幕内
		{
			float w_reciprocal = scanline.vertex_start.w_reciprocal;
			if (w_reciprocal >= _zbuffer[x])//这里不用z判断的原因：用z的话，没法表示无限远，设个很大的数？该设多少？rhw是z的倒数，初始化成rhw=0即无限远？任何比0大的都离摄像头更近。zbuffer里面初始值为0.
			{
				float w = 1.0f / w_reciprocal;
				_zbuffer[x] = w_reciprocal;//zbuffer里面存储z的倒数rhw

				if (render_state&RENDER_STATE_COLOR)//颜色图
				{
					float r = scanline.vertex_start.color.r * w;//因为之前颜色rgb除以过w（为了插值），现在乘以w，是还原本来的值。
					float g = scanline.vertex_start.color.g * w;
					float b = scanline.vertex_start.color.b * w;

					int R = static_cast<int>(r*255.0f);//计算0-255之间的颜色值
					int G = static_cast<int>(g*255.0f);
					int B = static_cast<int>(b*255.0f);

					R = mini3dmath::CMID(R, 0, 255);//去掉不合法的颜色值
					G = mini3dmath::CMID(G, 0, 255);
					B = mini3dmath::CMID(B, 0, 255);

					_framebuffer[x] = (R << 16) | (G << 8) | (B);//RGB颜色值以32位无符号整数的形式存入帧缓存framebuffer
				}
				if (render_state & RENDER_STATE_TEXTURE)//纹理图
				{
					float u = scanline.vertex_start.texturecoordinate.u *w;//因为之前纹理坐标uv除以过w（为了插值），现在乘以w，是还原本来的值。
					float v = scanline.vertex_start.texturecoordinate.v*w;
					unsigned int cc = device_texture_read(u, v);
					_framebuffer[x] = cc;//纹理值赋给帧缓存framebuffer
				}
			}
			scanline.vertex_start += scanline.step;//加上步长step，指向下一个扫描点
			if (x >= _width_window) break;
		}

	}
}

void device_t::device_render_trap(trapezoid_t& trap)
{
	scanline_t scanline;
	int j, top, bottom;
	top = (int)(trap.top + 0.5f);//四舍五入，扫描线的坐标只能整数
	bottom = (int)(trap.bottom + 0.5f);//四舍五入
	for (j = top; j < bottom; j++)
	{
		if (j >= 0 && j < height_window)
		{
			trapezoid_t::trapezoid_edge_interp(trap, (float)j + 0.5f);//初始化trap中y=(float)j + 0.5f的左右两边的点
			scanline.trapezoid_init_scan_line(trap, j);//初始化扫描线
			device_draw_scanline(scanline);//绘制扫描线
		}
		if (j >= height_window)break;
	}
}

void device_t::device_draw_primitive(const vertex_t& vertex1, const vertex_t& vertex2, const vertex_t& vertex3)
{
	vector_t p1, p2, p3, pos_transformed1, pos_transformed2, pos_transformed3;
	int render_state = this->render_state;

	// 按照 Transform 变化
	pos_transformed1 = transform.transform_apply(vertex1.position);
	pos_transformed2 = transform.transform_apply(vertex2.position);
	pos_transformed3 = transform.transform_apply(vertex3.position);

	// 裁剪，注意此处可以完善为具体判断几个点在 cvv内以及同cvv相交平面的坐标比例
	// 进行进一步精细裁剪，将一个分解为几个完全处在 cvv内的三角形
	if (mini3dmath::check_cvv(pos_transformed1) != 0)return;//三角形的三个顶点只要有一个不在裁剪空间内就剔除
	if (mini3dmath::check_cvv(pos_transformed2) != 0)return;
	if (mini3dmath::check_cvv(pos_transformed3) != 0)return;

	// 归一化（从齐次裁剪坐标转换到标准化设备坐标，即X,Y,Z坐标范围在0-1之间）
	p1 = transform.tranform_homogenize(pos_transformed1);
	p2 = transform.tranform_homogenize(pos_transformed2);
	p3 = transform.tranform_homogenize(pos_transformed3);

	// 纹理或者色彩绘制
	if (render_state & (RENDER_STATE_TEXTURE | RENDER_STATE_COLOR))
	{
		vertex_t t1 = vertex1, t2 = vertex2, t3 = vertex3;
		trapezoid_t traps[2];
		int n;

		t1.position = p1;
		t2.position = p2;
		t3.position = p3;
		t1.position.w = pos_transformed1.w;
		t2.position.w = pos_transformed2.w;
		t3.position.w = pos_transformed3.w;

		t1.vertex_w_reciprocal_init();// 初始化 w。透视纹理校正发生在顶点进入标准化设备空间
		t2.vertex_w_reciprocal_init();
		t3.vertex_w_reciprocal_init();

		// 拆分三角形为0-2个梯形，并且返回可用梯形数量
		n = trapezoid_t::trapezoid_init_triangle(traps, t1, t2, t3);

		if (n >= 1)device_render_trap(traps[0]);
		if (n >= 2)device_render_trap(traps[1]);
	}
	//线框模式
	if(render_state&RENDER_STATE_WIREFRAME)
	{
		device_draw_line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, foreground);
		device_draw_line((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y, foreground);
		device_draw_line((int)p3.x, (int)p3.y, (int)p2.x, (int)p2.y, foreground);
	}
}


