#include "../../h/Motion/VmdMotion.h"
#include <fstream>
#include <algorithm>
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

VmdMotion::VmdMotion() :
	pModl_(nullptr),
	motionSize(0),
	elapsedTime_(0.0f),
	frameMax_(0),
	gapFrame_(0),
	bLoop_(true),
	bFinish_(true)
{
	vector<VmdMotionData>().swap(vecVmdMotion_);
	unordered_map <string, vector<KeyFrame>>().swap(umapMotionData_);
}

VmdMotion::~VmdMotion()
{
	UnLoad();
}

bool VmdMotion::LoadVmdMotionFromFile(const char * pFileName, PmdModel & mdl, bool bLoop)
{
	pModl_ = &mdl;
	bLoop_ = bLoop;
	elapsedTime_ = 0.0f;
	gapFrame_ = 0;

	// VMDファイルからVMDデータを抽出
	ifstream ifs(pFileName, ios::binary);
	if(!ifs.is_open()){ return false; }

	VmdHeader vmdHeader;

	//ヘッタ―の読み込み（使わない部分）
	ifs.read((char*)&vmdHeader, sizeof(VmdHeader));

	//モーションデータ数読み込み
	ifs.read((char*)&motionSize, sizeof(motionSize));

	//モーションデータ数 > 0
	if(motionSize > 0)
	{
		vecVmdMotion_.resize(motionSize);

		// モーションデータ読み込み
		ifs.read((char*)&vecVmdMotion_[0], sizeof(VmdMotionData) * motionSize);
	}

	// モーションデータ格納
	frameMax_ = 0;
	for(auto& vmdMotion : vecVmdMotion_){
		umapMotionData_[vmdMotion.boneName].emplace_back(KeyFrame(
			vmdMotion.frameNo,
			vmdMotion.quaternion,
			Vector2D((float)vmdMotion.bezier[3] / 127.0f, (float)vmdMotion.bezier[7] / 127.0f),
			Vector2D((float)vmdMotion.bezier[11] / 127.0f, (float)vmdMotion.bezier[15] / 127.0f)
		));
		frameMax_ = max(frameMax_, vmdMotion.frameNo);
	}

	// フレーム番号でソート
	for(auto& motion : umapMotionData_){
		sort(
			motion.second.begin(), motion.second.end(),
			[](const KeyFrame& lval, const KeyFrame& rval)
		{
			return lval.frameNo <= rval.frameNo;
		}
		);
	}

	return true;
}

void VmdMotion::UpdatePmd(float speed)
{
	if(!pModl_){ return; }

	elapsedTime_ += 0.5f * speed;
	unsigned int frameNo = (unsigned int)(elapsedTime_);

	bFinish_ = false;
	if(frameNo >= frameMax_ && !gapFrame_){
		if(bLoop_){
			gapFrame_ = 0;
		}
	}
	if(frameNo >= frameMax_ + gapFrame_){
		if(bLoop_){
			gapFrame_ = 0;
			elapsedTime_ = 0.0f;
			frameNo = 0;
		} else{
			bFinish_ = true;
		}
	}

	fill(pModl_->vecBoneMatrix_.begin(), pModl_->vecBoneMatrix_.end(), Matrix4x4::Identity());

	for(auto& md : umapMotionData_){
		auto itBone = pModl_->mapBone_.find(md.first);
		if(itBone == pModl_->mapBone_.end()){ continue; }
		auto node = itBone->second;
		if(node.index == -1){ continue; }


		auto motions = md.second;
		auto rit = find_if(
			motions.rbegin(), motions.rend(),
			[frameNo](const KeyFrame& motion)
		{
			return motion.frameNo <= frameNo;
		}
		);

		if(rit == motions.rend()){ continue; }

		XMMATRIX rotation;
		auto it = rit.base();

		if(!gapFrame_){
			if(it != motions.end()){
				float t = (elapsedTime_ - static_cast<float>(rit->frameNo))
					/ static_cast<float>(it->frameNo - rit->frameNo);

				t = Bezier(t, it->p1, it->p2, 12);

				XMVECTOR q1 = XMVectorSet(rit->quaternion.x, rit->quaternion.y, rit->quaternion.z, rit->quaternion.w);
				XMVECTOR q2 = XMVectorSet(it->quaternion.x, it->quaternion.y, it->quaternion.z, it->quaternion.w);
				XMVECTOR  q = XMQuaternionSlerp(q1, q2, t);

				rotation = XMMatrixRotationQuaternion(q);
			} else{
				XMVECTOR q = XMVectorSet(rit->quaternion.x, rit->quaternion.y, rit->quaternion.z, rit->quaternion.w);

				rotation = XMMatrixRotationQuaternion(q);
			}
		} else{
			rit = find_if(
				motions.rbegin(), motions.rend(),
				[frameNo](const KeyFrame& motion)
			{
				return motion.frameNo == 0;
			}
			);
			auto rsit = find_if(
				motions.rbegin(), motions.rend(),
				[frameNo](const KeyFrame& motion)
			{
				return motion.frameNo <= frameNo;
			}
			);
			if(rsit != motions.rend()){
				float t = (elapsedTime_ - static_cast<float>(frameMax_ + gapFrame_))
					/ static_cast<float>(0.0f - gapFrame_);

				t = Bezier(t, rsit->p1, rsit->p2, 12);

				XMVECTOR q1 = XMVectorSet(rit->quaternion.x, rit->quaternion.y, rit->quaternion.z, rit->quaternion.w);
				XMVECTOR q2 = XMVectorSet(rsit->quaternion.x, rsit->quaternion.y, rsit->quaternion.z, rsit->quaternion.w);
				XMVECTOR  q = XMQuaternionSlerp(q1, q2, t);

				rotation = XMMatrixRotationQuaternion(q);
			} else{
				XMVECTOR q = XMVectorSet(rit->quaternion.x, rit->quaternion.y, rit->quaternion.z, rit->quaternion.w);

				rotation = XMMatrixRotationQuaternion(q);
			}
		}

		auto& pos = node.pos;
		auto mat = XMMatrixTranslation(-pos.x, -pos.y, -pos.z);
		mat *= rotation;
		mat *= XMMatrixTranslation(pos.x, pos.y, pos.z);

		for(int i = 0; i < 4; ++i){
			for(int j = 0; j < 4; ++j){
				pModl_->vecBoneMatrix_[node.index].r[i][j] = mat.r[i].m128_f32[j];
			}
		}
	}

	RecursiveMatrixMultiply(&pModl_->mapBone_["センター"], Matrix4x4::Identity());
}

void VmdMotion::Reset()
{
	gapFrame_ = 0;
	elapsedTime_ = 0.0f;
	bFinish_ = false;
}

void VmdMotion::UnLoad()
{
	motionSize = 0;
	gapFrame_ = 0;
	elapsedTime_ = 0.0f;
	bFinish_ = true;
	vector<VmdMotionData>().swap(vecVmdMotion_);
}

void VmdMotion::RecursiveMatrixMultiply(Bone * node, const Matrix4x4 & mat)
{
	pModl_->vecBoneMatrix_[node->index] *= mat;

	for(auto& cnode : node->vecChildrenPtr){
		RecursiveMatrixMultiply(cnode, pModl_->vecBoneMatrix_[node->index]);
	}
}
