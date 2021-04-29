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
		// 单位矩阵
		static Matrix3f getIdentityMatrix();
		// 绕任意轴旋转的矩阵，顺时针
		static Matrix3f getRotationMatrix(const Vector3& axis, float theta);
		// 根据四元数获取旋转矩阵
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
		bool operator !=(const T& right) const ;
		Matrix4f operator =(const Matrix4f& right);
		Matrix4f operator *(const Matrix4f& right) const;
		Matrix4f operator /(const float right) const;
		Vector4 operator *(const Vector4& right);
		// 获取转置矩阵
		static Matrix4f getTransposeMatrix(const Matrix4f& matrixToGetTranspose);
		// 获取逆矩阵（使用伴随矩阵的方法）
		static Matrix4f getMatrixInvert(const Matrix4f& matrixToBeInverted);
		// 获取伴随矩阵
		static Matrix4f getAdjugate(const Matrix4f matrixToGetAdjoint);
		// 获取逆矩阵的转置
		static Matrix4f getInvertTranspose(const Matrix4f m4f);
		// 单位矩阵
		static Matrix4f getIdentityMatrix();
		// 平移变换矩阵
		static 	Matrix4f getTranslateMatrix(const float& x, const float& y, const float& z);
		// 缩放变换矩阵
		static Matrix4f getScaleMatrix(const float& x, const float& y, const float& z);
		static Matrix4f getScaleMatrix(const Matrix4f& matrixToScale, const float scaleFactor);
		// 绕X轴的旋转矩阵，形参为角度，顺时针
		static Matrix4f getRotateXMatrix(const float& thetaX);
		// 绕Y轴的旋转矩阵，形参为角度，顺时针
		static Matrix4f getRotateYMatrix(const float& thetaY);
		// 绕Z轴的旋转矩阵,形参为角度，顺时针
		static Matrix4f getRotateZMatrix(const float& thetaZ);
		// 绕任意轴旋转的矩阵，顺时针；因为要检测旋转轴是单位特征，如果不是单位向量则要归一化
		static Matrix4f getRotationMatrix(Vector3& axisToRotateAround, float theta);
		// 获取透视投影矩阵
		static Matrix4f getPerspectiveMatrix(float fovYOZ, float aspectRatio, float zNear, float zFar);
		// 获取正交投影矩阵(Left-hand Coordinate)
		static Matrix4f getOrthographicMatrix(const float left, const float right, const float top, const float bottom, float zNear, float zFar);
		// 获取正交投影矩阵的逆矩阵 ---> 获取其转置矩阵即可
		static Matrix4f getMatrixInvertForOrthogonal(const Matrix4f& orthographicMatrix);
		// 建立并获取一个左手坐标系的相机变换(视角)矩阵
		static Matrix4f getLookAtMatrixForLeftHandCoordinate(Vector3* eyePos, const Vector3* lookAt, const Vector3* up, Vector3& newCameraUpAxis);
		static Matrix4f getLookAtMatrixForLeftHandCoordinate(Vector3& eyePos, const Vector3& lookAt, const Vector3& up);
		// 获取视口矩阵，视口变换矩阵的TopLeftX=0,TopLeftY=0,MaxDepth=1.0,MinDepth=0.0
		static Matrix4f  MatrixViewPort(const float& screenWidth, const float& screenHeight, const float& maxDepth, const float& minDepth, const float& topLeftX, const float& topLeftY);
		// 在点上执行矩阵变换
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