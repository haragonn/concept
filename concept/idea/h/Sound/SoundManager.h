/*==============================================================================
	[SoundManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SOUNDMANAGER_H
#define INCLUDE_IDEA_SOUNDMANAGER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <XAudio2.h>

class Sound;

//------------------------------------------------------------------------------
// クラス名　：SoundManager
// クラス概要：シングルトン
// 　　　　　　XAudio2オブジェクトへのインターフェイスとマスターボイスの管理を行う
//------------------------------------------------------------------------------
class SoundManager{
public:
	static SoundManager& Instance()	// 唯一のインスタンスを返す
	{
		static SoundManager s_Instance;
		return s_Instance;
	}

	bool Init();	// 初期化
	void UnInit();	// 終了処理

	bool CreateSourceVoice(IXAudio2SourceVoice** ppSourceVoice, WAVEFORMATEX* pSourceFormat);	// ソースボイスの生成

	void Register(Sound* pSound);		// 登録
	void UnRegister(Sound* pSound);		// 登録解除

private:
	IXAudio2* pXAudio2_;						// XAudio2オブジェクトへのインターフェイス

	IXAudio2MasteringVoice* pMasteringVoice_;	// マスターボイス

	bool bCom_;									// CoInitializeEx()したフラグ

	Sound* pListBegin_;	// リストの始まり
	Sound* pListEnd_;	// リストの終わり

	SoundManager();					// コンストラクタ
	~SoundManager(){ UnInit(); }	// デストラクタ

	// コピーコンストラクタの禁止
	SoundManager(const SoundManager& src){}
	SoundManager& operator=(const SoundManager& src){}
};

#endif // #ifndef INCLUDE_IDEA_SOUNDMANAGER_H