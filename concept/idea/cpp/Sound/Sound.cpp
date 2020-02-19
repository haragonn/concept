/*==============================================================================
	[Sound.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/Sound/Sound.h"
#include "../../h/Sound/SoundManager.h"
#include "../../h/Archive/ArchiveLoader.h"
#include "../../h/Storage/StorageManager.h"
#include "../../h/Utility//ideaUtility.h"
#include <XAudio2.h>

//------------------------------------------------------------------------------
// プロトタイプ宣言
//------------------------------------------------------------------------------
HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);	// チャンクの読み込み
HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);	// チャンクデータの読み込み

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
Sound::Sound() :
	pSourceVoice_(nullptr),
	pDataAudio_(nullptr),
	audioSize_(0),
	bPlay_(false),
	bLoop_(false),
	bStorage_(false),
	bRegistered_(false),
	pPrev_(nullptr),
	pNext_(nullptr)
{
	SoundManager::Instance().Register(this);
}

//------------------------------------------------------------------------------
// デストラクタ
//------------------------------------------------------------------------------
Sound::~Sound()
{
	UnLoad();	// 終了処理
	SoundManager::Instance().UnRegister(this);
}

//------------------------------------------------------------------------------
// waveファイルの読み込み
// 引数　：ファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Sound::LoadWaveFromFile(const char* pFileName)
{
	if(pSourceVoice_){ return false; }	//既に読み込み済みなら終了

	HRESULT hr;					// 成否
	HANDLE hFile;				// ファイルハンドル
	DWORD dwChunkSize = 0;		// チャンクサイズ
	DWORD dwChunkPosition = 0;	// チャンクの位置
	DWORD dwFileType;			// ファイルの種類
	WAVEFORMATEXTENSIBLE wfx = {0};	// オーディオ データのフォーマット
	XAUDIO2_BUFFER buffer = {0};		// バッファ

	// サウンドデータファイルの生成
	hFile = CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE){
		SetDebugMessage("WaveLoadError! [%s] をファイルから読み込めませんでした\n", pFileName);
		return false;
	}
	if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER){	// ファイルポインタを先頭に移動
		SetDebugMessage("WaveLoadError! [%s] をファイルから読み込めませんでした\n", pFileName);
		return false;
	}
	// WAVEファイルのチェック
	hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
	if(FAILED(hr)){
		SetDebugMessage("WaveLoadError! [%s] をファイルから読み込めませんでした\n", pFileName);
		return false;
	}
	hr = ReadChunkData(hFile, &dwFileType, sizeof(DWORD), dwChunkPosition);
	if(FAILED(hr)){
		SetDebugMessage("WaveLoadError! [%s] をファイルから読み込めませんでした\n", pFileName);
		return false;
	}
	if(dwFileType != 'EVAW'){
		SetDebugMessage("WaveLoadError! [%s] をファイルから読み込めませんでした\n", pFileName);
		return false;
	}
	// フォーマットチェック
	hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
	if(FAILED(hr)){
		SetDebugMessage("WaveLoadError! [%s] をファイルから読み込めませんでした\n", pFileName);
		return false;
	}
	hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	if(FAILED(hr)){
		SetDebugMessage("WaveLoadError! [%s] をファイルから読み込めませんでした\n", pFileName);
		return false;
	}
	// オーディオデータ読み込み
	hr = CheckChunk(hFile, 'atad', &audioSize_, &dwChunkPosition);
	if(FAILED(hr)){
		SetDebugMessage("WaveLoadError! [%s] をファイルから読み込めませんでした\n", pFileName);
		return false;
	}
	pDataAudio_ = (BYTE*)malloc(audioSize_);
	hr = ReadChunkData(hFile, pDataAudio_, audioSize_, dwChunkPosition);
	if(FAILED(hr)){
		SetDebugMessage("WaveLoadError! [%s] をファイルから読み込めませんでした\n", pFileName);
		return false;
	}
	// ソースボイスの生成
	if(!SoundManager::Instance().CreateSourceVoice(&pSourceVoice_, &(wfx.Format))){
		SetDebugMessage("WaveLoadError! [%s] をファイルから読み込めませんでした\n", pFileName);
		return false;
	}
	// バッファの値設定
	buffer.AudioBytes	= audioSize_;
	buffer.pAudioData	= pDataAudio_;
	buffer.Flags		= XAUDIO2_END_OF_STREAM;
	// オーディオバッファの登録
	pSourceVoice_->SubmitSourceBuffer(&buffer);
	// オーディオバッファの削除
	pSourceVoice_->FlushSourceBuffers();

	CloseHandle(hFile);

	return true;
}

bool Sound::LoadWaveFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
{
	if(pSourceVoice_){ return false; }	//既に読み込み済みなら終了

	ArchiveLoader loader;
	if(!loader.Load(pArchiveFileName, pFileName)){
		SetDebugMessage("WaveLoadError! [%s] を [%s] から読み込めませんでした\n", pFileName, pArchiveFileName);
		return false;
	}

	char tmpP[] = "p";
	char tempFileData[] = "wav_";
	char tmpM[] = "m";
	char tempFileNum[] = "000";
	char tmpT[] = ".t";
	char tempFileName[256] = "idea_";
	char tmpDot[] = ".";
	strcat_s(tempFileName, tempFileData);
	strcat_s(tempFileName, tempFileNum);
	strcat_s(tempFileName, tmpDot);
	strcat_s(tempFileName, tmpT);
	strcat_s(tempFileName, tmpM);
	strcat_s(tempFileName, tmpP);
	{
		FILE* fp;
		if(fopen_s(&fp, tempFileName, "wb") != 0){ return false; }

		if(fp == NULL) {
			return false;
		} else {
			fwrite(&loader.GetData()[0], loader.GetSize(), sizeof(BYTE), fp);
			fclose(fp);
		}
	}

	LoadWaveFromFile(tempFileName);

	FILE* fp;
	if(fopen_s(&fp, tempFileName, "wb") != 0){ return false; }

	if(fp == NULL) {
		return false;
	} else {
		fwrite("", 1, sizeof(BYTE), fp);
	}
	fclose(fp);

	DeleteFileA(tempFileName);

	return true;
}

//------------------------------------------------------------------------------
// ストレージからwaveファイルを読み込む
// 引数　：ファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Sound::LoadWaveFromStorage(const char* pFileName)
{
	if(pSourceVoice_){ return false; }	// 既に読み込み済みなら終了
	
	// ストレージから情報を読み込む
	Sound& Snd = StorageManager::Instance().GetSound(pFileName);
	pSourceVoice_ = Snd.pSourceVoice_;
	pDataAudio_ = Snd.pDataAudio_;
	audioSize_ = Snd.audioSize_;

	if(pSourceVoice_ || pDataAudio_){ bStorage_ = true; }	// ストレージ使用フラグをオンに
	else{
		pSourceVoice_ = nullptr;
		pDataAudio_ = nullptr;
		audioSize_ = 0;
		SetDebugMessage("WaveLoadError! [%s] をストレージから読み込めませんでした\n", pFileName);
	}

	return true;
}

//------------------------------------------------------------------------------
// ソースボイスの破棄
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Sound::UnLoad()
{
	if(!pSourceVoice_){ return; }
	Stop();	// 再生を止める
	if(!bStorage_){	// ストレージから読み込んだものの破棄はストレージに任せる
		pSourceVoice_->DestroyVoice();
		if(pDataAudio_){ free(pDataAudio_); }
	}
	pSourceVoice_ = nullptr;
	pDataAudio_ = nullptr;
	audioSize_ = 0;
	bPlay_ = false;
	bLoop_ = false;
	bStorage_ = false;
}

//------------------------------------------------------------------------------
// 音楽の再生
// 引数　：ループフラグ(bool bLoop),頭出しフラグ(bool bCueing),
// 　　　　再生位置(UINT32 begin)
// 戻り値：なし
//------------------------------------------------------------------------------
void Sound::Play(bool bLoop, bool bCueing, UINT32 begin)
{
	if(!pSourceVoice_){ return; }	// ソースボイスが存在しなければ終了

	bLoop_ = bLoop;	// ループフラグを設定

	if(bCueing){ Cueing(); }	// 頭出し

	if(!IsPlaying()){
		XAUDIO2_BUFFER buffer = {0};	// バッファ
		// バッファの値設定
		buffer.AudioBytes	= audioSize_;
		buffer.pAudioData	= pDataAudio_;
		buffer.PlayBegin	= begin;
		buffer.Flags		= XAUDIO2_END_OF_STREAM;
		buffer.LoopCount	= bLoop ? XAUDIO2_LOOP_INFINITE : 0;

		// オーディオバッファの登録
		if(FAILED(pSourceVoice_->SubmitSourceBuffer(&buffer))){ return; }
	}

	// 再生
	pSourceVoice_->Start(0);
	bPlay_ = true;
}

//------------------------------------------------------------------------------
// 音楽の停止
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Sound::Stop()
{
	if(!pSourceVoice_){ return; }	// ソースボイスが存在しなければ終了

	// 一時停止
	if(IsPlaying()){
		pSourceVoice_->Stop(0);
		bPlay_ = false;
	}
}

//------------------------------------------------------------------------------
// 音楽の音楽の頭出し
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Sound::Cueing()
{
	if(!pSourceVoice_){ return; }	// ソースボイスが存在しなければ終了

	// 一時停止
	if(IsPlaying()){
		Stop();
	}
	// オーディオバッファの削除
	pSourceVoice_->FlushSourceBuffers();
}

//------------------------------------------------------------------------------
// 音量の設定
// 引数　：音量の倍率(float volume)
// 戻り値：なし
//------------------------------------------------------------------------------
void Sound::SetVolume(float volume)
{
	if(!pSourceVoice_){ return; }	// ソースボイスが存在しなければ終了

	//if(volume > 1.0f){ volume = 1.0f; }	// 音割れするので1倍以上は認めない
	if(volume < 0.0f){ volume = 0.0f; }

	pSourceVoice_->SetVolume(volume);	// 音量の設定
}

//------------------------------------------------------------------------------
// 再生中か
// 引数　：なし
// 戻り値：再生中か
//------------------------------------------------------------------------------
bool Sound::IsPlaying()
{
	if(!pSourceVoice_){ return false; }	// ソースボイスが存在しなければfalse

	XAUDIO2_VOICE_STATE voiceState;	// ソースボイスの状態

	pSourceVoice_->GetState(&voiceState);	// 状態の取得

	if(!voiceState.BuffersQueued){ bPlay_ = false; }	// 現在処理中のバッファーがなければ再生していない

	return bPlay_;	// 現在の情報を返す
}

//------------------------------------------------------------------------------
// ループしているか
// 引数　：なし
// 戻り値：ループしているか
//------------------------------------------------------------------------------
bool Sound::IsLooping()
{
	if(!pSourceVoice_){ return false; }	// ソースボイスが存在しなければfalse

	return bLoop_;	// 現在の情報を返す
}

//------------------------------------------------------------------------------
// チャンクの読み込み
// 引数　：ファイルハンドル(HANDLE hFile),フォーマット(DWORD format)
// 　　　　チャンクサイズのポインター(DWORD* pChunkSize)
// 　　　　チャンクの位置のポインター(DWORD* pChunkDataPosition)
// 戻り値：成否
//------------------------------------------------------------------------------
HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition)
{
	HRESULT hr = S_OK;			// 成否
	DWORD dwRead;				// 読み込んだ位置
	DWORD dwChunkType;			// チャンクの種類
	DWORD dwChunkDataSize;		// チャンクデータのサイズ
	DWORD dwRIFFDataSize = 0;	// RIFFデータのサイズ
	DWORD dwFileType;			// ファイルの種類
	DWORD dwBytesRead = 0;		// バイトサイズ
	DWORD dwOffset = 0;			// オフセット
	
	if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{ return HRESULT_FROM_WIN32(GetLastError()); }	// ファイルポインタを先頭に移動
	
	while(hr == S_OK){
		if(ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{ hr = HRESULT_FROM_WIN32(GetLastError()); }	// チャンクの読み込み

		if(ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{ hr = HRESULT_FROM_WIN32(GetLastError()); }	// チャンクデータの読み込み

		switch(dwChunkType){
		case 'FFIR':
			dwRIFFDataSize  = dwChunkDataSize;
			dwChunkDataSize = 4;
			if(ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{ hr = HRESULT_FROM_WIN32(GetLastError()); }	// ファイルタイプの読み込み
			break;

		default:
			if(SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{ return HRESULT_FROM_WIN32(GetLastError()); }	// ファイルポインタをチャンクデータ分移動
		}

		dwOffset += sizeof(DWORD) * 2;
		if(dwChunkType == format){
			*pChunkSize			= dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if(dwBytesRead >= dwRIFFDataSize){ return S_FALSE; }
	}
	
	return S_OK;
}

//------------------------------------------------------------------------------
// チャンクデータの読み込み
// 引数　：ファイルハンドル(HANDLE hFile),バッファ(void* pBuffer)
// 　　　　バッファサイズ(DWORD dwBuffersize)
// 　　　　バッファのオフセット(DWORD dwBufferoffset)
// 戻り値：成否
//------------------------------------------------------------------------------
HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;	// 読み込んだ位置
	
	if(SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{ return HRESULT_FROM_WIN32(GetLastError()); }	// ファイルポインタを指定位置まで移動

	if(ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{ return HRESULT_FROM_WIN32(GetLastError()); }	// データの読み込み
	
	return S_OK;
}