#include "../../h/Environment/Camera.h"
#include "../../../idea/h/Framework/GraphicManager.h"
#include "../../h/3D/Object.h"
#include "../../../idea/h/Utility/ideaUtility.h"
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <directxmath.h>

using namespace std;
using namespace DirectX;

Camera::Camera() :
	aspect_(640.0f / 480.0f),
	nearZ_(0.0001f),
	farZ_(10000.0f),
	eye_(Vector3D(0.0f, 0.0f, -5.0f)),
	focus_(Vector3D(0.0f, 0.0f, 0.0f)),
	upY_(1.0f)
{
	GraphicManager& gm = GraphicManager::Instance();

	XMFLOAT4X4 matProj;
	XMStoreFloat4x4(&matProj, XMMatrixIdentity());
	XMStoreFloat4x4(&matProj, XMMatrixPerspectiveFovLH(DegreeToRadian(60), (float)gm.GetWidth() / (float)gm.GetHeight(), 0.0001f, 10000.0f));
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			proj_.r[i][j] = matProj.m[i][j];
		}
	}

	XMFLOAT4X4 matView;
	XMStoreFloat4x4(&matView, XMMatrixIdentity());
	XMVECTOR eye = XMVectorSet(eye_.x, eye_.y, eye_.z, 1.0f);
	XMVECTOR focus = XMVectorSet(focus_.x, focus_.y, focus_.z, 1.0f);
	XMVECTOR up = XMVectorSet(0.0f, upY_, 0.0f, 1.0f);
	XMStoreFloat4x4(&matView, XMMatrixLookAtLH(eye, focus, up));
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			view_.r[i][j] = matView.m[i][j];
		}
	}

	viewPort_.topLeftX = 0.0f;
	viewPort_.topLeftY = 0.0f;
	viewPort_.width = (float)gm.GetWidth();
	viewPort_.height = (float)gm.GetHeight();
	viewPort_.minDepth = 0.0f;
	viewPort_.maxDepth = 1.0f;

	vector<Object*>().swap(vecObjPtr_);
}

Camera::~Camera()
{
	ResetObject();
}

void Camera::Init(float viewAngle, float aspect, float nearZ, float farZ)
{
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){ return; }

	viewAngle_ = viewAngle;
	aspect_ = aspect;
	nearZ_ = nearZ;
	farZ_ = farZ;

	XMFLOAT4X4 matProj;
	XMStoreFloat4x4(&matProj, XMMatrixIdentity());
	XMStoreFloat4x4(&matProj, XMMatrixPerspectiveFovLH(viewAngle_, aspect_, nearZ_, (farZ_ > nearZ_) ? farZ_ : nearZ_ + 0.0001f));
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			proj_.r[i][j] = matProj.m[i][j];
		}
	}

	eye_ = Vector3D(0.0f, 0.0f, -5.0f);
	focus_ = Vector3D(0.0f, 0.0f, 0.0f);
	upY_ = 1.0f;

	viewPort_.topLeftX = 0.0f;
	viewPort_.topLeftY = 0.0f;
	viewPort_.width = (float)gm.GetWidth();
	viewPort_.height = (float)gm.GetHeight();
	viewPort_.minDepth = 0.0f;
	viewPort_.maxDepth = 1.0f;

	UpdateViewMatrix();
}

void Camera::SetViewAngle(float viewAngle)
{
	viewAngle_ = viewAngle;
	XMFLOAT4X4 matProj;
	XMStoreFloat4x4(&matProj, XMMatrixIdentity());
	XMStoreFloat4x4(&matProj, XMMatrixPerspectiveFovLH(viewAngle_, aspect_, nearZ_, (farZ_ > nearZ_) ? farZ_ : nearZ_ + 0.0001f));
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			proj_.r[i][j] = matProj.m[i][j];
		}
	}
}

void Camera::SetPos(Vector3D pos)
{
	eye_ = pos;

	UpdateViewMatrix();
}

void Camera::SetPos(float posX, float posY, float posZ)
{
	SetPos(Vector3D(posX, posY, posZ));
}

void Camera::MovePos(Vector3D axis)
{
	eye_ += axis;

	UpdateViewMatrix();
}

void Camera::MovePos(float axisX, float axisY, float axisZ)
{
	MovePos(Vector3D(axisX, axisY, axisZ));
}

void Camera::SetFocus(Vector3D pos)
{
	focus_ = pos;

	UpdateViewMatrix();
}

void Camera::SetFocus(float posX, float posY, float posZ)
{
	SetFocus(Vector3D(posX, posY, posZ));
}

void Camera::MoveFocus(Vector3D axis)
{
	focus_ += axis;

	UpdateViewMatrix();
}

void Camera::MoveFocus(float axisX, float axisY, float axisZ)
{
	MoveFocus(Vector3D(axisX, axisY, axisZ));
}

void Camera::SetViewPort(float topLeftX, float topLeftY, float width, float height)
{
	viewPort_.topLeftX = topLeftX;
	viewPort_.topLeftY = topLeftY;
	viewPort_.width = width;
	viewPort_.height = height;
	viewPort_.minDepth = 0.0f;
	viewPort_.maxDepth = 1.0f;
}

void Camera::AddObject(Object& object)
{
	Object* pObj = &object;
	auto it = find(vecObjPtr_.begin(), vecObjPtr_.end(), pObj);

	if(it != vecObjPtr_.end()){ return; }

	vecObjPtr_.push_back(pObj);

	auto itc = find(pObj->vecCameraPtr_.begin(), pObj->vecCameraPtr_.end(), this);

	if(itc == pObj->vecCameraPtr_.end()){
		pObj->vecCameraPtr_.push_back(this);
	}
}

