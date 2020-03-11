#ifndef INCLUDE_IDEA_SPRITEINSTANCING_H
#define INCLUDE_IDEA_SPRITEINSTANCING_H

#include "../Utility/ideaMath.h"
#include "../Utility/ideaColor.h"
#include <vector>

//------------------------------------------------------------------------------
// �O���錾
//------------------------------------------------------------------------------
class Sprite;
class Texture;

class SpriteInstancing{
public:
	SpriteInstancing();					// �R���X�g���N�^
	~SpriteInstancing(){ UnInit(); }	// �f�X�g���N�^
	void Init(float width, float height);
	void UnInit();
	void SetSize(float width, float height);				// �T�C�Y�̐ݒ�
	float GetHalfWidth()const{ return halfWidth_; }			// ���̔����̒l�̎擾
	float GetHalfHeight()const{ return halfHeight_; }		// �����̔����̒l�擾
	float GetWidth()const{ return halfWidth_ * 2.0f; }		// ���̎擾
	float GetHeight()const{ return halfHeight_ * 2.0f; }	// �����̎擾
	void SetRotate(float rad);				// ��]�p�̐ݒ�
	void MoveRotate(float rad);				// ��]
	float GetRoteate()const{ return rad_; }	// ��]�p�̎擾
	void SetColor(Color color);	// �F�̐ݒ�
	void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);	// �F�̐ݒ�
	void AddSprite(Sprite& sprite);		// �o�^
	void RemoveSprite(Sprite& sprite);		// �o�^����
	void ResetSprite();
	void DrawRect(int blend = 0);	// ��`�̕`��
	void DrawTexture(const Texture& tex, int blend = 0);	// �e�N�X�`���̕`��
	void DrawDividedTexture(const Texture& tex, int uNum, int vNum, int blend = 0);	// ���������ꂽ�e�N�X�`���̕`��
	void DrawDelimitedTexture(const Texture& tex, float u, float v, float width, float height, int blend = 0);	// �\���͈͂��w�肵���e�N�X�`���[�̕`��

private:
	Color color_;
	float halfWidth_;		// �����̕�
	float halfHeight_;		// �����̍���
	bool bReversedX_;		// ���]�t���O(X��)
	bool bReversedY_;		// ���]�t���O(Y��)
	float rad_;				// �p�x
	std::vector<Sprite*> vecSpritePtr_;
};


#endif	// #ifndef INCLUDE_IDEA_SPRITEINSTANCING_H