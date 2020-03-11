#ifndef INCLUDE_IDEA_IDEATYPE_H
#define INCLUDE_IDEA_IDEATYPE_H

#include "ideaMath.h"
#include "ideaColor.h"

#define WIN32_LEAN_AND_MEAN
#include <directxmath.h>

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
	DirectX::XMFLOAT4X4 proj;
};

// �C���X�^���V���O�X�g���N�`���o�b�t�@
struct PerInstanceData
{
	DirectX::XMFLOAT4 pos;
};

#endif	// #ifndef INCLUDE_IDEA_IDEATYPE_H