#ifndef _Vector2_h_
#define _Vector2_h_

#include "IMath.h"

template<class T>
class Vector2
{
public:
	Vector2()
	{
		m[0] = 0;
		m[1] = 0;
	}

	Vector2(T x_, T y_)
	{
		m[0] = x_;
		m[1] = y_;
	}

	Vector2(T v_)
	{
		m[0] = v_;
		m[1] = v_;
	}

	~Vector2()
	{
	}

	T& operator[](int i)
	{
		assert(i >= 0 && i < 2);
		return m[i];
	}

	const T& operator[](int i) const
	{
		assert(i >= 0 && i < 2);
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

	Vector2& operator += (const Vector2& v)
	{
		m[0] += v.m[0];
		m[1] += v.m[1];
		m[2] += v.m[2];

		return *this;
	}

	Vector2& operator -= (const Vector2& v)
	{
		m[0] -= v.m[0];
		m[1] -= v.m[1];
		m[2] -= v.m[2];

		return *this;
	}

	Vector2& operator *= (const Vector2& v)
	{
		m[0] *= v.m[0];
		m[1] *= v.m[1];
		m[2] *= v.m[2];

		return *this;
	}

	Vector2& operator /= (const Vector2& v)
	{
		m[0] /= v.m[0];
		m[1] /= v.m[1];
		m[2] /= v.m[2];

		return *this;
	}

	Vector2& operator += (const T& v)
	{
		m[0] += v;
		m[1] += v;
		m[2] += v;

		return *this;
	}

	Vector2& operator -= (const T& v)
	{
		m[0] -= v;
		m[1] -= v;
		m[2] -= v;

		return *this;
	}

	Vector2& operator *= (const T& v)
	{
		m[0] *= v;
		m[1] *= v;
		m[2] *= v;

		return *this;
	}

	Vector2& operator /= (const T& v)
	{
		m[0] /= v;
		m[1] /= v;
		m[2] /= v;

		return *this;
	}

	friend Vector2 operator + (const Vector2& v0, const Vector2& v1)
	{
		return Vector2(v0.m[0] + v1.m[0], v0.m[1] + v1.m[1]);
	}

	friend Vector2 operator - (const Vector2& v0, const Vector2& v1)
	{
		return Vector2(v0.m[0] - v1.m[0], v0.m[1] - v1.m[1]);
	}

	friend Vector2 operator * (const Vector2& v0, const Vector2& v1)
	{
		return Vector2(v0.m[0] * v1.m[0], v0.m[1] * v1.m[1]);
	}

	friend Vector2 operator / (const Vector2& v0, const Vector2& v1)
	{
		return Vector2(v0.m[0] / v1.m[0], v0.m[1] / v1.m[1]);
	}

	friend Vector2 operator + (const Vector2& v0, const T& v1)
	{
		return Vector2(v0.m[0] + v1, v0.m[1] + v1);
	}

	friend Vector2 operator - (const Vector2& v0, const T& v1)
	{
		return Vector2(v0.m[0] - v1, v0.m[1] - v1);
	}

	friend Vector2 operator * (const Vector2& v0, const T& v1)
	{
		return Vector2(v0.m[0] * v1, v0.m[1] * v1);
	}

	friend Vector2 operator / (const Vector2& v0, const T& v1)
	{
		return Vector2(v0.m[0] / v1, v0.m[1] / v1);
	}

	float Dot(const Vector2& v) const
	{
		return m[0] * v.m[0] + m[1] * v.m[1];
	}

	float SqrLength() const
	{
		return this->Dot(*this);
	}

	float Length() const
	{
		return sqrt(Length());
	}

	friend Vector2 Normalize(const Vector2& normal)
	{
		float length = Length();

		return normal / length;
	}
private:
	T m[2];
};

typedef Vector2<unsigned int> uvec2;
typedef Vector2<int> ivec2;
typedef Vector2<float> vec2;
typedef Vector2<double> dvec2;

#endif