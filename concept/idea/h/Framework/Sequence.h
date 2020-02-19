/*==============================================================================
	[Sequence.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SEQUENCE_H
#define INCLUDE_IDEA_SEQUENCE_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>

class Scene;

//------------------------------------------------------------------------------
// クラス名　：Sequence
// クラス概要：シングルトン
// 　　　　　　シーケンス制御を行うクラス
//------------------------------------------------------------------------------
class Sequence{
public:
	static Sequence& Instance()	// 唯一のインスタンスを返す
	{
		static Sequence s_Instance;
		return s_Instance;
	}

	bool Init(Scene* pInitScene);	// 初期化
	void UnInit();	// 終了処理

	int Update(bool bRedy);	// 更新

	void Draw();	// 描画

private:
	friend class Framework;

	Scene* pScene_;				// 現在のシーン

	HANDLE hAsyncLoadThread_;	// 非同期読み込みスレッドのハンドル
	std::atomic<bool> bLoadCompleted_;		// 非同期読み込みの完了

	friend DWORD WINAPI AsyncLoadThread(void* vp);
	void AsyncLoad();

	Sequence();		// コンストラクタ
	~Sequence();	// デストラクタ

	// コピーコンストラクタの禁止
	Sequence(const Sequence& src){}
	Sequence& operator=(const Sequence& src){}
};

#endif	// #ifndef INCLUDE_SEQUENCE_H