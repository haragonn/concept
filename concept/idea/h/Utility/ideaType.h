#ifndef INCLUDE_IDEA_IDEATYPE_H
#define INCLUDE_IDEA_IDEATYPE_H

#include "ideaMath.h"
#include "ideaColor.h"

// 2D���_�f�[�^�\����
struct VertexData2D
{
	Vector3D pos;
	Color color;
	Vector2D tex;
};

// 2D�萔�o�b�t�@
struct ConstBuffer2D
{
	Matrix4x4 proj;
};

// �C���X�^���V���O�X�g���N�`���o�b�t�@
struct PerInstanceData
{
	Vector4D pos;
};

#endif	// #ifndef INCLUDE_IDEA_IDEATYPE_H