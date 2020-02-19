/*==============================================================================
	[Actor3D.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_EIDOS_ACTOR3D_H
#define INCLUDE_EIDOS_ACTOR3D_H

#include "../../../idea/h/Utility/ideaMath.h"
#include "../../../idea/h/Utility/ideaColor.h"

//------------------------------------------------------------------------------
// クラス名　：Actor3D
// クラス概要：3D上での位置,大きさ,回転,色を持つクラス
// 　　　　　　コリジョン機能を持つ
//------------------------------------------------------------------------------
class Actor3D{
public:
	Actor3D();
	virtual ~Actor3D();

	// 初期化
	virtual void Init(Vector3D pos);
	virtual void Init(float posX, float posY, float posZ);
	// 終了
	virtual void UnInit();

	// 位置の設定
	void SetPos(Vector3D pos);
	void SetPos(float posX, float posY, float posZ);

	// 位置の移動
	void MovePos(Vector3D axis);
	void MovePos(float axisX, float axisY, float axisZ);

	// 位置の取得
	Vector3D GetPos()const{ return pos_; }
	float GetPosX()const{ return pos_.x; }
	float GetPosY()const{ return pos_.y; }
	float GetPosZ()const{ return pos_.z; }

	// 拡縮倍率の設定
	void SetScale(Vector3D scale);
	void SetScale(float scaleX, float scaleY, float scaleZ);

	// 拡縮倍率の取得
	Vector3D GetScale()const{ return scale_; }
	float GetScaleX()const{ return scale_.x; }
	float GetScaleY()const{ return scale_.y; }
	float GetScaleZ()const{ return scale_.z; }

	// 回転角の設定
	void SetRotate(Vector3D rot);
	void SetRotate(float pitch, float yaw, float roll);

	// 回転
	void MoveRotate(Vector3D rot);
	void MoveRotate(float pitch, float yaw, float roll);

	// 回転角の取得
	Vector3D GetRotele()const{ return rot_; }
	float GetRotateX()const{ return rot_.x; }
	float GetRotateY()const{ return rot_.y; }
	float GetRotateZ()const{ return rot_.z; }

	// 色の設定
	void SetColor(Color color);
	void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

	// 色の取得
	float GetColorR()const{ return color_.r; }
	float GetColorG()const{ return color_.g; }
	float GetColorB()const{ return color_.b; }
	float GetColorA()const{ return color_.a; }

	//DirectX::XMFLOAT4X4* GetWorldMatrixPtr()const{ return pMatWorld_; }

	// 球の接触判定
	bool CheckHitSphere(Actor3D& target);

	// 箱の接触判定
	bool CheckHitBox(Actor3D& target);

protected:
	Vector3D pos_;		// 位置

	Vector3D scale_;	// 拡縮倍率

	Vector3D rot_;		// 回転角

	Color color_;		// 色

	// OBB
	struct OBB{
		Vector3D c;
		Vector3D u[3];
		Vector3D e;
	}obb_;

	Matrix4x4 world_;

	//DirectX::XMFLOAT4X4* pMatWorld_;	// ワールドマトリクス

private:
	// ワールド的陸の更新
	inline void UpdateWorldMatrix();
};

#endif	// #ifndef INCLUDE_EIDOS_ACTOR3D_H