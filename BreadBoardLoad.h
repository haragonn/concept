#pragma once
#include "WinMain.h"

class BreadBoardLoad : public AsyncLoadScene{
public:
	void Load()override;

	void Init()override;

	void UnInit()override{}

	Scene* Update()override;

	void Draw()override;

private:
	Sprite sprBG_;
};