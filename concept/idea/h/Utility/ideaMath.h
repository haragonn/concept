#ifndef INCLUDE_IDEA_IDEAMATH_H
#define INCLUDE_IDEA_IDEAMATH_H

#include "ideaUtility.h"

#include <math.h>

namespace{
	constexpr float ideaPI = 3.141592654f;		// ÉŒ
	constexpr float ROOT2 = 1.414213562f;		// Å„2
	constexpr float EPSILON = 1.192092896e-07f;	// floatÇÃåÎç∑
}

// DegreeäpÇ©ÇÁRadianäpÇ÷ïœä∑
inline constexpr float DegreeToRadian(float dig)
{
	return dig / 180.0f * ideaPI;
}

// RadianäpÇ©ÇÁDegreeäpÇ÷ïœä∑
inline constexpr float RadianToDegree(float rad)
{
	return rad / ideaPI * 180.0f;
}

struct Vector2D{
	union{
		struct{
			float x;
			float y;
		};

		float r[2];
	};

	Vector2D() : x(0.0f), y(0.0f){}
	Vector2D(float x, float y) : x(x), y(y){}

	float& operator[](const int n)
	{
		Assert(n < 2);

		return r[n];
	}

	Vector2D& operator=(const Vector2D& r)
	{
		x = r.x;
		y = r.y;

		return (*this);
	}

	Vector2D& operator+()
	{
		return (*this);
	}

	Vector2D operator-()const
	{
		return Vector2D(x * -1.0f, y * -1.0f);
	}

	Vector2D operator+(const Vector2D& r)const
	{
		return Vector2D(x + r.x, y + r.y);
	}

	Vector2D& operator+=(const Vector2D& r)
	{
		x += r.x;
		y += r.y;

		return (*this);
	}

	Vector2D operator-(const Vector2D& r)const
	{
		return Vector2D(x - r.x, y - r.y);
	}

	Vector2D& operator-=(const Vector2D& r)
	{
		x -= r.x;
		y -= r.y;

		return (*this);
	}

	Vector2D operator*(float r)const
	{
		return Vector2D(x * r, y * r);
	}

	Vector2D& operator*=(float r)
	{
		x *= r;
		y *= r;

		return (*this);
	}

	Vector2D operator*(const Vector2D& r)const
	{
		return Vector2D(x * r.x, y * r.y);
	}

	Vector2D operator/(float f)const
	{
		Assert(f != 0.0f);

		return Vector2D(x / f, y / f);
	}

	Vector2D& operator/=(float f)
	{
		Assert(f != 0.0f);

		x /= f;
		y /= f;

		return (*this);
	}

	Vector2D operator/(const Vector2D& r)const
	{
		Assert(r.x != 0.0f && r.y != 0.0f);

		return Vector2D(x / r.x, y / r.y);
	}

	friend Vector2D operator*(float f, const Vector2D &r)
	{
		return Vector2D(r.x * f, r.y * f);
	}

	friend Vector2D operator/(float f, const Vector2D &r)
	{
		Assert(f != 0.0f);

		return Vector2D(r.x / f, r.y / f);
	}

	bool operator==(const Vector2D& r)const
	{
		return (x == r.x) && (y == r.y);
	}

	float Length()const
	{
		return sqrtf(x * x + y * y);
	}

	float LengthSquare()const
	{
		return x * x + y * y;
	}

	Vector2D Normalized()const
	{
		const float len = Length();

		if(len > 0.0f){
			return  Vector2D(x / len, y / len);
		}

		return Vector2D(0.0f, 0.0f);
	}

	float Dot(const Vector2D& r)const
	{
		return x * r.x + y * r.y;
	}

	float Cross(const Vector2D& r)const
	{
		return x * r.y - y * r.x;
	}
};

struct Vector3D{
	union{
		struct{
			float x;
			float y;
			float z;
		};

		float r[3];
	};


	Vector3D() : x(0.0f), y(0.0f), z(0.0f){}
	Vector3D(float x, float y, float z) : x(x), y(y), z(z){}

	float& operator[](const int n)
	{
		Assert(n < 3);

		return r[n];
	}

