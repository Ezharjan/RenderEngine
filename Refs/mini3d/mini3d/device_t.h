#pragma once
#include "transform_t.h"
#include "scanline_t.h"
#define RENDER_STATE_WIREFRAME      1		// ��Ⱦ�߿�
#define RENDER_STATE_TEXTURE        2		// ��Ⱦ����
#define RENDER_STATE_COLOR          4		// ��Ⱦ��ɫ
class device_t
{
public:
	device_t();
	~device_t();

	//�豸��ʼ����fbΪ�ⲿ֡���棬��null�������ⲿ֡���棨ÿ��4�ֽڶ��룩
	void device_init(int width, int height, void* fb);
	// ɾ���豸
	void device_destroy();
	// ���õ�ǰ����,pitch:ÿ��ɨ���ߵ��ֽ����������� (�����ر�ʾ��ͼ���� * ÿ�����ֽ���) + �ڴ���䡣
	void device_set_texture(void* bits, long pitch, int w, int h);
	//���framebuffer �� zbuffer
	void device_clear(int mode);
	//����
	void device_draw_pixel(int x, int y, unsigned int color);
	//����
	void device_draw_line(int x1, int y1, int x2, int y2, unsigned int color);
	//���������ȡ����
	unsigned int device_texture_read(float u,float v);
	//����ɨ����
	void device_draw_scanline(scanline_t& scanline);
	//��Ⱦ����trap������Ⱦ����
	void device_render_trap(trapezoid_t &trap);
	// ���� render_state ����ԭʼ������
	void device_draw_primitive(const vertex_t& vertex1, const vertex_t& vertex2, const vertex_t& vertex3);


	transform_t transform;      // ����任��
	int width_window;                  // ���ڿ��
	int height_window;                 // ���ڸ߶�
	unsigned int **framebuffer; // ���ػ��棺framebuffer[y] ����� y��
	float **zbuffer;            // ��Ȼ��棺zbuffer[y] Ϊ�� y��ָ��
	unsigned int** texture;		// ����ͬ����ÿ������
	int tex_width;              // ������
	int tex_height;             // ����߶�
	float max_u;                // ��������ȣ�tex_width - 1
	float max_v;                // �������߶ȣ�tex_height - 1
	int render_state;           // ��Ⱦ״̬
	unsigned int background;	//������ɫ
	unsigned int foreground;	//�߿���ɫ

};

