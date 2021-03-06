#include "BreadBoard2.h"

class BreadBoard2::Impl{
public:
	Controller ctr_;

	WrapCamera wcmr_;
	WrapCamera wcmr2_;
	ShadowCamera scmr_;


	Sprite sprBG_;
	Texture texBG_;

	PlaneMesh pln_;

	Cube cb_;

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

	im.wcmr_.Init(DegreeToRadian(65.5f), S_W / S_H, 0.1f, 10000.0f, 10.0f);
	im.wcmr_.SetFocus(0.0f, 3.0f, 0.0f);
	im.wcmr_.SetViewPort(0.0f, 0.0f, S_W, S_H);
	im.wcmr_.SetRotate(DegreeToRadian(0), 0.0f);

	im.wcmr2_.Init(DegreeToRadian(65.5f), C_W / S_H, 0.1f, 10000.0f, 5.0f);
	im.wcmr2_.SetViewPort(0.0f, 0.0f, C_W, S_H);
	im.wcmr2_.SetRotate(DegreeToRadian(-90), 0.0f);

	im.scmr_.Init(Vector3D(0.2f, 1.0f, 0.6f), 1.5f);
	//im.scmr_.SetFocus(-1.0f, -1.0f, 0.0f);
	//im.scmr_.SetPos(10, 10, 0);

	im.sprBG_.Init(C_W, C_H, S_W, S_H);
	im.texBG_.LoadImageFromFile("data/TEXTURE/grid02.bmp");

	im.cb_.Init(0.0f, 1.0f, 0.0f);
	im.cb_.SetScale(2.0f, 2.0f, 2.0f);
	im.cb_.SetRotate(0.0f, DegreeToRadian(45), 0.0f);
	im.wcmr_.AddObject(im.cb_);
	//im.scmr_.AddObject(im.cb_);
	//im.cb_.SetDelimitedTexture(im.texBG_, 0.0f, 0.0f, 30.0f, 30.0f);
	im.cb_.SetShadow(im.scmr_);

	im.pmSakuya_.Init(0.0f, 0.0f, 0.0f);
	im.pmSakuya_.SetScale(0.24f, 0.24f, 0.24f);
	im.pmSakuya_.SetRotate(0.0f, DegreeToRadian(180), 0.0f);
	//im.pmSakuya_.LoadPmdMeshFromFile("model/初音ミク.pmd");
	im.pmSakuya_.LoadPmdMeshFromFile("model/十六夜咲夜Type-S.pmd");
	im.wcmr_.AddObject(im.pmSakuya_);
	im.wcmr2_.AddObject(im.pmSakuya_);
	im.scmr_.AddObject(im.pmSakuya_);

	im.pln_.Init(0, 0, 0);
	im.pln_.Create(0, 0, 10, 10, 10, 10);
	im.wcmr_.AddObject(im.pln_);
	im.pln_.SetTexture(im.texBG_);
	im.pln_.SetShadow(im.scmr_);

	im.vm_.LoadVmdMotionFromFile("motion/歩く.vmd");
	im.vm2_.LoadVmdMotionFromFile("motion/_待機.vmd");

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
		im.vm_.UpdateVmd(im.pmSakuya_, true);
	} else{
		im.vm2_.UpdateVmd(im.pmSakuya_, true);
	}

	//im.pmSakuya_.MoveRotate(0.0f, 0.01f, 0.0f);


	Vector2D move(im.ctr_.GetLAxisX(), im.ctr_.GetLAxisY());
	Vector2D tMove = move;
	float tmpX = (move.x * cos(im.wcmr_.GetYaw()) - move.y * cos(im.wcmr_.GetYaw() - ideaMath::PI * 0.5f));
	float tmpZ = (move.y * cos(im.wcmr_.GetYaw()) + move.x * cos(im.wcmr_.GetYaw() - ideaMath::PI * 0.5f));
	move.x = tmpX;
	move.y = tmpZ;
	move = move.Normalized();
	move *= tMove.Length();
	move *= 0.1f;

	Vector3D vv(move.x, 0.0f, move.y);
	im.pmSakuya_.MovePos(vv);
	im.scmr_.MovePos(vv);
	im.scmr_.MoveFocus(vv);
	im.wcmr_.SetFocus(im.pmSakuya_.GetPos().x, im.pmSakuya_.GetPos().y + 2.0f, im.pmSakuya_.GetPos().z);

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

	im.scmr_.DrawObject();
	im.wcmr_.DrawObject();
	//im.wcmr2_.DrawObject();

	im.fnt_.DrawFormatText(0.0f, 0.0f, "%3d", im.time_ / 60);
}