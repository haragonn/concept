#ifndef INCLUDE_EIDOS_LIGHT_H
#define INCLUDE_EIDOS_LIGHT_H

#include "../../../idea/h/Utility/ideaMath.h"

class Light{
public:
	void SetLight(Vector3D axis);
	void SetLight(float axisX, float axisY, float axisZ);
};

#endif	// #ifndef INCLUDE_EIDOS_LIGHT_H