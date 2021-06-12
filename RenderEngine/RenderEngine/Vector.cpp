#include <math.h>
#include "Vector.h"
#include "Math.h"

namespace RenderEngine {

	/************************************************************************/
	/* Vector2                                                              */
	/************************************************************************/

	void Vector2::operator=(const Vector2 & right)
	{
		this->setX(right.getX());
		this->setY(right.getY());
	}

	Vector2 Vector2::operator+(const Vector2& right) const
	{
		Vector2 result(this->getX() + right.getX(), this->getY() + right.getY());
		return result;
	}

	Vector2 Vector2::operator-(const Vector2& right) const
	{
		Vector2 result(this->getX() - right.getX(), this->getY() - right.getY());
		return result;
	}

	template<typename T>
	Vector2 Vector2::operator*(const T& k) const
	{
		Vector2 result(this->getX() * k, this->getY() * k);
		return result;
	}

	Vector2 Vector2::operator /(const float& k) const
	{
		if (k == 0.f || abs(k - 0.f) < EPSILON) {
			return Vector2();
		}
		float reciprocalK = 1 / k;
		Vector2 result(this->getX()*reciprocalK, this->getY()*reciprocalK);
		return result;
	}

	float Vector2::operator*(const Vector2& right) const
	{
		return (this->getX() * right.getX() + this->getY() * right.getY());
	}

	void Vector2::swap(Vector2 & vecA, Vector2 & vecB)
	{
		if (vecA == vecB)return;
		Vector2 tmp(vecA.getX(), vecA.getY());
		vecA = vecB;
		vecB = tmp;
	}

	float Vector2::getLength()
	{
		return sqrtf(powf(this->getX(), 2) + powf(this->getY(), 2));
	}



	/************************************************************************/
	/* Vector3                                                              */
	/************************************************************************/


	bool Vector3::operator==(const float right) const
	{
		bool xIsEqual = fabsf(this->getX() - right) <= EPSILON;
		bool yIsEqual = fabsf(this->getY() - right) <= EPSILON;
		bool zIsEqual = fabsf(this->getZ() - right) <= EPSILON;
		return (xIsEqual && yIsEqual && zIsEqual);
	}

	bool Vector3::operator==(const Vector3 & right) const
	{
		bool xIsEqual = fabsf(this->getX() - right.getX()) <= EPSILON;
		bool yIsEqual = fabsf(this->getY() - right.getY()) <= EPSILON;
		bool zIsEqual = fabsf(this->getZ() - right.getZ()) <= EPSILON;
		return (xIsEqual && yIsEqual && zIsEqual);
	}

	Vector3 Vector3::operator+(const Vector3 & right) const
	{
		Vector3 result(this->getX() + right.getX(), this->getY() + right.getY(), this->getZ() + right.getZ());
		return result;
	}

	Vector3 Vector3::operator-(const Vector3 & right) const
	{
		Vector3 result(this->getX() - right.getX(), this->getY() - right.getY(), this->getZ() - right.getZ());
		return result;
	}

	Vector3 Vector3::operator*(const float k) const
	{
		Vector3 result(this->getX() * k, this->getY() * k, this->getZ() * k);
		return result;
	}


	float Vector3::operator*(const Vector3 & right) const
	{
		return (this->getX()*right.getX() + this->getY()*right.getY() + this->getZ()*right.getZ());
	}

	template<typename T>
	Vector3 Vector3::operator*(const T& right) const
	{
		Vector3 result(
			this->getX()*right.matrix[0][0] + this->getY()*right.matrix[0][1] + this->getZ() * right.matrix[0][2],
			this->getY()*right.matrix[1][0] + this->getY()*right.matrix[1][1] + this->getZ() * right.matrix[1][2],
			this->getZ()*right.matrix[2][0] + this->getY()*right.matrix[2][1] + this->getZ() * right.matrix[2][2]);
		return result;
	}

	Vector3 Vector3::operator^(const Vector3 & right) const
	{
		Vector3 result;
		result.setX(this->getY() * right.getZ() - this->getZ() * right.getY());
		result.setY(this->getZ() * right.getX() - this->getX() * right.getZ());
		result.setZ(this->getX() * right.getY() - this->getY() * right.getX());
		return result;
	}

	void Vector3::Normalize()
	{
		float length = sqrtf(powf(this->getX(), 2) + powf(this->getY(), 2) + powf(this->getZ(), 2));
		this->setX(this->getX() / length);
		this->setY(this->getY() / length);
		this->setZ(this->getZ() / length);
	}

	Vector3 Vector3::Normalize(const Vector3& vecToBeNormalized)
	{
		Vector3 result;
		float length = sqrtf(powf(vecToBeNormalized.getX(), 2) + powf(vecToBeNormalized.getY(), 2) + powf(vecToBeNormalized.getZ(), 2));
		result.setX(vecToBeNormalized.getX() / length);
		result.setY(vecToBeNormalized.getY() / length);
		result.setZ(vecToBeNormalized.getZ() / length);
		return result;
	}

	float Vector3::getLength()
	{
		return sqrtf(powf(this->getX(), 2) + powf(this->getY(), 2) + powf(this->getZ(), 2));
	}




