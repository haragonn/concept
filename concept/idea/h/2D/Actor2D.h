/*==============================================================================
	[Actor2D.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_ACTOR2D_H
#define INCLUDE_IDEA_ACTOR2D_H

#include "../Utility/ideaMath.h"
#include "../Utility/ideaColor.h"

//------------------------------------------------------------------------------
// �N���X���@�FActor2D
// �N���X�T�v�F2D��ł̈ʒu�Ƒ傫��,�p�x�����N���X
// �@�@�@�@�@�@�R���W������t�H���[�@�\������
//------------------------------------------------------------------------------
class Actor2D{
public:
	Actor2D();
	virtual ~Actor2D();

	// ������
	virtual void Init(Vector2D pos, Vector2D size);
	virtual void Init(Vector2D pos, float width, float height);
	virtual void Init(float posX, float posY, float width, float height);

	// �I��
	virtual void UnInit();

	// ���W�̐ݒ�
	void SetPos(Vector2D pos);
	void SetPos(float posX, float posY);

	// ���W�̈ړ�
	void MovePos(Vector2D axis);
	void MovePos(float axisX, float axisY);

	// ���W�̎擾
	Vector2D& GetPos(){ return pos_; }
	float GetPosX()const{ return pos_.x; }
	float GetPosY()const{ return pos_.y; }

	// �O�t���[���̍��W�̎擾
	float GetPreX()const{ return prePos_.x; }
	float GetPreY()const{ return prePos_.y; }

	// �T�C�Y�̐ݒ�
	void SetSize(Vector2D size);
	void SetSize(float width, float height);

	// �T�C�Y�̎擾
	Vector2D GetSize()const{ return size_; }
	float GetHalfWidth()const{ return size_.x * 0.5f; }
	float GetHalfHeight()const{ return size_.y * 0.5f; }
	float GetWidth()const{ return size_.x; }
	float GetHeight()const{ return size_.y; }

	// ��]�p�̐ݒ�
	void SetRotate(float rad);

	// ��]
	void MoveRotate(float rad);

	// ��]�p�̎擾
	float GetRoteate()const{ return rad_; }

	// �F�̐ݒ�
	void SetColor(Color color);
	void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

	// ��`�̐ڐG����
	bool CheckHitRect(const Actor2D& target);

	// �~�̐ڐG����
	bool CheckHitCircle(const Actor2D& target);

	// �ʒu�֌W�擾
	float GetRelation(const Actor2D& target);
	float GetPreFrameRelation(const Actor2D& target);

	// �Ǐ]����^�[�Q�b�g�̐ݒ�
	void FollowActor(Actor2D& target);

	// �t�H�����[����߂�
	void QuitFollower();

protected:
	Vector2D pos_;			// ���W
	Vector2D prePos_;		// �O�t���[���̍��W

	Vector2D size_;			// �T�C�Y

	float rad_;				// �p�x

	bool bReversedU_;		// ���]�t���O(U)
	bool bReversedV_;		// ���]�t���O(V)

	Color color_;			// �F

private:
	friend class Actor2DManager;

	bool bRegistered_;		// �o�^�t���O
	Actor2D* pPrev_;		// �O�̃m�[�h
	Actor2D* pNext_;		// ���̃m�[�h
	Actor2D* pLeader_;		// ���[�_�[�̃|�C���^�[
	Actor2D* pFollower_;	// �t�H�����[�̃|�C���^�[

	// �t�H�����[�̈ʒu���ړ�
	void MoveFollower(Vector2D axis);

	// �t�H�����[�̉��U
	void ResetFollower(Actor2D* pTarget);

	// ���W�̕ێ�
	void WriteDownPosition();
};

#endif	// #ifndef INCLUDE_IDEA_ACTOR2D_H