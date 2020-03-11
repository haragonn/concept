#ifndef INCLUDE_IDEA_IDEAMATH_H
#define INCLUDE_IDEA_IDEAMATH_H

#include "ideaUtility.h"

#include <math.h>

// 定数群
namespace ideaMath{
	constexpr float PI = 3.141592654f;			// π
	constexpr float ROOT2 = 1.414213562f;		// √2
	constexpr float ROOT3 = 1.732050807f;		// √3
	constexpr float ROOT4 = 2.000000000f;		// √4
	constexpr float ROOT5 = 2.236067977f;		// √5
	constexpr float ROOT6 = 2.449489742f;		// √6
	constexpr float ROOT7 = 2.645751311f;		// √7
	constexpr float ROOT8 = 2.828427124f;		// √8
	constexpr float ROOT9 = 3.000000000f;		// √9
	constexpr float ROOT10 = 3.162277660f;		// √10
	constexpr float EPSILON = 1.192092896e-07f;	// floatの誤差
}

// 値を比較し大きい方の値を返す
template <typename T>
inline T Max(T a, T b)
{
	return (a > b) ? a : b;
}

// 値を比較し小さい方の値を返す
template <typename T>
inline T Min(T a, T b)
{
	return (a < b) ? a : b;
}

// 値を指定された範囲内に収める
template <typename T>
inline T Clamp(T val, T min, T max)
{
	return Max(min, Min(max, val));
}

// 値の符号を返す
template <typename T>
inline T Sign(T val)
{
	return (val < T(0)) ? T(-1) : T(1);
}

// 値の絶対値を返す
template <typename T>
inline T Abs(T val)
{
	return val * Sign(val);
}

// Degree角からRadian角へ変換
inline constexpr float DegreeToRadian(float dig)
{
	return dig / 180.0f * ideaMath::PI;
}

// Radian角からDegree角へ変換
inline constexpr float RadianToDegree(float rad)
{
	return rad / ideaMath::PI * 180.0f;
}

// 秒からフレームへ変換
inline constexpr unsigned int SecondToFrame(unsigned int sec, unsigned int fps = 60U)
{
	return sec * fps;
}

// 分からフレームへ変換
inline constexpr unsigned int MinuteToFrame(unsigned int min, unsigned int fps = 60U)
{
	return SecondToFrame(min * 60U);
}

// 時間からフレームへ変換
inline constexpr unsigned int HourToFrame(unsigned int hour, unsigned int fps = 60U)
{
	return MinuteToFrame(hour * 60U);
}

// フレームから秒へ変換
inline constexpr unsigned int FrameToSecond(unsigned int frame, unsigned int fps = 60U)
{
	Assert(fps);
	return frame / SecondToFrame(1U, fps);
}

// フレームから分へ変換
inline constexpr unsigned int FrameToMinute(unsigned int frame, unsigned int fps = 60U)
{
	Assert(fps);
	return frame / MinuteToFrame(1U, fps);
}

// フレームから時間へ変換
inline constexpr unsigned int FrameToHour(unsigned int frame, unsigned int fps = 60U)
{
	Assert(fps);
	return frame / HourToFrame(1U, fps);
}