	/************************************************************************/
	/* Vector4                                                              */
	/************************************************************************/

	bool Vector4::operator==(float right) const
	{
		bool xIsEqual = fabsf(this->getX() - right) <= EPSILON;
		bool yIsEqual = fabsf(this->getY() - right) <= EPSILON;
		bool zIsEqual = fabsf(this->getZ() - right) <= EPSILON;
		bool wIsEqual = fabsf(this->getW() - right) <= EPSILON;
		return (xIsEqual && yIsEqual && zIsEqual && wIsEqual);
	}

	bool Vector4::operator==(const Vector4 & right) const
	{
		bool xIsEqual = fabsf(this->getX() - right.getX()) <= EPSILON;
		bool yIsEqual = fabsf(this->getY() - right.getY()) <= EPSILON;
		bool zIsEqual = fabsf(this->getZ() - right.getZ()) <= EPSILON;
		bool wIsEqual = fabsf(this->getW() - right.getW()) <= EPSILON;
		return (xIsEqual && yIsEqual && zIsEqual && wIsEqual);
	}

	bool Vector4::operator!=(const Vector4 & right) const
	{
		bool xIsInequal = fabsf(this->getX() - right.getX()) > EPSILON;
		bool yIsInequal = fabsf(this->getY() - right.getY()) > EPSILON;
		bool zIsInequal = fabsf(this->getZ() - right.getZ()) > EPSILON;
		bool wIsInequal = fabsf(this->getW() - right.getW()) > EPSILON;
		return (xIsInequal || yIsInequal || zIsInequal || wIsInequal);
	}

	Vector4 Vector4::operator +(const Vector4 & right) const
	{
		Vector4 result(this->getX() + right.getX(), this->getY() + right.getY(), this->getZ() + right.getZ(), this->getW() + right.getW());
		return result;
	}

	Vector4 Vector4::operator-(const Vector4 & right) const
	{
		Vector4 result(this->getX() - right.getX(), this->getY() - right.getY(), this->getZ() - right.getZ(), this->getW() - right.getW());
		return result;
	}

	Vector4 Vector4::operator*(const float k) const
	{
		Vector4 result(this->getX()*k, this->getY()*k, this->getZ() *k, this->getW() *k);
		return result;
	}

	Vector4 Vector4::operator/(const float k) const
	{
		float reciprocalK = 1 / k;
		Vector4 result(this->getX()*reciprocalK, this->getY()*reciprocalK, this->getZ() *reciprocalK, this->getW() *reciprocalK);
		return result;
	}

	float Vector4::operator *(const Vector4 & right) const
	{
		return this->getX()*right.getX() + this->getY()*right.getY() + this->getZ()*right.getZ() + this->getW()*right.getW();
	}

	Vector4 Vector4::operator ^(const Vector4& right) const
	{
		Vector4 result(this->getY()*right.getZ() - this->getZ()*right.getY(),
			this->getZ()*right.getX() - this->getX()*right.getZ(),
			this->getX()* right.getY() - this->getY()*right.getX());

		return result;
	}

	Vector4 Vector4::getInterpolateVector(const Vector4 & vecA, const Vector4 & vecB, float factor)
	{
		Vector4 result(
			LinearInerpolate(vecA.getX(), vecB.getX(), factor),
			LinearInerpolate(vecA.getY(), vecB.getY(), factor),
			LinearInerpolate(vecA.getZ(), vecB.getZ(), factor),
			1.f
		);
		return result;
	}

	Vector4 Vector4::getQuaternion(const Vector3 & lastPoint, const Vector3 & currentPoint)
	{
		Vector4 result;

		Vector3 perp = lastPoint ^ currentPoint;
		if (perp.getLength() > EPSILON)
		{
			result.setX(perp.getX());
			result.setY(perp.getY());
			result.setZ(perp.getZ());
			// w=cos(rotationAngle/2) ---> formula
			result.setW(lastPoint * currentPoint);
		}
		else
		{
			result.setX(.0f);
			result.setY(.0f);
			result.setZ(.0f);
			result.setW(.0f);
		}

		return result;
	}

	void Vector4::Normalize()
	{
		float length = sqrtf(powf(this->getX(), 2) + powf(this->getY(), 2) + powf(this->getZ(), 2));
		this->setX(this->getX() / length);
		this->setY(this->getY() / length);
		this->setZ(this->getZ() / length);
	}

	Vector4 Vector4::GetNormalizedVector() const 
	{
		Vector4 result;
		float length = sqrtf(powf(this->getX(), 2) + powf(this->getY(), 2) + powf(this->getZ(), 2));
		result.setX(this->getX() / length);
		result.setY(this->getY() / length);
		result.setZ(this->getZ() / length);
		return result;
	}

	bool Vector4::swap(Vector4 & v1, Vector4 & v2)
	{
		if (v1 != v2)
		{
			std::swap(v1.m_x, v2.m_x);
			std::swap(v1.m_y, v2.m_y);
			std::swap(v1.m_z, v2.m_z);
			std::swap(v1.m_w, v2.m_w);
		}
		return true;
	}
}
