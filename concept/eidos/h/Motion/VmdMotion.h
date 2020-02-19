#ifndef INCLUDE_EIDOS_VMDMOTION_H
#define INCLUDE_EIDOS_VMDMOTION_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../../idea/h/Utility/Timer.h"
#include "../../../idea/h/Utility/ideaMath.h"
#include "../../../idea/h/Utility/ideaUtility.h"
#include "../../../eidos/h/Mesh/PmdModel.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <list>

#pragma pack(push,1)

// VMDのヘッダー
struct VmdHeader 
{
	char vmdHeader[30];		//"Vocaloid Motion Data 0002"
	char vmdModelName[20];  // カメラの場合:"カメラ・照明"
};

//モーション
struct VmdMotionData
{
	char boneName[15];					//ボーン名
	unsigned long frameNo;				//フレーム番号
	float location[3];					// 移動量
	Quaternion quaternion;					// 回転量 (モデルローカル座標系
	unsigned char bezier[64];	// 補完
};

// 表情データ
struct VmdSkin
{ 
	char skinName[15]; // 表情名
	unsigned long flameNo; // フレーム番号
	float weight; // 表情の設定値(表情スライダーの値)
};

#pragma pack(pop)

struct KeyFrame{
	unsigned int frameNo;
	Quaternion quaternion;
	Vector2D p1;
	Vector2D p2;
	KeyFrame(unsigned int no, Quaternion q, Vector2D ip1, Vector2D ip2)
		: frameNo(no), quaternion(q), p1(ip1), p2(ip2){}
};

class VmdMotion{
public:
	VmdMotion();
	~VmdMotion();
	bool LoadVmdMotionFromFile(const char* pFileName, PmdModel & mdl, bool bLoop = false);
	void UpdatePmd(float speed);
	void Reset();
	bool IsFinish()const{ return bFinish_; }
	void UnLoad();

private:
	PmdModel* pModl_;
	unsigned long motionSize;
	std::vector<VmdMotionData> vecVmdMotion_;
	std::unordered_map <std::string, std::vector<KeyFrame>> umapMotionData_;

	float elapsedTime_;
	unsigned long frameMax_;
	unsigned long gapFrame_;
	bool bLoop_;
	bool bFinish_;

	void RecursiveMatrixMultiply(Bone* node, const Matrix4x4& mat);
};

#endif	// #ifndef INCLUDE_EIDOS_VMDMOTION_H