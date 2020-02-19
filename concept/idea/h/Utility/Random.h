/*==============================================================================
	[Random.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_RANDOM_H
#define INCLUDE_IDEA_RANDOM_H

#include <vector>

//------------------------------------------------------------------------------
// �N���X���@�FRandom
// �N���X�T�v�F�����𐶐�����N���X
//------------------------------------------------------------------------------
class Random{
public:
	Random();	// �R���X�g���N�^
	Random(unsigned long seed){ SetSeed(seed); }	// �R���X�g���N�^
	void SetSeed(unsigned long seed);				// �V�[�h�l�̐ݒ�
	unsigned long GetSeed()const{ return seed_; }	// �V�[�h�l�̎擾
	unsigned long GetRand();						// �����̎擾(0�`4,294,967,295)
	unsigned long GetRand(unsigned long max){ return GetRand() % (max + 1U); }	// �����̎擾(0�`�w�肵���ő�l)
	unsigned long GetRand(unsigned long min, unsigned long max){ return (min < max) ? GetRand() % (max - min + 1U) + min : min; }	// �����̎擾(0�`�w�肵���ő�l)
	float GetRandF(float min, float max, unsigned long resolution = 4294967294U);
	void SetList(unsigned long min, unsigned long max);
	unsigned long GetRandNotDuplicate();
	bool IsListEmpty(){ return !(vecList_.size()); }

private:
	unsigned long seed_;	// �V�[�h�l
	unsigned long x_[4];	// �s��

	std::vector<unsigned long> vecList_;
};

#endif // #ifndef INCLUDE_IDEA_RANDOM_H