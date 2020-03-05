/*==============================================================================
	[WinMain.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "concept/Framework.h"
#include "WinMain.h"
#include "BreadBoardLoad.h"
#include "BreadBoard2.h"
#include "BreadBoard3.h"

#include <string>

namespace{
	const char* GAME_TITLE = "concept";

	const unsigned int FRAME_RATE = 60U;
	const unsigned int FRAME_SKIP = 0U;

	const bool SHOW_FPS = true;

	const bool WINDOW_MODE = true;

	const bool USE_NETWORK = false;
}

class TempScene : public Scene{
public:
	void Init()override
	{
	}

	void UnInit()override
	{
	}

	Scene* Update()override
	{
		if(Fade::IsFadeOutCompleted()){ return new TempScene; }
		if(false){ Fade::SetFade(30); }

		return this;
	}

	void Draw()override
	{
	}
};

class TempAsyncLoadScene : public AsyncLoadScene{
public:
	void Load()override
	{
	}

	void Init()override
	{
	}

	void UnInit()override
	{
	}

	Scene* Update()override
	{
		if(Fade::IsFadeOutCompleted()){ return new TempScene; }
		if(IsLoadCompleted()){ Fade::SetFade(30); }

		return this;
	}

	void Draw()override
	{
	}
};

class Demo : public AsyncLoadScene{
public:
	void Load()override
	{
	}

	void Init()override
	{
		cnt_ = 0;
		anime_ = 0;
		scale_ = 15.0f;

		sprBG_.Init(S_W * 0.5f, S_H * 0.5f, S_W, S_H);
		sprIcon_.Init(S_W * 0.5f, S_H * 0.4f, S_W * 0.3f * scale_, S_W * 0.3f * scale_);
		sprFont_.Init(S_W * 0.5f, S_H * 0.8f, S_W * 0.55f, S_W * 0.15f);
		sprFont_.SetColor(1.0f, 1.0f, 1.0f, 0.0f);
		sprCircle_.Init(S_W - 100.0f, S_H - 70.0f, 50.0f, 50.0f);
		sprCircle_.SetColor(0.0f, 0.0f, 0.0f, 1.0f);


		texIcon_.LoadImageFromStorage("logo00.bmp", 4, 1);
		texFont_.LoadImageFromStorage("logo01.bmp");
		snd_[0].LoadWaveFromStorage("se_logo00.wav");
		snd_[1].LoadWaveFromStorage("se_logo01.wav");
	}

	void UnInit()override
	{
		Storage::UnLoadImage("logo00.bmp");
		Storage::UnLoadImage("logo01.bmp");
		Storage::UnLoadWave("se_logo00.wav");
		Storage::UnLoadWave("se_logo01.wav");
	}

	Scene* Update()override
	{
		if(Fade::IsFadeOutCompleted()){ return new TempScene; }

		if(scale_ > 1.0f){
			scale_ *= 0.85f;
		}
		else{
			scale_ = 1.0f;
			if(anime_ < 31){
				++anime_;
			}
			if(anime_ == 1){
				snd_[0].Play();
			}
			if(anime_ == 30){
				snd_[1].Play();
			}
		}

		sprIcon_.SetSize(SCREEN_WIDTH * 0.3f * scale_, SCREEN_WIDTH * 0.3f * scale_);

		if(cnt_ >= 210 && IsLoadCompleted()){ Fade::SetFade(30); }

		++cnt_;

		return this;
	}

	void Draw()override
	{
		sprBG_.DrawRect();

		if(cnt_ <= 30){
			sprIcon_.SetColor(1.0f, 1.0f, 1.0f, cnt_ / 30.0f);
		}
		sprIcon_.DrawDividedTexture(texIcon_, anime_ < 20?0:(anime_ - 20) / 3, 0);

		if(cnt_ >= 40 && cnt_ <= 90){
			sprFont_.SetColor(1.0f, 1.0f, 1.0f, (cnt_ - 40.0f) / 50.0f);
		}
		sprFont_.DrawTexture(texFont_);

		if(cnt_ > 240){
			sprCircle_.DrawCircle((cnt_ - 240) / 120.0f);
		}
	}

private:
	int cnt_;
	int anime_;
	float scale_;

	Sprite sprBG_;
	Sprite sprIcon_;
	Sprite sprFont_;
	Sprite sprCircle_;

	Texture texIcon_;
	Texture texFont_;

	Sound snd_[2];
};

class DemoLoad : public AsyncLoadScene{
public:
	void Load()override
	{
		Storage::LoadImageFromArchiveFile("logo.dat", "logo00.bmp");
		Storage::LoadImageFromArchiveFile("logo.dat", "logo01.bmp");
		Storage::LoadWaveFromArchiveFile("logo.dat", "se_logo00.wav");
		Storage::LoadWaveFromArchiveFile("logo.dat", "se_logo01.wav");
	}

	void Init()override
	{
		sprBG_.Init(C_W, C_H, S_W, S_H);
		sprBG_.SetColor(ideaColor::BLACK);
	}

	void UnInit()override{}

	Scene* Update()override
	{
		if(Fade::IsFadeOutCompleted()){ return new Demo; }
		if(IsLoadCompleted()){ Fade::SetFade(10, ideaColor::BLACK); }

		return this;
	}

	void Draw()override
	{
		sprBG_.DrawRect();
	}

private:
	Sprite sprBG_;
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	SetLeakCheckFlag();	// メモリリークチェックのフラグをオンに

	// 2重起動の防止
	HANDLE hMutex = CreateMutex(NULL, TRUE, GAME_TITLE);
	if(GetLastError() == ERROR_ALREADY_EXISTS){
		std::string mes = GAME_TITLE;
		mes += "は既に起動しています。";
		MessageBox(NULL, mes.c_str(), NULL, NULL);
		return -1;
	}

	Framework& fm = Framework::Instance();

	// idea起動
	if(!fm.Init(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_MODE, FRAME_RATE, FRAME_SKIP, USE_NETWORK, GAME_TITLE)){ return -1; }

	// タイトルの表示
	fm.SetWindowFormatText(GAME_TITLE);

	// フレームレートの簡易表示
	fm.SetFpsSimpleDisplayFlag(SHOW_FPS);

	// マウスカーソルの表示設定
	fm.SetMouseCursorVisibleFlag(true, false);

	//fm.Run(new DemoLoad);	// 実行
	fm.Run(new BreadBoardLoad);	// 実行
	//fm.Run(new BreadBoard2);	// 実行
	//fm.Run(new BreadBoard3);	// 実行

	fm.UnInit();	// 終了処理

	if(hMutex){ CloseHandle(hMutex); }	// ミューテックスの解放

	return 0;
}
