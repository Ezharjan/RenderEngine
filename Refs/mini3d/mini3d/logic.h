#pragma once
#include "vertex_t.h"
#include <vector>
class device_t;

//�߼������Ⱦ����
class logic
{
public:
	logic()
	{
		//�������8������
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

	//����
	void draw_plane(device_t& device, int a, int b, int c, int d);
	//��������
	void draw_box(device_t& device, float theta);
	//��ʼ�����
	void camera_at_zero(device_t& device, float x, float y, float z);
	//��ʼ��������������ֵ��device
	void init_texture(device_t& device);

	//������İ˸�������Ϣ
	std::vector<vertex_t> mesh;
};

