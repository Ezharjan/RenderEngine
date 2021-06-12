#include "logic.h"
#include "device_t.h"
#include "matrix_t.h"
#include "mini3dmath.h"




logic::~logic()
{
}

void logic::draw_plane(device_t& device, int a, int b, int c, int d)
{
	vertex_t p1 = mesh[a], p2 = mesh[b], p3 = mesh[c], p4 = mesh[d];
	p1.texturecoordinate.u = 0, p1.texturecoordinate.v = 0, p2.texturecoordinate.u = 0, p2.texturecoordinate.v = 1;
	p3.texturecoordinate.u = 1, p3.texturecoordinate.v = 1, p4.texturecoordinate.u = 1, p4.texturecoordinate.v = 0;

	device.device_draw_primitive(p1, p2, p3);
	device.device_draw_primitive(p3, p4, p1);
}

void logic::draw_box(device_t& device, float theta)
{
	matrix_t m;
	mini3dmath::setRotateMatrix(m, -1, 0.5, 1, theta);//设置旋转矩阵
	device.transform.world = m;
	device.transform.transform_update();
		
	draw_plane(device, 0, 1, 2, 3);//逆时针绘制面
	draw_plane(device, 4, 5, 6, 7);
	draw_plane(device, 0, 4, 5, 1);
	draw_plane(device, 1, 5, 6, 2);
	draw_plane(device, 2, 6, 7, 3);
	draw_plane(device, 3, 7, 4, 0);
}

void logic::camera_at_zero(device_t& device, float x, float y, float z)
{
	vector_t eye = { x, y, z, 1 }, at = { 0, 0, 0, 1 }, up = { 0, 0, 1, 1 };
	mini3dmath::setLookatMatrix(device.transform.view, eye, at, up);
	device.transform.transform_update();
}

void logic::init_texture(device_t& device)
{
	static unsigned int texture[256][256];
	int i, j;
	for (j = 0; j < 256; j++) {
		for (i = 0; i < 256; i++) {
			int x = i / 32, y = j / 32;//一个色块的大小为32*32
			texture[j][i] = ((x + y) & 1) ? 0xffffff : 0x3fbcef;//0xffffff代表白色，0x3fbcef代表蓝色。给纹理赋值，蓝白相间。
		}
	}
	device.device_set_texture(texture, 256 * 4, 256, 256);
}


