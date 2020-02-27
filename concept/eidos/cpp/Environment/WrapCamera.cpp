#include "../../h/Environment/WrapCamera.h"
#include "../../../idea//h/Utility/ideaUtility.h"
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

WrapCamera::WrapCamera() :
	farZ_(1000.0f),
	distance_(1.0f),
	pitchThreshold_(- ideaPI, ideaPI),
	isWrap_(false)
{
}

void WrapCamera::Init(float viewAngle, float aspect, float nearZ, float farZ, float distance)
{
	nearZ_ = nearZ;
	farZ_ = farZ;

	Camera::Init(viewAngle, aspect, nearZ_, farZ_);

	distance_ = fabsf(max(nearZ, min(farZ, distance)));

	rot_ = Vector2D(0.0f, 0.0f);

	targetQtn_ = qtn_ = Quaternion::Euler(0.0f, 0.0f, 0.0f);

	pitchThreshold_ = Vector2D(-ideaPI * 0.4925f, ideaPI * 0.4925f);

	t_ = 0.0f;

	UpdatePosition();
}

void WrapCamera::SetFocus(Vector3D pos)
{
	focus_ = pos;

	UpdatePosition();
	UpdateViewMatrix();
}

void WrapCamera::SetFocus(float posX, float posY, float posZ)
{
	SetFocus(Vector3D(posX, posY, posZ));
}

void WrapCamera::MoveFocus(Vector3D axis)
{
	focus_ += axis;

	UpdatePosition();
	UpdateViewMatrix();
}

void WrapCamera::MoveFocus(float axisX, float axisY, float axisZ)
{
	MoveFocus(Vector3D(axisX, axisY, axisZ));
}


void WrapCamera::SetDistance(float distance)
{
	distance_ = max(nearZ_, min(farZ_, distance));

	UpdatePosition();
}

void WrapCamera::MoveDistance(float distance)
{
	distance_ += distance;
	distance_ = max(nearZ_, min(farZ_, distance_));

	UpdatePosition();
}

void WrapCamera::SetRotate(Vector2D rot)
{
	rot_ = rot;
	qtn_ = Quaternion::Euler(rot_.y, rot_.x, 0.0f);

	UpdatePosition();
}

void WrapCamera::SetRotate(float yaw, float pitch)
{
	SetRotate(Vector2D(yaw, pitch));
}

void WrapCamera::MoveRotate(Vector2D rot)
{
	rot_ += rot;
	qtn_ = Quaternion::Euler(rot_.y, rot_.x, 0.0f);

	UpdatePosition();
}

void WrapCamera::MoveRotate(float yaw, float pitch)
{
	MoveRotate(Vector2D(yaw, pitch));
}

void WrapCamera::MoveRotate(Vector2D rot, float speed)
{
	rot = rot.Normalized() * speed;
	rot_ += rot;
	qtn_ = Quaternion::Euler(rot_.y, rot_.x, 0.0f);

	UpdatePosition();
}

void WrapCamera::MoveRotate(float yaw, float pitch, float speed)
{
	MoveRotate(Vector2D(yaw, pitch), speed);
}

void WrapCamera::SetPitchThreshold(Vector2D pitch)
{
	if(pitch.x < -ideaPI * 0.4925f){
		pitch.x = -ideaPI * 0.4925f;
	}
	if(pitch.y > ideaPI * 0.4925f){
		pitch.y = ideaPI * 0.4925f;
	}

	if(pitch.x <= pitch.y){
		pitchThreshold_ = pitch;
	}

	UpdatePosition();
}

void WrapCamera::SetPitchThreshold(float pitchMin, float pithMax)
{
	SetPitchThreshold(Vector2D(pitchMin, pithMax));
}

void WrapCamera::SetWrapTarget(Vector2D rot)
{
	if(!isWrap_){
		isWrap_ = true;

		startQtn_ = qtn_;

		// 正規化
		rot.x = fmodf(rot.x, ideaPI * 2.0f);
		if(rot.x < 0.0f){
			rot.x += ideaPI * 2.0f;
		}

		rot.y = fmodf(rot.y, ideaPI * 2.0f);
		if(rot.y < 0.0f){
			rot.y += ideaPI * 2.0f;
		}

		targetQtn_ = Quaternion::Euler(rot.y, rot.x, 0.0f);
	}
}

void WrapCamera::SetWrapTarget(float yaw, float pitch)
{
	SetWrapTarget(Vector2D(yaw, pitch));
}

void WrapCamera::UpdateWrap(float delta)
{
	if(isWrap_){
		t_ += delta;

		if(t_ < 1.0f){
			qtn_ = Lerp::Linear(startQtn_, targetQtn_, t_);
		} else {
			isWrap_ = false;
			t_ = 0.0f;
			startQtn_ = qtn_ = targetQtn_;
		}
	} else{
		t_ = 0.0f;
		targetQtn_ = startQtn_ = qtn_;
	}

	UpdatePosition();
}

void WrapCamera::InterruptionWrap()
{
	isWrap_ = false;
	t_ = 0.0f;
	targetQtn_ = startQtn_ = qtn_;
}

void WrapCamera::UpdatePosition()
{
	rot_.x = qtn_.ToEuler().y;
	rot_.y = qtn_.ToEuler().x;

	// 正規化
	rot_.x = fmodf(rot_.x, ideaPI * 2.0f);
	if(rot_.x < 0.0f){
		rot_.x += ideaPI * 2.0f;
	}

	rot_.y = fmodf(rot_.y, ideaPI * 2.0f);
	if(rot_.y < 0.0f){
		rot_.y += ideaPI * 2.0f;
	}

	// 閾値
	Vector2D tmpRot = rot_;

	if(tmpRot.y > ideaPI){
		tmpRot.y = tmpRot.y - ideaPI * 2.0f;
	}

	if(tmpRot.y < pitchThreshold_.x){
		rot_.y = pitchThreshold_.x + ideaPI * 2.0f;
		while(rot_.y > ideaPI * 2.0f)
		{
			rot_.y -= ideaPI * 2.0f;
		}
	}else if(tmpRot.y > pitchThreshold_.y){
		rot_.y = pitchThreshold_.y;
	}

	// upの修正
	if(rot_.y < ideaPI * 0.5f){
		upY_ = 1.0f;
	} else if(rot_.y < ideaPI * 1.5f){
		upY_ = -1.0f;
	} else{
		upY_ = 1.0f;
	}

	// カメラポジションの更新
	eye_ = focus_;
	eye_.x += distance_ * sinf(rot_.x) * cosf(rot_.y);
	eye_.y += distance_ * sinf(rot_.y);
	eye_.z += distance_ * -cosf(rot_.x) * cosf(rot_.y);

	UpdateViewMatrix();
}