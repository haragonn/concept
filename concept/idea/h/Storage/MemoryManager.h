/*==============================================================================
	[MemoryManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_MEMORYMANAGER_H
#define INCLUDE_IDEA_MEMORYMANAGER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <map>
#define WIN32_LEAN_AND_MEAN

//------------------------------------------------------------------------------
// �N���X���@�FMemoryManager
// �N���X�T�v�F�V���O���g��
// �@�@�@�@�@�@�V�[�����܂����Œl��ێ�����N���X
//------------------------------------------------------------------------------
class MemoryManager{
public:
	static MemoryManager& Instance(void)	// �B��̃C���X�^���X��Ԃ�
	{
		static MemoryManager s_Instance;
		return s_Instance;
	}
	void SetInt(const char* pKey, int value);			// �������L�[�Ɋ֘A�t��
	int GetInt(const char* pKey);						// �����̎擾
	bool HasKeyInt(const char* pKey);					// �w�肵���L�[�Ɗ֘A�t����ꂽ���������݂��邩
	void SetFloat(const char* pKey, float value);		// ���������_�����L�[�Ɋ֘A�t��
	float GetFloat(const char* pKey);					// ���������_���̎擾
	bool HasKeyFloat(const char* pKey);					// �w�肵���L�[�Ɗ֘A�t����ꂽ���������_�������݂��邩
	void SetDouble(const char* pKey, double value);		// �{���x���������_�����L�[�Ɋ֘A�t��
	double GetDouble(const char* pKey);					// �{���x���������_���̎擾
	bool HasKeyDouble(const char* pKey);				// �w�肵���L�[�Ɗ֘A�t����ꂽ�{���x���������_�������݂��邩

private:
	std::map<const char*, int> mapInt_;		// �����̃}�b�v
	std::map<const char*, float> mapFloat_;	// ���������_���̃}�b�v
	std::map<const char*, double> mapDouble_;	// �{���x���������_���̃}�b�v

	MemoryManager(){}	// �R���X�g���N�^
	~MemoryManager(){}	// �f�X�g���N�^

	// �R�s�[�R���X�g���N�^�̋֎~
	MemoryManager(const MemoryManager& src);
	MemoryManager& operator=(const MemoryManager& src);
};

#endif	// #ifndef INCLUDE_IDEA_MEMORYMANAGER_H