	Vector3D& operator=(const Vector3D& r)
	{
		x = r.x;
		y = r.y;
		z = r.z;

		return (*this);
	}

	Vector3D& operator+()
	{
		return (*this);
	}

	Vector3D operator-()const
	{
		return Vector3D(x * -1.0f, y * -1.0f, z * -1.0f);
	}

	Vector3D operator+(const Vector3D& r)const
	{
		return Vector3D(x + r.x, y + r.y, z + r.z);
	}

	Vector3D& operator+=(const Vector3D& r)
	{
		x += r.x;
		y += r.y;
		z += r.z;

		return (*this);
	}

	Vector3D operator-(const Vector3D& r)const
	{
		return Vector3D(x - r.x, y - r.y, z - r.z);
	}

	Vector3D& operator-=(const Vector3D& r)
	{
		x -= r.x;
		y -= r.y;
		z -= r.z;

		return (*this);
	}

	Vector3D operator*(float f)const
	{
		return Vector3D(x * f, y * f, z * f);
	}

	Vector3D& operator*=(float f)
	{
		x *= f;
		y *= f;
		z *= f;

		return (*this);
	}

	Vector3D operator*(const Vector3D& r)const
	{
		return Vector3D(x * r.x, y * r.y, z * r.z);
	}

	Vector3D operator/(float f)const
	{
		Assert(f != 0.0f);

		return Vector3D(x / f, y / f, z / f);
	}

	Vector3D& operator/=(float f)
	{
		Assert(f != 0.0f);

		x /= f;
		y /= f;
		z /= f;

		return (*this);
	}

	Vector3D operator/(const Vector3D& r)const
	{
		Assert(r.x != 0.0f && r.y != 0.0f && r.z != 0.0f);

		return Vector3D(x / r.x, y / r.y, z / r.z);
	}

	Vector3D& operator/=(const Vector3D& r)
	{
		(*this) = (*this) / r;

		return (*this);
	}

	friend Vector3D operator*(float f, const Vector3D &r)
	{
		return Vector3D(r.x * f, r.y * f, r.z * f);
	}

	friend Vector3D operator/(float f, const Vector3D &r)
	{
		Assert(f != 0.0f);

		return Vector3D(r.x / f, r.y / f, r.z / f);
	}

	bool operator==(const Vector3D& r)const
	{
		return (x == r.x) && (y == r.y) && (z == r.z);
	}

	float Length()const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float LengthSquare()const
	{
		return x * x + y * y + z * z;
	}

	Vector3D Normalized()const
	{
		const float len = Length();

		if(len > 0.0f){
			return  Vector3D(x / len, y / len, z / len);
		}

		return Vector3D(0.0f, 0.0f, 0.0f);
	}

	float Dot(const Vector3D& r)const
	{
		return x * r.x + y * r.y + z * r.z;
	}

	Vector3D Cross(const Vector3D& r)const
	{
		return Vector3D((y * r.z) - (z * r.y), (z * r.x) - (x * r.z), (x * r.y) - (y * r.x));
	}
};

struct Vector4D{
	union{
		struct{
			float x;
			float y;
			float z;
			float w;
		};

		float r[4];
	};

	Vector4D() : x(0.0f), y(0.0f), z(0.0f), w(0.0f){}
	Vector4D(float x, float y, float z, float w) : x(x), y(y), z(z), w(w){}

	float& operator[](const int n)
	{
		Assert(n < 4);

		return r[n];
	}

	Vector4D& operator=(const Vector4D& r)
	{
		x = r.x;
		y = r.y;
		z = r.z;
		w = r.w;

		return (*this);
	}

	Vector4D& operator+()
	{
		return (*this);
	}

	Vector4D operator-()const
	{
		return Vector4D(x * -1.0f, y * -1.0f, z * -1.0f, w * -1.0f);
	}

	Vector4D operator+(const Vector4D& r)const
	{
		return Vector4D(x + r.x, y + r.y, z + r.z, w + r.w);
	}

	Vector4D& operator+=(const Vector4D& r)
	{
		x += r.x;
		y += r.y;
		z += r.z;
		w += r.w;

		return (*this);
	}