// 2次元ベクトル
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

	float& operator[](const int idx)
	{
		Assert(idx < 2);

		return r[idx];
	}

	Vector2D& operator=(const Vector2D& v)
	{
		x = v.x;
		y = v.y;

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

	Vector2D operator+(const Vector2D& v)const
	{
		return Vector2D(x + v.x, y + v.y);
	}

	Vector2D& operator+=(const Vector2D& v)
	{
		x += v.x;
		y += v.y;

		return (*this);
	}

	Vector2D operator-(const Vector2D& v)const
	{
		return Vector2D(x - v.x, y - v.y);
	}

	Vector2D& operator-=(const Vector2D& v)
	{
		x -= v.x;
		y -= v.y;

		return (*this);
	}

	Vector2D operator*(float f)const
	{
		return Vector2D(x * f, y * f);
	}

	Vector2D& operator*=(float f)
	{
		x *= f;
		y *= f;

		return (*this);
	}

	Vector2D operator*(const Vector2D& v)const
	{
		return Vector2D(x * v.x, y * v.y);
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

	Vector2D operator/(const Vector2D& v)const
	{
		Assert(v.x != 0.0f && v.y != 0.0f);

		return Vector2D(x / v.x, y / v.y);
	}

	friend Vector2D operator*(float f, const Vector2D& v)
	{
		return Vector2D(v.x * f, v.y * f);
	}

	friend Vector2D operator/(float f, const Vector2D& v)
	{
		Assert(f != 0.0f);

		return Vector2D(v.x / f, v.y / f);
	}

	bool operator==(const Vector2D& v)const
	{
		return (x == v.x) && (y == v.y);
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

		if(len == 0.0f){ return Vector2D(0.0f, 0.0f); }

		return  Vector2D(x / len, y / len);
	}

	float Dot(const Vector2D& v)const
	{
		return x * v.x + y * v.y;
	}

	float Cross(const Vector2D& v)const
	{
		return x * v.y - y * v.x;
	}
};

// ベジェ補間
inline float Bezier(float x, Vector2D p1, Vector2D p2, unsigned int n)
{
	if(p1.x == p1.y && p2.x == p2.y){ return x; }

	const float k0 = 1.0f + 3.0f * p1.x - 3.0f * p2.x;
	const float k1 = 3.0f * p2.x - 6.0f * p1.x;
	const float k2 = 3.0f * p1.x;

	float t = x;

	for(unsigned int i = 0U; i < n; ++i){
		float ft = k0 * t * t * t + k1 * t * t + k2 * t - x;

		if(abs(ft) < 0.000001f){ break; }

		t -= ft * 0.5f;
	}

	float r = 1.0f - t;

	return t * t * t + 3.0f * t * t * r * p2.y + 3.0f * t * r * r * p1.y;
}


// 3次元ベクトル
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

	float& operator[](const int idx)
	{
		Assert(idx < 3);

		return r[idx];
	}

	Vector3D& operator=(const Vector3D& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;

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

	Vector3D operator+(const Vector3D& v)const
	{
		return Vector3D(x + v.x, y + v.y, z + v.z);
	}

	Vector3D& operator+=(const Vector3D& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return (*this);
	}

	Vector3D operator-(const Vector3D& v)const
	{
		return Vector3D(x - v.x, y - v.y, z - v.z);
	}

	Vector3D& operator-=(const Vector3D& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

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

	Vector3D operator*(const Vector3D& v)const
	{
		return Vector3D(x * v.x, y * v.y, z * v.z);
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

	Vector3D operator/(const Vector3D& v)const
	{
		Assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f);

		return Vector3D(x / v.x, y / v.y, z / v.z);
	}

	Vector3D& operator/=(const Vector3D& v)
	{
		(*this) = (*this) / v;

		return (*this);
	}

	friend Vector3D operator*(float f, const Vector3D& v)
	{
		return Vector3D(v.x * f, v.y * f, v.z * f);
	}

	friend Vector3D operator/(float f, const Vector3D& v)
	{
		Assert(f != 0.0f);

		return Vector3D(v.x / f, v.y / f, v.z / f);
	}

	bool operator==(const Vector3D& v)const
	{
		return (x == v.x) && (y == v.y) && (z == v.z);
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

		if(len == 0.0f){ return Vector3D(0.0f, 0.0f, 0.0f); }

		return  Vector3D(x / len, y / len, z / len);
	}

	float Dot(const Vector3D& v)const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	Vector3D Cross(const Vector3D& v)const
	{
		return Vector3D((y * v.z) - (z * v.y), (z * v.x) - (x * v.z), (x * v.y) - (y * v.x));
	}
};


// 4次元ベクトル
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

	float& operator[](const int idx)
	{
		Assert(idx < 4);

		return r[idx];
	}

	Vector4D& operator=(const Vector4D& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;

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

	Vector4D operator+(const Vector4D& v)const
	{
		return Vector4D(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	Vector4D& operator+=(const Vector4D& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;

		return (*this);
	}

	Vector4D operator-(const Vector4D& v)const
	{
		return Vector4D(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	Vector4D& operator-=(const Vector4D& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;

		return (*this);
	}

	Vector4D operator*(float f)const
	{
		return Vector4D(x * f, y * f, z * f, w * f);
	}

	Vector4D& operator*=(float f)
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;

		return (*this);
	}

	Vector4D operator*(const Vector4D& v)const
	{
		return Vector4D(x * v.x, y * v.y, z * v.z, w * v.w);
	}

	Vector4D& operator*=(const Vector4D& v)
	{
		(*this) = (*this) * v;

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

	Vector4D operator/(const Vector4D& v)const
	{
		Assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f && v.w == 0.0f);

		return Vector4D(x / v.x, y / v.y, z / v.z, w / v.w);
	}

	Vector4D& operator/=(const Vector4D& v)
	{
		(*this) = (*this) / v;

		return (*this);
	}

	friend Vector4D operator*(float f, const Vector4D& v)
	{
		return Vector4D(v.x * f, v.y * f, v.z * f, v.w / f);
	}

	friend Vector4D operator/(float f, const Vector4D& v)
	{
		Assert(f != 0.0f);

		return Vector4D(v.x / f, v.y / f, v.z / f, v.w / f);
	}

	bool operator==(const Vector4D& v)const
	{
		return (x == v.x) && (y == v.y) && (z == v.z) && (w == v.w);
	}
};


// 四元数
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

	float& operator[](const int idx)
	{
		Assert(idx < 4);

		return r[idx];
	}

	Quaternion& operator=(Quaternion q)
	{
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;

		return (*this);
	}

	Quaternion& operator+()
	{
		return (*this);
	}

	Quaternion operator-()const
	{
		return Quaternion(x * -1.0f, y * -1.0f, z * -1.0f, w * -1.0f);
	}

	Quaternion operator+(Quaternion q)const
	{
		return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
	}

	Quaternion& operator+=(const Quaternion& q)
	{
		x += q.x;
		y += q.y;
		z += q.z;
		w += q.w;

		return (*this);
	}

	Quaternion operator-(Quaternion& q)const
	{
		return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
	}

	Quaternion& operator-=(const Quaternion& q)
	{
		x -= q.x;
		y -= q.y;
		z -= q.z;
		w -= q.w;

		return (*this);
	}

	Quaternion operator*(float f)const
	{
		return Quaternion(x * f, y * f, z * f, w * f);
	}

	Quaternion operator*(Quaternion q)const
	{
		float wx = w * q.w - (v.Dot(q.v));

		Vector3D vx = q.v * w + v * q.w + v.Cross(q.v);

		return Quaternion(vx, wx);
	}

	bool operator==(const Quaternion& q)const
	{
		return (x == q.x) && (y == q.y) && (z == q.z) && (w == q.w);
	}

	float Dot(const Quaternion& q)const
	{
		return x * q.x + y * q.y + z * q.z + w * q.w;
	}

	Quaternion CQ()const
	{
		return Quaternion(-x, -y, -z, w);
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

		Vector3D res;

		if(mtx[2][1] == 1.0f){
			res.x = ideaMath::PI * 0.5f;
			res.y = 0.0f;
			res.z = atan2f(mtx[1][0], mtx[0][0]);

		} else if(mtx[2][1] == -1.0f){
			res.x = -ideaMath::PI * 0.5f;
			res.y = 0.0f;
			res.z = atan2f(mtx[1][0], mtx[0][0]);
		} else{
			res.x = asinf(-mtx[2][1]);
			res.y = atan2f(mtx[2][0], mtx[2][2]);
			res.z = atan2f(mtx[0][1], mtx[1][1]);
		}

		return res;
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

// 4x4行列
struct Matrix4x4{
#pragma pack(push,1)
	union{
		struct
		{
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};

		float r[4][4];
	};
#pragma pack(pop)

	Matrix4x4() : m00(1.0f), m01(0.0f), m02(0.0f), m03(0.0f), m10(0.0f), m11(1.0f), m12(0.0f), m13(0.0f), m20(0.0f), m21(0.0f), m22(1.0f), m23(0.0f), m30(0.0f), m31(0.0f), m32(0.0f), m33(1.0f){}

	Matrix4x4(float _m00, float _m01, float _m02, float _m03, float _m10, float _m11, float _m12, float _m13, float _m20, float _m21, float _m22, float _m23, float _m30, float _m31, float _m32, float _m33) : m00(_m00), m01(_m01), m02(_m02), m03(_m03), m10(_m10), m11(_m11), m12(_m12), m13(_m13), m20(_m20), m21(_m21), m22(_m22), m23(_m23), m30(_m30), m31(_m31), m32(_m32), m33(_m33){}

	Matrix4x4& operator=(const Matrix4x4& m)
	{
		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				this->r[i][j] = m.r[i][j];
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

	Matrix4x4 operator+(const Matrix4x4& m) const
	{
		Matrix4x4 res;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				res.r[i][j] = this->r[i][j] + m.r[i][j];
			}
		}

		return res;
	}

	Matrix4x4& operator+=(const Matrix4x4& m)
	{
		(*this) = (*this) + m;

		return (*this);
	}

	Matrix4x4 operator-(const Matrix4x4& m) const
	{
		Matrix4x4 res;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				res.r[i][j] = this->r[i][j] - m.r[i][j];
			}
		}

		return res;
	}

	Matrix4x4& operator-=(const Matrix4x4& m)
	{
		(*this) = (*this) - m;

		return (*this);
	}

	Matrix4x4 operator*(const float& f) const
	{
		Matrix4x4 res;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				res.r[i][j] = this->r[i][j] * f;
			}
		}

		return res;
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

	Vector4D operator*(const Vector4D& v) const
	{
		Vector4D res;

		res.x = this->m00 * v.x + this->m01 * v.y + this->m02 * v.z + this->m03 * v.w;
		res.y = this->m10 * v.x + this->m11 * v.y + this->m12 * v.z + this->m13 * v.w;
		res.z = this->m20 * v.x + this->m21 * v.y + this->m22 * v.z + this->m23 * v.w;
		res.w = this->m30 * v.x + this->m31 * v.y + this->m32 * v.z + this->m33 * v.w;

		return res;
	}

	Matrix4x4 operator*(const Matrix4x4& m) const
	{
		Matrix4x4 res;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				res.r[i][j] = 0.0f;
				for(int k = 0; k < 4; ++k){
					res.r[i][j] += this->r[i][k] * m.r[k][j];
				}
			}
		}

		return res;
	}

	Matrix4x4& operator*=(const Matrix4x4& m)
	{
		(*this) = (*this) * m;

		return (*this);
	}

	Matrix4x4 operator/(const float& f) const
	{
		Assert(f != 0.0f);

		Matrix4x4 res;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				res.r[i][j] = this->r[i][j] / f;
			}
		}

		return res;
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

	static Matrix4x4& Identity()
	{
		static Matrix4x4 s_matIdentity;

		return s_matIdentity;
	}

	static Matrix4x4 MatrixTranslation(float x, float y, float z)
	{
		Matrix4x4 res;

		res.m03 = x;
		res.m13 = y;
		res.m23 = z;

		return res;
	}

	static Matrix4x4 MatrixTranslation(Vector3D t)
	{
		return MatrixTranslation(t.x, t.y, t.z);
	}

	static Matrix4x4 MatrixRotationPitch(float pitch)
	{
		Matrix4x4 res;

		res.m11 = cosf(pitch);
		res.m12 = -sinf(pitch);
		res.m21 = sinf(pitch);
		res.m22 = cosf(pitch);

		return res;
	}

	static Matrix4x4 MatrixRotationYaw(float yaw)
	{
		Matrix4x4 res;

		res.m00 = cosf(yaw);
		res.m02 = sinf(yaw);
		res.m20 = -sinf(yaw);
		res.m22 = cosf(yaw);

		return res;
	}

	static Matrix4x4 MatrixRotationRoll(float roll)
	{
		Matrix4x4 res;

		res.m00 = cosf(roll);
		res.m01 = -sinf(roll);
		res.m10 = sinf(roll);
		res.m11 = cosf(roll);

		return res;
	}

	static Matrix4x4 MatrixRotationPitchYawRoll(float pitch, float yaw, float roll)
	{
		Matrix4x4 res;

		res = MatrixRotationPitch(pitch) * MatrixRotationYaw(yaw) * MatrixRotationRoll(roll);

		return res;
	}
	static Matrix4x4 MatrixRotationRoll(Vector3D pitchYawRoll)
	{
		return MatrixRotationPitchYawRoll(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
	}

	static Matrix4x4 MatrixScaling(float x, float y, float z)
	{
		Matrix4x4 res;

		res.m00 = x;
		res.m11 = y;
		res.m22 = z;

		return res;
	}

	static Matrix4x4 MatrixScaling(Vector3D s)
	{
		return MatrixScaling(s.x, s.y, s.z);
	}

	Matrix4x4 Inverse()const
	{
		Matrix4x4 res;
		Matrix4x4 tmp = (*this);
		float det;

		for(int i = 0; i < 4; ++i){
			if(r[i][i] == 0.0f){ return tmp; }
			det = 1 / tmp.r[i][i];

			for(int j = 0; j < 4; ++j){
				tmp.r[i][j] *= det;
				res.r[i][j] *= det;
			}

			for(int j = 0; j < 4; ++j){
				if(i != j){
					det = tmp.r[j][i];
					for(int k = 0; k < 4; ++k){
						tmp.r[j][k] -= tmp.r[i][k] * det;
						res.r[j][k] -= res.r[i][k] * det;
					}
				}
			}
		}

		return res;
	}

	Matrix4x4 Transpose()const
	{
		Matrix4x4 res;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				res.r[i][j] = r[j][i];
			}
		}

		return res;
	}
};

