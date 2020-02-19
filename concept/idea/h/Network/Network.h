/*==============================================================================
	[Network.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_NETWORK_H
#define INCLUDE_IDEA_NETWORK_H

//------------------------------------------------------------------------------
// �萔��`
//------------------------------------------------------------------------------
const unsigned int WAIT_INFINITY = (unsigned int)-1;	// �����ɑҋ@

//------------------------------------------------------------------------------
// �N���X���@�FNetwork
// �N���X�T�v�FUDP�ʐM�𒇉��N���X
// �@�@�@�@�@�@����M���̍X�V,�Ǘ����s��
//------------------------------------------------------------------------------
class Network{
public:
	Network();	// �R���X�g���N�^
	bool SetUpServer(unsigned short portNum);	// �T�[�o�[�̗����グ
	bool SetUpClient(unsigned short IPAddrA, unsigned short IPAddrB, unsigned short IPAddrC, unsigned short IPAddrD, unsigned short portNum);	// �N���C�A���g�̗����グ
	void Close();			// �ʐM�ؒf
	bool IsSetUp()const;	// �ʐM�������オ���Ă��邩�ǂ���
	bool IsServer()const;	// �T�[�o�[���ǂ���
	bool IsConnect()const;	// �ʐM���Ȃ����Ă��邩�ǂ���
	bool WaitConnect(unsigned int timeOut = WAIT_INFINITY);	// �ʐM�̓���
	char GetChar(unsigned int section = 99, unsigned int key = 7)const;	// ��M�f�[�^�̎擾
	short GetShort(unsigned int section = 99, unsigned int key = 3)const;	///
	int GetInt(unsigned int section = 99, unsigned int key = 1)const;		///
	float GetFloat(unsigned int section = 99, unsigned int key = 1)const;	///
	double GetDouble(unsigned int section = 99)const;						///
	bool SetChar(unsigned int section = 99, unsigned int key = 7, char Value = 0);			// ���M�f�[�^�̐ݒ�
	bool SetShort(unsigned int section = 99, unsigned int key = 3, short Value = 0);		///
	bool SetInt(unsigned int section = 99, unsigned int key = 1, int Value = 0);			///
	bool SetFloat(unsigned int section = 99, unsigned int key = 1, float Value = 0.0f);		///
	bool SetDouble(unsigned int section = 99, double Value = 0.0);							///
	void Flash();						// ����M�f�[�^�̏���
	unsigned long GetSeed();			// �V�[�h�l�̎擾
	unsigned int GetFrequency();		// �ʐM�p�x�̎擾
	int GetLag();						// �ʐM���O�̎擾

private:
	unsigned long long serial_;		// �f�[�^�̒ʂ��ԍ�
};

#endif	// #ifndef INCLUDE_IDEA_NETWORK_H