/*==============================================================================
	[TextureHolder.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_TEXTUREHOLDER_H
#define INCLUDE_IDEA_TEXTUREHOLDER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "Texture.h"

//------------------------------------------------------------------------------
// クラス名　：TextureHolder
// クラス概要：テクスチャー保持の際,テクスチャ―のdeleteの対応する基底クラス
//------------------------------------------------------------------------------
class TextureHolder{
protected:
	friend class Texture;

	Texture* pTex_;

	TextureHolder() : pTex_(nullptr){}	// コンストラクタ
	virtual ~TextureHolder()	// デストラクタ
	{
		ExclusionTexture();
	}

	void SetTexture(Texture* pTex)
	{
		ExclusionTexture();
		if(pTex){
			pTex_ = pTex;
			auto it = find(pTex_->vecTexHolderPtr_.begin(), pTex_->vecTexHolderPtr_.end(), this);
			if(it == pTex_->vecTexHolderPtr_.end()){
				pTex_->vecTexHolderPtr_.push_back(this);
			}
		}
	}

	void ExclusionTexture()
	{
		if(pTex_){
			auto it = find(pTex_->vecTexHolderPtr_.begin(), pTex_->vecTexHolderPtr_.end(), this);
			if(it != pTex_->vecTexHolderPtr_.end()){
				pTex_->vecTexHolderPtr_.erase(it);
			}
			pTex_ = nullptr;
		}
	}
};

#endif	// #ifndef INCLUDE_IDEA_TEXTUREHOLDER_H
