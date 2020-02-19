/*==============================================================================
	[Sprite.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SPRITE_H
#define INCLUDE_IDEA_SPRITE_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "Actor2D.h"
#include <vector>

//------------------------------------------------------------------------------
// 前方宣言
//------------------------------------------------------------------------------
class Texture;
class SpriteInstancing;

//------------------------------------------------------------------------------
// クラス名　：Sprite
// クラス概要：Actor2Dを継承したクラス,矩形や円の描画を行う
//------------------------------------------------------------------------------
class Sprite : public Actor2D{
public:
	Sprite();
	~Sprite();

	// 矩形の描画
	void DrawRect(int blend = 0);

	// 円の描画
	void DrawCircle(float ratio = 1.0f, int blend = 0);

	// 光源の描画
	void DrawPhoton(float ratio = 1.0f, int blend = 0);

	// テクスチャの描画
	void DrawTexture(const Texture& tex, int blend = 0);

	// 等分割されたテクスチャの描画
	void DrawDividedTexture(const Texture& tex, int uNum, int vNum, int blend = 0);

	// 表示範囲を指定したテクスチャーの描画
	void DrawDelimitedTexture(const Texture& tex, float u, float v, float width, float height, int blend = 0);

private:
	friend class SpriteInstancing;

	std::vector<SpriteInstancing*> vecSpriteInstancingPtr_;

};

#endif	// #ifndef INCLUDE_IDEA_SPRITE_H