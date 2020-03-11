#ifndef INCLUDE_IDEA_SPRITEINSTANCING_H
#define INCLUDE_IDEA_SPRITEINSTANCING_H

#include "../Utility/ideaMath.h"
#include "../Utility/ideaColor.h"
#include <vector>

//------------------------------------------------------------------------------
// 前方宣言
//------------------------------------------------------------------------------
class Sprite;
class Texture;

class SpriteInstancing{
public:
	SpriteInstancing();					// コンストラクタ
	~SpriteInstancing(){ UnInit(); }	// デストラクタ
	void Init(float width, float height);
	void UnInit();
	void SetSize(float width, float height);				// サイズの設定
	float GetHalfWidth()const{ return halfWidth_; }			// 幅の半分の値の取得
	float GetHalfHeight()const{ return halfHeight_; }		// 高さの半分の値取得
	float GetWidth()const{ return halfWidth_ * 2.0f; }		// 幅の取得
	float GetHeight()const{ return halfHeight_ * 2.0f; }	// 高さの取得
	void SetRotate(float rad);				// 回転角の設定
	void MoveRotate(float rad);				// 回転
	float GetRoteate()const{ return rad_; }	// 回転角の取得
	void SetColor(Color color);	// 色の設定
	void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);	// 色の設定
	void AddSprite(Sprite& sprite);		// 登録
	void RemoveSprite(Sprite& sprite);		// 登録解除
	void ResetSprite();
	void DrawRect(int blend = 0);	// 矩形の描画
	void DrawTexture(const Texture& tex, int blend = 0);	// テクスチャの描画
	void DrawDividedTexture(const Texture& tex, int uNum, int vNum, int blend = 0);	// 等分割されたテクスチャの描画
	void DrawDelimitedTexture(const Texture& tex, float u, float v, float width, float height, int blend = 0);	// 表示範囲を指定したテクスチャーの描画

private:
	Color color_;
	float halfWidth_;		// 半分の幅
	float halfHeight_;		// 半分の高さ
	bool bReversedX_;		// 反転フラグ(X軸)
	bool bReversedY_;		// 反転フラグ(Y軸)
	float rad_;				// 角度
	std::vector<Sprite*> vecSpritePtr_;
};


#endif	// #ifndef INCLUDE_IDEA_SPRITEINSTANCING_H