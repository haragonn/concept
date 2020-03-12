#ifndef INCLUDE_EIDOS_VMDMOTION_H
#define INCLUDE_EIDOS_VMDMOTION_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../Utility/eidosType.h"
#include "../../../idea/h/Utility/Timer.h"
#include "../../../idea/h/Utility/ideaMath.h"
#include "../../../idea/h/Utility/ideaUtility.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <list>

class PmdModel;
class PmxModel;
struct VmdBone;

class VmdMotion{
public:
	VmdMotion();
	~VmdMotion();

	bool LoadVmdMotionFromFile(const char* pFileName);

	void UnLoad();

	void UpdateVmd(PmdModel& pmd, bool loop = true, float speed = 1.0f);
	void UpdateVmd(PmxModel& pmx, bool loop = true, float speed = 1.0f);

	void Reset();

	bool IsFinish()const{ return bFinish_; }


private:
	struct KeyFrame{
		unsigned int frameNo;
		Quaternion quaternion;
		Vector2D p1;
		Vector2D p2;
		KeyFrame(unsigned int no, Quaternion q, Vector2D ip1, Vector2D ip2)
			: frameNo(no), quaternion(q), p1(ip1), p2(ip2){}
	};

	unsigned long motionSize;
	std::vector<VmdMotionData> vecVmdMotion_;
	std::unordered_map <std::string, std::vector<KeyFrame>> umapMotionData_;

	float elapsedTime_;

	unsigned long frameMax_;
	unsigned long gapFrame_;

	bool bFinish_;

	void RecursiveMatrixMultiply(PmdModel& pmd, VmdBone* node, const Matrix4x4& mat);
	void RecursiveMatrixMultiply(PmxModel& pmx, VmdBone* node, const Matrix4x4& mat);
};

#endif	// #ifndef INCLUDE_EIDOS_VMDMOTION_H