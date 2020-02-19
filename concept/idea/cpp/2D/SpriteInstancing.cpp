#include "../../h/2D/SpriteInstancing.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/2D/SpriteInstancingManager.h"
#include "../../h/2D/SpriteManager.h"
#include "../../h/2D/Sprite.h"
#include "../../h/Texture/Texture.h"
#include "../../h/Utility/ideaMath.h"
#include <algorithm>

//------------------------------------------------------------------------------
// using namespace
//------------------------------------------------------------------------------
using namespace std;
using namespace DirectX;

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
SpriteInstancing::SpriteInstancing() :
	halfWidth_(0.0f),
	halfHeight_(0.0f),
	bReversedX_(false),
	bReversedY_(false),
	rad_(0.0f)
{
	vector<Sprite*>().swap(vecSpritePtr_);
}

void SpriteInstancing::Init(float width, float height)
{
	SetSize(width, height);
	rad_ = 0.0f;
	color_ = Color(1.0f, 1.0f, 1.0f, 1.0f);
}

void SpriteInstancing::UnInit()
{
	halfWidth_ = 0.0f;
	halfHeight_ = 0.0f;
	rad_ = 0.0f;
	color_ = Color(0.0f, 0.0f, 0.0f, 0.0f);
	vector<Sprite*>().swap(vecSpritePtr_);
}


//------------------------------------------------------------------------------
// 大きさの設定
// 引数　：幅(float width),高さ(float height)
// 戻り値：なし
//------------------------------------------------------------------------------
void SpriteInstancing::SetSize(float width, float height)
{
	// 引数の反映
	halfWidth_ = width * 0.5f;
	halfHeight_ = height * 0.5f;

	// 値がマイナスの時は反転フラグをオンにして正数にする
	bReversedX_ = false;
	if(halfWidth_ < 0.0f){
		halfWidth_ *= -1.0f;
		bReversedX_ = true;
	}
	bReversedY_ = false;
	if(halfHeight_ < 0.0f){
		halfHeight_ *= -1.0f;
		bReversedY_ = true;
	}
}

//------------------------------------------------------------------------------
// 回転角の設定
// 引数　：回転角(float rad)
// 戻り値：なし
//------------------------------------------------------------------------------
void SpriteInstancing::SetRotate(float rad)
{
	// 引数の反映
	rad_ = rad;
}

//------------------------------------------------------------------------------
// 回転
// 引数　：回転量(float rad)
// 戻り値：なし
//------------------------------------------------------------------------------
void SpriteInstancing::MoveRotate(float rad)
{
	// 回転量をプラスする
	rad_ += rad;
}

void SpriteInstancing::SetColor(Color color)
{
	color_ = color;
}

//------------------------------------------------------------------------------
// 色の設定
// 引数　：赤成分(float r),緑成分(float g),青成分(float b),a値(float a)
// 戻り値：なし
//------------------------------------------------------------------------------
void SpriteInstancing::SetColor(float r, float g, float b, float a)
{
	color_.r = max(0.0f, min(1.0f, r));
	color_.g = max(0.0f, min(1.0f, g));
	color_.b = max(0.0f, min(1.0f, b));
	color_.a = max(0.0f, min(1.0f, a));
}

void SpriteInstancing::AddSprite(Sprite & sprite)
{
	// 画面外なら終了
	GraphicManager& gm = GraphicManager::Instance();

	float longVicinity = max(halfWidth_, halfHeight_);

	if(sprite.pos_.x + longVicinity * ROOT2 < 0.0f
		|| sprite.pos_.x - longVicinity * ROOT2 > gm.GetWidth()
		|| sprite.pos_.y + longVicinity * ROOT2 < 0.0f
		|| sprite.pos_.y - longVicinity * ROOT2 > gm.GetHeight()){ return; }

	Sprite* pSpr = &sprite;
	auto it = find(vecSpritePtr_.begin(), vecSpritePtr_.end(), pSpr);

	if(it != vecSpritePtr_.end()){ return; }

	vecSpritePtr_.push_back(pSpr);

	auto itc = find(pSpr->vecSpriteInstancingPtr_.begin(), pSpr->vecSpriteInstancingPtr_.end(), this);

	if(itc == pSpr->vecSpriteInstancingPtr_.end()){
		pSpr->vecSpriteInstancingPtr_.push_back(this);
	}
}

