/*==============================================================================
	[Memory.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_MEMORY_H
#define INCLUDE_IDEA_MEMORY_H

//------------------------------------------------------------------------------
// �N���X���@�FMemory
// �N���X�T�v�F�V�[�����܂����Œl��ێ�����N���X
//------------------------------------------------------------------------------
class Memory{
public:
	Memory(){}
	static void SetInt(const char* pKey, int value);			// �������L�[�Ɋ֘A�t��
	static int GetInt(const char* pKey);						// �����̎擾
	static bool HasKeyInt(const char* pKey);					// �w�肵���L�[�Ɗ֘A�t����ꂽ���������݂��邩
	static void SetFloat(const char* pKey, float value);		// ���������_�����L�[�Ɋ֘A�t��
	static float GetFloat(const char* pKey);					// ���������_���̎擾
	static bool HasKeyFloat(const char* pKey);					// �w�肵���L�[�Ɗ֘A�t����ꂽ���������_�������݂��邩
	static void SetDouble(const char* pKey, double value);		// �{���x���������_�����L�[�Ɋ֘A�t��
	static double GetDouble(const char* pKey);					// �{���x���������_���̎擾
	static bool HasKeyDouble(const char* pKey);				// �w�肵���L�[�Ɗ֘A�t����ꂽ�{���x���������_�������݂��邩
};

#endif	// #ifndef INCLUDE_IDEA_MEMORY_H