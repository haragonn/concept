#pragma once

#include "IFileLoader.h"
#include "PathManager.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/**
�����t�H���_�ɎU���Ă���t�@�C�����A1�̃t�H���_���ɑ��݂��邩�̂悤�ɉ��z�I�Ɉ�����t�@�C�����[�_.
�p�X�Ǘ��ƃ��[�h�@�\������.
*/
class VirtualFileLoader : public IFileLoader, public PathManager
{
public:
	struct LoaderPair
	{
		SFileLoader loader;	//	���[�_�[.
		std::string	path;	//	��_�p�X��.

		LoaderPair(SFileLoader loader, std::string path);
	};

	//	�X�^�b�N�Ƃ��Ă���.
	std::vector<LoaderPair> loaders_;

public:
	/**
	�V�����t�@�C�����[�_��o�^����. �X�^�b�N�Ƃ��Đς܂�A�ォ��ς񂾂��̂��D�悵�ė��p�����.
	loader�œn�����C���X�^���X�́ApopLoader()���Ă΂ꂽ���ƁACVirtualFileLoader�̃C���X�^���X���j�󂳂ꂽ���ɍ폜�����.
	@param	loader	�t�@�C�����[�_�̃C���X�^���X.
	@param	path	��_�p�X��.
	@return			�G���[�R�[�h. loader��NULL�̏ꍇ�ɃG���[�ƂȂ�.
	*/
	virtual LRESULT PushLoader(SFileLoader loader, const char *path);

	/**
	�V�����t�@�C�����[�_��o�^����. �X�^�b�N�Ƃ��Đς܂�A�ォ��ς񂾂��̂��D�悵�ė��p�����.
	loader�œn�����C���X�^���X�́ApopLoader()���Ă΂ꂽ���ƁACVirtualFileLoader�̃C���X�^���X���j�󂳂ꂽ���ɍ폜�����.
	@param	loader	�t�@�C�����[�_�̃C���X�^���X.
	@param	path	��_�p�X��.
	@return			�G���[�R�[�h. loader��NULL�̏ꍇ�ɃG���[�ƂȂ�.
	*/
	//virtual LRESULT pushLoader(SFileLoader loader, const WCHAR *path);

	/**
	�Ō�ɓo�^���ꂽ�t�@�C�����[�_���X�^�b�N���牺�낷.
	@return			�G���[�R�[�h. �t�@�C�����[�_��1���ς܂�Ă��Ȃ����ɌĂяo���ꂽ�ꍇ�ɃG���[�ƂȂ�.
	*/
	virtual LRESULT PopLoader();

	/**
	�t�@�C�������[�h����.
	���̍ہA�ォ��o�^���ꂽ�t�@�C�����[�_��D��I�Ɏg��.
	���[�h�Ɏ��s�����ꍇ�A���ɓo�^����Ă���t�@�C�����[�_�Ń��[�h�����݂�Ƃ���������J��Ԃ�.
	�S�Ẵt�@�C�����[�_�Ń��[�h�����s�����ꍇ�ɃG���[��Ԃ�.
	@param	out			���[�h�����f�[�^���i�[����z��ւ̎Q��. ���[�h�Ɏ��s�����ꍇ�͕ω��͖���.
	@param	filename	�t�@�C����.
	@param	offset		�ǂݍ��݊J�n�ʒu�I�t�Z�b�g. �t�@�C���T�C�Y�����傫���l���w�肳��Ă����ꍇ�A�G���[�ƂȂ�.
	@param	size		�ǂݍ��ރT�C�Y����. 0���w�肵���ꍇ�̓t�@�C���̏I�[�܂œǂݍ���.
	@return				�G���[�R�[�h.
	*/
	virtual LRESULT Load(std::vector<BYTE>& out, const char* filename, DWORD offset = 0, DWORD size = 0)const;	
};