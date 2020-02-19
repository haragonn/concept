/*==============================================================================
	[Sprite.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SPRITE_H
#define INCLUDE_IDEA_SPRITE_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Actor2D.h"
#include <vector>

//------------------------------------------------------------------------------
// �O���錾
//------------------------------------------------------------------------------
class Texture;
class SpriteInstancing;

//------------------------------------------------------------------------------
// �N���X���@�FSprite
// �N���X�T�v�FActor2D���p�������N���X,��`��~�̕`����s��
//------------------------------------------------------------------------------
class Sprite : public Actor2D{
public:
	Sprite();
	~Sprite();

	// ��`�̕`��
	void DrawRect(int blend = 0);

	// �~�̕`��
	void DrawCircle(float ratio = 1.0f, int blend = 0);

	// �����̕`��
	void DrawPhoton(float ratio = 1.0f, int blend = 0);

	// �e�N�X�`���̕`��
	void DrawTexture(const Texture& tex, int blend = 0);

	// ���������ꂽ�e�N�X�`���̕`��
	void DrawDividedTexture(const Texture& tex, int uNum, int vNum, int blend = 0);

	// �\���͈͂��w�肵���e�N�X�`���[�̕`��
	void DrawDelimitedTexture(const Texture& tex, float u, float v, float width, float height, int blend = 0);

private:
	friend class SpriteInstancing;

	std::vector<SpriteInstancing*> vecSpriteInstancingPtr_;

};

#endif	// #ifndef INCLUDE_IDEA_SPRITE_H