#include "../../h/Utility/ideaMath.h"

#define WIN32_LEAN_AND_MEAN
#include <directxmath.h>

using namespace DirectX;

Matrix4x4 Matrix4x4::Inverse()const
{
	XMMATRIX xmat = XMMatrixIdentity();

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			xmat.r[i].m128_f32[j] = r[i][j];
		}
	}

	XMMATRIX ximat = XMMatrixInverse(nullptr, xmat);

	Matrix4x4 imat;

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			imat.r[i][j] = ximat.r[i].m128_f32[j];
		}
	}

	return imat;
}

float Bezier(float x, Vector2D p1, Vector2D p2, unsigned int n)
{
	if(p1.x == p1.y && p2.x == p2.y){ return x; }

	const float k0 = 1.0f + 3.0f * p1.x - 3.0f * p2.x;
	const float k1 = 3.0f * p2.x - 6.0f * p1.x;
	const float k2 = 3.0f * p1.x;

	float t = x;

	for(unsigned int i = 0U; i < n; ++i){
		float ft = k0 * t * t * t + k1 * t * t + k2 * t - x;

		if(abs(ft) <= 0.0000001f){ break; }

		t -= ft * 0.5f;
	}

	float r = 1.0f - t;

	return t * t * t + 3.0f * t * t * r * p2.y + 3.0f * t * r * r * p1.y;
}

Quaternion Lerp::Linear(Quaternion & q1, Quaternion & q2, float t)
{
	if(t > 1.0f){ t = 1.0f; }
	if(t < 0.0f){ t = 0.0f; }

	XMVECTOR v1 = XMVectorSet(q1.x, q1.y, q1.z, q1.w);
	XMVECTOR v2 = XMVectorSet(q2.x, q2.y, q2.z, q2.w);
	XMVECTOR  v = XMQuaternionSlerp(v1, v2, t);

	Quaternion q;

	q.x = v.m128_f32[0];
	q.y = v.m128_f32[1];
	q.z = v.m128_f32[2];
	q.w = v.m128_f32[3];

	return q;
}

Quaternion Lerp::EaseIn(Quaternion & q1, Quaternion & q2, float t)
{
	t *= t;

	return Linear(q1, q2, t);
}

Quaternion Lerp::EaseOut(Quaternion & q1, Quaternion & q2, float t)
{
	t = t * (2.0f - t);

	return Linear(q1, q2, t);
}

Quaternion Lerp::EaseInEaseOut(Quaternion & q1, Quaternion & q2, float t)
{
	t = t * t * (3.0f - 2.0f * t);

	return Linear(q1, q2, t);
}