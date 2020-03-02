#pragma once
#include "WinMain.h"
#include "Player.h"

class Wall{
private:
	Cube cbNorth_;
	Cube cbNorthViwe_[32];
	Cube cbSouth_;
	Cube cbSouthViwe_[32];
	Cube cbEast_;
	Cube cbEastViwe_[32];
	Cube cbWest_;
	Cube cbWestViwe_[32];
	Texture texWall_;

public:
	void Init(WrapCamera& cmr, ShadowCamera& scmr)
	{
		texWall_.LoadImageFromFile("data/TEXTURE/grid05.bmp");


		cbNorth_.Init(0.0f, 70.0f, 40.0f);
		cbNorth_.SetScale(80.0f, 140.0f, 0.0f);
		cbNorth_.SetDelimitedTexture(texWall_, 0.0f, 0.0f, 8.0f, 14.0f);
		cbNorth_.SetRotate(0.0f, ideaPI * 1.0f, 0.0f);
		cbNorth_.SetShadow(scmr);

		cbSouth_.Init(0.0f, 70.0f, -40.0f);
		cbSouth_.SetScale(80.0f, 140.0f, 0.0f);
		cbSouth_.SetDelimitedTexture(texWall_, 0.0f, 0.0f, 8.0f, 14.0f);
		cbSouth_.SetShadow(scmr);

		cbWest_.Init(40.0f, 70.0f, 0.0f);
		cbWest_.SetScale(80.0f, 140.0f, 0.0f);
		cbWest_.SetDelimitedTexture(texWall_, 0.0f, 0.0f, 8.0f, 14.0f);
		cbWest_.SetRotate(0.0f, ideaPI * 0.5f, 0.0f);
		cbWest_.SetShadow(scmr);

		cbEast_.Init(-40.0f, 70.0f, 0.0f);
		cbEast_.SetScale(80.0f, 140.0f, 0.0f);
		cbEast_.SetDelimitedTexture(texWall_, 0.0f, 0.0f, 8.0f, 14.0f);
		cbEast_.SetRotate(0.0f, ideaPI * 1.5f, 0.0f);
		cbEast_.SetShadow(scmr);

		cmr.AddObject(cbSouth_);
		cmr.AddObject(cbNorth_);
		cmr.AddObject(cbWest_);
		cmr.AddObject(cbEast_);

		for(int i = 32 - 1; i >= 0; --i){
			cbNorthViwe_[i].Init(1.5625f * 2.0f * i - 50.0f, 5.0f, 40.0f - 9 * ROOT2);
			cbNorthViwe_[i].SetScale(10.0f, 10.0f, 10.0f);
			cbNorthViwe_[i].SetRotate(0.0f, ideaPI * 1.0f, 0.0f);

			cbSouthViwe_[i].Init(1.5625f * 2.0f * i - 50.0f, 5.0f, -40.0f - 9 * ROOT2);
			cbSouthViwe_[i].SetScale(10.0f, 10.0f, 10.0f);

			cbWestViwe_[i].Init(40.0f - 9 * ROOT2, 5.0f, 1.5625f * 2.0f * i - 50.0f);
			cbWestViwe_[i].SetScale(10.0f, 10.0f, 10.0f);
			cbWestViwe_[i].SetRotate(0.0f, ideaPI * 0.5f, 0.0f);

			cbEastViwe_[i].Init(-40.0f - 9 * ROOT2, 5.0f, 1.5625f * 2.0f * i - 50.0f);
			cbEastViwe_[i].SetScale(10.0f, 10.0f, 10.0f);
			cbEastViwe_[i].SetRotate(0.0f, ideaPI * 1.5f, 0.0f);

			//cmr.AddObject(cbNorthViwe_[i]);
			//cmr.AddObject(cbSouthViwe_[i]);
			//cmr.AddObject(cbWestViwe_[i]);
			//cmr.AddObject(cbEastViwe_[i]);
		}

	}

	void Update(WrapCamera& cmr, Player& player)
	{
		bool bHit_ = false;
		for(int i = 32 - 1; i >= 0; --i){
			cbSouthViwe_[i].SetPos(1.5625f * 2.0f * i - 50.0f, 3.5f, -40.0f - 4.65f);
			if(cmr.CheckHitRayToSphere(cbSouthViwe_[i])){
				bHit_ = true;
			}
		}
		if(bHit_){
			cbSouth_.SetColor(ideaColor::CYAN.ReplaceAlpha(0.2f));
		} else{
			cbSouth_.SetColor(ideaColor::CYAN);
		}

		bHit_ = false;
		for(int i = 32 - 1; i >= 0; --i){
			cbNorthViwe_[i].SetPos(1.5625f * 2.0f * i - 50.0f, 3.5f, 40.0f + 4.65f);
			if(cmr.CheckHitRayToSphere(cbNorthViwe_[i])){
				bHit_ = true;
			}
		}
		if(bHit_){
			cbNorth_.SetColor(ideaColor::CYAN.ReplaceAlpha(0.2f));
		} else{
			cbNorth_.SetColor(ideaColor::CYAN);
		}

		bHit_ = false;
		for(int i = 32 - 1; i >= 0; --i){
			cbEastViwe_[i].SetPos(-40.0f - 4.65f, 3.5f, 1.5625f * 2.0f * i - 50.0f);
			if(cmr.CheckHitRayToSphere(cbEastViwe_[i])){
				bHit_ = true;
			}
		}
		if(bHit_){
			cbEast_.SetColor(ideaColor::CYAN.ReplaceAlpha(0.2f));
		} else{
			cbEast_.SetColor(ideaColor::CYAN);
		}

		bHit_ = false;
		for(int i = 32 - 1; i >= 0; --i){
			cbWestViwe_[i].SetPos(40.0f + 4.65f, 3.5f, 1.5625f * 2.0f * i - 50.0f);
			if(cmr.CheckHitRayToSphere(cbWestViwe_[i])){
				bHit_ = true;
			}
		}
		if(bHit_){
			cbWest_.SetColor(ideaColor::CYAN.ReplaceAlpha(0.2f));
		} else{
			cbWest_.SetColor(ideaColor::CYAN);
		}


		for(int i = 32 - 1; i >= 0; --i){
			cbSouthViwe_[i].SetPos(1.5625f * 2.0f * i - 50.0f, 3.5f, -44.65f);
			player.CheckHitWall(cbSouthViwe_[i]);
		}
		for(int i = 32 - 1; i >= 0; --i){
			cbNorthViwe_[i].SetPos(1.5625f * 2.0f * i - 50.0f, 3.5f, 44.65f);
			player.CheckHitWall(cbNorthViwe_[i]);
		}
		for(int i = 32 - 1; i >= 0; --i){
			cbWestViwe_[i].SetPos(-44.65f, 3.5f, 1.5625f * 2.0f * i - 50.0f);
			player.CheckHitWall(cbWestViwe_[i]);
		}
		for(int i = 32 - 1; i >= 0; --i){
			cbEastViwe_[i].SetPos(44.65f, 3.5f, 1.5625f * 2.0f * i - 50.0f);
			player.CheckHitWall(cbEastViwe_[i]);
		}
	}
};