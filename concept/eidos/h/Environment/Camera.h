#ifndef INCLUDE_EIDOS_CAMERA_H
#define INCLUDE_EIDOS_CAMERA_H

//------------------------------------------------------------------------------
// ÉCÉìÉNÉãÅ[ÉhÉtÉ@ÉCÉã
//------------------------------------------------------------------------------
#include <vector>
#include "../../../idea/h/Utility/ideaMath.h"

class Object;

class Camera{
public:
	struct ViewPort{
		float topLeftX;
		float topLeftY;
		float width;
		float height;
		float minDepth;
		float maxDepth;
	};

	Camera();
	virtual ~Camera();

	void Init(float viewAngle, float aspect, float nearZ, float farZ);

	virtual void SetViewAngle(float viewAngle);

	virtual void SetPos(Vector3D pos);
	virtual void SetPos(float posX, float posY, float posZ);
	virtual void MovePos(Vector3D axis);
	virtual void MovePos(float axisX, float axisY, float axisZ);
	Vector3D GetPos()const{ return eye_; }

	virtual void SetFocus(Vector3D pos);
	virtual void SetFocus(float posX, float posY, float posZ);
	virtual void MoveFocus(Vector3D axis);
	virtual void MoveFocus(float axisX, float axisY, float axisZ);
	Vector3D GetFocus()const{ return focus_; }

	void SetViewPort(float topLeftX, float topLeftY, float width, float height);

	void AddObject(Object& object);		// ìoò^
	void RemoveObject(Object& object);	// ìoò^âèú
	void ResetObject();
	void DrawObject();

	bool IsVisible(Object& object);
	bool CheckHitRayToSphere(Object& object);

	Matrix4x4& GetProjectionMatrix(){ return proj_; }
	Matrix4x4& GetViewMatrix(){ return view_; }
	ViewPort& GetViewPort(){ return viewPort_; }

protected:
	float viewAngle_;
	float aspect_;
	float nearZ_;
	float farZ_;

	Vector3D eye_;
	Vector3D focus_;
	float upY_;

	Matrix4x4 proj_;
	Matrix4x4 view_;
	ViewPort viewPort_;

	std::vector<Object*> vecObjPtr_;

	inline void UpdateViewMatrix();
};

#endif	// #ifndef INCLUDE_EIDOS_CAMERA_H