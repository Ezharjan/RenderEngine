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
	background = 0xc0c0c0;//��ɫ��0xc0c0c0
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
	for (y = 0; y < height; y++)//һ���е����framebuffer
	{
		unsigned int *dst = framebuffer[y];
		unsigned int  cc = (height - 1 - y) * 230 / (height - 1);//�������ý���ɫ��0-230�������
		cc = (cc << 16) | (cc << 8) | cc;
		if (mode == 0)cc = background;
		for (x = width_window; x > 0; dst++, x--)dst[0] = cc;
	}
	for (y = 0; y < height_window; y++)//һ���е����zbuffer
	{
		float *dst = zbuffer[y];
		for (x = width_window; x > 0; dst++, x--)dst[0] = 0.0f;//zbuffer��ʼֵȫΪ0����������Զ
	}

}

void device_t::device_draw_pixel(int x, int y, unsigned color)
{
	if (((unsigned int)x) < (unsigned int)width_window && ((unsigned int)y) < (unsigned int)height_window) {//�жϵ��Ƿ�����Ļ��
		framebuffer[y][x] = color;
	}
}

void device_t::device_draw_line(int x1, int y1, int x2, int y2, unsigned color)
{
	int x, y, rem = 0;
	if (x1 == x2&&y1 == y2)//�����غϵ����
	{
		device_draw_pixel(x1, y1, color);
	}
	else if (x1 == x2)//��y��ƽ�е��߶�
	{
		int inc = (y1 <= y2) ? 1 : -1;
		for (y = y1; y != y2; y += inc)device_draw_pixel(x1, y, color);
		device_draw_pixel(x2, y2, color);
	}
	else if (y1 == y2)//��x��ƽ�е��߶�
	{
		int inc = (x1 <= x2) ? 1 : -1;
		for (x = x1; x != x2; x += inc) device_draw_pixel(x, y1, color);
		device_draw_pixel(x2, y2, color);
	}
	else//����xy��ƽ�е��߶�
	{
		int dx = (x1 < x2) ? (x2 - x1) : (x1 - x2);//�߶����˵���x��ľ���
		int dy = (y1 < y2) ? (y2 - y1) : (y1 - y2);//�߶����˵���y��ľ���
		if (dx >= dy)
		{
			for (x = x1, y = y1; x <= x2; x++)
			{
				device_draw_pixel(x, y, color);
				rem += dy;//����ͼ�ͺ������ˣ����ֻ������ͼƬ
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
	x = (int)(u + 0.5f);//��������������ڽ������Ժ���Կ��������Բ�ֵ���Ż� 
	y = (int)(v + 0.5f);
	x = mini3dmath::CMID(x, 0, tex_width - 1);//��֤�������궼�ڷ�Χ��
	y = mini3dmath::CMID(y, 0, tex_height - 1);
	return texture[y][x];
}

void device_t::device_draw_scanline(scanline_t& scanline)
{
	unsigned int *_framebuffer = framebuffer[scanline.y_vertex_start];
	float *_zbuffer = zbuffer[scanline.y_vertex_start];
	int x = scanline.x_vertex_start;//���Ƶ����x
	int width_scanline = scanline.width_scanline;//���ƿ��
	int _width_window = width_window;//�豸���
	int renderstate = render_state;//����ģʽ
	for (; width_scanline > 0; x++, width_scanline--)
	{
		if (x >= 0 && x < _width_window)//�ж�x�Ƿ�����Ļ��
		{
			float w_reciprocal = scanline.vertex_start.w_reciprocal;
			if (w_reciprocal >= _zbuffer[x])//���ﲻ��z�жϵ�ԭ����z�Ļ���û����ʾ����Զ������ܴ������������٣�rhw��z�ĵ�������ʼ����rhw=0������Զ���κα�0��Ķ�������ͷ������zbuffer�����ʼֵΪ0.
			{
				float w = 1.0f / w_reciprocal;
				_zbuffer[x] = w_reciprocal;//zbuffer����洢z�ĵ���rhw

				if (render_state&RENDER_STATE_COLOR)//��ɫͼ
				{
					float r = scanline.vertex_start.color.r * w;//��Ϊ֮ǰ��ɫrgb���Թ�w��Ϊ�˲�ֵ�������ڳ���w���ǻ�ԭ������ֵ��
					float g = scanline.vertex_start.color.g * w;
					float b = scanline.vertex_start.color.b * w;

					int R = static_cast<int>(r*255.0f);//����0-255֮�����ɫֵ
					int G = static_cast<int>(g*255.0f);
					int B = static_cast<int>(b*255.0f);

					R = mini3dmath::CMID(R, 0, 255);//ȥ�����Ϸ�����ɫֵ
					G = mini3dmath::CMID(G, 0, 255);
					B = mini3dmath::CMID(B, 0, 255);

					_framebuffer[x] = (R << 16) | (G << 8) | (B);//RGB��ɫֵ��32λ�޷�����������ʽ����֡����framebuffer
				}
				if (render_state & RENDER_STATE_TEXTURE)//����ͼ
				{
					float u = scanline.vertex_start.texturecoordinate.u *w;//��Ϊ֮ǰ��������uv���Թ�w��Ϊ�˲�ֵ�������ڳ���w���ǻ�ԭ������ֵ��
					float v = scanline.vertex_start.texturecoordinate.v*w;
					unsigned int cc = device_texture_read(u, v);
					_framebuffer[x] = cc;//����ֵ����֡����framebuffer
				}
			}
			scanline.vertex_start += scanline.step;//���ϲ���step��ָ����һ��ɨ���
			if (x >= _width_window) break;
		}

	}
}

void device_t::device_render_trap(trapezoid_t& trap)
{
	scanline_t scanline;
	int j, top, bottom;
	top = (int)(trap.top + 0.5f);//�������룬ɨ���ߵ�����ֻ������
	bottom = (int)(trap.bottom + 0.5f);//��������
	for (j = top; j < bottom; j++)
	{
		if (j >= 0 && j < height_window)
		{
			trapezoid_t::trapezoid_edge_interp(trap, (float)j + 0.5f);//��ʼ��trap��y=(float)j + 0.5f���������ߵĵ�
			scanline.trapezoid_init_scan_line(trap, j);//��ʼ��ɨ����
			device_draw_scanline(scanline);//����ɨ����
		}
		if (j >= height_window)break;
	}
}

void device_t::device_draw_primitive(const vertex_t& vertex1, const vertex_t& vertex2, const vertex_t& vertex3)
{
	vector_t p1, p2, p3, pos_transformed1, pos_transformed2, pos_transformed3;
	int render_state = this->render_state;

	// ���� Transform �仯
	pos_transformed1 = transform.transform_apply(vertex1.position);
	pos_transformed2 = transform.transform_apply(vertex2.position);
	pos_transformed3 = transform.transform_apply(vertex3.position);

	// �ü���ע��˴���������Ϊ�����жϼ������� cvv���Լ�ͬcvv�ཻƽ����������
	// ���н�һ����ϸ�ü�����һ���ֽ�Ϊ������ȫ���� cvv�ڵ�������
	if (mini3dmath::check_cvv(pos_transformed1) != 0)return;//�����ε���������ֻҪ��һ�����ڲü��ռ��ھ��޳�
	if (mini3dmath::check_cvv(pos_transformed2) != 0)return;
	if (mini3dmath::check_cvv(pos_transformed3) != 0)return;

	// ��һ��������βü�����ת������׼���豸���꣬��X,Y,Z���귶Χ��0-1֮�䣩
	p1 = transform.tranform_homogenize(pos_transformed1);
	p2 = transform.tranform_homogenize(pos_transformed2);
	p3 = transform.tranform_homogenize(pos_transformed3);

	// �������ɫ�ʻ���
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

		t1.vertex_w_reciprocal_init();// ��ʼ�� w��͸������У�������ڶ�������׼���豸�ռ�
		t2.vertex_w_reciprocal_init();
		t3.vertex_w_reciprocal_init();

		// ���������Ϊ0-2�����Σ����ҷ��ؿ�����������
		n = trapezoid_t::trapezoid_init_triangle(traps, t1, t2, t3);

		if (n >= 1)device_render_trap(traps[0]);
		if (n >= 2)device_render_trap(traps[1]);
	}
	//�߿�ģʽ
	if(render_state&RENDER_STATE_WIREFRAME)
	{
		device_draw_line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, foreground);
		device_draw_line((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y, foreground);
		device_draw_line((int)p3.x, (int)p3.y, (int)p2.x, (int)p2.y, foreground);
	}
}


