/*==============================================================================
	[Object.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_EIDOS_OBJECT_H
#define INCLUDE_EIDOS_OBJECT_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Actor3D.h"
#include "../Environment/Camera.h"
#include <vector>

// ���_�\����
struct MeshVertexData
{
	Vector3D pos;
	Vector3D nor;
	Vector2D tex;
};

//------------------------------------------------------------------------------
// �N���X���@�FObject
// �N���X�T�v�FActor3D���p�������N���X,�J�����ɉf����̂̊��N���X
//------------------------------------------------------------------------------
class Object : public Actor3D{
public:
	Object()
	{
		std::vector<Camera*>().swap(vecCameraPtr_);
	}

	virtual ~Object()
	{
		if(vecCameraPtr_.size() > 0){
			for(auto it = std::begin(vecCameraPtr_), itEnd = std::end(vecCameraPtr_); it != itEnd; ++it){
				if(*it){
					(*it)->RemoveObject(*this);
				}
			}
		}
	}

protected:
	friend class Camera;

	std::vector<Camera*> vecCameraPtr_;

	virtual void Draw(Camera* pCamera) = 0;
};

#endif	// #ifndef INCLUDE_EIDOS_OBJECT_H