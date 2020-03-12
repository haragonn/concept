#include "../../h/Environment/Light.h"
#include "../../h/3D/ObjectManager.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

void Light::SetLight(Vector3D direction)
{
	SetLight(direction.x, direction.y, direction.z);
}

void Light::SetLight(float directionX, float directionY, float directionZ)
{
	ObjectManager::Instance().SetLight(directionX, directionY, directionZ);
}

Vector3D Light::GetLightDirection()
{
	ObjectManager om = ObjectManager::Instance();
	return Vector3D(om.GetLight().m128_f32[0], om.GetLight().m128_f32[1], om.GetLight().m128_f32[2]);
}
