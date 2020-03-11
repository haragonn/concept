#pragma once
#include "WinMain.h"

class Player{
private:
	Controller ctr_;

	PmdModel pmSakuya_;
	Cube cbSakuya_;
	VmdMotion vmSakuyaMove_;
	VmdMotion vmSakuyaStay_;

	float sakuHead_;
	float rad_;
	float rad2_;
	bool bSet_;
	bool bHit_;
	bool bHit2_;
	int interval_;
	int interval2_;

	float accuracy_;
	float concentration_;
	Sprite sprConcentration_;
	Sprite sprConcentrationMax_;

	Vector3D prePos_;
	float moveSpeed_;
	float accel_;
	float distance_;
	float backAccel_;
	Vector3D back;

	int damage_;

	bool bKacha_;

	Sound sndShot_;
	Sound sndKacha_;
	Sound sndShild_;
	Sound sndDamage_;

	Random rnd_;

	Font fnt_;

public:
	void Init(WrapCamera& cmr, ShadowCamera& scmr)
	{
		ctr_.Init(1, true);
		ctr_.SetConfig(PadButton::BACK, KeyCode::BACKSPACE);

		pmSakuya_.Init(0.0f, 0.0f, -20.0f);
		scmr.MovePos(0.0f, 0.5f, -20.0f);
		scmr.MoveFocus(0.0f, 0.5f, -20.0f);
		pmSakuya_.SetScale(0.135f, 0.135f, 0.135f);
		pmSakuya_.SetRotate(0.0f, DegreeToRadian(180), 0.0f);
		pmSakuya_.LoadPmdMeshFromStorage("model/十六夜咲夜Type-S.pmd");
		cmr.AddObject(pmSakuya_);
		scmr.AddObject(pmSakuya_);

		vmSakuyaMove_.LoadVmdMotionFromFile("motion/歩く.vmd");
		vmSakuyaStay_.LoadVmdMotionFromFile("motion/_待機.vmd");

		cbSakuya_.Init(0.0f, 3.0f, -20.0f);
		cbSakuya_.SetScale(0.5f, 0.5f, 0.5f);

		sakuHead_ = pmSakuya_.GetPosY() + 5.0f;
		rad_ = 0.0f;
		rad2_ = 0.0f;
		bSet_ = false;
		bHit_ = false;
		bHit2_ = false;
		interval_ = 0;
		interval2_ = 0;

		moveSpeed_ = 0.12f;
		accel_ = 0.0f;
		distance_ = 10.0f;
		backAccel_ = 0.0f;
		back = Vector3D(0.0f, 0.0f, 0.0f);
		bKacha_ = false;

		accuracy_ = 0.0f;
		concentration_ = 0.0f;

		sprConcentration_.Init(S_W * 0.95f, S_H * 0.5f, 30.0f, 300.0f);
		sprConcentrationMax_.Init(S_W * 0.95f, S_H * 0.5f + 150.0f, 60.0f, 5.0f);

		fnt_.Init(48, 800, "メイリオ");
		damage_ = 0;

		sndShot_.LoadWaveFromFile("data/SOUND/bon001.wav");
		sndShot_.SetVolume(0.5f);
		sndKacha_.LoadWaveFromFile("data/SOUND/handgun-ready1.wav");
		sndShild_.LoadWaveFromFile("data/SOUND/viyan000.wav");
		sndShild_.SetVolume(0.5f);
		sndDamage_.LoadWaveFromFile("data/SOUND/gyan000.wav");

		prePos_ = cbSakuya_.GetPos();
	}