	Vector4D operator-(const Vector4D& r)const
	{
		return Vector4D(x - r.x, y - r.y, z - r.z, w - r.w);
	}

	Vector4D& operator-=(const Vector4D& r)
	{
		x -= r.x;
		y -= r.y;
		z -= r.z;
		w -= r.w;

		return (*this);
	}

	Vector4D operator*(float r)const
	{
		return Vector4D(x * r, y * r, z * r, w * r);
	}

	Vector4D& operator*=(float r)
	{
		x *= r;
		y *= r;
		z *= r;
		w *= r;

		return (*this);
	}

	Vector4D operator*(const Vector4D& r)const
	{
		return Vector4D(x * r.x, y * r.y, z * r.z, w * r.w);
	}

	Vector4D& operator*=(const Vector4D& r)
	{
		(*this) = (*this) * r;

		return (*this);
	}

	Vector4D operator/(float f)const
	{
		Assert(f != 0.0f);

		return Vector4D(x / f, y / f, z / f, w / f);
	}

	Vector4D& operator/=(float f)
	{
		Assert(f != 0.0f);

		x /= f;
		y /= f;
		z /= f;
		w /= f;

		return (*this);
	}

	Vector4D operator/(const Vector4D& r)const
	{
		Assert(r.x != 0.0f && r.y != 0.0f && r.z != 0.0f && r.w == 0.0f);

		return Vector4D(x / r.x, y / r.y, z / r.z, w / r.w);
	}

	Vector4D& operator/=(const Vector4D& r)
	{
		(*this) = (*this) / r;

		return (*this);
	}

	friend Vector4D operator*(float f, const Vector4D &r)
	{
		return Vector4D(r.x * f, r.y * f, r.z * f, r.w / f);
	}

	friend Vector4D operator/(float f, const Vector4D &r)
	{
		Assert(f != 0.0f);

		return Vector4D(r.x / f, r.y / f, r.z / f, r.w / f);
	}

	bool operator==(const Vector4D& r)const
	{
		return (x == r.x) && (y == r.y) && (z == r.z) && (w == r.w);
	}
};

struct Quaternion{
	union{
		struct{
			float x;
			float y;
			float z;
			float w;
		};

		struct{
			Vector3D v;
			float w;
		};

