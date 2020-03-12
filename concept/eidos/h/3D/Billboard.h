/*==============================================================================
[Billboard.h]
Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_EIDOS_BILLBOARD_H
#define INCLUDE_EIDOS_BILLBOARD_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Object.h"
#include "../../../idea/h/Texture/TextureHolder.h"

//------------------------------------------------------------------------------
// �O���錾
//------------------------------------------------------------------------------
class Camera;
class Texture;

//------------------------------------------------------------------------------
// �N���X���@�FCube
// �N���X�T�v�FActor3D���p�������N���X,�����̂̕`����s��
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
	inline void DrawBillboard(Camera* pCamera, int blend = 0);	// ��`�̕`��
	inline void DrawTextureBillboard(Camera* pCamera, const Texture& tex, int blend = 0);	// ��`�̕`��
	inline void DrawDividedTextureBillboard(Camera* pCamera, const Texture& tex, int uNum, int vNum, int blend = 0);

	inline void UpdateMatrix(Camera* pCamera);
};

#endif	// #ifndef INCLUDE_EIDOS_BILLBOARD_H