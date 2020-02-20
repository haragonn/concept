#pragma once

#include "WinMain.h"

class BreadBoard : public Scene{
public:
	BreadBoard();
	~BreadBoard();

	void Init()override;

	void UnInit()override;

	Scene* Update()override;

	void Draw()override;

private:
	class Impl;
	Impl* pImpl_;
};