	void Update(WrapCamera& cmr, ShadowCamera& scmr, Object& target)
	{
		ctr_.Update();
		int set = ctr_.GetButton(PadButton::L);
		int dash = ctr_.GetButton(PadButton::R);
		int shot = ctr_.GetButton(PadButton::X);

		scmr.MovePos(cbSakuya_.GetPos() - prePos_);
		scmr.MoveFocus(cbSakuya_.GetPos() - prePos_);
		prePos_ = cbSakuya_.GetPos();

		if(damage_){
			--damage_;
		}

		if(bHit_){
			bHit_ = !bHit_;
		}

		if(interval_){
			--interval_;
		}
		if(interval2_){
			--interval2_;
		}

		if(set == 1 || dash){
			accel_ = 0.0f;
			bKacha_ = false;
		}

		if(accel_ > 0.9f){
			accel_ = 1.0f;
		} else{
			accel_ += (1.0f - accel_) * 0.016f;
		}

		bSet_ = false;

		if(set && !dash && backAccel_ <= 0.0f){
			bSet_ = true;
		}

		if(shot >= 1 && backAccel_ <= 0.0f && interval_ == 0){
			if(set && !dash){
				sndShot_.Play();
				bHit_ = true;
				bHit2_ = true;
				if(interval2_){
					bHit2_ = false;
				}
				interval_ = 15;
				interval2_ = 30;
			}
		}

		if(!cmr.IsWrap()
			&& (!set || dash)){
			float cameraSpeed = 0.015f;
			Vector2D cm(-ctr_.GetRAxisX(), ctr_.GetRAxisY() * 0.8f);
			cm *= cameraSpeed;
			cmr.MoveRotate(cm.x, cm.y);
			if((set && dash) && cm.Normalized().LengthSquare() > 0.001f && backAccel_ <= 0.0f){
				pmSakuya_.SetRotate(0.0f, ideaMath::PI - cmr.GetYaw(), 0.0f);
			}
		}

		if(set){
			moveSpeed_ += (0.06f - moveSpeed_) * 0.075f;
			if(dash){
				moveSpeed_ += (0.06f - moveSpeed_) * 0.075f;
				sakuHead_ += (pmSakuya_.GetPosY() + 2.0f - sakuHead_) * 0.0664f;
				distance_ += (4.0f - distance_) * 0.0664f;
			} else{
				sakuHead_ += (pmSakuya_.GetPosY() + 3.1f - sakuHead_) * 0.3f * accel_;
				distance_ += (1.2f - distance_) * 0.334f * accel_;
			}
		} else{
			sakuHead_ += (pmSakuya_.GetPosY() + 2.0f - sakuHead_) * 0.0664f;
			distance_ += (4.0f - distance_) * 0.0664f;
		}

		cmr.SetFocus(pmSakuya_.GetPosX(), sakuHead_, pmSakuya_.GetPosZ());
		cmr.SetDistance(distance_);

		if(!bKacha_ && set && !dash && backAccel_ <= 0.0f){
			bKacha_ = true;
			sndKacha_.Play();
		}

		if(dash){
			moveSpeed_ += (0.20f - moveSpeed_) * 0.1f;
		} else if(!set){
			moveSpeed_ += (0.12f - moveSpeed_) * 0.25f;
		}

		Vector2D move(ctr_.GetLAxisX(), ctr_.GetLAxisY());
		Vector2D tMove = move;
		float tmpX = (move.x * cos(cmr.GetYaw()) - move.y * cos(cmr.GetYaw() - ideaMath::PI * 0.5f));
		float tmpZ = (move.y * cos(cmr.GetYaw()) + move.x * cos(cmr.GetYaw() - ideaMath::PI * 0.5f));
		move.x = tmpX;
		move.y = tmpZ;
		move = move.Normalized();
		move *= tMove.Length();
		move *= moveSpeed_;

		Vector3D vv(move.x, 0.0f, move.y);

		if(backAccel_ > 0.0f){
			vmSakuyaStay_.Reset();
			vmSakuyaStay_.UpdateVmd(pmSakuya_, false, 0.0f);
		}else if(set && !dash){
			vmSakuyaStay_.Reset();
			vmSakuyaStay_.UpdateVmd(pmSakuya_, false, 0.0f);
		}else if(move.Normalized().LengthSquare() > 0.001f){
			vmSakuyaMove_.UpdateVmd(pmSakuya_, true, moveSpeed_ * 10.0f);
			vmSakuyaStay_.Reset();
		} else{
			vmSakuyaStay_.UpdateVmd(pmSakuya_, true, 1.0f);
			vmSakuyaMove_.Reset();
		}


		if(backAccel_ <= 0.0f){
			pmSakuya_.MovePos(vv);
			cbSakuya_.MovePos(vv);
		}
		if(backAccel_ < 0.2f){
			backAccel_ = 0.0f;
			back = Vector3D(0.0f, 0.0f, 0.0f);
		} else{
			backAccel_ += -backAccel_ * 0.045f;
			moveSpeed_ += (0.01f - moveSpeed_) * 0.0334f;
		}

		back.x = back.x;
		back.y = 0.0f;
		back.z = back.z;
		back = back.Normalized() * 0.25f * backAccel_;

		pmSakuya_.MovePos(back);
		cbSakuya_.MovePos(back);
		if(bHit_){
			accuracy_ = max(0.0f, min(1.0f, rnd_.GetRand(100) * 0.0025f - 0.05f + concentration_));
			if(concentration_ >= 1.0f){
				accuracy_ = 1.0f;
			}
			accuracy_ = max(0.0f, min(1.0f, accuracy_));
		}

		if(set == 1 && !dash){
			concentration_ = 0.0f;
			Vector3D vvvv;
			vvvv = cmr.GetPos() - target.GetPos();
			if(vvvv.Normalized().LengthSquare() > 0.001f){
				float rad = atan2f(vvvv.z, vvvv.x);
				if(sinf(rad - cmr.GetYaw() - ideaMath::PI) > sin(DegreeToRadian(90.0f - 39.6f))){
					concentration_ = 0.1f;
				}
				if(sinf(rad - cmr.GetYaw() - ideaMath::PI) > sin(DegreeToRadian(90.0f - 10.0f))){
					concentration_ = 0.25f;
				}
			}
		}
		float len = max(20.0f, (target.GetPos() - GetPos()).Length());
		static float fff = 0.0f;
		fff += -fff * 0.125f;
		if(bHit_ && bHit2_){
			concentration_ -= 0.2f;
			sprConcentration_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.7f) * fff + ideaColor::SILVER * (1.0f - fff));
			sprConcentrationMax_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.7f) * fff + ideaColor::SILVER * (1.0f - fff));
			fnt_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.7f) * fff + ideaColor::SILVER * (1.0f - fff));
			if(concentration_ < 0.1f){
				concentration_ = 0.1f;
			}

		} else if(interval_){
			concentration_ -= 0.05f;
			sprConcentration_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.7f) * fff + ideaColor::SILVER * (1.0f - fff));
			sprConcentrationMax_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.7f) * fff + ideaColor::SILVER * (1.0f - fff));
			fnt_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.7f) * fff + ideaColor::SILVER * (1.0f - fff));
			if(concentration_ < 0.1f){
				concentration_ = 0.1f;
			}
		} else if(set && !dash){
			//concentration_ += 0.006f * max(1.0f, (1.0f - (move.Length() - moveSpeed_) / moveSpeed_ * 0.9f)) * (((40.0f - min(35.0f, len)) / 9.0f) * 0.7f + 0.3f);
			concentration_ += 0.012f * (((40.0f - min(30.0f, len)) / 10.0f) * 0.8f + 0.2f);
			sprConcentration_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.7f) * fff + ideaColor::SILVER * (1.0f - fff));
			sprConcentrationMax_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.7f) * fff + ideaColor::SILVER * (1.0f - fff));
			fnt_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.7f) * fff + ideaColor::SILVER * (1.0f - fff));
			if(concentration_ >= 1.0f){
				fff = 1.0f;
				concentration_ = 1.0f;
				sprConcentration_.SetColor(ideaColor::SILVER.ReplaceAlpha(0.65f));
				sprConcentration_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.8f) * fff + ideaColor::SILVER * (1.0f - fff));
				sprConcentrationMax_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.8f) * fff + ideaColor::SILVER * (1.0f - fff));
				fnt_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.8f) * fff + ideaColor::SILVER * (1.0f - fff));
			}
		} else{
			concentration_ += (0.0f - concentration_) * 0.1f;
			sprConcentration_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.6f * concentration_) * fff + ideaColor::SILVER * (1.0f - fff));
			sprConcentrationMax_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.4f * concentration_) * fff + ideaColor::SILVER * (1.0f - fff));
			fnt_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.4f * concentration_) * fff + ideaColor::SILVER * (1.0f - fff));
		}
		if(backAccel_ > 0.0f){
			concentration_ += (0.0f - concentration_) * 0.1f;
			sprConcentration_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.6f * concentration_) * fff + ideaColor::SILVER * (1.0f - fff));
			sprConcentrationMax_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.4f * concentration_) * fff + ideaColor::SILVER * (1.0f - fff));
			fnt_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(0.4f * concentration_) * fff + ideaColor::SILVER * (1.0f - fff));
		}

		if(!set){
			if(vv.Normalized().LengthSquare() > 0.001f && backAccel_ <= 0.0f){
				Vector3D vvv(0.0f, 0.0f, 1.0f);
				rad2_ = atan2f(vv.z, vv.x) + ideaMath::PI * 0.5f;
				pmSakuya_.SetRotate(0.0f, -rad2_, 0.0f);
			}
		} else if(!dash){
			Vector3D vvvv;
			vvvv = pmSakuya_.GetPos() - target.GetPos();
			if(vvvv.Normalized().LengthSquare() > 0.001f){
				rad2_ = -atan2f(vvvv.z, vvvv.x) + ideaMath::PI * 0.5f;
				pmSakuya_.SetRotate(0.0f, rad2_, 0.0f);
				cmr.InterruptionWrap();
				cmr.SetWrapTarget(ideaMath::PI * 1.0f - pmSakuya_.GetRotateY(), DegreeToRadian(10));
			}
		} else{
			if(vv.Normalized().LengthSquare() > 0.001f && backAccel_ <= 0.0f){
				Vector3D vvv(0.0f, 0.0f, 1.0f);
				rad2_ = atan2f(vv.z, vv.x) + ideaMath::PI * 0.5f;
				pmSakuya_.SetRotate(0.0f, -rad2_, 0.0f);
			}
			//if(set == 1){
			//	rad_ = ideaMath::PI * 1.0f - pmSakuya_.GetRotateY();
			//	cmr.WrapInterruption();
			//	cmr.WrapTarget(rad_, DegreeToRadian(15));
			//}
		}


		cmr.UpdateWrap(0.1f);

	}

	bool GetHit()const{ return bHit_; }
	bool GetSet()const{ return bSet_; }
	float GetAccuracy()const{ return accuracy_; }

	bool CheckHit(Object& obj, bool at, int& time)
	{
		if(cbSakuya_.CheckHitSphere(obj)){
			if(!at){
				sndShild_.Play();
			} else if(!damage_){
				sndDamage_.Play();
				//time += 5 * 60;
			}
			if(at){
				damage_ = 30;
			}
			bKacha_ = false;
			backAccel_ = 3.0f;
			back = cbSakuya_.GetPos() - obj.GetPos();
			rad2_ = atan2f(-back.z, -back.x) + ideaMath::PI * 0.5f;
			pmSakuya_.SetRotate(0.0f, -rad2_, 0.0f);

			return true;
		}

		return false;
	}

	bool CheckHitLaser(Object& obj, Object& target, int& time)
	{
		if(cbSakuya_.CheckHitBox(obj)){
			if(!damage_){
				sndDamage_.Play();
				//time += 5 * 60;
			}
			damage_ = 30;
			bKacha_ = false;
			backAccel_ = 3.0f;
			back = cbSakuya_.GetPos() - target.GetPos();
			rad2_ = atan2f(-back.z, -back.x) + ideaMath::PI * 0.5f;
			pmSakuya_.SetRotate(0.0f, -rad2_, 0.0f);

			return true;
		}

		return false;
	}

	void CheckHitWall(Object& obj)
	{
		while(cbSakuya_.CheckHitBox(obj)){
			Vector3D v(sinf(obj.GetRotateY()), 0.0f, cosf(obj.GetRotateY()));
			v.y = 0.0f;
			v = v.Normalized() * 0.001f;
			pmSakuya_.MovePos(v);
			cbSakuya_.MovePos(v);
		}
	}

	Vector3D GetPos()const{ return pmSakuya_.GetPos(); }

	void DrawSprite(Object& target)
	{
		sprConcentration_.Init(S_W * 0.31f * (1.0f - concentration_) + C_W * concentration_, S_H * 0.375f, 7.5f, 100.0f);
		sprConcentration_.DrawRect();
		sprConcentration_.Init(S_W * 0.69f * (1.0f - concentration_) + C_W * concentration_, S_H * 0.375f, 7.5f, 100.0f);
		sprConcentration_.DrawRect();
		sprConcentrationMax_.SetRotate(0.0f);
		sprConcentrationMax_.Init(C_W, S_H * 0.375f, 50.0f, 7.5f);
		sprConcentrationMax_.DrawRect();
		sprConcentrationMax_.Init(C_W, S_H * 0.375f, 7.5f, 50.0f);
		sprConcentrationMax_.DrawRect();

		sprConcentrationMax_.SetRotate(DegreeToRadian(-45.0f));
		sprConcentrationMax_.SetSize(65.0f, 7.5f);
		sprConcentrationMax_.SetPos(C_W - 200.0f, S_H * 0.375f - 100.0f);
		sprConcentrationMax_.DrawRect();
		sprConcentrationMax_.SetPos(C_W + 200.0f, S_H * 0.375f + 100.0f);
		sprConcentrationMax_.DrawRect();

		sprConcentrationMax_.SetRotate(DegreeToRadian(45.0f));
		sprConcentrationMax_.SetPos(C_W - 200.0f, S_H * 0.375f + 100.0f);
		sprConcentrationMax_.DrawRect();
		sprConcentrationMax_.SetPos(C_W + 200.0f, S_H * 0.375f - 100.0f);
		sprConcentrationMax_.DrawRect();
		if(concentration_ >= 1.0f){
			fnt_.DrawFormatText(C_W - 24.0f * 2.0f, S_H * 0.65f - 125.0f, "%3d%%", (100));
		} else if(concentration_ >= 0.1f){
			fnt_.DrawFormatText(C_W - 24.0f * 2.0f, S_H * 0.65f - 125.0f, " %2d%%", (int)(100 * concentration_));
		}

		//fnt_.DrawFormatText(C_W, C_H, "%f", (target.GetPos() - GetPos()).Length());
		//fnt_.DrawFormatText(C_W, C_H + 40.0f, "%f", accuracy_);
	}
};