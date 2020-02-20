#pragma once

#include "WinMain.h"

class BreadBoard3 : public Scene{
public:
	BreadBoard3();
	~BreadBoard3();

	void Init()override;

	void UnInit()override;

	Scene* Update()override;

	void Draw()override;

private:
	class Impl;
	Impl* pImpl_;
};