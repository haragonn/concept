/*==============================================================================
	[Random.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_RANDOM_H
#define INCLUDE_IDEA_RANDOM_H

#include <vector>

//------------------------------------------------------------------------------
// クラス名　：Random
// クラス概要：乱数を生成するクラス
//------------------------------------------------------------------------------
class Random{
public:
	Random();	// コンストラクタ
	Random(unsigned long seed){ SetSeed(seed); }	// コンストラクタ
	void SetSeed(unsigned long seed);				// シード値の設定
	unsigned long GetSeed()const{ return seed_; }	// シード値の取得
	unsigned long GetRand();						// 乱数の取得(0～4,294,967,295)
	unsigned long GetRand(unsigned long max){ return GetRand() % (max + 1U); }	// 乱数の取得(0～指定した最大値)
	unsigned long GetRand(unsigned long min, unsigned long max){ return (min < max) ? GetRand() % (max - min + 1U) + min : min; }	// 乱数の取得(0～指定した最大値)
	float GetRandF(float min, float max, unsigned long resolution = 4294967294U);
	void SetList(unsigned long min, unsigned long max);
	unsigned long GetRandNotDuplicate();
	bool IsListEmpty(){ return !(vecList_.size()); }

private:
	unsigned long seed_;	// シード値
	unsigned long x_[4];	// 行列

	std::vector<unsigned long> vecList_;
};

#endif // #ifndef INCLUDE_IDEA_RANDOM_H