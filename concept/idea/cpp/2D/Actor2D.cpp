/*==============================================================================
	[Actor2D.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/2D/Actor2D.h"
#include "../../h/2D/Actor2DManager.h"
#include "../../h/Utility/ideaMath.h"
#include <algorithm>

using namespace std;

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
Actor2D::Actor2D() :
	bReversedU_(false),
	bReversedV_(false),
	rad_(0.0f),
	bRegistered_(false),
	pPrev_(nullptr),
	pNext_(nullptr),
	pLeader_(nullptr),
	pFollower_(nullptr)
{
	Actor2DManager::Instance().Register(this);
}

//------------------------------------------------------------------------------
// デストラクタ
//------------------------------------------------------------------------------
Actor2D::~Actor2D()
{
	UnInit();
	Actor2DManager::Instance().UnRegister(this);
}

void Actor2D::Init(Vector2D pos, Vector2D size)
{
	SetPos(pos);
	SetSize(size);
	SetRotate(0.0f);
}

void Actor2D::Init(Vector2D pos, float width, float height)
{
	SetPos(pos);
	SetSize(Vector2D(width, height));
	SetRotate(0.0f);
}

//------------------------------------------------------------------------------
// 初期化
// 引数　：X座標(float posX),Y座標(float posY),
// 　　　　幅(float width),高さ(float height)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2D::Init(float posX, float posY, float width, float height)
{
	SetPos(Vector2D(posX, posY));
	SetSize(Vector2D(width, height));
	SetRotate(0.0f);
}

//------------------------------------------------------------------------------
// 終了
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2D::UnInit()
{
	// 初期化
	pos_ = Vector2D(0.0f, 0.0f);
	prePos_ = Vector2D(0.0f, 0.0f);
	size_ = Vector2D(0.0f, 0.0f);
	rad_ = 0.0f;
	color_ = Color(0.0f, 0.0f, 0.0f, 0.0f);

	// フォロー関係の解散
	if(pLeader_){
		QuitFollower();
	} else{
		ResetFollower(pFollower_);
	}
}
void Actor2D::SetPos(Vector2D pos)
{
	// 位置の保持
	Vector2D tPos = pos_;

	pos_ = pos;

	// リーダーならばフォロワーを移動させる
	if(pFollower_ && !pLeader_){ pFollower_->MoveFollower(pos_ - tPos); }
}
//------------------------------------------------------------------------------
// 位置の設定
// 引数　：X座標(float posX),Y座標(float posY)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2D::SetPos(float posX, float posY)
{
	SetPos(Vector2D(posX, posY));
}

void Actor2D::MovePos(Vector2D axis)
{
	pos_ += axis;

	// リーダーならばフォロワーを移動させる
	if(pFollower_ && !pLeader_){ pFollower_->MoveFollower(axis); }
}

//------------------------------------------------------------------------------
// 位置の移動
// 引数　：X軸の移動量(float axisX),Y軸の移動量(float axisY)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2D::MovePos(float axisX, float axisY)
{
	MovePos(Vector2D(axisX, axisY));
}

//------------------------------------------------------------------------------
// サイズの設定
// 引数　：サイズ(Vector2D size)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2D::SetSize(Vector2D size)
{
	size_ = size;

	// 値がマイナスの時は反転フラグをオンにして正数にする
	bReversedU_ = false;

	if(size_.x < 0.0f){
		size_.x *= -1.0f;
		bReversedU_ = true;
	}

	bReversedV_ = false;

	if(size_.y < 0.0f){
		size_.y *= -1.0f;
		bReversedV_ = true;
	}
}

//------------------------------------------------------------------------------
// サイズの設定
// 引数　：幅(float width),高さ(float height)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2D::SetSize(float width, float height)
{
	SetSize(Vector2D(width, height));
}

//------------------------------------------------------------------------------
// 回転角の設定
// 引数　：回転角(float rad)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2D::SetRotate(float rad)
{
	rad_ = rad;
}

//------------------------------------------------------------------------------
// 回転
// 引数　：回転量(float rad)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2D::MoveRotate(float rad)
{
	rad_ += rad;
}

void Actor2D::SetColor(Color color)
{
	SetColor(color.r, color.g, color.b, color.a);
}

//------------------------------------------------------------------------------
// 色の設定
// 引数　：赤成分(float r),緑成分(float g),青成分(float b),a値(float a)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2D::SetColor(float r, float g, float b, float a)
{
	color_.r = max(0.0f, min(1.0f, r));
	color_.g = max(0.0f, min(1.0f, g));
	color_.b = max(0.0f, min(1.0f, b));
	color_.a = max(0.0f, min(1.0f, a));
}

//------------------------------------------------------------------------------
// 矩形の接触判定
// 引数　：対象となるActor2D(const Actor2D& target)
// 戻り値：接触判定
//------------------------------------------------------------------------------
bool Actor2D::CheckHitRect(const Actor2D& target)
{
	if(abs(rad_) < EPSILON && abs(target.rad_) < EPSILON){	// 回転していない矩形同士かどうか
		// 頂点情報
		float x1 = pos_.x - size_.x * 0.5f;
		float x2 = pos_.x + size_.x * 0.5f;
		float y1 = pos_.y - size_.y * 0.5f;
		float y2 = pos_.y + size_.y * 0.5f;
		float x3 = target.pos_.x - target.size_.x * 0.5f;
		float x4 = target.pos_.x + target.size_.x * 0.5f;
		float y3 = target.pos_.y - target.size_.y * 0.5f;
		float y4 = target.pos_.y + target.size_.y * 0.5f;

		// 矩形判定
		if(x1 < x4 && x2 > x3&& y1 < y4 && y2 > y3){ return true; }

		// 移動量の計算
		float movementX = fabsf((pos_.x - prePos_.x) + (target.pos_.x - target.prePos_.x));
		float movementY = fabsf((pos_.y - prePos_.y) + (target.pos_.y - target.prePos_.y));

		// 当たり判定を飛び越えていないか
		float width = size_.x + target.size_.x;
		float height = size_.y + target.size_.y;
		if(movementX < width && movementY < height){ return false; }

		// 当たり判定の大きさに合わせ移動量を分解する
		int division = 1;

		if(movementX - size_.x > movementY - size_.y){
			division = (int)(movementX / size_.x) + 1;
		} else{
			division = (int)(movementY / size_.y) + 1;
		}

		float axisX1 = (pos_.x - prePos_.x) / division;
		float axisY1 = (pos_.y - prePos_.y) / division;
		float axisX2 = (target.pos_.x - target.prePos_.x) / division;
		float axisY2 = (target.pos_.y - target.prePos_.y) / division;

		// 前フレームの頂点情報
		x1 = prePos_.x - size_.x * 0.5f;
		x2 = prePos_.x + size_.x * 0.5f;
		y1 = prePos_.y - size_.y * 0.5f;
		y2 = prePos_.y + size_.y * 0.5f;
		x3 = target.prePos_.x - target.size_.x * 0.5f;
		x4 = target.prePos_.x + target.size_.x * 0.5f;
		y3 = target.prePos_.y - target.size_.y * 0.5f;
		y4 = target.prePos_.y + target.size_.y * 0.5f;

		// 少しずつ動かしながら判定
		for(int i = 0; i < division; ++i){
			x1 += axisX1;
			x2 += axisX1;
			y1 += axisY1;
			y2 += axisY1;
			x3 += axisX2;
			x4 += axisX2;
			y3 += axisY2;
			y4 += axisY2;
			// 矩形判定
			if(x1 < x4 && x2 > x3&& y1 < y4 && y2 > y3){ return true; }
		}
	} else{
		// 頂点情報の計算(上を＋とする)
		float vx[4] = {};
		float vy[4] = {};
		float x, y, axisX, axisY;
		float tSin = sinf(rad_);
		float tCos = cosf(rad_);

		for(int i = 4 - 1; i >= 0; --i){
			x = (i % 2) ? size_.x * 0.5f : -(size_.x * 0.5f);
			y = (i < 2) ? size_.y * 0.5f : -(size_.y * 0.5f);
			axisX = x * tCos - y * tSin;
			axisY = x * tSin + y * tCos;
			vx[i] = axisX + pos_.x;
			vy[i] = axisY + pos_.y;
		}

		float tvx[4] = {};
		float tvy[4] = {};
		tSin = sinf(target.rad_);
		tCos = cosf(target.rad_);

		for(int i = 4 - 1; i >= 0; --i){
			x = (i % 2) ? target.size_.x * 0.5f : -(target.size_.x * 0.5f);
			y = (i < 2) ? target.size_.y * 0.5f : -(target.size_.y * 0.5f);
			axisX = x * tCos - y * tSin;
			axisY = x * tSin + y * tCos;
			tvx[i] = axisX + target.pos_.x;
			tvy[i] = axisY + target.pos_.y;
		}

		// 右回りベクトル
		Vector2D vec1 = Vector2D(vx[1], vy[1]) - Vector2D(vx[0], vy[0]);
		Vector2D vec2 = Vector2D(vx[3], vy[3]) - Vector2D(vx[1], vy[1]);
		Vector2D vec3 = Vector2D(vx[2], vy[2]) - Vector2D(vx[3], vy[3]);
		Vector2D vec4 = Vector2D(vx[0], vy[0]) - Vector2D(vx[2], vy[2]);

		// 頂点から頂点へのベクトル
		Vector2D vec11 = Vector2D(tvx[0], tvy[0]) - Vector2D(vx[0], vy[0]);
		Vector2D vec21 = Vector2D(tvx[0], tvy[0]) - Vector2D(vx[1], vy[1]);
		Vector2D vec31 = Vector2D(tvx[0], tvy[0]) - Vector2D(vx[3], vy[3]);
		Vector2D vec41 = Vector2D(tvx[0], tvy[0]) - Vector2D(vx[2], vy[2]);
		Vector2D vec12 = Vector2D(tvx[1], tvy[1]) - Vector2D(vx[0], vy[0]);
		Vector2D vec22 = Vector2D(tvx[1], tvy[1]) - Vector2D(vx[1], vy[1]);
		Vector2D vec32 = Vector2D(tvx[1], tvy[1]) - Vector2D(vx[3], vy[3]);
		Vector2D vec42 = Vector2D(tvx[1], tvy[1]) - Vector2D(vx[2], vy[2]);
		Vector2D vec13 = Vector2D(tvx[2], tvy[2]) - Vector2D(vx[0], vy[0]);
		Vector2D vec23 = Vector2D(tvx[2], tvy[2]) - Vector2D(vx[1], vy[1]);
		Vector2D vec33 = Vector2D(tvx[2], tvy[2]) - Vector2D(vx[3], vy[3]);
		Vector2D vec43 = Vector2D(tvx[2], tvy[2]) - Vector2D(vx[2], vy[2]);
		Vector2D vec14 = Vector2D(tvx[3], tvy[3]) - Vector2D(vx[0], vy[0]);
		Vector2D vec24 = Vector2D(tvx[3], tvy[3]) - Vector2D(vx[1], vy[1]);
		Vector2D vec34 = Vector2D(tvx[3], tvy[3]) - Vector2D(vx[3], vy[3]);
		Vector2D vec44 = Vector2D(tvx[3], tvy[3]) - Vector2D(vx[2], vy[2]);

		// 相手が自身の内側にあるか
		if(vec1.Cross(vec11) <= 0.0f &&
			vec2.Cross(vec21) <= 0.0f &&
			vec3.Cross(vec31) <= 0.0f &&
			vec4.Cross(vec41) <= 0.0f ||
			vec1.Cross(vec12) <= 0.0f &&
			vec2.Cross(vec22) <= 0.0f &&
			vec3.Cross(vec32) <= 0.0f &&
			vec4.Cross(vec42) <= 0.0f ||
			vec1.Cross(vec13) <= 0.0f &&
			vec2.Cross(vec23) <= 0.0f &&
			vec3.Cross(vec33) <= 0.0f &&
			vec4.Cross(vec43) <= 0.0f ||
			vec1.Cross(vec14) <= 0.0f &&
			vec2.Cross(vec24) <= 0.0f &&
			vec3.Cross(vec34) <= 0.0f &&
			vec4.Cross(vec44) <= 0.0f){
			return true;
		}

		// 右回りベクトル
		vec1 = Vector2D(tvx[1], tvy[1]) - Vector2D(tvx[0], tvy[0]);
		vec2 = Vector2D(tvx[3], tvy[3]) - Vector2D(tvx[1], tvy[1]);
		vec3 = Vector2D(tvx[2], tvy[2]) - Vector2D(tvx[3], tvy[3]);
		vec4 = Vector2D(tvx[0], tvy[0]) - Vector2D(tvx[2], tvy[2]);

		// 頂点から頂点へのベクトル
		vec11 = Vector2D(vx[0], vy[0]) - Vector2D(tvx[0], tvy[0]);
		vec21 = Vector2D(vx[0], vy[0]) - Vector2D(tvx[1], tvy[1]);
		vec31 = Vector2D(vx[0], vy[0]) - Vector2D(tvx[3], tvy[3]);
		vec41 = Vector2D(vx[0], vy[0]) - Vector2D(tvx[2], tvy[2]);
		vec12 = Vector2D(vx[1], vy[1]) - Vector2D(tvx[0], tvy[0]);
		vec22 = Vector2D(vx[1], vy[1]) - Vector2D(tvx[1], tvy[1]);
		vec32 = Vector2D(vx[1], vy[1]) - Vector2D(tvx[3], tvy[3]);
		vec42 = Vector2D(vx[1], vy[1]) - Vector2D(tvx[2], tvy[2]);
		vec13 = Vector2D(vx[2], vy[2]) - Vector2D(tvx[0], tvy[0]);
		vec23 = Vector2D(vx[2], vy[2]) - Vector2D(tvx[1], tvy[1]);
		vec33 = Vector2D(vx[2], vy[2]) - Vector2D(tvx[3], tvy[3]);
		vec43 = Vector2D(vx[2], vy[2]) - Vector2D(tvx[2], tvy[2]);
		vec14 = Vector2D(vx[3], vy[3]) - Vector2D(tvx[0], tvy[0]);
		vec24 = Vector2D(vx[3], vy[3]) - Vector2D(tvx[1], tvy[1]);
		vec34 = Vector2D(vx[3], vy[3]) - Vector2D(tvx[3], tvy[3]);
		vec44 = Vector2D(vx[3], vy[3]) - Vector2D(tvx[2], tvy[2]);

		// 自身が相手の内側にあるか
		if(vec1.Cross(vec11) <= 0.0f &&
			vec2.Cross(vec21) <= 0.0f &&
			vec3.Cross(vec31) <= 0.0f &&
			vec4.Cross(vec41) <= 0.0f ||
			vec1.Cross(vec12) <= 0.0f &&
			vec2.Cross(vec22) <= 0.0f &&
			vec3.Cross(vec32) <= 0.0f &&
			vec4.Cross(vec42) <= 0.0f ||
			vec1.Cross(vec13) <= 0.0f &&
			vec2.Cross(vec23) <= 0.0f &&
			vec3.Cross(vec33) <= 0.0f &&
			vec4.Cross(vec43) <= 0.0f ||
			vec1.Cross(vec14) <= 0.0f &&
			vec2.Cross(vec24) <= 0.0f &&
			vec3.Cross(vec34) <= 0.0f &&
			vec4.Cross(vec44) <= 0.0f){
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------
// 円の接触判定
// 引数　：対象となるActor2D(const Actor2D& target)
// 戻り値：接触判定
//------------------------------------------------------------------------------
bool Actor2D::CheckHitCircle(const Actor2D& target)
{
	// 頂点情報
	float x = pos_.x - target.pos_.x;
	float y = pos_.y - target.pos_.y;
	float r = max(size_.x * 0.5f, size_.y * 0.5f) + max(target.size_.x * 0.5f, target.size_.y * 0.5f);	// 幅と高さの大きい方を指標とする

	// 円の判定
	if(r == 0.0f){ return false; }
	if((x * x) + (y * y) < r * r){ return true; }

	// 移動量の計算
	float movementX = (pos_.x - prePos_.x) + (target.pos_.x - target.prePos_.x);
	float movementY = (pos_.y - prePos_.y) + (target.pos_.y - target.prePos_.y);
	if(movementX < 0){ movementX *= -1; }
	if(movementY < 0){ movementY *= -1; }

	// 当たり判定を飛び越えていないか
	if(movementX < r && movementY < r){ return false; }

	// 当たり判定の大きさに合わせ移動量を分解する
	int division = 1;

	if(movementX > movementY){
		division = (int)(movementX / r) + 1;
	} else{
		division = (int)(movementY / r) + 1;
	}

	float axisX1 = (pos_.x - prePos_.x) / division;
	float axisY1 = (pos_.y - prePos_.y) / division;
	float axisX2 = (target.pos_.x - target.prePos_.x) / division;
	float axisY2 = (target.pos_.y - target.prePos_.y) / division;

	// 前フレームの頂点情報
	float posX1 = prePos_.x;
	float posY1 = prePos_.y;
	float posX2 = target.prePos_.x;
	float posY2 = target.prePos_.y;

	// 少しずつ動かしながら判定
	for(int i = 0; i < division; ++i){
		posX1 += axisX1;
		posY1 += axisY1;
		posX2 += axisX2;
		posY2 += axisY2;
		x = posX1 - posX2;
		y = posY1 - posY2;

		if((x * x) + (y * y) < r * r){ return true; }	// 円の判定
	}

	return false;
}

//------------------------------------------------------------------------------
// 位置関係取得
// 引数　：対象となるActor2Dのポインター(const Actor2D& target)
// 戻り値：上を0.0fとしたラジアン角
//------------------------------------------------------------------------------
float Actor2D::GetRelation(const Actor2D& target)
{
	return ideaPI - atan2f(target.pos_.x - pos_.x, target.pos_.y - pos_.y);
}

//------------------------------------------------------------------------------
// 前フレームの位置関係取得
// 引数　：対象となるActor2Dのポインター(const Actor2D& target)
// 戻り値：上を0.0fとしたラジアン角
//------------------------------------------------------------------------------
float Actor2D::GetPreFrameRelation(const Actor2D& target)
{
	return ideaPI - atan2f(target.prePos_.x - prePos_.x, target.prePos_.y - prePos_.y);
}

//------------------------------------------------------------------------------
// 追従するターゲットの設定
// 引数　：対象となるActor2D(const Actor2D& target)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2D::FollowActor(Actor2D& target)
{
	if(&target != this){	// 自分は対象となりえない
		if(!target.pFollower_){	// ターゲットにフォロワーが存在しなければ
			// 相互参照
			pLeader_ = &target;
			target.pFollower_ = this;
		} else{ FollowActor(*(target.pFollower_)); }	// ターゲットのフォロワーをターゲットとして再登録
	}
}

//------------------------------------------------------------------------------
// フォロワーをやめる
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2D::QuitFollower()
{
	if(pLeader_){	// リーダーがいるのなら
		if(pFollower_){	// フォロワーがいるのなら
			// リーダーとフォロワーをマッチさせる
			pLeader_->pFollower_ = pFollower_;
			pFollower_->pLeader_ = pLeader_;
		} else{ pLeader_->pFollower_ = nullptr; }

		pFollower_ = nullptr;
		pLeader_ = nullptr;
	}
}

//------------------------------------------------------------------------------
// フォロワーの位置を移動
// 引数　：X軸の移動量(float axisX),Y軸の移動量(float axisY)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2D::MoveFollower(Vector2D axis)
{
	// 移動
	pos_ += axis;

	// 次のフォロワーへとつなぐ
	if(pFollower_){ pFollower_->MoveFollower(axis); }
}

//------------------------------------------------------------------------------
// フォロワーの解散
// 引数　：対象となるActor2Dのポインター(Actor2D* pTarget)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2D::ResetFollower(Actor2D* pTarget)
{
	if(pTarget){
		ResetFollower(pTarget->pFollower_);
		pTarget->QuitFollower();
	}
}

//------------------------------------------------------------------------------
// 座標の保持
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2D::WriteDownPosition()
{
	// 前フレームの位置を保存
	prePos_ = pos_;
}
