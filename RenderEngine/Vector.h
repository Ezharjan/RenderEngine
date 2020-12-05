#pragma once


#ifndef __Vector__
#define __Vector__



namespace RenderEngine {

	struct Vector2
	{
		float x, y;
		float operator*(const Vector2& right) const
		{
			return (this->x*right.x + this->y*right.y);
		}
	};


	class Vector3
	{
	public:
		Vector3() : m_x(0), m_y(0), m_z(0) {};
		Vector3(float param) : m_x(param), m_y(param), m_z(param) {};
		Vector3(const float& a0, const float& a1, const float& a2) :
			m_x(a0), m_y(a1), m_z(a2) {};
		~Vector3() {};
		Vector3 operator +(const Vector3& right)const;
		Vector3 operator -(const Vector3& right)const;
		Vector3 operator *(const float k)const;
		float operator *(const Vector3& right)const;
		template<typename T>
		Vector3 operator *(const T& right) const;
		Vector3 operator ^(const Vector3& right)const; // vector cross product
		bool operator ==(float aNumbuer) {
			return (this->getX() == aNumbuer && this->getY() && aNumbuer && this->getZ() == aNumbuer);
		}

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


	inline Vector3 Vector3::operator+(const Vector3 & right) const
	{
		Vector3 result(this->getX() + right.getX(), this->getY() + right.getY(), this->getZ() + right.getZ());
		return result;
	}

	inline Vector3 Vector3::operator-(const Vector3 & right) const
	{
		Vector3 result(this->getX() - right.getX(), this->getY() - right.getY(), this->getZ() - right.getZ());
		return result;
	}

	inline Vector3 Vector3::operator*(const float k) const
	{
		Vector3 result(this->getX() * k, this->getY() * k, this->getZ() * k);
		return result;
	}


	inline float Vector3::operator*(const Vector3 & right) const
	{
		return (this->getX()*right.getX() + this->getY()*right.getY() + this->getZ()*right.getZ());
	}

	template<typename T>
	inline Vector3 Vector3::operator*(const T& right) const
	{
		Vector3 result(
			this->getX()*right.matrix[0][0] + this->getY()*right.matrix[0][1] + this->getZ() * right.matrix[0][2],
			this->getY()*right.matrix[1][0] + this->getY()*right.matrix[1][1] + this->getZ() * right.matrix[1][2],
			this->getZ()*right.matrix[2][0] + this->getY()*right.matrix[2][1] + this->getZ() * right.matrix[2][2]);
		return result;
	}




	inline Vector3 Vector3::operator^(const Vector3 & right) const
	{
		Vector3 result;
		result.setX(this->getY() * right.getZ() - this->getZ() * right.getY());
		result.setY(this->getZ() * right.getX() - this->getX() * right.getZ());
		result.setZ(this->getX() * right.getY() - this->getY() * right.getX());
		return result;
	}

	inline float Vector3::getLength()
	{
		return sqrtf(powf(this->getX(), 2) + pow(this->getY(), 2) + pow(this->getZ(), 2));
	}



	class Vector4
	{
	public:
		Vector4() :
			m_x(0), m_y(0), m_z(0), m_w(0) {};
		Vector4(const float& a0, const float& a1, const float& a2, const float& a3 = 0) :
			m_x(a0), m_y(a1), m_z(a2), m_w(a3) {};
		~Vector4() {};
		Vector4 operator +(const Vector4& right)const;
		Vector4 operator -(const Vector4& right)const;
		Vector4 operator *(const float k)const;
		float operator *(const Vector4& right)const;
		Vector4 operator ^(const Vector4& right)const; // vector cross product

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



	inline Vector4 Vector4::operator +(const Vector4 & right) const
	{
		Vector4 result(this->getX() + right.getX(), this->getY() + right.getY(), this->getZ() + right.getZ(), this->getW() + right.getW());
		return result;
	}

	inline Vector4 Vector4::operator-(const Vector4 & right) const
	{
		Vector4 result(this->getX() - right.getX(), this->getY() - right.getY(), this->getZ() - right.getZ(), this->getW() - right.getW());
		return result;
	}

	inline Vector4 Vector4::operator*(const float k) const
	{
		Vector4 result(this->getX()*k, this->getY()*k, this->getZ() *k, this->getW() *k);
		return result;
	}

	inline float Vector4::operator *(const Vector4 & right) const
	{
		return this->getX()*right.getX() + this->getY()*right.getY() + this->getZ()*right.getZ() + this->getW()*right.getW();
	}

	inline Vector4 Vector4::operator ^(const Vector4& right) const
	{
		Vector4 result(this->getY()*right.getZ() - this->getZ()*right.getY(),
			this->getZ()*right.getX() - this->getX()*right.getZ(),
			this->getX()* right.getY() - this->getY()*right.getX());

		return result;
	}
	
}

#endif // !__Vector__

