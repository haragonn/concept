#include "../../h/Environment/Light.h"
#include "../../h/3D/ObjectManager.h"

void Light::SetLight(Vector3D axis)
{
	SetLight(axis.x, axis.y, axis.z);
}

void Light::SetLight(float axisX, float axisY, float axisZ)
{
	ObjectManager::Instance().SetLight(axisX, axisY, axisZ);
}
