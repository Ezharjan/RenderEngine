#pragma once

#include <iostream>

namespace RenderEngine {

	class Vector2
	{
	public:
		Vector2() : m_x(0), m_y(0) {};
		Vector2(const Vector2& vecToBeCopied) {
			this->setX(vecToBeCopied.getX());
			this->setY(vecToBeCopied.getY());
		};
		Vector2(float param) : m_x(param), m_y(param) {};
		Vector2(const float& a0, const float& a1) :
			m_x(a0), m_y(a1) {};
		~Vector2() {};
		void operator =(const Vector2& right);
		Vector2 operator +(const Vector2& right)const;
		Vector2 operator -(const Vector2& right)const;
		template<typename T>
		Vector2 operator *(const T& right) const;
		Vector2 operator /(const float& right) const;
		float operator *(const Vector2& right)const;
		bool operator ==(const float aNumbuer) {
			return (this->getX() == aNumbuer && this->getY() && aNumbuer);
		}
		bool operator ==(const Vector2& rightVec) {
			return (this->getX() == rightVec.getX() && this->getY() == rightVec.getY());
		}
		static void swap(Vector2& vecA, Vector2& vecB);

		float getX()const { return m_x; }
		float getY()const { return m_y; }
		void setX(const float& x) { m_x = x; }
		void setY(const float& y) { m_y = y; }
		float getLength();


	private:
		float m_x;
		float m_y;
	};

	class Vector3
	{
	public:
		Vector3() : m_x(0), m_y(0), m_z(0) {};
		Vector3(const Vector3& vecToBeCopied) {
			this->setX(vecToBeCopied.getX());
			this->setY(vecToBeCopied.getY());
			this->setZ(vecToBeCopied.getZ());
		};
		Vector3(float param) : m_x(param), m_y(param), m_z(param) {};
		Vector3(const float& a0, const float& a1, const float& a2) :
			m_x(a0), m_y(a1), m_z(a2) {};
		~Vector3() {};
		bool operator ==(float right)const;
		bool operator ==(const Vector3& right) const;
		Vector3 operator +(const Vector3& right)const;
		Vector3 operator -(const Vector3& right)const;
		Vector3 operator *(const float k)const;
		float operator *(const Vector3& right)const;
		template<typename T>
		Vector3 operator *(const T& right) const;
		Vector3 operator ^(const Vector3& right)const; // vector cross product
		void Normalize();
		static Vector3 Normalize(const Vector3& vecToBeNormalized);

		float getX()const { return m_x; }
		float getY()const { return m_y; }
		float getZ()const { return m_z; }
		float getR()const { return m_x; }
		float getG()const { return m_y; }
		float getB()const { return m_z; }
		void setX(const float& x) { m_x = x; }
		void setY(const float& y) { m_y = y; }
		void setZ(const float& z) { m_z = z; }
		void setR(const float& x) { m_x = x; }
		void setG(const float& y) { m_y = y; }
		void setB(const float& z) { m_z = z; }
		float getLength();


	private:
		float m_x;
		float m_y;
		float m_z;
	};

	class Vector4
	{
	public:
		Vector4() :
			m_x(0), m_y(0), m_z(0), m_w(0) {};
		Vector4(const Vector4& vecToBeCopied) {
			this->setX(vecToBeCopied.getX());
			this->setY(vecToBeCopied.getY());
			this->setZ(vecToBeCopied.getZ());
			this->setW(vecToBeCopied.getW());
		};
		Vector4(const Vector3& vecToBeCopied, bool isPoint = false) {
			this->setX(vecToBeCopied.getX());
			this->setY(vecToBeCopied.getY());
			this->setZ(vecToBeCopied.getZ());
			if (isPoint)this->setW(1);
			else this->setW(0);
		};
		Vector4(const float& a0, const float& a1, const float& a2, const float& a3 = 0) :
			m_x(a0), m_y(a1), m_z(a2), m_w(a3) {};
		~Vector4() {};
		bool operator ==(float right)const;
		bool operator ==(const Vector4& right)const;
		bool operator !=(const Vector4& right)const;
		Vector4 operator +(const Vector4& right)const;
		Vector4 operator -(const Vector4& right)const;
		Vector4 operator *(const float k)const;
		Vector4 operator /(const float k)const;
		float operator *(const Vector4& right)const;
		Vector4 operator ^(const Vector4& right)const; // vector cross product
		Vector4 getInterpolateVector(const Vector4& vecA, const Vector4& vecB, float factor);
		Vector4 getQuaternion(const Vector3 & lastPoint, const Vector3 & currentPoint);
		void Normalize();
		Vector4 GetNormalizedVector() const;
		static bool swap(Vector4& v1, Vector4& v2);

		float getX()const { return m_x; }
		float getY()const { return m_y; }
		float getZ()const { return m_z; }
		float getW()const { return m_w; }
		float getR()const { return m_x; }
		float getG()const { return m_y; }
		float getB()const { return m_z; }
		float getA()const { return m_w; }
		void setX(const float& x) { m_x = x; }
		void setY(const float& y) { m_y = y; }
		void setZ(const float& z) { m_z = z; }
		void setW(const float& w) { m_w = w; }
		void setR(const float& r) { m_x = r; }
		void setG(const float& g) { m_y = g; }
		void setB(const float& b) { m_z = b; }
		void setA(const float& a) { m_w = a; }

	private:
		float m_x;
		float m_y;
		float m_z;
		float m_w;
	};


	inline std::ostream& operator <<(std::ostream& os, const Vector4& vec4) { os << "x: " << vec4.getX() << "; y: " << vec4.getY() << "; z: " << vec4.getZ() << "; w: " << vec4.getW() << ". "; return os; }
	inline std::ostream& operator <<(std::ostream& os, const Vector3& vec3) { os << "x: " << vec3.getX() << "; y: " << vec3.getY() << "; z: " << vec3.getZ() << ". "; return os; }
	inline std::ostream& operator <<(std::ostream& os, const Vector2& vec2) { os << "x: " << vec2.getX() << "; y: " << vec2.getY() << ". "; return os; }
}