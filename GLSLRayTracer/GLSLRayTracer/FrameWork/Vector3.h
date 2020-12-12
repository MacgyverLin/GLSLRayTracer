#ifndef _Vector3_h_
#define _Vector3_h_

#include "IMath.h"

template<class T>
class Vector3
{
public:
	Vector3()
	{
		m[0] = 0.0;
		m[1] = 0.0;
		m[2] = 0.0;
	}

	Vector3(T x_, T y_, T z_)
	{
		m[0] = x_;
		m[1] = y_;
		m[2] = z_;
	}

	Vector3(T v_)
	{
		m[0] = v_;
		m[1] = v_;
		m[2] = v_;
	}

	~Vector3()
	{
	}

	T& operator[](int i)
	{
		assert(i >= 0 && i < 3);
		return m[i];
	}

	const T& operator[](int i) const
	{
		assert(i >= 0 && i < 3);
		return m[i];
	}

	T& X()
	{
		return m[0];
	}

	const T& X() const
	{
		return m[0];
	}

	T& Y()
	{
		return m[1];
	}

	const T& Y() const
	{
		return m[1];
	}

	T& Z()
	{
		return m[2];
	}

	const T& Z() const
	{
		return m[2];
	}

	Vector3& operator += (const Vector3& v)
	{
		m[0] += v.m[0]; m[1] += v.m[1];	m[2] += v.m[2];

		return *this;
	}

	Vector3& operator -= (const Vector3& v)
	{
		m[0] -= v.m[0]; m[1] -= v.m[1]; m[2] -= v.m[2];

		return *this;
	}

	Vector3& operator *= (const Vector3& v)
	{
		m[0] *= v.m[0]; m[1] *= v.m[1]; m[2] *= v.m[2];

		return *this;
	}

	Vector3& operator /= (const Vector3& v)
	{
		m[0] /= v.m[0]; m[1] /= v.m[1]; m[2] /= v.m[2];

		return *this;
	}

	Vector3& operator += (const T& v)
	{
		m[0] += v; m[1] += v; m[2] += v;

		return *this;
	}

	Vector3& operator -= (const T& v)
	{
		m[0] -= v; m[1] -= v; m[2] -= v;

		return *this;
	}

	Vector3& operator *= (const T& v)
	{
		m[0] *= v; m[1] *= v; m[2] *= v;

		return *this;
	}

	Vector3& operator /= (const T& v)
	{
		m[0] /= v; m[1] /= v; m[2] /= v;

		return *this;
	}

	friend Vector3 operator + (const Vector3& v0, const Vector3& v1)
	{
		return Vector3(v0.m[0] + v1.m[0], v0.m[1] + v1.m[1], v0.m[2] + v1.m[2]);
	}

	friend Vector3 operator - (const Vector3& v0, const Vector3& v1)
	{
		return Vector3(v0.m[0] - v1.m[0], v0.m[1] - v1.m[1], v0.m[2] - v1.m[2]);
	}

	friend Vector3 operator * (const Vector3& v0, const Vector3& v1)
	{
		return Vector3(v0.m[0] * v1.m[0], v0.m[1] * v1.m[1], v0.m[2] * v1.m[2]);
	}

	friend Vector3 operator / (const Vector3& v0, const Vector3& v1)
	{
		return Vector3(v0.m[0] / v1.m[0], v0.m[1] / v1.m[1], v0.m[2] / v1.m[2]);
	}

	friend Vector3 operator + (const Vector3& v0, const T& v1)
	{
		return Vector3(v0.m[0] + v1, v0.m[1] + v1, v0.m[2] + v1);
	}

	friend Vector3 operator - (const Vector3& v0, const T& v1)
	{
		return Vector3(v0.m[0] - v1, v0.m[1] - v1, v0.m[2] - v1);
	}

	friend Vector3 operator * (const Vector3& v0, const T& v1)
	{
		return Vector3(v0.m[0] * v1, v0.m[1] * v1, v0.m[2] * v1);
	}

	friend Vector3 operator / (const Vector3& v0, const T& v1)
	{
		return Vector3(v0.m[0] / v1, v0.m[1] / v1, v0.m[2] / v1);
	}

	friend Vector3 Min(const Vector3& v0, const Vector3& v1)
	{
		Vector3 result;

		result.X() = Math::Min(v0.X(), v1.X());
		result.Y() = Math::Min(v0.Y(), v1.Y());
		result.Z() = Math::Min(v0.Z(), v1.Z());
		
		return result;
	}

	friend Vector3 Max(const Vector3& v0, const Vector3& v1)
	{
		Vector3 result;

		result.X() = Math::Max(v0.X(), v1.X());
		result.Y() = Math::Max(v0.Y(), v1.Y());
		result.Z() = Math::Max(v0.Z(), v1.Z());

		return result;
	}

	float Dot(const Vector3& v) const
	{
		return m[0] * v.m[0] + m[1] * v.m[1] + m[2] * v.m[2];
	}

	friend float Dot(const Vector3& v0, const Vector3& v1)
	{
		return v0.Dot(v1);
	}

	float SqrLength() const
	{
		return this->Dot(*this);
	}

	friend float SqrLength(const Vector3& v)
	{
		return v.SqrLength();
	}

	float Length() const
	{
		return sqrt(SqrLength());
	}

	friend float Length(const Vector3& v)
	{
		return v.Length();
	}

	Vector3 Cross(const Vector3& v) const
	{
		return Vector3
		(
			this->m[1] * v.m[2] - this->m[2] * v.m[1],
			this->m[2] * v.m[0] - this->m[0] * v.m[2],
			this->m[0] * v.m[1] - this->m[1] * v.m[0]
		);
	}

	friend Vector3 Cross(const Vector3& v0, const Vector3& v1)
	{
		return Vector3
		(
			v0.m[1] * v1.m[2] - v0.m[2] * v1.m[1],
			v0.m[2] * v1.m[0] - v0.m[0] * v1.m[2],
			v0.m[0] * v1.m[1] - v0.m[1] * v1.m[0]
		);
	}

	Vector3 Normalize() const
	{
		float length = Length();

		return *this / length;
	}

	friend Vector3 Normalize(const Vector3& v)
	{
		return v.Normalize();
	}
private:
	T m[3];
};

typedef Vector3<unsigned int> uvec3;
typedef Vector3<int> ivec3;
typedef Vector3<float> vec3;
typedef Vector3<double> dvec3;

#endif