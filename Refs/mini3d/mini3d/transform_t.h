#pragma once
#include "matrix_t.h"
#include "vector_t.h"
#include "point_t.h"

//����任
class transform_t
{
public:
	transform_t();
	~transform_t();

	// mvp������£����� transform = world * view * projection
	void transform_update();
	// mvp�����ʼ����������Ļ����
	void transform_init(const int width, const int height);
	//mvp����任
	vector_t transform_apply(const vector_t& point_untransformed)const;
	// ��һ�����õ���Ļ���ꡣ������͸�ӳ������䵽0-1֮�䡢���ݳ��������Ļӳ�䣩
	vector_t tranform_homogenize(const vector_t& x)const;


	matrix_t world;			//��������任
	matrix_t view;			//��ͼ����任
	matrix_t projection;	//ͶӰ����任
	matrix_t transform;		//transform = world * view * projection
	float width, height;	//��Ļ��С

};

