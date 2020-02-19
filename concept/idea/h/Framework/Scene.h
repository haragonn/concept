/*==============================================================================
	[Scene.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SCENE_H
#define INCLUDE_IDEA_SCENE_H

//------------------------------------------------------------------------------
// クラス名　：Scene
// クラス概要：ゲームシーンクラスの基底となるクラス
//------------------------------------------------------------------------------
class Scene{
public:
	Scene() : bAsync_(false){}	// コンストラクタ
	virtual ~Scene(){}			// デストラクタ

	virtual void Init(){}			// 初期化
	virtual void UnInit(){}			// 終了処理

	virtual Scene* Update() = 0;	// 更新

	virtual void Draw() = 0;		// 描画

private:
	friend class Sequence;
	friend class AsyncLoadScene;

	bool bAsync_;	// 非同期読み込みフラグ
};

#endif	// #ifndef INCLUDE_IDEA_SCENE_H