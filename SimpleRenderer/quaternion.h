#pragma once

class Quaternion
{
	union
	{
		float q[4];
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		__m128 m128;
	};

	static const Quaternion identity;

	Quaternion() = default;

	Quaternion(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	//��������
	Quaternion(const Quaternion& q)
	{
		memcpy(this, (void*)& q, sizeof(Quaternion));
	}

	//��ֵ���������
	Quaternion& operator=(const Quaternion& q)
	{
		memcpy(this, (void*)& q, sizeof(Quaternion));
		return *this;
	}

	float& operator[](int idx);
	Quaternion operator*(const Quaternion& q);
	Vector3 operator*(const Vector3& q);

	static float Dot(const Quaternion& q1, const Quaternion& q2);

	void Normalize();
	static Quaternion Normalize(const Quaternion& q);
	Quaternion normalized();

	static Quaternion AngleAxis(float angle, Vector3& axis);
	void ToAngleAxis(float& angle, Vector3& axis);
	static Quaternion Euler(Vector3& euler);
	Vector3 Euler();

	float Norm();
	Quaternion Conjugate();
	Quaternion UnitInverse();
	Quaternion Inverse();
};

const Quaternion Quaternion::identity = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

//�����±������
inline float& Quaternion::operator[](int idx)
{
	return q[idx];
}

//���س˷�
Quaternion Quaternion::operator*(const Quaternion& q)
{
	return Quaternion((float)((double)w * (double)q.x + (double)x * (double)q.w + (double)y * (double)q.z - (double)z * (double)q.y), (float)((double)w * (double)q.y + (double)y * (double)q.w + (double)z * (double)q.x - (double)x * (double)q.z), (float)((double)w * (double)q.z + (double)z * (double)q.w + (double)x * (double)q.y - (double)y * (double)q.x), (float)((double)w * (double)q.w - (double)x * (double)q.x - (double)y * (double)q.y - (double)z * (double)q.z));
}

//������������ת q * v * q^-1
Vector3 Quaternion::operator*(const Vector3& v)
{
	float num1 = x * 2.0f;
	float num2 = y * 2.0f;
	float num3 = z * 2.0f;
	float num4 = x * num1;
	float num5 = y * num2;
	float num6 = z * num3;
	float num7 = x * num2;
	float num8 = x * num3;
	float num9 = y * num3;
	float num10 = w * num1;
	float num11 = w * num2;
	float num12 = w * num3;
	Vector3 res;
	res.x = (float)((1.0 - ((double)num5 + (double)num6)) * (double)v.x + ((double)num7 - (double)num12) * (double)v.y + ((double)num8 + (double)num11) * (double)v.z);
	res.y = (float)(((double)num7 + (double)num12) * (double)v.x + (1.0 - ((double)num4 + (double)num6)) * (double)v.y + ((double)num9 - (double)num10) * (double)v.z);
	res.z = (float)(((double)num8 - (double)num11) * (double)v.x + ((double)num9 + (double)num10) * (double)v.y + (1.0 - ((double)num4 + (double)num5)) * (double)v.z);
	return res;
}

//���
inline float Quaternion::Dot(const Quaternion& q1, const Quaternion& q2)
{
#ifdef SIMD_ASM
	__declspec(align(16)) Quaternion res;
	_asm
	{
		mov esi, q1;
		mov edi, q2;
		movaps xmm0, [esi];
		mulps xmm0, [edi];
		movaps xmm1, xmm0;
		shufps xmm1, xmm0, SIMD_SHUFFLE(0x01, 0x00, 0x03, 0x02);
		addps xmm1, xmm0;
		shufps xmm0, xmm1, SIMD_SHUFFLE(0x02, 0x03, 0x00, 0x01);
		addps xmm0, xmm1;
		movaps res, xmm0;
	}
	return res.z;
#else
	return (float)((double)q1.x * (double)q2.x + (double)q1.y * (double)q2.y + (double)q1.z * (double)q2.z + (double)q1.w * (double)q2.w);
#endif
}

//��Ԫ������
inline void Quaternion::Normalize()
{
	*this = Quaternion::Normalize(*this);
}

inline Quaternion Quaternion::Normalize(const Quaternion& q)
{
	float num = sqrtf(Quaternion::Dot(q, q));
	if ((double)num < (double)Epsilon)
		return Quaternion::identity;
	return Quaternion(q.x / num, q.y / num, q.z / num, q.w / num);
}

inline Quaternion Quaternion::normalized()
{
	return Quaternion::Normalize(*this);
}

//������ת�����ת�Ƕȴ���һ����Ԫ��
inline Quaternion Quaternion::AngleAxis(float angle, Vector3& axis)
{
	Vector3 newAxis = axis.normalized();
	float halftheta = 0.5f * angle * Deg2Rad;
	float sinhalftheta = sinf(halftheta);
	return Quaternion(sinhalftheta * newAxis.x, sinhalftheta * newAxis.y, sinhalftheta * newAxis.z, cosf(halftheta));
}

//����Ԫ��ת��Ϊ��ת�����ת�Ƕ�
inline void Quaternion::ToAngleAxis(float& angle, Vector3& axis)
{
	angle = acosf(w);
	float invsinangle = 1 / sinf(angle);
	axis.x = x * invsinangle;
	axis.y = y * invsinangle;
	axis.z = z * invsinangle;
	angle *= 2.0f * Rad2Deg;
}

//����ŷ���Ǵ���һ����Ԫ�� yxz
inline Quaternion Quaternion::Euler(Vector3& euler)
{
	float halfx = 0.5f * euler.x * Deg2Rad;
	float halfy = 0.5f * euler.y * Deg2Rad;
	float halfz = 0.5f * euler.z * Deg2Rad;
	float sinhalfx = sinf(halfx);
	float coshalfx = cosf(halfx);
	float sinhalfy = sinf(halfy);
	float coshalfy = cosf(halfy);
	float sinhalfz = sinf(halfz);
	float coshalfz = cosf(halfz);
	float coshalfxcoshalfy = coshalfx * coshalfy;
	float sinhalfxsinhalfy = sinhalfx * sinhalfy;
	float coshalfxsinhalfy = coshalfx * sinhalfy;
	float sinhalfxcoshalfy = sinhalfx * coshalfy;
	Quaternion q;
	q.x = coshalfxsinhalfy * sinhalfz + sinhalfxcoshalfy * coshalfz;
	q.y = coshalfxsinhalfy * coshalfz - sinhalfxcoshalfy * sinhalfz;
	q.z = coshalfxcoshalfy * sinhalfz - sinhalfxsinhalfy * coshalfz;
	q.w = coshalfxcoshalfy * coshalfz + sinhalfxsinhalfy * sinhalfz;
	return q;
}

//��Ԫ��ת��Ϊŷ����
inline Vector3 Quaternion::Euler()
{
	Vector3 res;
	float xsqr = x * x;
	res.x = asinf(2 * (w * x - y * z)) * Rad2Deg;
	//atan2���Է���-�е��� atanֻ�ܷ���-��/2����/2
	res.y = atan2f(2 * (w * y + x * z), 1 - 2 * (xsqr + y * y)) * Rad2Deg;
	res.z = atan2f(2 * (w * z + x * y), 1 - 2 * (xsqr + z * z)) * Rad2Deg;
	return res;
}

//��Ԫ����ģ
float Quaternion::Norm()
{
	return sqrtf(x * x + y * y + z * z + w * w);
}

//��Ԫ������
Quaternion Quaternion::Conjugate()
{
	return Quaternion(-x, -y, -z, w);
}

//��λ��Ԫ��������ڹ���
Quaternion Quaternion::UnitInverse()
{
	return Conjugate();
}

//��Ԫ������
Quaternion Quaternion::Inverse()
{
	float invNorm = 1 / Norm();
	return Quaternion(-x * invNorm, -y * invNorm, -z * invNorm, w * invNorm);
}