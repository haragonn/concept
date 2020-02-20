#pragma once
#include "WinMain.h"
#include "Player.h"

class Target{
private:
	Cube cbTarget_;
	Cube cbShild_;
	ObjModel omShild_;
	Cube cbAttack_[3];

	bool bTekisei_;

	int hit_;
	float shildA_;

	Billboard bd_[12];
	int  billLife_[12];

	float posX_;
	float posZ_;

	float life_;
	Billboard bdLife_[2];
	Texture texLife_;
	bool bRecovery_;
	int  bomd_;
	float preLife_;
	int attack_;
	int time_;

	Texture texTri_;
	Billboard bdTri_;

	float af_;

	Texture texTarget_;
	Texture texEf_;

	Sound sndRecovery_;
	Sound sndHit_;
	Sound sndCritical_;
	Sound sndCip_;
	Sound sndSiren_;
	Sound sndBom_;
	Sound sndLaser_;

	Random rnd_;

public:
	void Init(WrapCamera& cmr, float posX, float posZ)
	{
		posX_ = posX;
		posZ_ = posZ;

		sndRecovery_.LoadWaveFromStorage("data/SOUND/pyuwaan000.wav");
		sndRecovery_.SetVolume(0.5f);
		sndSiren_.LoadWaveFromStorage("data/SOUND/siren000.wav");
		sndSiren_.SetVolume(1.75f);
		sndBom_.LoadWaveFromStorage("data/SOUND/boost000.wav");
		sndBom_.SetVolume(0.5f);
		sndLaser_.LoadWaveFromStorage("data/SOUND/laser1.wav");

		sndHit_.LoadWaveFromStorage("data/SOUND/bushuun000.wav");
		sndCritical_.LoadWaveFromStorage("data/SOUND/batyuun100.wav");
		sndCritical_.SetVolume(1.0f);
		sndCip_.LoadWaveFromStorage("data/SOUND/bushu000.wav");

		texTarget_.LoadImageFromStorage("data/TEXTURE/plane00.bmp");

		cbTarget_.Init(posX_, 3.0f, posZ_);
		cbTarget_.SetScale(3.0f, 3.0f, 3.0f);
		cbTarget_.SetColor(1.0f, 1.0f, 0.0f, 0.5f);
		cbTarget_.SetRotate(RadianToDegree(0.0f), RadianToDegree(45.0f), 0.0f);
		cbTarget_.SetTexture(texTarget_);
		cmr.AddObject(cbTarget_);

		cbAttack_[0].Init(posX_, 3.0f, posZ_);
		cbAttack_[0].SetScale(80.0f, 2.0f, 3.0f);
		cbAttack_[0].SetColor(ideaColor::MAGENTA.ReplaceAlpha(0.5f));
		cbAttack_[0].SetRotate(0.0f, DegreeToRadian(0.0f), 0.0f);
		cbAttack_[1].Init(posX_, 3.0f, posZ_);
		cbAttack_[1].SetScale(80.0f, 2.0f, 3.0f);
		cbAttack_[1].SetColor(ideaColor::MAGENTA.ReplaceAlpha(0.5f));
		cbAttack_[1].SetRotate(0.0f, DegreeToRadian(120.0f), 0.0f);

		cmr.AddObject(cbAttack_[1]);
		cmr.AddObject(cbAttack_[0]);

		omShild_.LoadObjMeshFromStorage("data/OBJ/sphere.obj");
		omShild_.Init(posX_, 3.0f, posZ_);
		omShild_.SetScale(4.0f, 4.0f, 4.0f);
		omShild_.SetColor(ideaColor::CLEAR);

		cbShild_.Init(posX_, 3.0f, posZ_);
		cbShild_.SetScale(7.0f, 7.0f, 7.0f);
		texEf_.LoadImageFromStorage("data/TEXTURE/ef_hit.png", 8, 1);

		for(int i = 12 - 1; i >= 0; --i){
			billLife_[i] = 0;
			bd_[i].Init(posX_, 5.0f, posZ_);
			bd_[i].SetScale(0.75f, 0.75f, 0.75f);
			bd_[i].SetDividedTexture(texEf_, 0, 0);
			cmr.AddObject(bd_[i]);
		}

		life_ = 0.49f;
		preLife_ = 2.0f;
		bRecovery_ = false;
		bTekisei_ = false;
		bomd_ = 0;
		attack_ = 0;
		time_ = 0;

		texLife_.LoadImageFromStorage("data/TEXTURE/bar01.bmp");
		bdLife_[0].Init(posX_, 6.5f, posZ_);
		bdLife_[0].SetScale(10.0f, 0.334f, 0.0f);
		bdLife_[0].SetColor(ideaColor::GREEN.ReplaceAlpha(1.0f) + ideaColor::SOFT_BLACK + ideaColor::SOFT_BLACK + ideaColor::SOFT_BLACK);
		bdLife_[0].SetTexture(texLife_);

		texTri_.LoadImageFromStorage("data/TEXTURE/arc_triangle000.png", 2, 1);
		bdTri_.Init(posX_, 9.0f, posZ_);
		bdTri_.SetScale(3.0f, 3.0f, 0.0f);
		bdTri_.SetDividedTexture(texTri_, 0, 0);
		//cmr.AddObject(bdTri_);

		hit_ = 0;
		shildA_ = 0.0f;
		af_ = 0.0f;
	}