void SpriteInstancing::RemoveSprite(Sprite & sprite)
{
	Sprite* pSpr = &sprite;
	auto it = find(vecSpritePtr_.begin(), vecSpritePtr_.end(), pSpr);

	if(it == vecSpritePtr_.end()){ return; }

	auto itc = find((*it)->vecSpriteInstancingPtr_.begin(), (*it)->vecSpriteInstancingPtr_.end(), this);
	if(itc == (*it)->vecSpriteInstancingPtr_.end()){ return; }
	(*itc) = nullptr;

	vecSpritePtr_.erase(it);
}

void SpriteInstancing::ResetSprite()
{
	if(vecSpritePtr_.size() > 0){
		for(auto it = begin(vecSpritePtr_), itEnd = end(vecSpritePtr_); it != itEnd; ++it){
			auto itc = find((*it)->vecSpriteInstancingPtr_.begin(), (*it)->vecSpriteInstancingPtr_.end(), this);
			if(itc == (*it)->vecSpriteInstancingPtr_.end()){ continue; }
			(*itc) = nullptr;
		}
		vector<Sprite*>().swap(vecSpritePtr_);
	}
}

//------------------------------------------------------------------------------
// 矩形の描画
// 引数　：合成方法(int blend)
// 戻り値：なし
//------------------------------------------------------------------------------
void SpriteInstancing::DrawRect(int blend)
{
	// 準備ができていなければ終了
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();
	SpriteInstancingManager& sim = SpriteInstancingManager::Instance();

	if(!gm.GetContextPtr()
		|| !sm.GetPixelShederDefaultPtr()
		|| !sm.GetRectVertexBufferPtr()
		|| !sim.GetVertexShederPtr()
		|| vecSpritePtr_.size() == 0){ return; }

	// 頂点情報
	VertexData2D vd[SpriteManager::RECT_VERTEX_NUM];
	vd[0] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 0.0f) };
	vd[1] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 0.0f) };
	vd[2] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 1.0f) };
	vd[3] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 1.0f) };

	// 頂点情報の計算
	float x, y, axisX, axisY;
	float tSin = sinf(rad_);
	float tCos = cosf(rad_);
	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth_ : -halfWidth_;
		y = (i > 1) ? halfHeight_ : -halfHeight_;
		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;
		vd[i].pos.x = axisX;
		vd[i].pos.y = axisY;
	}

	// バッファ書き込み
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vd, sizeof(VertexData2D) * SpriteManager::RECT_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// 頂点バッファのセット
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	gm.GetContextPtr()->Map(sim.GetInstanceDataBufferPtr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );

	PerInstanceData* pInstanceData = (PerInstanceData*)MappedResource.pData;

	int i = 0;
	for(auto it = begin(vecSpritePtr_), itEnd = end(vecSpritePtr_); it != itEnd && i < SpriteInstancingManager::INSTANCE_MAX; ++i, ++it){
		pInstanceData[i].pos.x = (*it)->pos_.x;
		pInstanceData[i].pos.y = (*it)->pos_.y;
		pInstanceData[i].pos.z = 0.0f;
		pInstanceData[i].pos.w = 1.0f;
	}

	gm.GetContextPtr()->Unmap(sim.GetInstanceDataBufferPtr(), 0);

	ID3D11ShaderResourceView* pSrv = sim.GetTransformShaderResourceViewPtr();
	gm.GetContextPtr()->VSSetShaderResources(1, 1, &pSrv);

	// ブレンディングのセット
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	}else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	}else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(sim.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)gm.GetWidth();
	viewPort.Height = (FLOAT)gm.GetHeight();
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//ポリゴン描画
	gm.GetContextPtr()->DrawInstanced(SpriteManager::RECT_VERTEX_NUM, min(SpriteInstancingManager::INSTANCE_MAX, vecSpritePtr_.size()), 0, 0);
}

