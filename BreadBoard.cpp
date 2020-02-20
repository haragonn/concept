#include "BreadBoard.h"
#include "Player.h"
#include "Wall.h"
#include "Target.h"
#include "BreadBoardLoad.h"

class BreadBoard::Impl{
public:
	Sprite sprBG_;

	Controller ctr_;

	Font fnt_;
	Font fnt2_;
	int time_;
	int min_;
	int sec_;
	int ssec_;
	int minR_;
	int secR_;
	int ssecR_;

	WrapCamera wcmr_;

	PlaneMesh pm_;

	Texture texPlain_;

	Wall wll_;

	Player player_;

	Target trg_;
	Target trg2_;

	Sound sndBGM_;
	//Sound sndBGM2_;
	Sound sndGet_;
};

BreadBoard::BreadBoard() :
	pImpl_(new BreadBoard::Impl)
{
	Assert(pImpl_);
}

BreadBoard::~BreadBoard()
{
	SafeDelete(pImpl_);
}

void BreadBoard::Init()
{
	BreadBoard::Impl& im = *pImpl_;

	im.ctr_.Init(1, true);
	im.ctr_.SetConfig(PadButton::START, KeyCode::F1);

	im.sprBG_.Init(C_W, C_H, S_W, S_H);
	im.sprBG_.SetColor(ideaColor::SOFT_BLACK);

	im.wcmr_.Init(DegreeToRadian(65.5f), S_W / S_H, 0.0001f, 10000.0f, 3.5f);
	im.wcmr_.SetFocus(0.0f, 5.0f, 0.0f);
	im.wcmr_.SetPitchThreshold(DegreeToRadian(-20), DegreeToRadian(60));
	im.wcmr_.SetViewPort(0.0f, 0.0f, S_W, S_H);
	im.wcmr_.SetRotate(DegreeToRadian(0), DegreeToRadian(10));

	im.player_.Init(im.wcmr_);
	im.trg_.Init(im.wcmr_, 0.0f, 0.0f);
	//im.trg2_.Init(im.wcmr_, -5.0f, 0.0f);

	im.texPlain_.LoadImageFromFile("data/TEXTURE/grid05.bmp");

	im.pm_.Init(0.0f, 0.0f, 0.0f);
	im.pm_.Create(0.0f, 0.0f, 10.0f, 10.0f, 8, 8);
	im.pm_.SetColor(ideaColor::CYAN);
	im.pm_.SetTexture(im.texPlain_);
	im.wcmr_.AddObject(im.pm_);

	im.trg_.Init2(im.wcmr_);
	//im.trg2_.Init2(im.wcmr_);
	im.wll_.Init(im.wcmr_);
	im.time_ = 0;
	im.min_ = 0;
	im.sec_ = 0;
	im.ssec_ = 0;
	im.minR_ = 59;
	im.secR_ = 59;
	im.ssecR_ = 99;

	im.fnt_.Init(32, 800, "メイリオ");
	im.fnt2_.Init(40);

	im.sndBGM_.LoadWaveFromFile("data/SOUND/night.wav");
	im.sndBGM_.SetVolume(1.0f);
	im.sndGet_.LoadWaveFromFile("data/SOUND/get000.wav");
	im.sndGet_.SetVolume(1.0f);
	//im.sndBGM2_.LoadWaveFromFile("data/SOUND/Good-bye my earth.wav");
	//im.sndBGM2_.SetVolume(1.0f);

	im.sndBGM_.Play(true);
}

void BreadBoard::UnInit()
{
}

Scene * BreadBoard::Update()
{
	BreadBoard::Impl& im = *pImpl_;

	im.ctr_.Update();

	if(im.ctr_.GetButton(PadButton::START) == 1){
		Fade::SetFade(40, ideaColor::SOFT_BLACK);
	}
	if(Fade::IsFadeOutCompleted()){
		return new BreadBoardLoad;
	}

	//if(im.trg_.IsTekisei() && im.sndBGM_.IsPlaying()){
	//	im.sndBGM_.Stop();
	//	im.sndBGM2_.Play(true);
	//}
	im.player_.Update(im.wcmr_, im.trg_.GetShild());
	//im.trg2_.Update(im.wcmr_, im.player_);
	if(im.trg_.Update(im.wcmr_, im.player_)){
		if(im.trg_.IsTekisei()){
			if(im.time_ < im.minR_ * 60 * 60 + im.secR_ * 60 + im.ssecR_){
				im.sndGet_.Play();
				im.minR_ = im.min_;
				im.secR_ = im.sec_;
				im.ssecR_ = im.ssec_;
			}
		}
		im.time_ = 0;
	}
	im.wll_.Update(im.wcmr_, im.player_);

	if(im.player_.CheckHit(im.trg_.GetShild(), im.trg_.IsAttack(), im.time_)){
		im.trg_.HitReaction();
	}
	if(im.player_.CheckHit(im.trg2_.GetShild(), im.trg2_.IsAttack(), im.time_)){
		im.trg2_.HitReaction();
	}

	im.player_.CheckHitLaser(im.trg_.GetLaser(), im.trg_.GetShild(), im.time_);
	im.player_.CheckHitLaser(im.trg_.GetLaser2(), im.trg_.GetShild(), im.time_);

	++im.time_;

	im.min_ = (im.time_ / 60 / 60) % 60;
	im.sec_ = (im.time_ / 60) % 60;
	im.ssec_ = im.time_ % 100;

	return this;
}

void BreadBoard::Draw()
{
	BreadBoard::Impl& im = *pImpl_;

	im.sprBG_.DrawRect();
	im.wcmr_.DrawObject();

	im.player_.DrawSprite(im.trg_.GetShild());

	im.fnt_.DrawFormatText(0.0f, 32.0f * 0.0f, " Lスティック：移動");
	im.fnt_.DrawFormatText(0.0f, 32.0f * 1.0f, " Rスティック：カメラ操作");
	im.fnt_.DrawFormatText(0.0f, 32.0f * 2.0f, " Lボタン長押し：構える");
	im.fnt_.DrawFormatText(0.0f, 32.0f * 3.0f, " Rボタン長押し：走る");
	im.fnt_.DrawFormatText(0.0f, 32.0f * 4.0f, " 構えながらXボタン：射撃");

	if(im.trg_.IsTekisei()){
		im.fnt2_.DrawFormatText(C_W - 10.0f * 8.0f, 0.0f, "%02d:%02d:%02d", im.min_, im.sec_, im.ssec_);
	im.fnt2_.DrawFormatText(S_W - 20.0f * 9.0f, 0.0f, "%02d:%02d:%02d", im.minR_, im.secR_, im.ssecR_);
	}
}
