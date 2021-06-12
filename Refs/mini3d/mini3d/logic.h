#pragma once
#include "vertex_t.h"
#include <vector>
class device_t;

//逻辑层的渲染代码
class logic
{
public:
	logic()
	{
		//六面体的8个顶点
		vertex_t v1(1, -1, 1, 1, 0, 0, 1.0f, 0.2f, 0.2f, 1);
		vertex_t v2(-1, -1, 1, 1, 0, 1, 0.2f, 1.0f, 0.2f, 1);
		vertex_t v3(-1, 1, 1, 1, 1, 1, 0.2f, 0.2f, 1.0f, 1);
		vertex_t v4(1, 1, 1, 1, 1, 0, 1.0f, 0.2f, 1.0f, 1);
		vertex_t v5(1, -1, -1, 1, 0, 0, 1.0f, 1.0f, 0.2f, 1);
		vertex_t v6(-1, -1, -1, 1, 0, 1, 0.2f, 1.0f, 1.0f, 1);
		vertex_t v7(-1, 1, -1, 1, 1, 1, 1.0f, 0.3f, 0.3f, 1);
		vertex_t v8(1, 1, -1, 1, 1, 0, 0.2f, 1.0f, 0.3f, 1);

		mesh.push_back(v1);
		mesh.push_back(v2);
		mesh.push_back(v3);
		mesh.push_back(v4);
		mesh.push_back(v5);
		mesh.push_back(v6);
		mesh.push_back(v7);
		mesh.push_back(v8);
	};
	~logic();

	//画面
	void draw_plane(device_t& device, int a, int b, int c, int d);
	//画六面体
	void draw_box(device_t& device, float theta);
	//初始化相机
	void camera_at_zero(device_t& device, float x, float y, float z);
	//初始化纹理，并将纹理赋值到device
	void init_texture(device_t& device);

	//六面体的八个顶点信息
	std::vector<vertex_t> mesh;
};

