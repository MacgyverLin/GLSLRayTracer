#ifndef _Vector3_h_
#define _Vector3_h_

template<class T>
class Vector3
{
public:
	Vector3()
	{
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

	T& x()
	{
		return m[0];
	}

	const T& x() const
	{
		return m[0];
	}

	T& y()
	{
		return m[1];
	}

	const T& y() const
	{
		return m[1];
	}

	T& z()
	{
		return m[2];
	}

	const T& z() const
	{
		return m[2];
	}

	Vector3& operator += (const Vector3& v)
	{
		m[0] += v.m[0];
		m[1] += v.m[1];
		m[2] += v.m[2];

		return *this;
	}

	Vector3& operator -= (const Vector3& v)
	{
		m[0] -= v.m[0];
		m[1] -= v.m[1];
		m[2] -= v.m[2];

		return *this;
	}

	Vector3& operator *= (const Vector3& v)
	{
		m[0] *= v.m[0];
		m[1] *= v.m[1];
		m[2] *= v.m[2];

		return *this;
	}

	Vector3& operator /= (const Vector3& v)
	{
		m[0] /= v.m[0];
		m[1] /= v.m[1];
		m[2] /= v.m[2];

		return *this;
	}

	Vector3& operator += (const T& v)
	{
		m[0] += v;
		m[1] += v;
		m[2] += v;

		return *this;
	}

	Vector3& operator -= (const T& v)
	{
		m[0] -= v;
		m[1] -= v;
		m[2] -= v;

		return *this;
	}

	Vector3& operator *= (const T& v)
	{
		m[0] *= v;
		m[1] *= v;
		m[2] *= v;

		return *this;
	}

	Vector3& operator /= (const T& v)
	{
		m[0] /= v;
		m[1] /= v;
		m[2] /= v;

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

	float Dot(const Vector3& v) const
	{
		return m[0] * v.m[0] + m[1] * v.m[1];
	}

	float SqrLength() const
	{
		return this->Dot(*this);
	}

	float Length() const
	{
		return sqrt(SqrLength());
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