#include "BreadBoardLoad.h"
#include "BreadBoard.h"

void BreadBoardLoad::Load()
{
	Storage::LoadObjMeshFromFile("data/OBJ/sphere.obj");
	Storage::LoadPmdMeshFromFile("model/è\òZñÈçÁñÈType-S.pmd");

	Storage::LoadWaveFromFile("data/SOUND/pyuwaan000.wav");
	Storage::LoadWaveFromFile("data/SOUND/siren000.wav");
	Storage::LoadWaveFromFile("data/SOUND/boost000.wav");
	Storage::LoadWaveFromFile("data/SOUND/laser1.wav");
	Storage::LoadWaveFromFile("data/SOUND/bushuun000.wav");
	Storage::LoadWaveFromFile("data/SOUND/batyuun100.wav");
	Storage::LoadWaveFromFile("data/SOUND/bushu000.wav");

	Storage::LoadImageFromFile("data/TEXTURE/plane00.bmp");
	Storage::LoadImageFromFile("data/TEXTURE/ef_hit.png");
	Storage::LoadImageFromFile("data/TEXTURE/bar01.bmp");
	Storage::LoadImageFromFile("data/TEXTURE/arc_triangle000.png");
}

void BreadBoardLoad::Init()
{
	sprBG_.Init(C_W, C_H, S_W, S_H);
	sprBG_.SetColor(ideaColor::BLACK);
}

void UnInit(){}

Scene* BreadBoardLoad::Update()
{
	if(Fade::IsFadeOutCompleted()){ return new BreadBoard; }
	if(IsLoadCompleted()){ Fade::SetFade(10, ideaColor::BLACK); }

	return this;
}

void BreadBoardLoad::Draw()
{
	sprBG_.DrawRect();
}