#pragma once
#include "Vector.h"

#ifndef __Matrix__
#define __Matrix__



namespace RenderEngine {

#define MATH_PI 3.141592657519
#define MATH_Epsilon 1.0e-5

	float RadToDegree(const float& rad) {
		return (rad * 180.f / MATH_PI);
	}

	float DegreeToRad(const float& degree) {
		return (degree * MATH_PI / 180.f);
	}


	struct Matrix4f
	{
		float matrix[4][4];
		Matrix4f operator *(const Matrix4f& right) const {
			Matrix4f result;
			ZeroMemory(&result, sizeof(result));
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result.matrix[i][j] = this->matrix[i][0] * right.matrix[0][j]
						+ this->matrix[i][1] * right.matrix[1][j]
						+ this->matrix[i][2] * right.matrix[2][j]
						+ this->matrix[i][3] * right.matrix[3][j];
				}
			}
			return result;
		}
		// 矩阵乘向量，矩阵的行数要等于向量的列数，M4X4与V4刚好都满足，结果是向量
		Vector4 operator *(const Vector4& right)
		{
			Vector4 result(
				this->matrix[0][0] * right.getX() + this->matrix[1][0] * right.getY() + this->matrix[2][0] * right.getZ() + this->matrix[3][0] * right.getW(),
				this->matrix[0][1] * right.getX() + this->matrix[1][1] * right.getY() + this->matrix[2][1] * right.getZ() + this->matrix[3][1] * right.getW(),
				this->matrix[0][2] * right.getX() + this->matrix[1][2] * right.getY() + this->matrix[2][2] * right.getZ() + this->matrix[3][2] * right.getW(),
				this->matrix[0][3] * right.getX() + this->matrix[1][3] * right.getY() + this->matrix[2][3] * right.getZ() + this->matrix[3][3] * right.getW()
			);
			return result;
		}


		// 构造一个单位矩阵
		static Matrix4f getIdentityMatrix()
		{
			Matrix4f identityMatrix;

			// 清除为0
			ZeroMemory(&identityMatrix, sizeof(identityMatrix));

			identityMatrix.matrix[0][0] = 1.0f;
			identityMatrix.matrix[1][1] = 1.0f;
			identityMatrix.matrix[2][2] = 1.0f;
			identityMatrix.matrix[3][3] = 1.0f;

			return identityMatrix;
		}

		// 构造一个平移变换矩阵
		static 	Matrix4f getTranslateMatrix(const float& x, const float& y, const float& z)
		{
			Matrix4f TranslateMatrix;

			// 清除为0
			ZeroMemory(&TranslateMatrix, sizeof(TranslateMatrix));

			TranslateMatrix.matrix[0][0] = 1.0f;
			TranslateMatrix.matrix[1][1] = 1.0f;
			TranslateMatrix.matrix[2][2] = 1.0f;
			TranslateMatrix.matrix[3][3] = 1.0f;

			TranslateMatrix.matrix[3][0] = x;
			TranslateMatrix.matrix[3][1] = y;
			TranslateMatrix.matrix[3][2] = z;

			return TranslateMatrix;
		}

		// 构造一个缩放变换矩阵
		static Matrix4f getScaleMatrix(const float& x, const float& y, const float& z)
		{
			Matrix4f ScaleMatrix;

			// 清除为0
			ZeroMemory(&ScaleMatrix, sizeof(ScaleMatrix));

			ScaleMatrix.matrix[0][0] = x;
			ScaleMatrix.matrix[1][1] = y;
			ScaleMatrix.matrix[2][2] = z;
			ScaleMatrix.matrix[3][3] = 1.0f;

			return  ScaleMatrix;
		}

		// 构造一个绕X轴的旋转矩阵，形参为角度，顺时针
		static Matrix4f getRotateXMatrix(const float& thetaX)
		{
			Matrix4f RotateXMatirx;

			// 清除为0
			ZeroMemory(&RotateXMatirx, sizeof(RotateXMatirx));

			float angle = RadToDegree(thetaX);

			RotateXMatirx.matrix[0][0] = 1.0f;
			RotateXMatirx.matrix[3][3] = 1.0f;

			float cosine = (float)cos(angle);
			float sine = (float)sin(angle);
			RotateXMatirx.matrix[1][1] = cosine;
			RotateXMatirx.matrix[2][2] = cosine;
			RotateXMatirx.matrix[1][2] = sine;
			RotateXMatirx.matrix[2][1] = -sine;

			return RotateXMatirx;
		}


		// 构造一个绕Y轴的旋转矩阵，形参为角度，顺时针
		static Matrix4f getRotateYMatrix(const float& thetaY)
		{
			Matrix4f RotateYMatrix;
			// 清除为0
			ZeroMemory(&RotateYMatrix, sizeof(RotateYMatrix));

			float angle = RadToDegree(thetaY);

			RotateYMatrix.matrix[1][1] = 1.0f;
			RotateYMatrix.matrix[3][3] = 1.0f;

			float cosine = (float)cos(angle);
			float sine = (float)sin(angle);
			RotateYMatrix.matrix[0][0] = cosine;
			RotateYMatrix.matrix[2][2] = cosine;
			RotateYMatrix.matrix[0][2] = -sine;
			RotateYMatrix.matrix[2][0] = sine;

			return RotateYMatrix;
		}


		// 构造一个绕Z轴的旋转矩阵,形参为角度，顺时针
		static Matrix4f getRotateZMatrix(const float& thetaZ)
		{
			Matrix4f RotateZMatrix;

			// 清除为0
			ZeroMemory(&RotateZMatrix, sizeof(RotateZMatrix));

			RotateZMatrix.matrix[2][2] = 1.0f;
			RotateZMatrix.matrix[3][3] = 1.0f;

			float angle = RadToDegree(thetaZ);

			float cosine = (float)cos(angle);
			float sine = (float)sin(angle);
			RotateZMatrix.matrix[0][0] = cosine;
			RotateZMatrix.matrix[1][1] = cosine;
			RotateZMatrix.matrix[0][1] = sine;
			RotateZMatrix.matrix[1][0] = -sine;

			return RotateZMatrix;
		}


		Matrix4f getRotationMatrix(const Vector4& axis, float theta)
		{
			theta = RadToDegree(theta);

			float cosA = cos(theta);
			float sinA = sin(theta);

			// Axis must be a unit vector
			if (axis.getW() == 0 || (pow(axis.getX(), 2) + pow(axis.getY(), 2) + pow(axis.getZ(), 2)) == 1)
			{
				return Matrix4f::getIdentityMatrix();
			}


			float nx = axis.getX();
			float ny = axis.getY();
			float nz = axis.getZ();

			float _cosA = 1 - cosA;
			float nx_cosA = nx * _cosA;
			float nz_cosA = nz * _cosA;
			float nxnx_cosA = nx * nx_cosA;
			float nxny_cosA = ny * nx_cosA;
			float nxnz_cosA = nz * nx_cosA;
			float nyny_cosA = ny * ny*_cosA;
			float nynz_cosA = ny * nz_cosA;
			float nznz_cosA = nz * nz_cosA;
			float nxsinA = nx * sinA;
			float nzsinA = nz * sinA;
			float nysinA = ny * sinA;

			Matrix4f rotationMatrx;
			rotationMatrx.matrix[0][0] = nxnx_cosA + cosA;
			rotationMatrx.matrix[0][1] = nxny_cosA + nzsinA;
			rotationMatrx.matrix[0][2] = nxnz_cosA - nysinA;
			rotationMatrx.matrix[0][3] = 0;
			rotationMatrx.matrix[1][0] = nxny_cosA - nzsinA;
			rotationMatrx.matrix[1][1] = nyny_cosA + cosA;
			rotationMatrx.matrix[1][2] = nynz_cosA + nxsinA;
			rotationMatrx.matrix[1][3] = 0;
			rotationMatrx.matrix[2][0] = nxnz_cosA + nysinA;
			rotationMatrx.matrix[2][1] = nynz_cosA - nxsinA;
			rotationMatrx.matrix[2][2] = nznz_cosA + cosA;
			rotationMatrx.matrix[2][3] = 0;
			rotationMatrx.matrix[3][0] = 0;
			rotationMatrx.matrix[3][1] = 0;
			rotationMatrx.matrix[3][2] = 0;
			rotationMatrx.matrix[3][3] = 1;

			return rotationMatrx;
		}
	};

	struct Matrix3f
	{
		float matrix[3][3];
		Matrix3f operator *(const Matrix3f& right) const {
			Matrix3f result;
			ZeroMemory(&result, sizeof(result));
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					result.matrix[i][j] = this->matrix[i][0] * right.matrix[0][j]
						+ this->matrix[i][1] * right.matrix[1][j]
						+ this->matrix[i][2] * right.matrix[2][j];
				}
			}
			return result;
		}
		// 矩阵乘向量，矩阵的行数要等于向量的列数，M4X4与V4刚好都满足，结果是向量
		Vector3 operator *(const Vector3& right)
		{
			Vector3 result(
				this->matrix[0][0] * right.getX() + this->matrix[1][0] * right.getY() + this->matrix[2][0] * right.getZ(),
				this->matrix[0][1] * right.getX() + this->matrix[1][1] * right.getY() + this->matrix[2][1] * right.getZ(),
				this->matrix[0][2] * right.getX() + this->matrix[1][2] * right.getY() + this->matrix[2][2] * right.getZ()
			);
			return result;
		}


		// 构造一个单位矩阵
		static Matrix3f getIdentityMatrix()
		{
			Matrix3f identityMatrix;

			// 清除为0
			ZeroMemory(&identityMatrix, sizeof(identityMatrix));

			identityMatrix.matrix[0][0] = 1.0f;
			identityMatrix.matrix[1][1] = 1.0f;
			identityMatrix.matrix[2][2] = 1.0f;

			return identityMatrix;
		}

		Matrix3f getRotationMatrix(const Vector3& axis, float theta)
		{
			theta = RadToDegree(theta);

			float cosA = cos(theta);
			float sinA = sin(theta);

			// Axis must be a unit vector
			if ((pow(axis.getX(), 2) + pow(axis.getY(), 2) + pow(axis.getZ(), 2)) == 1)
			{
				return Matrix3f::getIdentityMatrix();
			}


			float nx = axis.getX();
			float ny = axis.getY();
			float nz = axis.getZ();

			float _cosA = 1 - cosA;
			float nx_cosA = nx * _cosA;
			float nz_cosA = nz * _cosA;
			float nxnx_cosA = nx * nx_cosA;
			float nxny_cosA = ny * nx_cosA;
			float nxnz_cosA = nz * nx_cosA;
			float nyny_cosA = ny * ny*_cosA;
			float nynz_cosA = ny * nz_cosA;
			float nznz_cosA = nz * nz_cosA;
			float nxsinA = nx * sinA;
			float nzsinA = nz * sinA;
			float nysinA = ny * sinA;

			Matrix3f rotationMatrx;
			rotationMatrx.matrix[0][0] = nxnx_cosA + cosA;
			rotationMatrx.matrix[0][1] = nxny_cosA + nzsinA;
			rotationMatrx.matrix[0][2] = nxnz_cosA - nysinA;
			rotationMatrx.matrix[1][0] = nxny_cosA - nzsinA;
			rotationMatrx.matrix[1][1] = nyny_cosA + cosA;
			rotationMatrx.matrix[1][2] = nynz_cosA + nxsinA;
			rotationMatrx.matrix[2][0] = nxnz_cosA + nysinA;
			rotationMatrx.matrix[2][1] = nynz_cosA - nxsinA;
			rotationMatrx.matrix[2][2] = nznz_cosA + cosA;
			return rotationMatrx;
		}
	};


	Matrix4f Matrix3fToMatrix4f(const Matrix3f& matrix3x3)
	{
		Matrix4f result;
		ZeroMemory(&result, sizeof(result));
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				result.matrix[i][j] = matrix3x3.matrix[i][j];
			}
		}
		return result;
	}

}

#endif // !__Matrix__
