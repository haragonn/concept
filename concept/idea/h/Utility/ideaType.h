#ifndef INCLUDE_IDEA_IDEATYPE_H
#define INCLUDE_IDEA_IDEATYPE_H

#include "ideaMath.h"
#include "ideaColor.h"

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
	Matrix4x4 proj;
};

// インスタンシングストラクチャバッファ
struct PerInstanceData
{
	Vector4D pos;
};

#endif	// #ifndef INCLUDE_IDEA_IDEATYPE_H