void Camera::RemoveObject(Object& object)
{
	Object* pObj = &object;
	auto it = find(vecObjPtr_.begin(), vecObjPtr_.end(), pObj);

	if(it == vecObjPtr_.end()){ return; }

	auto itc = find((*it)->vecCameraPtr_.begin(), (*it)->vecCameraPtr_.end(), this);
	if(itc == (*it)->vecCameraPtr_.end()){ return; }
	(*itc) = nullptr;

	vecObjPtr_.erase(it);
}

void Camera::ResetObject()
{
	if(vecObjPtr_.size() > 0){
		for(auto it = begin(vecObjPtr_), itEnd = end(vecObjPtr_); it != itEnd; ++it){
			auto itc = find((*it)->vecCameraPtr_.begin(), (*it)->vecCameraPtr_.end(), this);
			if(itc == (*it)->vecCameraPtr_.end()){ continue; }
			(*itc) = nullptr;
		}
		vector<Object*>().swap(vecObjPtr_);
	}
}

void Camera::DrawObject()
{
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){ return; }

	for(auto it = begin(vecObjPtr_), itEnd = end(vecObjPtr_); it != itEnd; ++it){
		if((*it)->GetColorA() == 1.0f){
			(*it)->Draw(this);
		}
	}
	for(auto it = begin(vecObjPtr_), itEnd = end(vecObjPtr_); it != itEnd; ++it){
		if((*it)->GetColorA() < 1.0f){
			(*it)->Draw(this);
		}
	}

	gm.EndMask();
}

bool Camera::IsVisible(Object & object)
{
	GraphicManager& gm = GraphicManager::Instance();

	XMVECTOR eye = XMVectorSet(eye_.x, eye_.y, eye_.z, 1.0f);
	XMVECTOR focus = XMVectorSet(focus_.x, focus_.y, focus_.z, 1.0f);
	XMVECTOR up = XMVectorSet(0.0f, upY_, 0.0f, 1.0f);

	XMMATRIX view = XMMatrixLookAtLH(eye, focus, up);

	XMMATRIX proj = XMMatrixPerspectiveFovLH(viewAngle_, aspect_, nearZ_, (farZ_ > nearZ_) ? farZ_ : nearZ_ + 0.0001f);

	XMVECTOR world;
	world.m128_f32[0] = object.GetPosX();
	world.m128_f32[1] = object.GetPosY();
	world.m128_f32[2] = object.GetPosZ();
	world.m128_f32[3] = 1.0f;

	world = XMVector3Transform(world, view);
	world = XMVector3Transform(world, proj);

	XMFLOAT3 temp;
	XMStoreFloat3(&temp, world);

	// ƒXƒNƒŠ[ƒ“•ÏŠ·
	float w = gm.GetWidth() / 2.0f;
	float h = gm.GetHeight() / 2.0f;
	XMMATRIX viewport = {
		w, 0, 0, 0,
		0, -h, 0, 0,
		0, 0, 1, 0,
		w, h, 0, 1
	};

	XMVECTOR viewVec = XMVectorSet(temp.x / temp.z, temp.y / temp.z, 1.0f, 1.0f);
	viewVec = XMVector3Transform(viewVec, viewport);

	if(viewVec.m128_f32[0] >= 0.0f
		&& viewVec.m128_f32[0] <= gm.GetWidth()
		&& viewVec.m128_f32[1] >= 0.0f
		&& viewVec.m128_f32[1] <= gm.GetHeight()){
		return true;
	}

	return false;
}

bool Camera::CheckHitRayToSphere(Object & object)
{
	Vector3D u = eye_ - object.GetPos();
	Vector3D v = focus_ - eye_;
	float r = max(max(object.GetScale().x, object.GetScale().y), object.GetScale().z) * 0.5f;

	float a = v.Dot(v);
	float b = v.Dot(u);
	float c = u.Dot(u) - r * r;

	if(a - 0.0001f <= 0.0f){
		return false;
	}

	float hit = b * b - a * c;
	if(hit < 0.0f) {
		return false;
	}

	float t = (-b - sqrtf(b * b - a * c)) / a;
	if(t > 1.0f){
		return false;
	}

	u = focus_ - object.GetPos();
	v = eye_ - focus_;
	r = max(max(object.GetScale().x, object.GetScale().y), object.GetScale().z) * 0.5f;

	a = v.Dot(v);
	b = v.Dot(u);
	c = u.Dot(u) - r * r;

	if(a - 0.0001f <= 0.0f){
		return false;
	}

	hit = b * b - a * c;
	if(hit < 0.0f) {
		return false;
	}

	t = (-b - sqrtf(b * b - a * c)) / a;
	if(t > 1.0f){
		return false;
	}

	return true;
}

void Camera::UpdateViewMatrix()
{
	if(eye_ == focus_){
		eye_.z -= nearZ_;
	}

	XMVECTOR eye = XMVectorSet(eye_.x, eye_.y, eye_.z, 1.0f);
	XMVECTOR focus = XMVectorSet(focus_.x, focus_.y, focus_.z, 1.0f);
	XMVECTOR up = XMVectorSet(0.0f, upY_, 0.0f, 1.0f);

	XMFLOAT4X4 matView;
	XMStoreFloat4x4(&matView, XMMatrixLookAtLH(eye, focus, up));
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			view_.r[i][j] = matView.m[i][j];
		}
	}
}
