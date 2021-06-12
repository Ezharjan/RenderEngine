#pragma once
#include "transform_t.h"
#include "scanline_t.h"
#define RENDER_STATE_WIREFRAME      1		// 渲染线框
#define RENDER_STATE_TEXTURE        2		// 渲染纹理
#define RENDER_STATE_COLOR          4		// 渲染颜色
class device_t
{
public:
	device_t();
	~device_t();

	//设备初始化，fb为外部帧缓存，非null将引用外部帧缓存（每行4字节对齐）
	void device_init(int width, int height, void* fb);
	// 删除设备
	void device_destroy();
	// 设置当前纹理,pitch:每条扫描线的字节数，它等于 (以像素表示的图像宽度 * 每像素字节数) + 内存填充。
	void device_set_texture(void* bits, long pitch, int w, int h);
	//清空framebuffer 和 zbuffer
	void device_clear(int mode);
	//画点
	void device_draw_pixel(int x, int y, unsigned int color);
	//画线
	void device_draw_line(int x1, int y1, int x2, int y2, unsigned int color);
	//根据坐标读取纹理
	unsigned int device_texture_read(float u,float v);
	//绘制扫描线
	void device_draw_scanline(scanline_t& scanline);
	//渲染梯形trap。主渲染函数
	void device_render_trap(trapezoid_t &trap);
	// 根据 render_state 绘制原始三角形
	void device_draw_primitive(const vertex_t& vertex1, const vertex_t& vertex2, const vertex_t& vertex3);


	transform_t transform;      // 坐标变换器
	int width_window;                  // 窗口宽度
	int height_window;                 // 窗口高度
	unsigned int **framebuffer; // 像素缓存：framebuffer[y] 代表第 y行
	float **zbuffer;            // 深度缓存：zbuffer[y] 为第 y行指针
	unsigned int** texture;		// 纹理：同样是每行索引
	int tex_width;              // 纹理宽度
	int tex_height;             // 纹理高度
	float max_u;                // 纹理最大宽度：tex_width - 1
	float max_v;                // 纹理最大高度：tex_height - 1
	int render_state;           // 渲染状态
	unsigned int background;	//背景颜色
	unsigned int foreground;	//线框颜色

};

