#ifndef INCLUDE_EIDOS_WRAPCAMERA_H
#define INCLUDE_EIDOS_WRAPCAMERA_H

#include "Camera.h"
#include "../../../idea/h/Utility/ideaMath.h"

class WrapCamera : public Camera{
public:
	WrapCamera();
	~WrapCamera(){}

	void Init(float viewAngle, float aspect, float nearZ, float farZ, float distance);

	void SetPos(Vector3D pos){}
	void SetPos(float posX, float posY, float posZ){}
	void MovePos(Vector3D axis){}
	void MovePos(float posX, float posY, float posZ){}

	void SetFocus(Vector3D pos);
	void SetFocus(float posX, float posY, float posZ);
	void MoveFocus(Vector3D axis);
	void MoveFocus(float axisX, float axisY, float axisZ);

	void SetDistance(float distance);
	void MoveDistance(float distance);
	float GetDistance()const{ return distance_; }

	void SetRotate(Vector2D rot);
	void SetRotate(float yaw, float pitch);
	void MoveRotate(Vector2D rot);
	void MoveRotate(float yaw, float pitch);
	void MoveRotate(Vector2D rot, float speed);
	void MoveRotate(float yaw, float pitch, float speed);
	float GetYaw(){ return rot_.x; }
	float GetPitch(){ return rot_.y; }

	void SetPitchThreshold(Vector2D pitch);
	void SetPitchThreshold(float pitchMin, float pithMax);

	void SetWrapTarget(Vector2D rot);
	void SetWrapTarget(float yaw, float pitch);
	void UpdateWrap(float delta);
	bool IsWrap()const{ return isWrap_; }
	void InterruptionWrap();

private:
	float farZ_;
	float distance_;

	Vector2D rot_;
	Quaternion qtn_;
	Vector2D pitchThreshold_;

	Quaternion startQtn_;
	Quaternion targetQtn_;
	bool isWrap_;
	float t_;

	inline void UpdatePosition();
};

#endif	// #ifndef INCLUDE_EIDOS_WRAPCAMERA_H