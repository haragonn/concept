/*==============================================================================
	[Texture.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_TEXTURE_H
#define INCLUDE_IDEA_TEXTURE_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include <vector>

class TextureHolder;
struct ID3D11Resource;
struct ID3D11ShaderResourceView;

//------------------------------------------------------------------------------
// クラス名　：Texture
// クラス概要：画像の読み込み,テクスチャリソースの管理を行う
//------------------------------------------------------------------------------
class Texture{
public:
	Texture();	// コンストラクタ
	~Texture();	// デストラクタ

	bool LoadImageFromFile(const char* pFileName, unsigned int divX = 1U, unsigned int divY = 1U);	// 画像の読み込み
	bool LoadImageFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int divX = 1U, unsigned int divY = 1U);	// アーカイブファイルから画像を読み込む
	bool LoadImageFromStorage(const char* pFileName, unsigned int divX = 1U, unsigned int divY = 1U);	// ストレージから画像を読み込む

	void UnLoad();	// 画像の破棄

	ID3D11ShaderResourceView* GetTextureViewPtr()const{ return pTextureView_; }

	float GetDivU()const{ return divU_; }
	float GetDivV()const{ return divV_; }

private:
	friend class StorageManager;
	friend class TextureHolder;

	char fileName_[256];	// ファイル名

	ID3D11ShaderResourceView*  pTextureView_;	// テクスチャビューのポインタ

	float divU_;	// 分割したU座標
	float divV_;	// 分割したV座標

	std::vector<TextureHolder*> vecTexHolderPtr_;	// ホルダーリスト

	bool bStorage_;	// ストレージ使用フラグ

	// コピーコンストラクタの禁止
	Texture(const Texture& src){}
	Texture& operator=(const Texture& src){}
};

#endif	// #ifndef INCLUDE_IDEA_TEXTURE_H
