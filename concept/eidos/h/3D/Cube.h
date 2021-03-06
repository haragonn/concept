/*==============================================================================
	[Cube.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_EIDOS_CUBE_H
#define INCLUDE_EIDOS_CUBE_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "Object.h"
#include "../../../idea/h/Texture/TextureHolder.h"
#include "../../../idea/h/Utility/ideaMath.h"

//------------------------------------------------------------------------------
// 前方宣言
//------------------------------------------------------------------------------
class Camera;
class ShadowCamera;
class Texture;

//------------------------------------------------------------------------------
// クラス名　：Cube
// クラス概要：Actor3Dを継承したクラス,立方体の描画を行う
//------------------------------------------------------------------------------
class Cube : public Object, public TextureHolder{
public:
	Cube();
	~Cube(){}

	void SetTexture(Texture& tex);
	void SetDividedTexture(Texture& tex, int uNum, int vNum);
	void SetDelimitedTexture(Texture& tex, float u, float v, float width, float height);
	void ExclusionTexture();

	void SetShadow(ShadowCamera& scmr);

private:
	int uNum_;
	int vNum_;


	Vector2D uv_;

	Vector2D size_;

	bool bDivided_;
	bool bDelimited_;

	ShadowCamera* pScmr_;

	void Draw(Camera* pCamera)override;
	inline void DrawCube(Camera* pCamera, int blend = 0);	// 矩形の描画
	inline void DrawShadowCube(Camera* pCamera, int blend = 0);	// 矩形の描画
	inline void DrawTextureCube(Camera* pCamera, const Texture& tex, int blend = 0);	// 矩形の描画
	inline void DrawTextureShadowCube(Camera* pCamera, const Texture& tex, int blend = 0);	// 矩形の描画
	inline void DrawDividedTextureCube(Camera* pCamera, const Texture& tex, int uNum, int vNum, int blend = 0);
	inline void DrawDividedTextureShadowCube(Camera* pCamera, const Texture& tex, int uNum, int vNum, int blend = 0);
	inline void DrawDelimitedTextureCube(Camera* pCamera, const Texture& tex, float u, float v, float width, float height, int blend = 0);
	inline void DrawDelimitedTextureShadowCube(Camera* pCamera, const Texture& tex, float u, float v, float width, float height, int blend = 0);

	inline void SetConstBuffer(Camera* pCamera);
	inline void SetConstBufferShadow(Camera* pCamera);
	inline void SetViewPort(Camera* pCamera);
};

#endif	// #ifndef INCLUDE_EIDOS_CUBE_H