	void Init2(WrapCamera& cmr)
	{
		cmr.AddObject(omShild_);
	}
	bool Update(WrapCamera& cmr, Player& player)
	{
		if(cmr.IsVisible(cbTarget_)){
			bdTri_.SetDividedTexture(texTri_, 1, 0);
			bdTri_.SetPos(cbTarget_.GetPosX(), 7.0f, cbTarget_.GetPosZ());
		} else{
			bdTri_.SetDividedTexture(texTri_, 0, 0);
			bdTri_.SetPos(cbTarget_.GetPosX(), 7.5f, cbTarget_.GetPosZ());
		}

		if(bTekisei_){
			++time_;
		}
		if(attack_){
			--attack_;
			time_ = 0;
			if(!attack_){
				sndSiren_.Stop();
			}
		}

		if(time_ >= 180 && !bomd_ && !bRecovery_){
			time_ = 0;
			attack_ = 120;
			sndSiren_.Play(true);
		}
		if(hit_){
			--hit_;
		}
		if(bomd_){
			preLife_ = life_;
			time_ = 120;
			omShild_.MoveRotate(0.00f, -0.04f, 0.00f);
			--bomd_;
			if(bomd_ == 180){
				sndBom_.Play();
			}
			if(!bomd_){
				omShild_.SetRotate(0.0f, 0.0f, 0.0f);
				sndSiren_.Stop();
			}
		}



		shildA_ += -shildA_ * 0.05f;
		if(shildA_ < 0.05f){
			shildA_ = 0.0f;
		}
		omShild_.SetPos(cbTarget_.GetPosX(), 3.0f, cbTarget_.GetPosZ());
		omShild_.SetScale(4.0f, 4.0f, 4.0f);
		omShild_.SetColor(ideaColor::SOFT_BLUE.ReplaceAlpha(min(0.6f, (shildA_ - 0.05f) * 2.5f)));
		cbShild_.SetScale(7.0f, 7.0f, 7.0f);

		if(bTekisei_ && preLife_ - life_ > 0.5f && !attack_ && !bRecovery_){
			preLife_ = life_;
			bomd_ = 240;
			sndSiren_.Play(true);
			omShild_.Init(cbTarget_.GetPosX(), 3.0f, cbTarget_.GetPosZ());
			omShild_.SetScale(0.0f, 0.0f, 0.0f);
			omShild_.SetColor(ideaColor::CLEAR);

			cbShild_.Init(cbTarget_.GetPosX(), 3.0f, cbTarget_.GetPosZ());
			cbShild_.SetScale(0.0f, 0.0f, 0.0f);
		}

		af_ += (0.0f - af_) * 0.04f;
		if(hit_){
			af_ = 1.0f;
		}
		if(cmr.CheckHitRayToSphere(omShild_)){
			cbTarget_.SetColor(ideaColor::ORANGE.ReplaceAlpha(0.5f) * af_ + ideaColor::SOFT_YELLOW * (1.0f - af_));
		} else{
			cbTarget_.SetColor(ideaColor::ORANGE * af_ + ideaColor::SOFT_YELLOW * (1.0f - af_));
		}
		if(bomd_){
			cbTarget_.SetColor(ideaColor::RED);
		}
		if(bRecovery_){
			cbTarget_.SetColor(ideaColor::SILVER);
		}


		if(bomd_ > 0 && bomd_ < 180){
			omShild_.SetPos(cbTarget_.GetPosX(), 0.0f, cbTarget_.GetPosZ());
			omShild_.SetScale(4.0f * (1.0f - ((180 - bomd_) / 180.0f)) + 38.0f * (((180 - bomd_) / 180.0f)), 4.0f * (1.0f - ((180 - bomd_) / 180.0f)) + 38.0f * (((180 - bomd_) / 180.0f)), 4.0f * (1.0f - ((180 - bomd_) / 180.0f)) + 38.0f * (((180 - bomd_) / 180.0f)));
			cbShild_.SetScale(7.0f * (1.0f - ((180 - bomd_) / 180.0f)) + 75.0f * (((180 - bomd_) / 180.0f)), 7.0f * (1.0f - ((180 - bomd_) / 180.0f)) + 75.0f * (((180 - bomd_) / 180.0f)), 7.0f * (1.0f - ((180 - bomd_) / 180.0f)) + 75.0f * (((180 - bomd_) / 180.0f)));
			omShild_.SetColor(ideaColor::SOFT_RED.ReplaceAlpha(min(0.6f, (1.0f - (180 - bomd_) / 210.0f) * 2.5f)));
		}

		static float rad = 0.0f;
		static float rad2 = 0.0f;
		static float t = 0.0f;
		static Quaternion q;
		static Quaternion q2;
		if(!attack_){
			cbTarget_.MoveRotate(0.01f, 0.01f, 0.01f);
		} else if(attack_ == 120){
			Vector3D vvvv;
			vvvv = cbTarget_.GetPos() - player.GetPos();
			if(vvvv.Normalized().LengthSquare() > 0.001f){
				rad2 = -atan2f(vvvv.z, vvvv.x) + ideaPI * 0.5f;
				q2 = Quaternion::Euler(0.0f, rad2, 0.0f);
			}
		} else if(attack_ <= 100){
			Vector3D vvvv;
			vvvv = cbTarget_.GetPos() - player.GetPos();
			if(vvvv.Normalized().LengthSquare() > 0.001f){
				rad2 = -atan2f(vvvv.z, vvvv.x) + ideaPI * 0.5f;
				q2 = Quaternion::Euler(0.0f, rad2, 0.0f);
			}
		}
		if(attack_ == 120){
			q = Quaternion::Euler(cbTarget_.GetRotele());
			t = 0.0f;
		}
		if(attack_ == 100){
			q = Quaternion::Euler(cbTarget_.GetRotele());
			t = 0.0f;
		}
		if(attack_ == 60){
			cbAttack_[0].SetScale(120.0f, 2.5f, 2.5f);
			cbAttack_[1].SetScale(120.0f, 2.5f, 2.5f);
			cbAttack_[2].SetScale(2.5f, 120.0f, 2.5f);
			sndLaser_.Play();
		} else if(!attack_){
			cbAttack_[0].SetScale(0.0f, 0.0f, 0.0f);
			cbAttack_[1].SetScale(0.0f, 0.0f, 0.0f);
			cbAttack_[2].SetScale(0.0f, 0.0f, 0.0f);
		}
		if(attack_){
			cbTarget_.SetColor(ideaColor::MAGENTA);
			if(attack_ < 80){
			} else if(attack_ < 100){
				t += 1.0f / 20.0f;
				cbTarget_.SetRotate(0.0f, Lerp::Linear(q, q2, t).ToEuler().y, 0.0f);
			} else if(attack_ < 120){
				t += 1.0f / 40.0f;
				cbTarget_.SetRotate(0.0f, Lerp::Linear(q, q2, t).ToEuler().y, 0.0f);
			}
		}

		cbAttack_[0].SetRotate(0.0f, cbTarget_.GetRotateY(), 0.00f);
		cbAttack_[1].SetRotate(0.0f, cbTarget_.GetRotateY() + ideaPI * 0.5f, 0.00f);
		cbAttack_[2].SetRotate(0.0f, cbTarget_.GetRotateY(), 0.00f);

		static float ff = 0.0f;
		ff += 0.005f;


		for(int i = 12 - 1; i >= 0; --i){
			if(billLife_[i]){
				--billLife_[i];
				bd_[i].SetDividedTexture(texEf_, min(7, max(0, 8 - billLife_[i] / 3)), 0);
			} else{
				bd_[i].SetDividedTexture(texEf_, 0, 0);
			}
		}

		if(player.GetHit() && !bRecovery_){
			for(int i = 12 - 1; i >= 0; --i){
				if(billLife_[i] == 0){
					Vector3D v = player.GetPos();
					float accuracy = player.GetAccuracy();
					float blur = DegreeToRadian((float)rnd_.GetRand(360));
					if(accuracy == 0.0f){
						break;
					}
					if(accuracy > 0.96f){
						sndCritical_.Play();
						life_ = max(0.0f, life_ - 0.1f);
					} else if(accuracy > 0.7f){
						sndHit_.Play();
						life_ = max(0.0f, life_ - 0.05f);
					} else if(accuracy > 0.1f){
						sndCip_.Play();
						life_ = max(0.0f, life_ - 0.0125f);
					} else{
						break;
					}
					v = v.Normalized();
					v = v - cbTarget_.GetPos().Normalized();
					v.y = 0.0f;
					v.x += min(0.35f, max(-0.35f, -0.5f + 0.5f * (accuracy))) * (sin(blur));
					v.y += min(0.35f, max(-0.35f, -0.5f + 0.5f * (accuracy))) * (cos(blur));
					v.z += min(0.35f, max(-0.35f, -0.5f + 0.5f * (accuracy))) * (sin(blur));
					v = v.Normalized();
					v *= cbTarget_.GetScaleX() + 1.5f;
					v += cbTarget_.GetPos();

					bd_[i].SetDividedTexture(texEf_, 0, 0);
					bd_[i].SetPos(v);
					billLife_[i] = 30;
					hit_ = 30;

					break;
				}
			}
		}

		bdLife_[0].SetScale(10.0f * life_, 0.5f, 0.0f);

		if(life_ == 0.0f && !bRecovery_){
			bRecovery_ = true;
			sndRecovery_.Play();
			if(!bTekisei_){
				cmr.AddObject(bdLife_[0]);
			}
			return true;
		}

		//bdLife_[0].SetColor(ideaColor::GREEN.ReplaceAlpha(1.0f) + ideaColor::SOFT_BLACK + ideaColor::SOFT_BLACK + ideaColor::SOFT_BLACK);
		bdLife_[0].SetColor(ideaColor::ORANGE * af_ + ideaColor::SOFT_GREEN * (1.0f - af_));
		if(bRecovery_){
			life_ += (2.0f - life_) * 0.1f;
			preLife_ = life_;
			time_ = 0;
			attack_ = 0;
			bomd_ = 0;
			sndSiren_.Stop();
			sndLaser_.Stop();
			sndBom_.Stop();

			bdLife_[0].SetColor(ideaColor::CYAN.ReplaceAlpha(1.0f) + ideaColor::SOFT_BLACK + ideaColor::SOFT_BLACK + ideaColor::SOFT_BLACK);
			if(life_ > 1.99f){
				life_ = 2.0f;
				time_ = 0;
				attack_ = 0;
				bomd_ = 0;
				bRecovery_ = false;
			}
		}
		if(life_ > 1.99f){
			bTekisei_ = true;
		}
		return false;
	}

	Cube& GetShild(){ return cbShild_; }
	Cube& GetLaser(){ return cbAttack_[0]; }
	Cube& GetLaser2(){ return cbAttack_[1]; }
	bool IsAttack(){ return bomd_; }
	bool IsTekisei()const{ return bTekisei_; }

	void HitReaction()
	{
		if(!bomd_){
			shildA_ = 1.0f;
		}
	}

};