#include "../../h/Utility/ideaMath.h"

#define WIN32_LEAN_AND_MEAN
#include <directxmath.h>

using namespace DirectX;

Quaternion Lerp::Linear(Quaternion & q1, Quaternion & q2, float t)
{
	t = Clamp(t, 0.0f, 1.0f);

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
	t = Clamp(t, 0.0f, 1.0f);

	t *= t;

	return Linear(q1, q2, t);
}

Quaternion Lerp::EaseOut(Quaternion & q1, Quaternion & q2, float t)
{
	t = Clamp(t, 0.0f, 1.0f);

	t = t * (2.0f - t);

	return Linear(q1, q2, t);
}

Quaternion Lerp::EaseInEaseOut(Quaternion & q1, Quaternion & q2, float t)
{
	t = Clamp(t, 0.0f, 1.0f);

	t = t * t * (3.0f - 2.0f * t);

	return Linear(q1, q2, t);
}