#ifndef INCLUDE_IDEA_FONT_H
#define INCLUDE_IDEA_FONT_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../Utility/ideaColor.h"
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

class Font{
public:
	Font();
	~Font();
	void Init(int size, int weight = FW_REGULAR, const char* pFontName = "ＭＳ ゴシック");
	void UnInit();
	void SetColor(Color color);
	void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
	void DrawFormatText(float posX, float posY, const char* pFormat, ...);

private:
	HFONT hf_;
	Color color_;
	std::unordered_map<UINT, TextureData> umTextureData_;
	static TextureData s_NullTextureData_;

	TextureData* CreateTextureData(UINT code);
	inline bool HasKeyTextureData(UINT code);

	// コピーコンストラクタの禁止
	Font(const Font& src){}
	Font& operator=(const Font& src){}
};


#endif	// #ifndef INCLUDE_IDEA_FONT_H
