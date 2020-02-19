/*==============================================================================
	[Random.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../../h/Utility/Random.h"
#include <ctime>

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
Random::Random()
{
	SetSeed(static_cast<unsigned long>(time(NULL)));	// �N�����Ԃ��V�[�h�l�Ƃ���
	std::vector<unsigned long>().swap(vecList_);
}

//------------------------------------------------------------------------------
// �V�[�h�l�̐ݒ�
// �����@�F�V�[�h�l(unsigned long seed)
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Random::SetSeed(unsigned long seed)
{
	// �V�[�h�l�̐ݒ�
	seed_ = seed;
	// Xorshift�@�ŗ����𐶐�����
	for(int i = 4 - 1; i >= 0; --i){
		x_[i] = seed_ = 1812433253U * (seed_ ^ (seed_ >> 30)) + i;
	}
}

//------------------------------------------------------------------------------
// �����̎擾(0�`4,294,967,295)
// �����@�F�Ȃ�
// �߂�l�F����(0�`4,294,967,295)
//------------------------------------------------------------------------------
unsigned long Random::GetRand()
{
	// Xorshift�@�ŗ����𐶐�����
	unsigned long Tmp = (x_[0] ^ (x_[0] << 11));
	x_[0] = x_[1]; x_[1] = x_[2]; x_[2] = x_[3];
	return (x_[3] = (x_[3] ^ (x_[3] >> 19)) ^ (Tmp ^ (Tmp >> 8)));
}

float Random::GetRandF(float min, float max, unsigned long resolution)
{
	if(min >= max){ return 0.0f; }
	float f = max - min;

	f = f / resolution;

	f *= GetRand(resolution);

	return f + min;
}

void Random::SetList(unsigned long min, unsigned long max)
{
	if(min >= max){ return; }

	std::vector<unsigned long>().swap(vecList_);

	for(unsigned int i = 0; i < max + 1 - min; ++i){
		vecList_.push_back(i + min);
	}
}

unsigned long Random::GetRandNotDuplicate()
{
	if(!vecList_.size()){ return (unsigned long)-1; }

	unsigned long t = GetRand((unsigned long)vecList_.size() - 1U);

	auto it = vecList_.begin() + t;

	unsigned long r = (*it);

	vecList_.erase(it);

	return r;
}
