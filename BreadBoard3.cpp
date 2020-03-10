#include "concept/eidos/h/Mesh/FbxModel.h"
#include "concept/eidos/h/Mesh/PmxModel.h"
#include "BreadBoard3.h"

class BreadBoard3::Impl{
public:
	Controller ctr_;

	Font fnt_;

	WrapCamera wcmr_;

	Sprite sprBG_;
	Texture texBG_;

	PmdModel pmSakuya_;
	PmxModel pxm_;

	VmdMotion vm_;

	int time_;
};

BreadBoard3::BreadBoard3() :
	pImpl_(new BreadBoard3::Impl)
{
	Assert(pImpl_);
}

BreadBoard3::~BreadBoard3()
{
	SafeDelete(pImpl_);
}

void BreadBoard3::Init()
{
	BreadBoard3::Impl& im = *pImpl_;

	im.ctr_.Init(1, true);

	im.fnt_.Init(30);

	im.wcmr_.Init(DegreeToRadian(65.5f), S_W / S_H, 0.1f, 1000.0f, 10.0f);

	im.sprBG_.Init(C_W, C_H, S_W, S_W);

	im.texBG_.LoadImageFromFile("data/TEXTURE/grid04.bmp");

	im.pxm_.LoadPmxMeshFromFile("model/初音/Tda式初音ミク・アペンド_Ver1.10.pmx");

	im.pxm_.Init(0.0f, -14.0f, 0.0f);

	im.wcmr_.AddObject(im.pxm_);

	im.vm_.LoadVmdMotionFromFile("motion/_待機.vmd", im.pxm_, true);

	im.time_ = 0;
}
void BreadBoard3::UnInit()
{
}

Scene * BreadBoard3::Update()
{
	BreadBoard3::Impl& im = *pImpl_;

	++im.time_;

	im.ctr_.Update();

	im.vm_.UpdatePmx(1.0f);

	if(!im.wcmr_.IsWrap()){
		float cameraSpeed = 0.02f;
		Vector2D cm(im.ctr_.GetRAxis().x, im.ctr_.GetRAxis().y * 0.8f);
		cm *= cameraSpeed;
		im.wcmr_.MoveRotate(cm.x, cm.y);
	}

	if(im.ctr_.GetButton(PadButton::R) == 1){
		im.wcmr_.SetWrapTarget(0.0f, 0.0f);
	}

	im.wcmr_.UpdateWrap(0.05f);

	return this;
}

void BreadBoard3::Draw()
{
	BreadBoard3::Impl& im = *pImpl_;

	im.sprBG_.DrawDelimitedTexture(im.texBG_, 0.0f, 0.0f, 10.0f, 10.0f);

	im.wcmr_.DrawObject();

	/*
	// 経過時間
	int hour = im.time_ / 60 / 60 / 60 % 60;
	int min = im.time_ / 60 / 60 % 60;
	int sec = im.time_ / 60 % 60;
	int ssec = im.time_ % 100;

	im.fnt_.DrawFormatText(0.0f, 0.0f, "%02d:%02d:%02d:%02d", hour, min, sec, ssec);

	// 入力
	im.fnt_.DrawFormatText(0.0f, 30.0f *  1, "↑:%04d", im.ctr_.GetUp());
	im.fnt_.DrawFormatText(0.0f, 30.0f *  2, "↓:%04d", im.ctr_.GetDown());
	im.fnt_.DrawFormatText(0.0f, 30.0f *  3, "←:%04d", im.ctr_.GetLeft());
	im.fnt_.DrawFormatText(0.0f, 30.0f *  4, "→:%04d", im.ctr_.GetRight());
	im.fnt_.DrawFormatText(0.0f, 30.0f *  5, "A :%04d", im.ctr_.GetButton(PadButton::A));
	im.fnt_.DrawFormatText(0.0f, 30.0f *  6, "B :%04d", im.ctr_.GetButton(PadButton::B));
	im.fnt_.DrawFormatText(0.0f, 30.0f *  7, "X :%04d", im.ctr_.GetButton(PadButton::X));
	im.fnt_.DrawFormatText(0.0f, 30.0f *  8, "Y :%04d", im.ctr_.GetButton(PadButton::Y));
	im.fnt_.DrawFormatText(0.0f, 30.0f *  9, "L :%04d", im.ctr_.GetButton(PadButton::L));
	im.fnt_.DrawFormatText(0.0f, 30.0f * 10, "R :%04d", im.ctr_.GetButton(PadButton::R));
	im.fnt_.DrawFormatText(0.0f, 30.0f * 11, "ST:%04d", im.ctr_.GetButton(PadButton::START));
	im.fnt_.DrawFormatText(0.0f, 30.0f * 12, "BA:%04d", im.ctr_.GetButton(PadButton::BACK));
	im.fnt_.DrawFormatText(0.0f, 30.0f * 13, "LS:%04d", im.ctr_.GetButton(PadButton::LS));
	im.fnt_.DrawFormatText(0.0f, 30.0f * 14, "RS:%04d", im.ctr_.GetButton(PadButton::RS));
	im.fnt_.DrawFormatText(0.0f, 30.0f * 15, "AX:%0.3f", im.ctr_.GetArrow().x);
	im.fnt_.DrawFormatText(0.0f, 30.0f * 16, "AY:%0.3f", im.ctr_.GetArrow().y);
	im.fnt_.DrawFormatText(0.0f, 30.0f * 17, "LX:%0.3f", im.ctr_.GetLAxis().x);
	im.fnt_.DrawFormatText(0.0f, 30.0f * 18, "LY:%0.3f", im.ctr_.GetLAxis().y);
	im.fnt_.DrawFormatText(0.0f, 30.0f * 19, "RX:%0.3f", im.ctr_.GetRAxis().x);
	im.fnt_.DrawFormatText(0.0f, 30.0f * 20, "RY:%0.3f", im.ctr_.GetRAxis().y);
	im.fnt_.DrawFormatText(0.0f, 30.0f * 21, "LT:%0.3f", im.ctr_.GetLTrigger());
	im.fnt_.DrawFormatText(0.0f, 30.0f * 22, "RT:%0.3f", im.ctr_.GetRTrigger());
	*/
}