		float r[4];
	};

	Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f){}
	Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w){}
	Quaternion(Vector3D& v, float w)
	{
		(*this).v = v;
		(*this).w = w;
	}

	float& operator[](const int n)
	{
		Assert(n < 4);

		return r[n];
	}

	Quaternion& operator=(Quaternion r)
	{
		x = r.x;
		y = r.y;
		z = r.z;
		w = r.w;

		return (*this);
	}

	Quaternion& operator+()
	{
		return (*this);
	}

	Quaternion operator-()const
	{
		return Quaternion(w * -1.0f, x * -1.0f, y * -1.0f, z * -1.0f);
	}

	Quaternion operator+(Quaternion r)const
	{
		return Quaternion(w + r.w, x + r.x, y + r.y, z + r.z);
	}

	Quaternion& operator+=(const Quaternion& r)
	{
		x += r.x;
		y += r.y;
		z += r.z;
		w += r.w;

		return (*this);
	}

	Quaternion operator-(Quaternion& r)const
	{
		return Quaternion(w - r.w, x - r.x, y - r.y, z - r.z);
	}

	Quaternion& operator-=(const Quaternion& r)
	{
		x -= r.x;
		y -= r.y;
		z -= r.z;
		w -= r.w;

		return (*this);
	}

	Quaternion operator*(float r)const
	{
		return Quaternion(w * r, x * r, y * r, z * r);
	}

	Quaternion operator*(Quaternion r)const
	{
		float wx = w * r.w - (v.Dot(r.v));

		Vector3D vx = r.v * w + v * r.w + v.Cross(r.v);

		return Quaternion(vx, wx);
	}

	bool operator==(const Quaternion& r)const
	{
		return (x == r.x) && (y == r.y) && (z == r.z) && (w == r.w);
	}

	float Dot(const Quaternion& r)const
	{
		return w * r.w + x * r.x + y * r.y + z * r.z;
	}

	Quaternion CQ()const
	{
		return Quaternion(w, -x, -y, -z);
	}

	Quaternion& Rotate(float rad, Vector3D axis)
	{
		v = (v.Cross(axis) * sinf(rad) + axis * cosf(rad) + (1.0f - cosf(rad)) * (v.Dot(axis)) * axis);

		return (*this);
	}

	Vector3D ToEuler()const
	{
		float mtx[3][3] = {};

		float x2 = x * x;
		float y2 = y * y;
		float z2 = z * z;

		float xy = x * y;
		float xz = x * z;
		float yz = y * z;
		float wx = w * x;
		float wy = w * y;
		float wz = w * z;

		mtx[0][0] = 1.0f - (2.0f * y2) - (2.0f * z2);
		mtx[0][1] = (2.0f * xy) + (2.0f * wz);

		mtx[1][0] = (2.0f * xy) - (2.0f * wz);
		mtx[1][1] = 1.0f - (2.0f * x2) - (2.0f * z2);

		mtx[2][0] = (2.0f * xz) + (2.0f * wy);
		mtx[2][1] = (2.0f * yz) - (2.0f * wx);
		mtx[2][2] = 1.0f - (2.0f * x2) - (2.0f * y2);

		Vector3D v;

		if(mtx[2][1] == 1.0f){
			v.x = ideaPI * 0.5f;
			v.y = 0.0f;
			v.z = atan2f(mtx[1][0], mtx[0][0]);

		} else if(mtx[2][1] == -1.0f){
			v.x = -ideaPI * 0.5f;
			v.y = 0.0f;
			v.z = atan2f(mtx[1][0], mtx[0][0]);
		} else{
			v.x = asinf(-mtx[2][1]);
			v.y = atan2f(mtx[2][0], mtx[2][2]);
			v.z = atan2f(mtx[0][1], mtx[1][1]);
		}

		return v;
	}

	static Quaternion Euler(float pitch, float yaw, float roll)
	{
		float cr = cosf(roll * 0.5f);
		float cy = cosf(yaw * 0.5f);
		float cp = cosf(pitch * 0.5f);

		float sr = sinf(roll * 0.5f);
		float sy = sinf(yaw * 0.5f);
		float sp = sinf(pitch * 0.5f);

		float qx = cr * cy * sp + sr * sy * cp;
		float qy = cr * sy * cp - sr * cy * sp;
		float qz = sr * cy * cp - cr * sy * sp;
		float qw = cr * cy * cp + sr * sy * sp;

		return Quaternion(qx, qy, qz, qw);
	}

	static Quaternion Euler(Vector3D pitchYawRoll)
	{
		return Euler(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
	}
};

struct Matrix4x4{
	union{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};

