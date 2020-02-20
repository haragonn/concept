#pragma once

#include "WinMain.h"

class BreadBoard2 : public Scene{
public:
	BreadBoard2();
	~BreadBoard2();

	void Init()override;

	void UnInit()override;

	Scene* Update()override;

	void Draw()override;

private:
	class Impl;
	Impl* pImpl_;
};