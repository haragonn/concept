/*==============================================================================
	[Memory.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/Storage/Memory.h"
#include "../../h/Storage/MemoryManager.h"

void Memory::SetInt(const char* pKey, int value)
{
	MemoryManager::Instance().SetInt(pKey, value);
}

int Memory::GetInt(const char* pKey)
{
	return MemoryManager::Instance().GetInt(pKey);
}

bool Memory::HasKeyInt(const char* pKey)
{
	return MemoryManager::Instance().HasKeyInt(pKey);
}

void Memory::SetFloat(const char* pKey, float value)
{
	MemoryManager::Instance().SetFloat(pKey, value);
}

float Memory::GetFloat(const char* pKey)
{
	return MemoryManager::Instance().GetFloat(pKey);
}

bool Memory::HasKeyFloat(const char* pKey)
{
	return MemoryManager::Instance().HasKeyFloat(pKey);
}

void Memory::SetDouble(const char* pKey, double value)
{
	MemoryManager::Instance().SetDouble(pKey, value);
}

double Memory::GetDouble(const char* pKey)
{
	return MemoryManager::Instance().GetDouble(pKey);
}

bool Memory::HasKeyDouble(const char* pKey)
{
	return MemoryManager::Instance().HasKeyDouble(pKey);
}
