/*==============================================================================
	[Actor3D.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/3D/Actor3D.h"
#include "../../../idea/h/Utility/ideaUtility.h"
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>

//------------------------------------------------------------------------------
// using namespace
//------------------------------------------------------------------------------
using namespace std;
using namespace DirectX;

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
Actor3D::Actor3D()
{
	UpdateWorldMatrix();
}

//------------------------------------------------------------------------------
// デストラクタ
//------------------------------------------------------------------------------
Actor3D::~Actor3D()
{
}

void Actor3D::Init(Vector3D pos)
{
	pos_ = pos;
	scale_ = Vector3D(1.0f, 1.0f, 1.0f);
	rot_ = Vector3D(0.0f, 0.0f, 0.0f);

	UpdateWorldMatrix();
}

//------------------------------------------------------------------------------
// 初期化
// 引数　：X座標(float posX),Y座標(float posY),Z座標(float posZ)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor3D::Init(float posX, float posY, float posZ)
{
	Init(Vector3D(posX, posY, posZ));
}

//------------------------------------------------------------------------------
// 終了
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor3D::UnInit()
{
	pos_ = Vector3D(0.0f, 0.0f, 0.0f);
	scale_ = Vector3D(0.0f, 0.0f, 0.0f);
	rot_ = Vector3D(0.0f, 0.0f, 0.0f);

	UpdateWorldMatrix();
}

void Actor3D::SetPos(Vector3D pos)
{
	pos_ = pos;

	UpdateWorldMatrix();
}

//------------------------------------------------------------------------------
// 位置の設定
// 引数　：X座標(float posX),Y座標(float posY),Z座標(float posZ)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor3D::SetPos(float posX, float posY, float posZ)
{
	SetPos(Vector3D(posX, posY, posZ));
}

void Actor3D::MovePos(Vector3D axis)
{
	pos_ += axis;

	UpdateWorldMatrix();
}

//------------------------------------------------------------------------------
// 位置の移動
// 引数　：X軸の移動量(float axisX),Y軸の移動量(float axisY),
// 　　　　Z軸の移動量(float axisZ)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor3D::MovePos(float axisX, float axisY, float axisZ)
{
	MovePos(Vector3D(axisX, axisY, axisY));
}

void Actor3D::SetScale(Vector3D scale)
{
	scale_ = scale;

	UpdateWorldMatrix();
}

void Actor3D::SetScale(float scaleX, float scaleY, float scaleZ)
{
	SetScale(Vector3D(scaleX, scaleY, scaleZ));
}

void Actor3D::SetRotate(Vector3D rot)
{
	rot_ = rot;

	UpdateWorldMatrix();
}

void Actor3D::SetRotate(float pitch, float yaw, float roll)
{
	SetRotate(Vector3D(pitch, yaw, roll));
}

void Actor3D::MoveRotate(Vector3D rot)
{
	rot_ += rot;

	UpdateWorldMatrix();
}

void Actor3D::MoveRotate(float pitch, float yaw, float roll)
{
	MoveRotate(Vector3D(pitch, yaw, roll));
}

void Actor3D::SetColor(Color color)
{
	SetColor(color.r, color.g, color.b, color.a);
}

void Actor3D::SetColor(float r, float g, float b, float a)
{
	color_.r = max(0.0f, min(1.0f, r));
	color_.g = max(0.0f, min(1.0f, g));
	color_.b = max(0.0f, min(1.0f, b));
	color_.a = max(0.0f, min(1.0f, a));
}

bool Actor3D::CheckHitSphere(Actor3D& target)
{
	float r1 = max(scale_.x, scale_.y);
	r1 = max(r1, scale_.z);
	r1 *= 0.5f;

	float r2 = max(target.scale_.x, target.scale_.y);
	r2 = max(r2, target.scale_.z);
	r2 *= 0.5f;

	Vector3D v = pos_ - target.pos_;

	if((r1 + r2) * (r1 + r2) > v.LengthSquare()){
		return true;
	}

	return false;
}

bool Actor3D::CheckHitBox(Actor3D& target)
{
	UpdateWorldMatrix();
	target.UpdateWorldMatrix();

	float R[3][3], absR[3][3];
	for(int i = 0; i < 3; ++i){
		for(int j = 0; j < 3; ++j){
			R[i][j] = obb_.u[i].Dot(target.obb_.u[j]);
			absR[i][j] = fabsf(R[i][j]) + EPSILON;
		}
	}

	Vector3D t = target.obb_.c - obb_.c;
	t = Vector3D(t.Dot(obb_.u[0]), t.Dot(obb_.u[1]), t.Dot(obb_.u[2]));

	float ra, rb;

	//軸L=A0, L=A1, L=A2判定
	for(int i = 0; i < 3; ++i)
	{
		ra = obb_.e[i];
		rb = target.obb_.e[0] * absR[i][0] + target.obb_.e[1] * absR[i][1] + target.obb_.e[2] * absR[i][2];
		if(fabsf(t[i]) > ra + rb){ return false; }
	}

	//軸L=B0, L=B1, L=B2判定
	for(int i = 0; i < 3; ++i)
	{
		ra = obb_.e[0] * absR[0][i] + obb_.e[1] * absR[1][i] + obb_.e[2] * absR[2][i];
		rb = target.obb_.e[i];
		if(fabsf(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb){ return false; }
	}

	//軸L=A0 X B0判定
	ra = obb_.e[1] * absR[2][0] + obb_.e[2] * absR[1][0];
	rb = target.obb_.e[1] * absR[0][2] + target.obb_.e[2] * absR[0][1];
	if(fabsf(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb){ return false; }

	//軸L=A0 X B1判定
	ra = obb_.e[1] * absR[2][1] + obb_.e[2] * absR[1][1];
	rb = target.obb_.e[0] * absR[0][2] + target.obb_.e[2] * absR[0][0];
	if(fabsf(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb){ return false; }

	//軸L=A0 X B2判定
	ra = obb_.e[1] * absR[2][2] + obb_.e[2] * absR[1][2];
	rb = target.obb_.e[0] * absR[0][1] + target.obb_.e[1] * absR[0][0];
	if(fabsf(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb){ return false; }

	//軸L=A1 X B0判定
	ra = obb_.e[0] * absR[2][0] + obb_.e[2] * absR[0][0];
	rb = target.obb_.e[1] * absR[1][2] + target.obb_.e[2] * absR[1][1];
	if(fabsf(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb){ return false; }

	//軸L=A1 X B1判定
	ra = obb_.e[0] * absR[2][1] + obb_.e[2] * absR[0][1];
	rb = target.obb_.e[0] * absR[1][2] + target.obb_.e[2] * absR[1][0];
	if(fabsf(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb){ return false; }

	//軸L=A1 X B2判定
	ra = obb_.e[0] * absR[2][2] + obb_.e[2] * absR[0][2];
	rb = target.obb_.e[0] * absR[1][1] + target.obb_.e[1] * absR[1][0];
	if(fabsf(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb){ return false; }

	//軸L=A2 X B0判定
	ra = obb_.e[0] * absR[1][0] + obb_.e[1] * absR[0][0];
	rb = target.obb_.e[1] * absR[2][2] + target.obb_.e[2] * absR[2][1];
	if(fabsf(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb){ return false; }

	//軸L=A2 X B1判定
	ra = obb_.e[0] * absR[1][1] + obb_.e[1] * absR[0][1];
	rb = target.obb_.e[0] * absR[2][2] + target.obb_.e[2] * absR[2][0];
	if(fabsf(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb){ return false; }

	//軸L=A2 X B2判定
	ra = obb_.e[0] * absR[1][2] + obb_.e[1] * absR[0][2];
	rb = target.obb_.e[0] * absR[2][1] + target.obb_.e[1] * absR[2][0];
	if(fabsf(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb){ return false; }

	return true;
}

inline void Actor3D::UpdateWorldMatrix()
{
	XMMATRIX matWorld, matScale, matRot;

	// ワールド行列の初期化
	matWorld = XMMatrixIdentity();

	// 拡大縮小処理
	matScale = XMMatrixScaling(scale_.x, scale_.y, scale_.z);


	// 0.0f ～ ideaPI * 2.0fの範囲に正規化
	while(rot_.x < 0.0f)
	{
		rot_.x += ideaPI * 2.0f;
	}
	while(rot_.x > ideaPI * 2.0f)
	{
		rot_.x -= ideaPI * 2.0f;
	}

	while(rot_.y < 0.0f)
	{
		rot_.y += ideaPI * 2.0f;
	}
	while(rot_.y > ideaPI * 2.0f)
	{
		rot_.y -= ideaPI * 2.0f;
	}

	while(rot_.z < 0.0f)
	{
		rot_.z += ideaPI * 2.0f;
	}
	while(rot_.z > ideaPI * 2.0f)
	{
		rot_.z -= ideaPI * 2.0f;
	}

	// 回転処理
	matRot = XMMatrixRotationRollPitchYaw(rot_.x, rot_.y, rot_.z);

	// 行列の合成
	matWorld = XMMatrixMultiply(matWorld, matScale);
	matWorld = XMMatrixMultiply(matWorld, matRot);

	// 平行移動
	matWorld.r[3].m128_f32[0] = pos_.x;
	matWorld.r[3].m128_f32[1] = pos_.y;
	matWorld.r[3].m128_f32[2] = pos_.z;

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			world_.r[i][j] = matWorld.r[i].m128_f32[j];
		}
	}

	// 拡大縮小、平行移動成分を合成しないマトリクスを用意する
	XMMATRIX mat = XMMatrixIdentity();
	mat = XMMatrixMultiply(mat, matRot);

	// OBBの計算
	obb_.c = pos_;
	obb_.u[0] = Vector3D(mat.r[0].m128_f32[0], mat.r[0].m128_f32[1], mat.r[0].m128_f32[2]);
	obb_.u[1] = Vector3D(mat.r[1].m128_f32[0], mat.r[1].m128_f32[1], mat.r[1].m128_f32[2]);
	obb_.u[2] = Vector3D(mat.r[2].m128_f32[0], mat.r[2].m128_f32[1], mat.r[2].m128_f32[2]);
	obb_.e = scale_ * 0.5f;
}