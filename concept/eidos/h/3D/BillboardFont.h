#ifndef INCLUDE_IDEA_BILLBOARDFONT_H
#define INCLUDE_IDEA_BILLBOARDFONT_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../../idea/h/Utility/ideaColor.h"
#include "Object.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unordered_map>

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

struct TextureData{
	ID3D11Texture2D* pFontTexture;
	ID3D11ShaderResourceView*  pTextureView;
	int fontWidth;
	int fontHeight;
};

class BillboardFont : public Object{
public:
	BillboardFont() : bYBillboard_(false){}
	~BillboardFont(){}
	void Init(int size, int weight = FW_REGULAR, const char* pFontName = "ＭＳ ゴシック");
	void UnInit();
	void SetColor(Color color);
	void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
	void DrawFormatText(float posX, float posY, const char* pFormat, ...);

private:
	int size_;
	HFONT hf_;
	Color color_;
	std::unordered_map<UINT, TextureData> umTextureData_;
	static TextureData s_NullTextureData_;
	bool bYBillboard_;

	void Draw(Camera* pCamera)override;
	inline void DrawBillboard(Camera* pCamera, int blend = 0);	// 矩形の描画
	TextureData* CreateTextureData(UINT code);
	inline bool HasKeyTextureData(UINT code);

	// コピーコンストラクタの禁止
	BillboardFont(const BillboardFont& src){}
	BillboardFont& operator=(const BillboardFont& src){}
};


#endif	// #ifndef INCLUDE_IDEA_BILLBOARDFONT_H