// 補間
struct Lerp{
	static inline float Linear(float p1, float p2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return p1 * (1.0f - t) + p2 * t;
	}

	static inline Vector2D Linear(Vector2D& v1, Vector2D& v2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector3D Linear(Vector3D& v1, Vector3D& v2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector4D Linear(Vector4D& v1, Vector4D& v2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return v1 * (1.0f - t) + v2 * t;
	}

	static Quaternion Linear(Quaternion& q1, Quaternion& q2, float t);

	static inline float EaseIn(float p1, float p2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		t *= t;

		return p1 * (1.0f - t) + p2 * t;
	}

	static inline Vector2D EaseIn(Vector2D& v1, Vector2D& v2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		t *= t;

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector3D EaseIn(Vector3D& v1, Vector3D& v2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		t *= t;

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector4D EaseIn(Vector4D& v1, Vector4D& v2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		t *= t;

		return v1 * (1.0f - t) + v2 * t;
	}

	static Quaternion EaseIn(Quaternion& q1, Quaternion& q2, float t);

	static inline float EaseOut(float p1, float p2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		t = t * (2.0f - t);

		return p1 * (1.0f - t) + p2 * t;
	}

	static inline Vector2D EaseOut(Vector2D& v1, Vector2D& v2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		t = t * (2.0f - t);

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector3D EaseOut(Vector3D& v1, Vector3D& v2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		t = t * (2.0f - t);

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector4D EaseOut(Vector4D& v1, Vector4D& v2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		t = t * (2.0f - t);

		return v1 * (1.0f - t) + v2 * t;
	}

	static Quaternion EaseOut(Quaternion& q1, Quaternion& q2, float t);

	static inline float EaseInEaseOut(float p1, float p2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		t = t * t * (3.0f - 2.0f * t);

		return p1 * (1.0f - t) + p2 * t;
	}

	static inline Vector2D EaseInEaseOut(Vector2D& v1, Vector2D& v2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		t = t * t * (3.0f - 2.0f * t);

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector3D EaseInEaseOut(Vector3D& v1, Vector3D& v2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		t = t * t * (3.0f - 2.0f * t);

		return v1 * (1.0f - t) + v2 * t;
	}

	static inline Vector4D EaseInEaseOut(Vector4D& v1, Vector4D& v2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		t = t * t * (3.0f - 2.0f * t);

		return v1 * (1.0f - t) + v2 * t;
	}

	static Quaternion EaseInEaseOut(Quaternion& q1, Quaternion& q2, float t);
};

#endif	// #ifndef INCLUDE_IDEA_IDEAMATH_H