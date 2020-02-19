/*==============================================================================
	[Singleton.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_SINGLETON_H
#define INCLUDE_SINGLETON_H

#include "ideaUtility.h"

template <typename T>
class Singleton{
public:
	static inline T& Instance()
	{
		if(!s_pInstance){
			Create();
		}
		return *s_pInstance;
	}

	static inline void Create()
	{
		if(!s_pInstance){
			s_pInstance = new T;
		}
	}

	static inline void Destroy()
	{
		SafeDelete(s_pInstance);
	}

protected :
	Singleton(){}

private:
	static T* s_pInstance;

	Singleton(const Singleton & src){}
	Singleton& operator=(const Singleton & src){}
};

template <typename T> T* Singleton<T>::s_pInstance = nullptr;

#endif	// #ifndef INCLUDE_SINGLETON_H