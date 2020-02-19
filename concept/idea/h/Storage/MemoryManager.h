/*==============================================================================
	[MemoryManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_MEMORYMANAGER_H
#define INCLUDE_IDEA_MEMORYMANAGER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include <map>
#define WIN32_LEAN_AND_MEAN

//------------------------------------------------------------------------------
// クラス名　：MemoryManager
// クラス概要：シングルトン
// 　　　　　　シーンをまたいで値を保持するクラス
//------------------------------------------------------------------------------
class MemoryManager{
public:
	static MemoryManager& Instance(void)	// 唯一のインスタンスを返す
	{
		static MemoryManager s_Instance;
		return s_Instance;
	}
	void SetInt(const char* pKey, int value);			// 整数をキーに関連付け
	int GetInt(const char* pKey);						// 整数の取得
	bool HasKeyInt(const char* pKey);					// 指定したキーと関連付けられた整数が存在するか
	void SetFloat(const char* pKey, float value);		// 浮動小数点数をキーに関連付け
	float GetFloat(const char* pKey);					// 浮動小数点数の取得
	bool HasKeyFloat(const char* pKey);					// 指定したキーと関連付けられた浮動小数点数が存在するか
	void SetDouble(const char* pKey, double value);		// 倍精度浮動小数点数をキーに関連付け
	double GetDouble(const char* pKey);					// 倍精度浮動小数点数の取得
	bool HasKeyDouble(const char* pKey);				// 指定したキーと関連付けられた倍精度浮動小数点数が存在するか

private:
	std::map<const char*, int> mapInt_;		// 整数のマップ
	std::map<const char*, float> mapFloat_;	// 浮動小数点数のマップ
	std::map<const char*, double> mapDouble_;	// 倍精度浮動小数点数のマップ

	MemoryManager(){}	// コンストラクタ
	~MemoryManager(){}	// デストラクタ

	// コピーコンストラクタの禁止
	MemoryManager(const MemoryManager& src);
	MemoryManager& operator=(const MemoryManager& src);
};

#endif	// #ifndef INCLUDE_IDEA_MEMORYMANAGER_H