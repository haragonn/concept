#ifndef INCLUDE_IDEA_IDEATYPE_H
#define INCLUDE_IDEA_IDEATYPE_H

#include "ideaMath.h"
#include "ideaColor.h"

#define WIN32_LEAN_AND_MEAN
#include <directxmath.h>

// 2D頂点データ構造体
struct VertexData2D
{
	Vector3D pos;
	Color color;
	Vector2D tex;
};

// 2D定数バッファ
struct ConstBuffer2D
{
	DirectX::XMFLOAT4X4 proj;
};

// インスタンシングストラクチャバッファ
struct PerInstanceData
{
	DirectX::XMFLOAT4 pos;
};

#endif	// #ifndef INCLUDE_IDEA_IDEATYPE_H