//------------------------------------------------------------------------------
// テクスチャの描画
// 引数　：テクスチャ(const Texture& tex),合成方法(int blend)
// 戻り値：なし
//------------------------------------------------------------------------------
void SpriteInstancing::DrawTexture(const Texture & tex, int blend)
{
	// 準備ができていなければ終了
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();
	SpriteInstancingManager& sim = SpriteInstancingManager::Instance();

	if(!gm.GetContextPtr()
		|| !sm.GetPixelShederDefaultPtr()
		|| !sm.GetPixelShederTexturePtr()
		|| !sm.GetRectVertexBufferPtr()
		|| !sim.GetVertexShederPtr()
		|| vecSpritePtr_.size() == 0){ return; }

	// 頂点情報
	VertexData2D vd[SpriteManager::RECT_VERTEX_NUM];
	vd[0] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 0.0f) };
	vd[1] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 0.0f) };
	vd[2] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 1.0f) };
	vd[3] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 1.0f) };

	// 頂点情報の計算
	float x, y, axisX, axisY;
	float tSin = sinf(rad_);
	float tCos = cosf(rad_);
	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth_ : -halfWidth_;
		y = (i > 1) ? halfHeight_ : -halfHeight_;
		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;
		vd[i].pos.x = axisX;
		vd[i].pos.y = axisY;
		vd[i].tex.x = (!!(i % 2) ^ bReversedX_) ? 1.0f : 0.0f;
		vd[i].tex.y = (!!(i > 1) ^ bReversedY_) ? 1.0f : 0.0f;
	}

	// バッファ書き込み
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vd, sizeof(VertexData2D) * SpriteManager::RECT_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// 頂点バッファのセット
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// テクスチャ書き込み
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	gm.GetContextPtr()->Map(sim.GetInstanceDataBufferPtr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );

	PerInstanceData* pInstanceData = (PerInstanceData*)MappedResource.pData;

	int i = 0;
	for(auto it = begin(vecSpritePtr_), itEnd = end(vecSpritePtr_); it != itEnd && i < SpriteInstancingManager::INSTANCE_MAX; ++i, ++it){
		pInstanceData[i].pos.x = (*it)->pos_.x;
		pInstanceData[i].pos.y = (*it)->pos_.y;
		pInstanceData[i].pos.z = 0.0f;
		pInstanceData[i].pos.w = 1.0f;
	}

	gm.GetContextPtr()->Unmap(sim.GetInstanceDataBufferPtr(), 0);

	ID3D11ShaderResourceView* pSrv = sim.GetTransformShaderResourceViewPtr();
	gm.GetContextPtr()->VSSetShaderResources(1, 1, &pSrv);

	// ブレンディングのセット
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	}else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	}else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(sim.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederTexturePtr(), NULL, 0);
	}else{
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)gm.GetWidth();
	viewPort.Height = (FLOAT)gm.GetHeight();
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//ポリゴン描画
	gm.GetContextPtr()->DrawInstanced(SpriteManager::RECT_VERTEX_NUM, min(SpriteInstancingManager::INSTANCE_MAX, vecSpritePtr_.size()), 0, 0);
}

//------------------------------------------------------------------------------
// 等分割されたテクスチャの描画
// 引数　：テクスチャ(const Texture& tex),U座標(int uNum),
// 　　　　V座標(int vNum),合成方法(int blend)
// 戻り値：なし
//------------------------------------------------------------------------------
void SpriteInstancing::DrawDividedTexture(const Texture & tex, int uNum, int vNum, int blend)
{

	// 準備ができていなければ終了
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();
	SpriteInstancingManager& sim = SpriteInstancingManager::Instance();

	if(!gm.GetContextPtr()
		|| !sm.GetPixelShederDefaultPtr()
		|| !sm.GetPixelShederTexturePtr()
		|| !sm.GetRectVertexBufferPtr()
		|| !sim.GetVertexShederPtr()
		|| vecSpritePtr_.size() == 0){ return; }

	// 頂点情報
	VertexData2D vd[SpriteManager::RECT_VERTEX_NUM];
	vd[0] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 0.0f) };
	vd[1] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 0.0f) };
	vd[2] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 1.0f) };
	vd[3] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 1.0f) };

	// 頂点情報の計算
	float x, y, axisX, axisY;
	float tSin = sinf(rad_);
	float tCos = cosf(rad_);
	float u1 = tex.GetDivU() * uNum;
	float u2 = tex.GetDivU() * (uNum + 1);
	float v1 = tex.GetDivV() * vNum;
	float v2 = tex.GetDivV() * (vNum + 1);
	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth_ : -halfWidth_;
		y = (i > 1) ? halfHeight_ : -halfHeight_;
		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;
		vd[i].pos.x = axisX;
		vd[i].pos.y = axisY;
		vd[i].tex.x = (!!(i % 2) ^ bReversedX_) ? u2 : u1;
		vd[i].tex.y = (!!(i > 1) ^ bReversedY_) ? v2 : v1;
	}

	// バッファ書き込み
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vd, sizeof(VertexData2D) * SpriteManager::RECT_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// 頂点バッファのセット
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// テクスチャ書き込み
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	gm.GetContextPtr()->Map(sim.GetInstanceDataBufferPtr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );

	PerInstanceData* pInstanceData = (PerInstanceData*)MappedResource.pData;

	int i = 0;
	for(auto it = begin(vecSpritePtr_), itEnd = end(vecSpritePtr_); it != itEnd && i < SpriteInstancingManager::INSTANCE_MAX; ++i, ++it){
		pInstanceData[i].pos.x = (*it)->pos_.x;
		pInstanceData[i].pos.y = (*it)->pos_.y;
		pInstanceData[i].pos.z = 0.0f;
		pInstanceData[i].pos.w = 1.0f;
	}

	gm.GetContextPtr()->Unmap(sim.GetInstanceDataBufferPtr(), 0);

	ID3D11ShaderResourceView* pSrv = sim.GetTransformShaderResourceViewPtr();
	gm.GetContextPtr()->VSSetShaderResources(1, 1, &pSrv);

	// ブレンディングのセット
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	}else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	}else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(sim.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederTexturePtr(), NULL, 0);
	}else{
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)gm.GetWidth();
	viewPort.Height = (FLOAT)gm.GetHeight();
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//ポリゴン描画
	gm.GetContextPtr()->DrawInstanced(SpriteManager::RECT_VERTEX_NUM, min(SpriteInstancingManager::INSTANCE_MAX, vecSpritePtr_.size()), 0, 0);
}

