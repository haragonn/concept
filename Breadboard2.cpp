#include "BreadBoard2.h"
class BreadBoard2::Impl{
public:
	Controller ctr_;

	WrapCamera wcmr_;
	WrapCamera wcmr2_;

	Sprite sprBG_;
	Texture texBG_;

	PmdModel pmSakuya_;
	VmdMotion vm_;
	VmdMotion vm2_;

	Font fnt_;

	int time_;
};

BreadBoard2::BreadBoard2() :
	pImpl_(new BreadBoard2::Impl)
{
	Assert(pImpl_);
}

BreadBoard2::~BreadBoard2()
{
	SafeDelete(pImpl_);
}

void BreadBoard2::Init()
{
	BreadBoard2::Impl& im = *pImpl_;

	im.ctr_.Init(1, true);
	im.ctr_.SetConfig(PadButton::BACK, KeyCode::BACKSPACE);

	im.wcmr_.Init(DegreeToRadian(65.5f), C_W / S_H, 0.0001f, 100000.0f, 5.0f);
	im.wcmr_.SetViewPort(C_W, 0.0f, C_W, S_H);
	im.wcmr_.SetRotate(DegreeToRadian(0), 0.0f);
	im.wcmr2_.Init(DegreeToRadian(65.5f), C_W / S_H, 0.0001f, 100000.0f, 5.0f);
	im.wcmr2_.SetViewPort(0.0f, 0.0f, C_W, S_H);
	im.wcmr2_.SetRotate(DegreeToRadian(-90), 0.0f);

	im.sprBG_.Init(C_W, C_H, S_W, S_H);
	im.texBG_.LoadImageFromFile("data/TEXTURE/grid04.bmp");

	im.pmSakuya_.Init(0.0f, -2.75f, 0.0f);
	im.pmSakuya_.SetScale(0.24f, 0.24f, 0.24f);
	im.pmSakuya_.SetRotate(0.0f, DegreeToRadian(0), 0.0f);
	//im.pmSakuya_.LoadPmdMeshFromFile("model/初音ミク.pmd");
	im.pmSakuya_.LoadPmdMeshFromFile("model/十六夜咲夜Type-S.pmd");
	im.wcmr_.AddObject(im.pmSakuya_);
	im.wcmr2_.AddObject(im.pmSakuya_);

	im.vm_.LoadVmdMotionFromFile("motion/歩く.vmd", im.pmSakuya_, true);
	im.vm2_.LoadVmdMotionFromFile("motion/_待機.vmd", im.pmSakuya_);

	im.fnt_.Init(40);
	im.time_ = 0;
}

void BreadBoard2::UnInit()
{
}

Scene * BreadBoard2::Update()
{
	BreadBoard2::Impl& im = *pImpl_;

	++im.time_;

	im.ctr_.Update();

	if(im.ctr_.GetButton(PadButton::A)){
		im.vm_.UpdatePmd(1.0f);
	} else{
		im.vm2_.UpdatePmd(1.0f);
	}

	if(!im.wcmr_.IsWrap()){
		float cameraSpeed = 0.02f;
		Vector2D cm(im.ctr_.GetRAxisX(), im.ctr_.GetRAxisY() * 0.8f);
		cm *= cameraSpeed;
		im.wcmr_.MoveRotate(cm.x, cm.y);
		im.wcmr2_.MoveRotate(cm.x, cm.y);
	}

	if(im.ctr_.GetButton(PadButton::R) == 1){
		im.wcmr_.SetWrapTarget(0.0f, 0.0f);
		im.wcmr2_.SetWrapTarget(DegreeToRadian(-90), 0.0f);
	}

	im.wcmr_.UpdateWrap(0.02f);
	im.wcmr2_.UpdateWrap(0.02f);

	return this;
}

void BreadBoard2::Draw()
{
	BreadBoard2::Impl& im = *pImpl_;

	im.sprBG_.Init(C_W * 0.5f, C_H, C_W, S_H);
	im.sprBG_.DrawDelimitedTexture(im.texBG_, 0.5f - C_W / S_H * 5.0f * 0.5f, 0.0f, C_W / S_H * 5.0f, 5.0f);
	im.sprBG_.Init(C_W * 1.5f, C_H, C_W, S_H);
	im.sprBG_.DrawDelimitedTexture(im.texBG_, 0.5f - C_W / S_H * 5.0f * 0.5f, 0.0f, C_W / S_H * 5.0f, 5.0f);

	im.wcmr_.DrawObject();
	im.wcmr2_.DrawObject();

	im.fnt_.DrawFormatText(0.0f, 0.0f, "%3d", im.time_ / 60);
}