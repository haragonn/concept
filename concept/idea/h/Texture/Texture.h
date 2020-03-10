/*==============================================================================
	[Texture.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_TEXTURE_H
#define INCLUDE_IDEA_TEXTURE_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <vector>

class TextureHolder;
struct ID3D11Resource;
struct ID3D11ShaderResourceView;

//------------------------------------------------------------------------------
// �N���X���@�FTexture
// �N���X�T�v�F�摜�̓ǂݍ���,�e�N�X�`�����\�[�X�̊Ǘ����s��
//------------------------------------------------------------------------------
class Texture{
public:
	Texture();	// �R���X�g���N�^
	~Texture();	// �f�X�g���N�^

	bool LoadImageFromFile(const char* pFileName, unsigned int divX = 1U, unsigned int divY = 1U);	// �摜�̓ǂݍ���
	bool LoadImageFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int divX = 1U, unsigned int divY = 1U);	// �A�[�J�C�u�t�@�C������摜��ǂݍ���
	bool LoadImageFromStorage(const char* pFileName, unsigned int divX = 1U, unsigned int divY = 1U);	// �X�g���[�W����摜��ǂݍ���

	void UnLoad();	// �摜�̔j��

	ID3D11ShaderResourceView* GetTextureViewPtr()const{ return pTextureView_; }

	float GetDivU()const{ return divU_; }
	float GetDivV()const{ return divV_; }

private:
	friend class StorageManager;
	friend class TextureHolder;

	char fileName_[256];	// �t�@�C����

	ID3D11ShaderResourceView*  pTextureView_;	// �e�N�X�`���r���[�̃|�C���^

	float divU_;	// ��������U���W
	float divV_;	// ��������V���W

	std::vector<TextureHolder*> vecTexHolderPtr_;	// �z���_�[���X�g

	bool bStorage_;	// �X�g���[�W�g�p�t���O

	// �R�s�[�R���X�g���N�^�̋֎~
	Texture(const Texture& src){}
	Texture& operator=(const Texture& src){}
};

#endif	// #ifndef INCLUDE_IDEA_TEXTURE_H