//------------------------------------------------------------------------------
// 表示範囲を指定したテクスチャーの描画
// 引数　：テクスチャ(const Texture& tex),U座標(int uNum),
// 　　　　V座標(int vNum),幅(float width),高さ(float width),
// 　　　　合成方法(int blend)
// 戻り値：なし
//------------------------------------------------------------------------------
void SpriteInstancing::DrawDelimitedTexture(const Texture & tex, float u, float v, float width, float height, int blend)
{

	// 準備ができていなければ終了
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();
	SpriteInstancingManager& sim = SpriteInstancingManager::Instance();

	if(!gm.GetContextPtr()
		|| !sm.GetPixelShederDefaultPtr()
		|| !sm.GetPixelShederTexturePtr()
		|| !sm.GetRectVertexBufferPtr()
		|| !sim.GetVertexShederPtr()){ return; }

	// 頂点情報
	VertexData2D vd[SpriteManager::RECT_VERTEX_NUM];
	vd[0] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 0.0f) };
	vd[1] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 0.0f) };
	vd[2] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 1.0f) };
	vd[3] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 1.0f) };

	// 頂点情報の計算
	float x, y, axisX, axisY;
	float tSin = sinf(rad_);
	float tCos = cosf(rad_);
	float u1 = u;
	float u2 = u + width;
	float v1 = v;
	float v2 = v + height;
	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth_ : -halfWidth_;
		y = (i > 1) ? halfHeight_ : -halfHeight_;
		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;
		vd[i].pos.x = axisX;
		vd[i].pos.y = axisY;
		vd[i].tex.x = (!!(i % 2) ^ bReversedX_) ? u2 : u1;
		vd[i].tex.y = (!!(i > 1) ^ bReversedY_) ? v2 : v1;
	}

	// バッファ書き込み
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vd, sizeof(VertexData2D) * SpriteManager::RECT_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// 頂点バッファのセット
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// テクスチャ書き込み
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	gm.GetContextPtr()->Map(sim.GetInstanceDataBufferPtr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );

	PerInstanceData* pInstanceData = (PerInstanceData*)MappedResource.pData;

	int i = 0;
	for(auto it = begin(vecSpritePtr_), itEnd = end(vecSpritePtr_); it != itEnd && i < SpriteInstancingManager::INSTANCE_MAX; ++i, ++it){
		pInstanceData[i].pos.x = (*it)->pos_.x;
		pInstanceData[i].pos.y = (*it)->pos_.y;
		pInstanceData[i].pos.z = 0.0f;
		pInstanceData[i].pos.w = 1.0f;
	}

	gm.GetContextPtr()->Unmap(sim.GetInstanceDataBufferPtr(), 0);

	ID3D11ShaderResourceView* pSrv = sim.GetTransformShaderResourceViewPtr();
	gm.GetContextPtr()->VSSetShaderResources(1, 1, &pSrv);

	// ブレンディングのセット
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	}else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	}else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(sim.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederTexturePtr(), NULL, 0);
	}else{
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)gm.GetWidth();
	viewPort.Height = (FLOAT)gm.GetHeight();
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//ポリゴン描画
	gm.GetContextPtr()->DrawInstanced(SpriteManager::RECT_VERTEX_NUM, min(SpriteInstancingManager::INSTANCE_MAX, vecSpritePtr_.size()), 0, 0);
}
