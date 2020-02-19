/*==============================================================================
	[Actor2D.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_ACTOR2D_H
#define INCLUDE_IDEA_ACTOR2D_H

#include "../Utility/ideaMath.h"
#include "../Utility/ideaColor.h"

//------------------------------------------------------------------------------
// クラス名　：Actor2D
// クラス概要：2D上での位置と大きさ,角度を持つクラス
// 　　　　　　コリジョンやフォロー機能を持つ
//------------------------------------------------------------------------------
class Actor2D{
public:
	Actor2D();
	virtual ~Actor2D();

	// 初期化
	virtual void Init(Vector2D pos, Vector2D size);
	virtual void Init(Vector2D pos, float width, float height);
	virtual void Init(float posX, float posY, float width, float height);

	// 終了
	virtual void UnInit();

	// 座標の設定
	void SetPos(Vector2D pos);
	void SetPos(float posX, float posY);

	// 座標の移動
	void MovePos(Vector2D axis);
	void MovePos(float axisX, float axisY);

	// 座標の取得
	Vector2D& GetPos(){ return pos_; }
	float GetPosX()const{ return pos_.x; }
	float GetPosY()const{ return pos_.y; }

	// 前フレームの座標の取得
	float GetPreX()const{ return prePos_.x; }
	float GetPreY()const{ return prePos_.y; }

	// サイズの設定
	void SetSize(Vector2D size);
	void SetSize(float width, float height);

	// サイズの取得
	Vector2D GetSize()const{ return size_; }
	float GetHalfWidth()const{ return size_.x * 0.5f; }
	float GetHalfHeight()const{ return size_.y * 0.5f; }
	float GetWidth()const{ return size_.x; }
	float GetHeight()const{ return size_.y; }

	// 回転角の設定
	void SetRotate(float rad);

	// 回転
	void MoveRotate(float rad);

	// 回転角の取得
	float GetRoteate()const{ return rad_; }

	// 色の設定
	void SetColor(Color color);
	void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

	// 矩形の接触判定
	bool CheckHitRect(const Actor2D& target);

	// 円の接触判定
	bool CheckHitCircle(const Actor2D& target);

	// 位置関係取得
	float GetRelation(const Actor2D& target);
	float GetPreFrameRelation(const Actor2D& target);

	// 追従するターゲットの設定
	void FollowActor(Actor2D& target);

	// フォロワーをやめる
	void QuitFollower();

protected:
	Vector2D pos_;			// 座標
	Vector2D prePos_;		// 前フレームの座標

	Vector2D size_;			// サイズ

	float rad_;				// 角度

	bool bReversedU_;		// 反転フラグ(U)
	bool bReversedV_;		// 反転フラグ(V)

	Color color_;			// 色

private:
	friend class Actor2DManager;

	bool bRegistered_;		// 登録フラグ
	Actor2D* pPrev_;		// 前のノード
	Actor2D* pNext_;		// 次のノード
	Actor2D* pLeader_;		// リーダーのポインター
	Actor2D* pFollower_;	// フォロワーのポインター

	// フォロワーの位置を移動
	void MoveFollower(Vector2D axis);

	// フォロワーの解散
	void ResetFollower(Actor2D* pTarget);

	// 座標の保持
	void WriteDownPosition();
};

#endif	// #ifndef INCLUDE_IDEA_ACTOR2D_H