		float r[4][4];
	};

	Matrix4x4() : _11(1.0f), _12(0.0f), _13(0.0f), _14(0.0f), _21(0.0f), _22(1.0f), _23(0.0f), _24(0.0f), _31(0.0f), _32(0.0f), _33(1.0f), _34(0.0f), _41(0.0f), _42(0.0f), _43(0.0f), _44(1.0f){}

	Matrix4x4& operator=(const Matrix4x4& r)
	{
		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				this->r[i][j] = r.r[i][j];
			}
		}

		return (*this);
	}

	Matrix4x4& operator+()
	{
		return (*this);
	}

	Matrix4x4 operator-() const
	{
		Matrix4x4 m;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				m.r[i][j] = -this->r[i][j];
			}
		}

		return m;
	}

	Matrix4x4 operator+(const Matrix4x4 &r) const
	{
		Matrix4x4 m;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				m.r[i][j] = this->r[i][j] + r.r[i][j];
			}
		}

		return m;
	}

	Matrix4x4& operator+=(const Matrix4x4& r)
	{
		(*this) = (*this) + r;

		return (*this);
	}

	Matrix4x4 operator-(const Matrix4x4 &r) const
	{
		Matrix4x4 m;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				m.r[i][j] = this->r[i][j] - r.r[i][j];
			}
		}

		return m;
	}

	Matrix4x4& operator-=(const Matrix4x4& r)
	{
		(*this) = (*this) - r;

		return (*this);
	}

	Matrix4x4 operator*(const float& f) const
	{
		Matrix4x4 m;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				m.r[i][j] = this->r[i][j] * f;
			}
		}

		return m;
	}

	Matrix4x4& operator*=(const float& f)
	{
		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				r[i][j] *= f;
			}
		}

		return (*this);
	}

	Matrix4x4 operator*(const Matrix4x4& r) const
	{
		Matrix4x4 m;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				m.r[i][j] = 0.0f;

				for(int k = 0; k < 4; ++k){
					m.r[i][j] += this->r[i][k] * r.r[k][j];
				}
			}
		}

		return m;
	}

	Matrix4x4& operator*=(const Matrix4x4& r)
	{
		(*this) = (*this) * r;

		return (*this);
	}

	Matrix4x4 operator/(const float& f) const
	{
		Assert(f != 0.0f);

		Matrix4x4 m;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				m.r[i][j] = this->r[i][j] / f;
			}
		}

		return m;
	}

	Matrix4x4& operator/=(const float& f)
	{
		Assert(f != 0.0f);

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				r[i][j] /= f;
			}
		}

		return (*this);
	}

	Matrix4x4 Inverse()const;

	static Matrix4x4& Identity()
	{
		static Matrix4x4 matIdentity;

		return matIdentity;
	}
};

float Bezier(float x, Vector2D p1, Vector2D p2, unsigned int n);

struct Lerp{
	static inline float Linear(float p1, float p2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		return p1 * (1.0f - t) + p2 * t;
	}

	static inline Vector2D Linear(Vector2D& v1, Vector2D& v2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector3D Linear(Vector3D& v1, Vector3D& v2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector4D Linear(Vector4D& v1, Vector4D& v2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		return v1 * (1.0f - t) + v2 * t;
	}

	static Quaternion Linear(Quaternion& q1, Quaternion& q2, float t);

	static inline float EaseIn(float p1, float p2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		t *= t;

		return p1 * (1.0f - t) + p2 * t;
	}

	static inline Vector2D EaseIn(Vector2D& v1, Vector2D& v2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		t *= t;

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector3D EaseIn(Vector3D& v1, Vector3D& v2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		t *= t;

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector4D EaseIn(Vector4D& v1, Vector4D& v2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		t *= t;

		return v1 * (1.0f - t) + v2 * t;
	}

	static Quaternion EaseIn(Quaternion& q1, Quaternion& q2, float t);

	static inline float EaseOut(float p1, float p2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		t = t * (2.0f - t);

		return p1 * (1.0f - t) + p2 * t;
	}

	static inline Vector2D EaseOut(Vector2D& v1, Vector2D& v2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		t = t * (2.0f - t);

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector3D EaseOut(Vector3D& v1, Vector3D& v2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		t = t * (2.0f - t);

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector4D EaseOut(Vector4D& v1, Vector4D& v2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		t = t * (2.0f - t);

		return v1 * (1.0f - t) + v2 * t;
	}

	static Quaternion EaseOut(Quaternion& q1, Quaternion& q2, float t);

	static inline float EaseInEaseOut(float p1, float p2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		t = t * t * (3.0f - 2.0f * t);

		return p1 * (1.0f - t) + p2 * t;
	}

	static inline Vector2D EaseInEaseOut(Vector2D& v1, Vector2D& v2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		t = t * t * (3.0f - 2.0f * t);

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector3D EaseInEaseOut(Vector3D& v1, Vector3D& v2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		t = t * t * (3.0f - 2.0f * t);

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector4D EaseInEaseOut(Vector4D& v1, Vector4D& v2, float t)
	{
		if(t > 1.0f){ t = 1.0f; }
		if(t < 0.0f){ t = 0.0f; }

		t = t * t * (3.0f - 2.0f * t);

		return v1 * (1.0f - t) + v2 * t;
	}

	static Quaternion EaseInEaseOut(Quaternion& q1, Quaternion& q2, float t);
};

#endif	// #ifndef INCLUDE_IDEA_IDEAMATH_H