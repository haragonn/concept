/*==============================================================================
[Billboard.h]
Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_EIDOS_BILLBOARD_H
#define INCLUDE_EIDOS_BILLBOARD_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "Object.h"
#include "../../../idea/h/Texture/TextureHolder.h"

//------------------------------------------------------------------------------
// 前方宣言
//------------------------------------------------------------------------------
class Camera;
class Texture;

//------------------------------------------------------------------------------
// クラス名　：Cube
// クラス概要：Actor3Dを継承したクラス,立方体の描画を行う
//------------------------------------------------------------------------------
class Billboard : public Object, public TextureHolder{
public:
	Billboard() : bYBillboard_(false){}
	~Billboard(){}

	void SetYBillboardFlag(bool flag){ bYBillboard_ = flag; }

	void SetTexture(Texture& tex);
	void SetDividedTexture(Texture& tex, int uNum, int vNum);
	void ExclusionTexture();

private:
	int uNum_;
	int vNum_;

	bool bDivided_;

	bool bYBillboard_;

	void Draw(Camera* pCamera)override;
	inline void DrawBillboard(Camera* pCamera, int blend = 0);	// 矩形の描画
	inline void DrawTextureBillboard(Camera* pCamera, const Texture& tex, int blend = 0);	// 矩形の描画
	inline void DrawDividedTextureBillboard(Camera* pCamera, const Texture& tex, int uNum, int vNum, int blend = 0);

	inline void UpdateMatrix(Camera* pCamera);
};

#endif	// #ifndef INCLUDE_EIDOS_BILLBOARD_H