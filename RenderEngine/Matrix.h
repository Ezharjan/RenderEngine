#pragma once

#include <windows.h>
#include "Vector.h"
#include<iostream>

namespace RenderEngine {

	float RadToDegree(const float& rad);
	float DegreeToRad(const float& degree);


	template<int nrows, int ncols> struct mat;


	class Matrix3f
	{
	public:
		Matrix3f() { Init(); };
		~Matrix3f() {};
		void Init() { ZeroMemory(&this->matrix, sizeof(this->matrix)); }
		Matrix3f operator *(const Matrix3f& right) const;
		Vector3 operator *(const Vector3& right) const;
		// ��λ����
		static Matrix3f getIdentityMatrix();
		// ����������ת�ľ���˳ʱ��
		static Matrix3f getRotationMatrix(const Vector3& axis, float theta);
		// ������Ԫ����ȡ��ת����
		static Matrix3f getRotationMatrixFromQuaternion(const Vector4& quaternion);

		float matrix[3][3];
	};



	class Matrix4f
	{
	public:
		Matrix4f() { Init(); }
		Matrix4f(const Matrix3f& matrix3f);
		~Matrix4f() {};
		void Init() { ZeroMemory(&this->matrix, sizeof(this->matrix)); }
		bool operator ==(const Matrix4f& right) const;
		bool operator ==(const float right) const;
		template<typename T>
		bool operator !=(const T& right) const;
		Matrix4f operator =(const Matrix4f& right);
		Matrix4f operator *(const Matrix4f& right) const;
		Matrix4f operator /(const float right) const;
		Vector4 operator *(const Vector4& right);
		// ��ȡת�þ���
		static Matrix4f getTransposeMatrix(const Matrix4f& matrixToGetTranspose);
		// ��ȡ�����ʹ�ð������ķ�����
		static Matrix4f getMatrixInvert(const Matrix4f& matrixToBeInverted);
		// ��ȡ�������
		static Matrix4f getAdjugate(const Matrix4f matrixToGetAdjoint);
		// ��ȡ������ת��
		static Matrix4f getInvertTranspose(const Matrix4f m4f);
		// ��λ����
		static Matrix4f getIdentityMatrix();
		// ƽ�Ʊ任����
		static 	Matrix4f getTranslateMatrix(const float& x, const float& y, const float& z);
		// ���ű任����
		static Matrix4f getScaleMatrix(const float& x, const float& y, const float& z);
		static Matrix4f getScaleMatrix(const Matrix4f& matrixToScale, const float scaleFactor);
		// ��X�����ת�����β�Ϊ�Ƕȣ�˳ʱ��
		static Matrix4f getRotateXMatrix(const float& thetaX);
		// ��Y�����ת�����β�Ϊ�Ƕȣ�˳ʱ��
		static Matrix4f getRotateYMatrix(const float& thetaY);
		// ��Z�����ת����,�β�Ϊ�Ƕȣ�˳ʱ��
		static Matrix4f getRotateZMatrix(const float& thetaZ);
		// ����������ת�ľ���˳ʱ�룻��ΪҪ�����ת���ǵ�λ������������ǵ�λ������Ҫ��һ��
		static Matrix4f getRotationMatrix(Vector3& axisToRotateAround, float theta);
		// ��ȡ͸��ͶӰ����
		static Matrix4f getPerspectiveMatrix(float fovYOZ, float aspectRatio, float zNear, float zFar);
		// ��ȡ����ͶӰ����(Left-hand Coordinate)
		static Matrix4f getOrthographicMatrix(const float left, const float right, const float top, const float bottom, float zNear, float zFar);
		// ��ȡ����ͶӰ���������� ---> ��ȡ��ת�þ��󼴿�
		static Matrix4f getMatrixInvertForOrthogonal(const Matrix4f& orthographicMatrix);
		// ��������ȡһ����������ϵ������任(�ӽ�)����
		static Matrix4f getLookAtMatrixForLeftHandCoordinate(Vector3* eyePos, const Vector3* lookAt, const Vector3* up, Vector3& newCameraUpAxis);
		static Matrix4f getLookAtMatrixForLeftHandCoordinate(Vector3& eyePos, const Vector3& lookAt, const Vector3& up);
		// ��ȡ�ӿھ����ӿڱ任�����TopLeftX=0,TopLeftY=0,MaxDepth=1.0,MinDepth=0.0
		static Matrix4f  MatrixViewPort(const float& screenWidth, const float& screenHeight, const float& maxDepth, const float& minDepth, const float& topLeftX, const float& topLeftY);
		// �ڵ���ִ�о���任
		void ApplyTransformToRelevantMatrix(Vector4& point, const Matrix4f& transformMatrix);


		float matrix[4][4];
	};

	inline std::ostream& operator <<(std::ostream& os, const Matrix4f m4f) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				os << m4f.matrix[i][j] << " \t";
			}
			os << std::endl;
		}
		return os;
	}
	inline std::ostream& operator <<(std::ostream& os, const Matrix3f m3f) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				os << m3f.matrix[i][j] << " \t";
			}
			os << std::endl;
		}
		return os;
	}
}