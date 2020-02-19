/*==============================================================================
	[Actor3D.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_EIDOS_ACTOR3D_H
#define INCLUDE_EIDOS_ACTOR3D_H

#include "../../../idea/h/Utility/ideaMath.h"
#include "../../../idea/h/Utility/ideaColor.h"

//------------------------------------------------------------------------------
// �N���X���@�FActor3D
// �N���X�T�v�F3D��ł̈ʒu,�傫��,��],�F�����N���X
// �@�@�@�@�@�@�R���W�����@�\������
//------------------------------------------------------------------------------
class Actor3D{
public:
	Actor3D();
	virtual ~Actor3D();

	// ������
	virtual void Init(Vector3D pos);
	virtual void Init(float posX, float posY, float posZ);
	// �I��
	virtual void UnInit();

	// �ʒu�̐ݒ�
	void SetPos(Vector3D pos);
	void SetPos(float posX, float posY, float posZ);

	// �ʒu�̈ړ�
	void MovePos(Vector3D axis);
	void MovePos(float axisX, float axisY, float axisZ);

	// �ʒu�̎擾
	Vector3D GetPos()const{ return pos_; }
	float GetPosX()const{ return pos_.x; }
	float GetPosY()const{ return pos_.y; }
	float GetPosZ()const{ return pos_.z; }

	// �g�k�{���̐ݒ�
	void SetScale(Vector3D scale);
	void SetScale(float scaleX, float scaleY, float scaleZ);

	// �g�k�{���̎擾
	Vector3D GetScale()const{ return scale_; }
	float GetScaleX()const{ return scale_.x; }
	float GetScaleY()const{ return scale_.y; }
	float GetScaleZ()const{ return scale_.z; }

	// ��]�p�̐ݒ�
	void SetRotate(Vector3D rot);
	void SetRotate(float pitch, float yaw, float roll);

	// ��]
	void MoveRotate(Vector3D rot);
	void MoveRotate(float pitch, float yaw, float roll);

	// ��]�p�̎擾
	Vector3D GetRotele()const{ return rot_; }
	float GetRotateX()const{ return rot_.x; }
	float GetRotateY()const{ return rot_.y; }
	float GetRotateZ()const{ return rot_.z; }

	// �F�̐ݒ�
	void SetColor(Color color);
	void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

	// �F�̎擾
	float GetColorR()const{ return color_.r; }
	float GetColorG()const{ return color_.g; }
	float GetColorB()const{ return color_.b; }
	float GetColorA()const{ return color_.a; }

	//DirectX::XMFLOAT4X4* GetWorldMatrixPtr()const{ return pMatWorld_; }

	// ���̐ڐG����
	bool CheckHitSphere(Actor3D& target);

	// ���̐ڐG����
	bool CheckHitBox(Actor3D& target);

protected:
	Vector3D pos_;		// �ʒu

	Vector3D scale_;	// �g�k�{��

	Vector3D rot_;		// ��]�p

	Color color_;		// �F

	// OBB
	struct OBB{
		Vector3D c;
		Vector3D u[3];
		Vector3D e;
	}obb_;

	Matrix4x4 world_;

	//DirectX::XMFLOAT4X4* pMatWorld_;	// ���[���h�}�g���N�X

private:
	// ���[���h�I���̍X�V
	inline void UpdateWorldMatrix();
};

#endif	// #ifndef INCLUDE_EIDOS_ACTOR3D_H