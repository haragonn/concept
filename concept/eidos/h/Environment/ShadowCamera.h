#pragma once

#include "Camera.h"
#include "../../../idea/h/Framework/GraphicManager.h"
#include "../../h/3D/Object.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>

class ShadowCamera : public Camera{
public:
	ShadowCamera() : bShadowDraw_(false){}

	void Init(Vector3D vecLight, float range);

	void DrawObject();

	bool GetShadowDrawFlag()const{ return bShadowDraw_; }

private:
	bool bShadowDraw_;

	void Init(float viewAngle, float aspect, float nearZ, float farZ){}
};