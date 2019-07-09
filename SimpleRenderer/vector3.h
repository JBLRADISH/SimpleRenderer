#pragma once

#include <math.h>

class Vector3
{
public:
	union
	{
		float v[4];
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
	};

	static Vector3 zero;

	Vector3(float x, float y, float z, float w = 0.0f)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	//��������
	Vector3(const Vector3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}

	//��ֵ���������
	Vector3& operator=(const Vector3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	float& operator[](int idx);
	bool operator==(const Vector3& v);
	bool operator!=(const Vector3& v);
	Vector3 operator+(const Vector3& v);
	Vector3 operator-(const Vector3& v);
	Vector3 operator*(float f);
	Vector3 operator/(float f);

	Vector3& Scale(const Vector3& v);
	static Vector3 Scale(const Vector3& v1, const Vector3& v2);

	float Magnitude();
	static float Magnitude(const Vector3& v);
	float SqrMagnitude();
	static float SqrMagnitude(const Vector3& v);

	void Normalize();
	static Vector3 Normalize(const Vector3& v);
	Vector3 normalized();

	static Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
	static float Dot(const Vector3& v1, const Vector3& v2);
	static float Distance(Vector3& v1, Vector3& v2);
	static Vector3 Project(Vector3& v1, Vector3& v2);
};

//������
Vector3 Vector3::zero = Vector3(0, 0, 0, 0);

//�����±������
inline float& Vector3::operator[](int idx)
{
	return v[idx];
}

//���رȽ������
inline bool Vector3::operator==(const Vector3& v)
{
	return (double)Vector3::SqrMagnitude(*this - v) < 9.99999943962493E-11;
}

//���رȽ������
inline bool Vector3::operator!=(const Vector3& v)
{
	return !(*this == v);
}

//�����ӷ�
inline Vector3 Vector3::operator+(const Vector3& v)
{
	return Vector3(x + v.x, y + v.y, z + v.z, w + v.w);
}

//��������
inline Vector3 Vector3::operator-(const Vector3& v)
{
	return Vector3(x - v.x, y - v.y, z - v.z, w - v.w);
}

//�����˷�
inline Vector3 Vector3::operator*(float f)
{
	return Vector3(x * f, y * f, z * f, w * f);
}

//��������
inline Vector3 Vector3::operator/(float f)
{
	return Vector3(x / f, y / f, z / f, w / f);
}

//���������
inline Vector3& Vector3::Scale(const Vector3& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;
	return *this;
}

inline Vector3 Vector3::Scale(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}

//��������
inline float Vector3::Magnitude()
{
	return sqrtf(Vector3::Dot(*this, *this));
}

inline float Vector3::Magnitude(const Vector3& v)
{
	return sqrtf(Vector3::Dot(v, v));
}

//��������ƽ��
inline float Vector3::SqrMagnitude()
{
	return Vector3::Dot(*this, *this);
}

inline float Vector3::SqrMagnitude(const Vector3& v)
{
	return Vector3::Dot(v, v);
}

//��������
inline void Vector3::Normalize()
{
	float num = Vector3::Magnitude(*this);
	if ((double)num > 9.99999974737875E-06)
		*this = *this / num;
	else
		*this = Vector3::zero;
}

inline Vector3 Vector3::Normalize(const Vector3& v)
{
	float num = Vector3::Magnitude(v);
	if ((double)num > 9.99999974737875E-06)
		return Vector3(v) / num;
	else
		return Vector3::zero;
}

inline Vector3 Vector3::normalized()
{
	return Vector3::Normalize(*this);
}

//Lerp��ֵ
inline Vector3 Vector3::Lerp(const Vector3& v1, const Vector3& v2, float t)
{
	return Vector3(v1.x + (v2.x - v1.x) * t, v1.y + (v2.y - v1.y) * t, v1.z + (v2.z - v1.z) * t, v1.w + (v2.w - v1.w) * t);
}

//���
inline float Vector3::Dot(const Vector3& v1, const Vector3& v2)
{
	return (float)((double)v1.x * (double)v2.x + (double)v1.y * (double)v2.y + (double)v1.z * (double)v2.z + (double)v1.w * (double)v2.w);
}

//��������
inline float Vector3::Distance(Vector3& v1, Vector3& v2)
{
	return Vector3::Magnitude(v1 - v2);
}

//ͶӰ
inline Vector3 Vector3::Project(Vector3& v1, Vector3& v2)
{
	return v2 * Vector3::Dot(v1, v2) / Vector3::Dot(v2, v2);
}