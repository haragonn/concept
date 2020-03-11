/*==============================================================================
	[Actor2DManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_ACTOR2DMANAGER_H
#define INCLUDE_ACTOR2DMANAGER_H

class Actor2D;

//------------------------------------------------------------------------------
// クラス名　：Actor2DManager
// クラス概要：シングルトン
// 　　　　　　Actor2Dの管理を行う
//------------------------------------------------------------------------------
class Actor2DManager{
public:
	// 唯一のインスタンス
	static Actor2DManager& Instance()
	{
		static Actor2DManager s_Instance;
		return s_Instance;
	}

	// 登録
	void Register(Actor2D* pActor2D);

	// 登録解除
	void UnRegister(Actor2D* pActor2D);

	// 座標の保持
	void WriteDownPosition();

private:
	Actor2D* pListBegin_;	// リストの始まり
	Actor2D* pListEnd_;		// リストの終わり

	// 生成の禁止
	Actor2DManager();
	~Actor2DManager(){}

	// コピーコンストラクタの禁止
	Actor2DManager(const Actor2DManager& src){}
	Actor2DManager& operator=(const Actor2DManager& src){}
};

#endif	// #ifndef INCLUDE_ACTOR2DMANAGER_H