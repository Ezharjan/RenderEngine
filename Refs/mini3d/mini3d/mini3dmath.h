#pragma once

#include"vector_t.h"

static class mini3dmath
{
public:
	mini3dmath();
	~mini3dmath();

	//��֤x��ֵ��min��max֮�䣬С��min�����min������max�����max���м�ֵ�����x
	static int CMID(int x, int min, int max);
	// �����ֵ��t Ϊ [0, 1] ֮�����ֵ
	static float interp(float x1, float x2, float t);
	//��������Ϊƽ�ƾ���
	static void setTranslateMatrix(matrix_t& matrix, const float x, const float y, const  float z);
	//�������ž���
	static void setScaleMatrix(matrix_t& matrix, float x, float y, float z);
	//������ת����
	static void setRotateMatrix(matrix_t& matrix, float x, float y, float z, float theta);
	//������ͼ����
	static void setLookatMatrix(matrix_t& matrix, const vector_t& eye, const vector_t& at, const vector_t& up);
	//����͸��ͶӰ���� D3DXMatrixPerspectiveFovLH��D3DͶӰ���󣬼����ͼ��ѧP43
	static void setPerspectiveMatrix(matrix_t& matrix, float fovy, float aspect, float znear, float zfar);
	// ����������ͬ cvv �ı߽�������׶�ü�
	static int check_cvv(const vector_t& v);
};

