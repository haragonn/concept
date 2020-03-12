#ifndef INCLUDE_EIDOS_LIGHT_H
#define INCLUDE_EIDOS_LIGHT_H

#include "../../../idea/h/Utility/ideaMath.h"

class Light{
public:
	static void SetLight(Vector3D direction);
	static void SetLight(float directionX, float directionY, float directionZ);

	static Vector3D GetLightDirection();
};

#endif	// #ifndef INCLUDE_EIDOS_LIGHT_H