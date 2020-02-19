#ifndef INCLUDE_EIDOS_PLAINMESH_H
#define INCLUDE_EIDOS_PLAINMESH_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../3D/Object.h"
#include "../../../idea/h/Texture/TextureHolder.h"

struct ID3D11Buffer;

class PlaneMesh : public Object, public TextureHolder{
public:
	PlaneMesh();
	~PlaneMesh(){ Release(); }
	bool Create(float centerX, float centerZ,float widthX, float z, unsigned int uNum = 1U, unsigned int vNum = 1U);
	void Release();
	void SetTexture(Texture& tex);
	void ExclusionTexture();

private:
	float widthX_;
	float widthZ_;
	unsigned int uNum_;
	unsigned int vNum_;
	int indexNum_;
	ID3D11Buffer* pVertexBuffer_;
	ID3D11Buffer* pIndexBuffer_;

	void Draw(Camera* pCamera)override;
	inline void DrawPlain(Camera* pCamera, int blend = 0);	// 矩形の描画
	inline void DrawTexturePlain(Camera* pCamera, const Texture& tex, int blend = 0);	// 矩形の描画
};

#endif	// #ifndef INCLUDE_EIDOS_PLAINMESH_H