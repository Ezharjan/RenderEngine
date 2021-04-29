#include <math.h>
#include "Matrix.h"
#include "Math.h"


namespace RenderEngine {


	/************************************************************************/
	/* Matrix3f                                                             */
	/************************************************************************/

	Matrix3f Matrix3f::operator *(const Matrix3f& right) const {
		Matrix3f result;
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

	Vector3 Matrix3f::operator*(const Vector3 & right) const
	{
		Vector3 result(
			this->matrix[0][0] * right.getX() + this->matrix[1][0] * right.getY() + this->matrix[2][0] * right.getZ(),
			this->matrix[0][1] * right.getX() + this->matrix[1][1] * right.getY() + this->matrix[2][1] * right.getZ(),
			this->matrix[0][2] * right.getX() + this->matrix[1][2] * right.getY() + this->matrix[2][2] * right.getZ()
		);
		return result;
	}

	Matrix3f Matrix3f::getIdentityMatrix()
	{
		Matrix3f identityMatrix;

		identityMatrix.matrix[0][0] = 1.0f;
		identityMatrix.matrix[1][1] = 1.0f;
		identityMatrix.matrix[2][2] = 1.0f;

		return identityMatrix;
	}

	Matrix3f Matrix3f::getRotationMatrix(const Vector3 & axis, float theta)
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

	Matrix3f Matrix3f::getRotationMatrixFromQuaternion(const Vector4 & quaternion)
	{
		Matrix3f result;

		float n, s;
		float xs, ys, zs;
		float wx, wy, wz;
		float xx, xy, xz;
		float yy, yz, zz;

		n = powf(quaternion.getX(), 2) + powf(quaternion.getY(), 2) + powf(quaternion.getZ(), 2) + powf(quaternion.getW(), 2);
		s = (n > 0.f) ? (2.f / n) : 0.f;

		xs = quaternion.getX() * s;  ys = quaternion.getY() * s;  zs = quaternion.getZ() * s;
		wx = quaternion.getW() * xs; wy = quaternion.getW() * ys; wz = quaternion.getW() * zs;

		xx = quaternion.getX()* xs; xy = quaternion.getX()* ys; xz = quaternion.getX()* zs;
		yy = quaternion.getY() * ys; yz = quaternion.getY() * zs; zz = quaternion.getZ() * zs;

		result.matrix[0][0] = 1.0f - (yy + zz);
		result.matrix[0][1] = xy - wz;
		result.matrix[0][2] = xz + wy;
		result.matrix[1][0] = xy + wz;
		result.matrix[1][1] = 1.0f - (xx + zz);
		result.matrix[1][2] = yz - wx;
		result.matrix[2][0] = xz - wy;
		result.matrix[2][1] = yz + wx;
		result.matrix[2][2] = 1.0f - (xx + yy);

		return result;
	}





	/************************************************************************/
	/* Matrix4f                                                             */
	/************************************************************************/

	Matrix4f::Matrix4f(const Matrix3f& matrix3f)
	{
		Init();
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				this->matrix[i][j] = matrix3f.matrix[i][j];
			}
		}
		this->matrix[3][3] = 1.f;
	}

	bool Matrix4f::operator==(const Matrix4f & right) const
	{
		int counter = 0;
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				if (this->matrix[i][j] - right.matrix[i][j] == 0.f)
				{
					counter++;
				}
			}
		}
		if (counter != 16) return false;
		else return true;
	}

	template<typename T>
	bool Matrix4f::operator!=(const T& right) const
	{
		return ((*this == right) ? false : true);
	}

	bool Matrix4f::operator==(const float right) const
	{
		int counter = 0;
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				if (this->matrix[i][j] - right == 0.f)
				{
					counter++;
				}
			}
		}
		if (counter != 16) return false;
		else return true;
	}

	Matrix4f Matrix4f::operator=(const Matrix4f & right)
	{
		if (this->matrix == right.matrix)
		{
			return *this;
		}
		if (this->matrix != NULL)
		{
			ZeroMemory(&this->matrix, sizeof(this->matrix));
		}
		if (this != &right)
		{
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 4; j++)
				{
					this->matrix[i][j] = right.matrix[i][j];
				}
			}
		}
		return *this;
	}

	Matrix4f Matrix4f::operator*(const Matrix4f & right) const
	{
		Matrix4f result;
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

	Matrix4f Matrix4f::operator/(const float k) const
	{
		Matrix4f result;
		float oneOver = 1.f / k;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				result.matrix[i][j] = this->matrix[i][j] * oneOver;
		return result;
	}

	Vector4 Matrix4f::operator*(const Vector4 & right)
	{
		Vector4 result(
			this->matrix[0][0] * right.getX() + this->matrix[1][0] * right.getY() + this->matrix[2][0] * right.getZ() + this->matrix[3][0] * right.getW(),
			this->matrix[0][1] * right.getX() + this->matrix[1][1] * right.getY() + this->matrix[2][1] * right.getZ() + this->matrix[3][1] * right.getW(),
			this->matrix[0][2] * right.getX() + this->matrix[1][2] * right.getY() + this->matrix[2][2] * right.getZ() + this->matrix[3][2] * right.getW(),
			this->matrix[0][3] * right.getX() + this->matrix[1][3] * right.getY() + this->matrix[2][3] * right.getZ() + this->matrix[3][3] * right.getW()
		);
		return result;
	}

	Matrix4f Matrix4f::getTransposeMatrix(const Matrix4f & matrixToGetTranspose)
	{
		Matrix4f result;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				result.matrix[j][i] = matrixToGetTranspose.matrix[i][j];
			}
		}

		return result;
	}

	Matrix4f Matrix4f::getMatrixInvert(const Matrix4f & matrixToBeInverted)
	{
		Matrix4f invertTranspose = getInvertTranspose(matrixToBeInverted);
		return getTransposeMatrix(invertTranspose);
	}

	Matrix4f Matrix4f::getAdjugate(const Matrix4f matrixToGetAdjoint)
	{
		return Matrix4f();
	}

	// function from glm 
	Matrix4f Matrix4f::getInvertTranspose(const Matrix4f m4f)
	{
		float subfactor00 = m4f.matrix[2][2] * m4f.matrix[3][3] - m4f.matrix[3][2] * m4f.matrix[2][3];
		float subfactor01 = m4f.matrix[2][1] * m4f.matrix[3][3] - m4f.matrix[3][1] * m4f.matrix[2][3];
		float subfactor02 = m4f.matrix[2][1] * m4f.matrix[3][2] - m4f.matrix[3][1] * m4f.matrix[2][2];
		float subfactor03 = m4f.matrix[2][0] * m4f.matrix[3][3] - m4f.matrix[3][0] * m4f.matrix[2][3];
		float subfactor04 = m4f.matrix[2][0] * m4f.matrix[3][2] - m4f.matrix[3][0] * m4f.matrix[2][2];
		float subfactor05 = m4f.matrix[2][0] * m4f.matrix[3][1] - m4f.matrix[3][0] * m4f.matrix[2][1];
		float subfactor06 = m4f.matrix[1][2] * m4f.matrix[3][3] - m4f.matrix[3][2] * m4f.matrix[1][3];
		float subfactor07 = m4f.matrix[1][1] * m4f.matrix[3][3] - m4f.matrix[3][1] * m4f.matrix[1][3];
		float subfactor08 = m4f.matrix[1][1] * m4f.matrix[3][2] - m4f.matrix[3][1] * m4f.matrix[1][2];
		float subfactor09 = m4f.matrix[1][0] * m4f.matrix[3][3] - m4f.matrix[3][0] * m4f.matrix[1][3];
		float subfactor10 = m4f.matrix[1][0] * m4f.matrix[3][2] - m4f.matrix[3][0] * m4f.matrix[1][2];
		float subfactor11 = m4f.matrix[1][0] * m4f.matrix[3][1] - m4f.matrix[3][0] * m4f.matrix[1][1];
		float subfactor12 = m4f.matrix[1][2] * m4f.matrix[2][3] - m4f.matrix[2][2] * m4f.matrix[1][3];
		float subfactor13 = m4f.matrix[1][1] * m4f.matrix[2][3] - m4f.matrix[2][1] * m4f.matrix[1][3];
		float subfactor14 = m4f.matrix[1][1] * m4f.matrix[2][2] - m4f.matrix[2][1] * m4f.matrix[1][2];
		float subfactor15 = m4f.matrix[1][0] * m4f.matrix[2][3] - m4f.matrix[2][0] * m4f.matrix[1][3];
		float subfactor16 = m4f.matrix[1][0] * m4f.matrix[2][2] - m4f.matrix[2][0] * m4f.matrix[1][2];
		float subfactor17 = m4f.matrix[1][0] * m4f.matrix[2][1] - m4f.matrix[2][0] * m4f.matrix[1][1];

		Matrix4f inversed;

		inversed.matrix[0][0] = +(m4f.matrix[1][1] * subfactor00 - m4f.matrix[1][2] * subfactor01 + m4f.matrix[1][3] * subfactor02);
		inversed.matrix[0][1] = -(m4f.matrix[1][0] * subfactor00 - m4f.matrix[1][2] * subfactor03 + m4f.matrix[1][3] * subfactor04);
		inversed.matrix[0][2] = +(m4f.matrix[1][0] * subfactor01 - m4f.matrix[1][1] * subfactor03 + m4f.matrix[1][3] * subfactor05);
		inversed.matrix[0][3] = -(m4f.matrix[1][0] * subfactor02 - m4f.matrix[1][1] * subfactor04 + m4f.matrix[1][2] * subfactor05);

		inversed.matrix[1][0] = -(m4f.matrix[0][1] * subfactor00 - m4f.matrix[0][2] * subfactor01 + m4f.matrix[0][3] * subfactor02);
		inversed.matrix[1][1] = +(m4f.matrix[0][0] * subfactor00 - m4f.matrix[0][2] * subfactor03 + m4f.matrix[0][3] * subfactor04);
		inversed.matrix[1][2] = -(m4f.matrix[0][0] * subfactor01 - m4f.matrix[0][1] * subfactor03 + m4f.matrix[0][3] * subfactor05);
		inversed.matrix[1][3] = +(m4f.matrix[0][0] * subfactor02 - m4f.matrix[0][1] * subfactor04 + m4f.matrix[0][2] * subfactor05);

		inversed.matrix[2][0] = +(m4f.matrix[0][1] * subfactor06 - m4f.matrix[0][2] * subfactor07 + m4f.matrix[0][3] * subfactor08);
		inversed.matrix[2][1] = -(m4f.matrix[0][0] * subfactor06 - m4f.matrix[0][2] * subfactor09 + m4f.matrix[0][3] * subfactor10);
		inversed.matrix[2][2] = +(m4f.matrix[0][0] * subfactor07 - m4f.matrix[0][1] * subfactor09 + m4f.matrix[0][3] * subfactor11);
		inversed.matrix[2][3] = -(m4f.matrix[0][0] * subfactor08 - m4f.matrix[0][1] * subfactor10 + m4f.matrix[0][2] * subfactor11);

		inversed.matrix[3][0] = -(m4f.matrix[0][1] * subfactor12 - m4f.matrix[0][2] * subfactor13 + m4f.matrix[0][3] * subfactor14);
		inversed.matrix[3][1] = +(m4f.matrix[0][0] * subfactor12 - m4f.matrix[0][2] * subfactor15 + m4f.matrix[0][3] * subfactor16);
		inversed.matrix[3][2] = -(m4f.matrix[0][0] * subfactor13 - m4f.matrix[0][1] * subfactor15 + m4f.matrix[0][3] * subfactor17);
		inversed.matrix[3][3] = +(m4f.matrix[0][0] * subfactor14 - m4f.matrix[0][1] * subfactor16 + m4f.matrix[0][2] * subfactor17);

		float determinant =
			+m4f.matrix[0][0] * inversed.matrix[0][0]
			+ m4f.matrix[0][1] * inversed.matrix[0][1]
			+ m4f.matrix[0][2] * inversed.matrix[0][2]
			+ m4f.matrix[0][3] * inversed.matrix[0][3];

		inversed = inversed / determinant;

		return inversed;
	}

	Matrix4f Matrix4f::getIdentityMatrix()
	{
		Matrix4f identityMatrix;

		identityMatrix.matrix[0][0] = 1.0f;
		identityMatrix.matrix[1][1] = 1.0f;
		identityMatrix.matrix[2][2] = 1.0f;
		identityMatrix.matrix[3][3] = 1.0f;

		return identityMatrix;
	}

	Matrix4f Matrix4f::getTranslateMatrix(const float& x, const float& y, const float& z)
	{
		Matrix4f translationMatrix;

		translationMatrix.matrix[0][0] = 1.0f;
		translationMatrix.matrix[1][1] = 1.0f;
		translationMatrix.matrix[2][2] = 1.0f;
		translationMatrix.matrix[3][3] = 1.0f;

		translationMatrix.matrix[3][0] = x;
		translationMatrix.matrix[3][1] = y;
		translationMatrix.matrix[3][2] = z;

		return translationMatrix;
	}

	Matrix4f Matrix4f::getScaleMatrix(const float& x, const float& y, const float& z)
	{
		Matrix4f scaleMatrix;

		scaleMatrix.matrix[0][0] = x;
		scaleMatrix.matrix[1][1] = y;
		scaleMatrix.matrix[2][2] = z;
		scaleMatrix.matrix[3][3] = 1.0f;

		return  scaleMatrix;
	}

	Matrix4f Matrix4f::getScaleMatrix(const Matrix4f & matrixToScale, const float scaleFactor)
	{
		Matrix4f result;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
				result.matrix[i][j] = matrixToScale.matrix[i][j] * scaleFactor;
		}

		return result;
	}

	Matrix4f 	Matrix4f::getRotateXMatrix(const float& thetaX)
	{
		Matrix4f rotateXMatirx;

		float angle = RadToDegree(thetaX);

		rotateXMatirx.matrix[0][0] = 1.0f;
		rotateXMatirx.matrix[3][3] = 1.0f;

		float cosine = (float)cos(angle);
		float sine = (float)sin(angle);
		rotateXMatirx.matrix[1][1] = cosine;
		rotateXMatirx.matrix[2][2] = cosine;
		rotateXMatirx.matrix[1][2] = sine;
		rotateXMatirx.matrix[2][1] = -sine;

		return rotateXMatirx;
	}

	Matrix4f 	Matrix4f::getRotateYMatrix(const float& thetaY)
	{
		Matrix4f rotateYMatrix;

		float angle = RadToDegree(thetaY);

		rotateYMatrix.matrix[1][1] = 1.0f;
		rotateYMatrix.matrix[3][3] = 1.0f;

		float cosine = (float)cos(angle);
		float sine = (float)sin(angle);
		rotateYMatrix.matrix[0][0] = cosine;
		rotateYMatrix.matrix[2][2] = cosine;
		rotateYMatrix.matrix[0][2] = -sine;
		rotateYMatrix.matrix[2][0] = sine;

		return rotateYMatrix;
	}

	Matrix4f Matrix4f::getRotateZMatrix(const float& thetaZ)
	{
		Matrix4f rotateZMatrix;

		rotateZMatrix.matrix[2][2] = 1.0f;
		rotateZMatrix.matrix[3][3] = 1.0f;

		float angle = RadToDegree(thetaZ);

		float cosine = (float)cos(angle);
		float sine = (float)sin(angle);
		rotateZMatrix.matrix[0][0] = cosine;
		rotateZMatrix.matrix[1][1] = cosine;
		rotateZMatrix.matrix[0][1] = sine;
		rotateZMatrix.matrix[1][0] = -sine;

		return rotateZMatrix;
	}

	Matrix4f Matrix4f::getRotationMatrix(Vector3& axisToRotateAround, float theta)
	{
		theta = DegreeToRad(theta);

		float cosA = cos(theta);
		float sinA = sin(theta);

		// Axis must be a unit vector
		float checker = (powf(axisToRotateAround.getX(), 2) + powf(axisToRotateAround.getY(), 2) + powf(axisToRotateAround.getZ(), 2));
		if (checker != 1) axisToRotateAround.Normalize();


		float nx = axisToRotateAround.getX();
		float ny = axisToRotateAround.getY();
		float nz = axisToRotateAround.getZ();

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


		/************************************************************************/
		/* In another way which results in the same effect.						*/
		/************************************************************************/
		/*
		// STL uses rad for ratation unit
		theta = DegreeToRad(theta);

		// Axis must be a unit vector
		float checker = (powf(axisToRotateAround.getX(), 2) + powf(axisToRotateAround.getY(), 2) + powf(axisToRotateAround.getZ(), 2));
		if (checker != 1) if (checker != 1) VectorNormalize(axisToRotateAround);

		Matrix4f rotationMatrix;
		float qsin = (float)sin(theta / 2);
		float qcos = (float)cos(theta / 2);
		Vector4 vec(axisToRotateAround.getX(),axisToRotateAround.getY(),axisToRotateAround.getZ(),1.0f);
		float w = qcos;
		VectorNormalize(vec);
		float x = vec.x * qsin;
		float y = vec.y * qsin;
		float z = vec.z * qsin;
		ZeroMemory(&rotationMatrix.matrix, sizeof(rotationMatrix.matrix));
		rotationMatrix.matrix[0][0] = 1 - 2 * y * y - 2 * z * z;
		rotationMatrix.matrix[1][0] = 2 * x * y - 2 * w * z;
		rotationMatrix.matrix[2][0] = 2 * x * z + 2 * w * y;
		rotationMatrix.matrix[0][1] = 2 * x * y + 2 * w * z;
		rotationMatrix.matrix[1][1] = 1 - 2 * x * x - 2 * z * z;
		rotationMatrix.matrix[2][1] = 2 * y * z - 2 * w * x;
		rotationMatrix.matrix[0][2] = 2 * x * z - 2 * w * y;
		rotationMatrix.matrix[1][2] = 2 * y * z - 2 * w * x;
		rotationMatrix.matrix[2][2] = 1 - 2 * x * x - 2 * y * y;
		rotationMatrix.matrix[3][3] = 1.f;
		return rotationMatrix;
		*/
	}

	Matrix4f Matrix4f::getPerspectiveMatrix(float fovYOZ, float aspectRatio, float zNear, float zFar)
	{
		fovYOZ = DegreeToRad(fovYOZ);

		float focalLength = 1.f / tanf(fovYOZ / 2);

		Matrix4f perspectiveMatrix;

		perspectiveMatrix.matrix[0][0] = focalLength / aspectRatio;
		perspectiveMatrix.matrix[1][1] = focalLength;
		perspectiveMatrix.matrix[2][2] = zFar / (zFar - zNear); // in left hand coordinate
		perspectiveMatrix.matrix[3][2] = -(zFar * zNear) / (zFar - zNear);
		perspectiveMatrix.matrix[2][3] = 1.f; // in left hand coordinate

		return perspectiveMatrix;

	}

	Matrix4f Matrix4f::getOrthographicMatrix(const float conusLeft, const float conusRight, const float conusBottom, const float conusTop, float zNear, float zFar)
	{
		Matrix4f orthographicMatrix;

		/// make z range : [0,1], left hand coordinate
		orthographicMatrix.matrix[0][0] = 2.f / (conusRight - conusLeft);
		orthographicMatrix.matrix[1][1] = 2.f / (conusTop - conusBottom);
		orthographicMatrix.matrix[2][2] = 1.f / (zFar - zNear); // in left hand coordinate
		orthographicMatrix.matrix[3][0] = -(conusRight + conusLeft) / (conusRight - conusLeft);
		orthographicMatrix.matrix[3][1] = -(conusTop + conusBottom) / (conusTop - conusBottom);
		orthographicMatrix.matrix[3][2] = -zNear / (zFar - zNear);
		orthographicMatrix.matrix[3][3] = 1.f;


		return orthographicMatrix;
	}

	Matrix4f Matrix4f::getMatrixInvertForOrthogonal(const Matrix4f & orthographicMatrix)
	{
		return getTransposeMatrix(orthographicMatrix);
	}

	Matrix4f Matrix4f::getLookAtMatrixForLeftHandCoordinate(Vector3 * eyePos, const Vector3 * lookAt, const Vector3 * up, Vector3 & newCameraUpAxis)
	{
		Vector3 zAxis = *lookAt - *eyePos;
		zAxis.Normalize();
		Vector3 xAxis = *up ^ zAxis;
		xAxis.Normalize();
		Vector3 yAxis = zAxis ^ xAxis;
		newCameraUpAxis = yAxis; // store the last upaxis for updating camera position

		Matrix4f result;

		/// when(major==raw) => do: below
		result.matrix[0][0] = xAxis.getX();
		result.matrix[0][1] = yAxis.getX();
		result.matrix[0][2] = zAxis.getX();
		result.matrix[0][3] = 0;
		result.matrix[1][0] = xAxis.getY();
		result.matrix[1][1] = yAxis.getY();
		result.matrix[1][2] = zAxis.getY();
		result.matrix[1][3] = 0;
		result.matrix[2][0] = xAxis.getZ();
		result.matrix[2][1] = yAxis.getZ();
		result.matrix[2][2] = zAxis.getZ();
		result.matrix[2][3] = 0;
		result.matrix[3][0] = -(xAxis * (*eyePos));
		result.matrix[3][1] = -(yAxis * (*eyePos));
		result.matrix[3][2] = -(zAxis * (*eyePos));
		result.matrix[3][3] = 1;

		/*
		Matrix-Structure(
				xaxis.x,			yaxis.x,			zaxis.x,			0,
				xaxis.y,			yaxis.y,			zaxis.y,			0,
				xaxis.z,			yaxis.z,			zaxis.z,			0,
				-xaxis.Dot(eyePos), -yaxis.Dot(eyePos), -zaxis.Dot(eyePos), 1
		);*/
		return result;
	}

	Matrix4f Matrix4f::getLookAtMatrixForLeftHandCoordinate(Vector3& eyePos, const Vector3& lookAt, const Vector3& up)
	{
		Vector3 xAxis, yAxis, zAxis;

		zAxis = lookAt - eyePos;
		zAxis.Normalize();

		xAxis = up ^ zAxis;
		xAxis.Normalize();

		yAxis = zAxis ^ xAxis;


		Matrix4f result;

		result.matrix[0][0] = xAxis.getX();
		result.matrix[1][0] = xAxis.getY();
		result.matrix[2][0] = xAxis.getZ();
		result.matrix[3][0] = -(xAxis * eyePos);

		result.matrix[0][1] = yAxis.getX();
		result.matrix[1][1] = yAxis.getY();
		result.matrix[2][1] = yAxis.getZ();
		result.matrix[3][1] = -(yAxis * eyePos);

		result.matrix[0][2] = zAxis.getX();
		result.matrix[1][2] = zAxis.getY();
		result.matrix[2][2] = zAxis.getZ();
		result.matrix[3][2] = -(zAxis* eyePos);

		result.matrix[3][3] = 1.f;

		return result;
	}

	Matrix4f Matrix4f::MatrixViewPort(const float & screenWidth, const float & screenHeight, const float & maxDepth, const float & minDepth, const float & topLeftX, const float & topLeftY)
	{
		Matrix4f viewportMatrix;

		viewportMatrix.matrix[0][0] = screenWidth / 2.0f;
		viewportMatrix.matrix[1][1] = -screenHeight / 2.0f;
		viewportMatrix.matrix[2][2] = maxDepth - minDepth;
		viewportMatrix.matrix[3][0] = topLeftX + screenWidth / 2.0f;
		viewportMatrix.matrix[3][1] = topLeftY + screenHeight / 2.0f;
		viewportMatrix.matrix[3][2] = minDepth;
		viewportMatrix.matrix[3][3] = 1.0f;

		return viewportMatrix;
	}

	void Matrix4f::ApplyTransformToRelevantMatrix(Vector4 & point, const Matrix4f & relevantMatrix)
	{
		// store the point first
		float x = point.getX();
		float y = point.getY();
		float z = point.getZ();
		float w = point.getW();
		point.setX(x * relevantMatrix.matrix[0][0] + y * relevantMatrix.matrix[1][0] + z * relevantMatrix.matrix[2][0] + w * relevantMatrix.matrix[3][0]);
		point.setY(x * relevantMatrix.matrix[0][1] + y * relevantMatrix.matrix[1][1] + z * relevantMatrix.matrix[2][1] + w * relevantMatrix.matrix[3][1]);
		point.setZ(x * relevantMatrix.matrix[0][2] + y * relevantMatrix.matrix[1][2] + z * relevantMatrix.matrix[2][2] + w * relevantMatrix.matrix[3][2]);
		point.setW(x * relevantMatrix.matrix[0][3] + y * relevantMatrix.matrix[1][3] + z * relevantMatrix.matrix[2][3] + w * relevantMatrix.matrix[3][3]);
	}
}
