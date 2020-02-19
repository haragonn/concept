/*==============================================================================
	[TextureHolder.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_TEXTUREHOLDER_H
#define INCLUDE_IDEA_TEXTUREHOLDER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Texture.h"

//------------------------------------------------------------------------------
// �N���X���@�FTextureHolder
// �N���X�T�v�F�e�N�X�`���[�ێ��̍�,�e�N�X�`���\��delete�̑Ή�������N���X
//------------------------------------------------------------------------------
class TextureHolder{
protected:
	friend class Texture;

	Texture* pTex_;

	TextureHolder() : pTex_(nullptr){}	// �R���X�g���N�^
	virtual ~TextureHolder()	// �f�X�g���N�^
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
