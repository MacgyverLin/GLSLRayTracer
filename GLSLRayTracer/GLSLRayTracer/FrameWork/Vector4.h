#ifndef _Vector4_h_
#define _Vector4_h_

#include "IMath.h"

template<class T>
class Vector4
{
public:
	Vector4()
	{
		m[0] = 0;
		m[1] = 0;
		m[2] = 0;
		m[3] = 0;
	}

	Vector4(T x_, T y_, T z_, T w_)
	{
		m[0] = x_;
		m[1] = y_;
		m[2] = z_;
		m[3] = w_;
	}

	Vector4(T v_)
	{
		m[0] = v_;
		m[1] = v_;
		m[2] = v_;
		m[3] = v_;
	}

	~Vector4()
	{
	}

	T& operator[](int i)
	{
		assert(i >= 0 && i < 4);
		return m[i];
	}

	const T& operator[](int i) const
	{
		assert(i >= 0 && i < 4);
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

	T& W()
	{
		return m[3];
	}

	const T& W() const
	{
		return m[3];
	}

	Vector4& operator += (const Vector4& v)
	{
		m[0] += v.m[0];
		m[1] += v.m[1];
		m[2] += v.m[2];
		m[3] += v.m[3];

		return *this;
	}

	Vector4& operator -= (const Vector4& v)
	{
		m[0] -= v.m[0];
		m[1] -= v.m[1];
		m[2] -= v.m[2];
		m[3] -= v.m[3];

		return *this;
	}

	Vector4& operator *= (const Vector4& v)
	{
		m[0] *= v.m[0];
		m[1] *= v.m[1];
		m[2] *= v.m[2];
		m[3] *= v.m[3];

		return *this;
	}

	Vector4& operator /= (const Vector4& v)
	{
		m[0] /= v.m[0];
		m[1] /= v.m[1];
		m[2] /= v.m[2];
		m[3] /= v.m[3];

		return *this;
	}

	Vector4& operator += (const T& v)
	{
		m[0] += v;
		m[1] += v;
		m[2] += v;
		m[3] += v;

		return *this;
	}

	Vector4& operator -= (const T& v)
	{
		m[0] -= v;
		m[1] -= v;
		m[2] -= v;
		m[3] -= v;

		return *this;
	}

	Vector4& operator *= (const T& v)
	{
		m[0] *= v;
		m[1] *= v;
		m[2] *= v;
		m[3] *= v;

		return *this;
	}

	Vector4& operator /= (const T& v)
	{
		m[0] /= v;
		m[1] /= v;
		m[2] /= v;
		m[3] /= v;

		return *this;
	}

	friend Vector4 operator + (const Vector4& v0, const Vector4& v1)
	{
		return Vector4(v0.m[0] + v1.m[0], v0.m[1] + v1.m[1], v0.m[2] + v1.m[2], v0.m[3] + v1.m[3]);
	}

	friend Vector4 operator - (const Vector4& v0, const Vector4& v1)
	{
		return Vector4(v0.m[0] - v1.m[0], v0.m[1] - v1.m[1], v0.m[2] - v1.m[2], v0.m[3] - v1.m[3]);
	}

	friend Vector4 operator * (const Vector4& v0, const Vector4& v1)
	{
		return Vector4(v0.m[0] * v1.m[0], v0.m[1] * v1.m[1], v0.m[2] * v1.m[2], v0.m[3] * v1.m[3]);
	}

	friend Vector4 operator / (const Vector4& v0, const Vector4& v1)
	{
		return Vector4(v0.m[0] / v1.m[0], v0.m[1] / v1.m[1], v0.m[2] / v1.m[2], v0.m[3] / v1.m[3]);
	}

	friend Vector4 operator + (const Vector4& v0, const T& v1)
	{
		return Vector4(v0.m[0] + v1, v0.m[1] + v1, v0.m[2] + v1, v0.m[3] + v1);
	}

	friend Vector4 operator - (const Vector4& v0, const T& v1)
	{
		return Vector4(v0.m[0] - v1, v0.m[1] - v1, v0.m[2] - v1, v0.m[3] - v1);
	}

	friend Vector4 operator * (const Vector4& v0, const T& v1)
	{
		return Vector4(v0.m[0] * v1, v0.m[1] * v1, v0.m[2] * v1, v0.m[3] * v1);
	}

	friend Vector4 operator / (const Vector4& v0, const T& v1)
	{
		return Vector4(v0.m[0] / v1, v0.m[1] / v1, v0.m[2] / v1, v0.m[3] / v1);
	}

	float Dot(const Vector4& v) const
	{
		return m[0] * v.m[0] + m[1] * v.m[1] + m[2] * v.m[2] + m[3] * v.m[3];
	}

	float SqrLength() const
	{
		return this->Dot(*this);
	}

	float Length() const
	{
		return sqrt(Length());
	}

	friend Vector4 Normalize(const Vector4& normal)
	{
		float length = Length();

		return normal / length;
	}
private:
	T m[4];
};

typedef Vector4<unsigned int> uvec4;
typedef Vector4<int> ivec4;
typedef Vector4<float> vec4;
typedef Vector4<double> dvec4;

#endif