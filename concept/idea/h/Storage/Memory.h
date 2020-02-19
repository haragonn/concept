/*==============================================================================
	[Memory.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_MEMORY_H
#define INCLUDE_IDEA_MEMORY_H

//------------------------------------------------------------------------------
// クラス名　：Memory
// クラス概要：シーンをまたいで値を保持するクラス
//------------------------------------------------------------------------------
class Memory{
public:
	Memory(){}
	static void SetInt(const char* pKey, int value);			// 整数をキーに関連付け
	static int GetInt(const char* pKey);						// 整数の取得
	static bool HasKeyInt(const char* pKey);					// 指定したキーと関連付けられた整数が存在するか
	static void SetFloat(const char* pKey, float value);		// 浮動小数点数をキーに関連付け
	static float GetFloat(const char* pKey);					// 浮動小数点数の取得
	static bool HasKeyFloat(const char* pKey);					// 指定したキーと関連付けられた浮動小数点数が存在するか
	static void SetDouble(const char* pKey, double value);		// 倍精度浮動小数点数をキーに関連付け
	static double GetDouble(const char* pKey);					// 倍精度浮動小数点数の取得
	static bool HasKeyDouble(const char* pKey);				// 指定したキーと関連付けられた倍精度浮動小数点数が存在するか
};

#endif	// #ifndef INCLUDE_IDEA_MEMORY_H