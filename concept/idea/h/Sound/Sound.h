/*==============================================================================
	[Sound.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLIDE_IDEA_SOUND_H
#define INCLIDE_IDEA_SOUND_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct IXAudio2SourceVoice;

//------------------------------------------------------------------------------
// クラス名　：Sound
// クラス概要：waveファイルの読み込み,ソースボイスの管理,再生を行う
//------------------------------------------------------------------------------
class Sound{
public:
	Sound();	// コンストラクタ
	~Sound();	// デストラクタ
	bool LoadWaveFromFile(const char* pFileName);		// waveファイルの読み込み
	bool LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName);		// アーカイブファイルからwaveファイルを読み込む
	bool LoadWaveFromStorage(const char* pFileName);	// ストレージからwaveファイルを読み込む
	void UnLoad();	// ソースボイスの破棄
	void Play(bool bLoop = false, bool bCueing = true, UINT32 begin = 0);	// 音楽の再生
	void Stop();	// 音楽の停止
	void Cueing();	// 音楽の頭出し
	void SetVolume(float volume = 1.0f);	// 音量の設定
	bool IsPlaying();	// 再生中か
	bool IsLooping();	// ループしているか

private:
	friend class SoundManager;

	IXAudio2SourceVoice* pSourceVoice_;	// ソースボイス
	BYTE* pDataAudio_;	// オーディオ情報
	DWORD audioSize_;	// オーディオサイズ
	bool bPlay_;		// 再生中か
	bool bLoop_;		// ループしているか
	bool bStorage_;		// ストレージ使用フラグ
	bool bRegistered_;	// 登録フラグ
	Sound* pPrev_;		// 前のノード
	Sound* pNext_;		// 次のノード
	
	// コピーコンストラクタの禁止
	Sound(const Sound& src){}
	Sound& operator=(const Sound& src){}
};

#endif // #ifndef INCLIDE_IDEA_SOUND_H