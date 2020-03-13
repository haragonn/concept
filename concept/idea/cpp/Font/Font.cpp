#include "../../h/Font/Font.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/2D/SpriteManager.h"
#include "../../h/Utility/ideaType.h"
#include "../../h/Utility/ideaUtility.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <locale.h>

using namespace std;

TextureData Font::s_NullTextureData_;

int StrSplit(const char *str, std::vector<char*>* dest)
{
	int i = 0;

	while(*str != '\0') {
		int code = (unsigned char)*str;
		if(code < 0x80) {
			char* arr = new char[2];
			sprintf_s(arr, 2, "%c", *str);
			++i;
			dest->push_back(arr);
			str += 1;
		} else if(code < 0xE0) {
			char* arr = new char[3];
			sprintf_s(arr, 3, "%c%c", *str, *(str + 1));
			++i;
			dest->push_back(arr);
			str += 2;
		} else if(code < 0xF0) {
			char* arr = new char[4];
			sprintf_s(arr, 4, "%c%c%c", *str, *(str + 1), *(str + 2));
			++i;
			dest->push_back(arr);
			str += 3;
		} else {
			char* arr = new char[5];
			sprintf_s(arr, 5, "%c%c%c%c", *str, *(str + 1), *(str + 2), *(str + 3));
			++i;
			dest->push_back(arr);
			str += 4;
		}
	}

	dest->push_back(NULL);

	return i;
}

Font::Font() :
	hf_(NULL)
{
	unordered_map<UINT, TextureData>().swap(umTextureData_);
}

Font::~Font()
{
	UnInit();
}

void Font::Init(int size, int weight, const char* pFontName)
{
	UnInit();

	LOGFONT lf =
	{
		size, 0, 0, 0, weight, 0, 0, 0,
		SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | FF_MODERN,
		"ＭＳ ゴシック"
	};
	strcpy_s(lf.lfFaceName, pFontName);

	hf_ = CreateFontIndirect(&lf);
}

void Font::UnInit()
{
	if(hf_){
		DeleteObject(hf_);
		hf_ = NULL;
	}

	auto it = umTextureData_.begin();
	auto itEnd = umTextureData_.end();
	for(; it != itEnd; ++it){
		SafeRelease(it->second.pFontTexture);
		SafeRelease(it->second.pTextureView);
	}
	unordered_map<UINT, TextureData>().swap(umTextureData_);
}

void Font::SetColor(Color color)
{
	color_ = color;
}

void Font::SetColor(float r, float g, float b, float a)
{
	color_.r = max(0.0f, min(1.0f, r));
	color_.g = max(0.0f, min(1.0f, g));
	color_.b = max(0.0f, min(1.0f, b));
	color_.a = max(0.0f, min(1.0f, a));
}

void Font::DrawFormatText(float posX, float posY, const char * pFormat, ...)
{
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();

	// 準備ができていなければ終了
	if(hf_ == NULL
		|| !gm.GetContextPtr()
		|| !sm.GetRectVertexBufferPtr()
		|| !sm.GetVertexShederPtr()
		|| !sm.GetPixelShederTexturePtr()){
		return;
	}

	vector<char> strBuffer;

	{
		va_list args;	// 可変個数の引数リスト

		// フォーマット指定された文字列を出力する
		va_start(args, pFormat);
		int len = _vscprintf(pFormat, args) + 1;
		strBuffer.resize(len);
		vsprintf_s(&strBuffer[0], len, pFormat, args);
		va_end(args);
	}

	const char* str = &strBuffer[0];
	vector<char*> vecCharPtr;
	int len = StrSplit(str, &vecCharPtr);

	float ofsX = 0.0f;

	for(int i = 0; i < len; ++i){
		const char* pSrc = vecCharPtr[i];
		wchar_t wc[3] = { 0x00 };
		int ret = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pSrc, 2, wc, 3);
		if(ret == 0){ return; }

		TextureData* pTd = CreateTextureData((UINT)(*wc));

		if(pTd->pTextureView){
			// 頂点情報
			VertexData2D vd[SpriteManager::RECT_VERTEX_NUM];
			vd[0] = { Vector3D(posX + ofsX, posY, 0.0f), color_, Vector2D(0.0f, 0.0f) };
			vd[1] = { Vector3D(posX + ofsX + pTd->fontWidth, posY, 0.0f), color_, Vector2D(1.0f, 0.0f) };
			vd[2] = { Vector3D(posX + ofsX, posY + pTd->fontHeight, 0.0f), color_, Vector2D(0.0f, 1.0f) };
			vd[3] = { Vector3D(posX + ofsX + pTd->fontWidth, posY + pTd->fontHeight, 0.0f), color_, Vector2D(1.0f, 1.0f) };

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

			gm.SetBlendState(BLEND_ALIGNMENT);

			// シェーダのセット
			gm.GetContextPtr()->VSSetShader(sm.GetVertexShederPtr(), NULL, 0);
			gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
			gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
			gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
			gm.GetContextPtr()->PSSetShader(sm.GetPixelShederTexturePtr(), NULL, 0);
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

			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTd->pTextureView);

			//ポリゴン描画
			gm.GetContextPtr()->Draw(SpriteManager::RECT_VERTEX_NUM, 0);
		}

		ofsX += pTd->fontWidth;
	}

	auto itEnd = vecCharPtr.end();
	for(auto it = vecCharPtr.begin(); it != itEnd; ++it){
		SafeDeleteArray((*it));
	}
}

TextureData * Font::CreateTextureData(UINT code)
{
	if(HasKeyTextureData(code)){ return &umTextureData_[code]; }
	if(hf_ == NULL){ return &s_NullTextureData_; }

	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hdc, hf_);

	// 階調の最大値
	const int gradFlag = GGO_GRAY4_BITMAP;
	int grad = 0;
	switch(gradFlag)
	{
	case GGO_GRAY2_BITMAP:
		grad = 4;
		break;
	case GGO_GRAY4_BITMAP:
		grad = 16;
		break;
	case GGO_GRAY8_BITMAP:
		grad = 64;
		break;
	}

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);
	GLYPHMETRICS glm;
	CONST MAT2 mat = { {0,1},{0,0},{0,0},{0,1} };
	DWORD size = GetGlyphOutlineW(hdc, code, gradFlag, &glm, 0, NULL, &mat);
	BYTE *pMono = new BYTE[size];
	GetGlyphOutlineW(hdc, code, gradFlag, &glm, size, pMono, &mat);

	SelectObject(hdc, oldFont);
	ReleaseDC(NULL, hdc);

	//フォントの幅と高さ
	int fontWidth = glm.gmCellIncX;
	int fontHeight = tm.tmHeight;

	ID3D11Texture2D * pFontTexture = nullptr;
	ID3D11ShaderResourceView*  pTextureView = nullptr;	// テクスチャビューのポインタ
														//フォントを書き込むテクスチャ作成
	D3D11_TEXTURE2D_DESC fontTextureDesc;
	ZeroMemory(&fontTextureDesc, sizeof(fontTextureDesc));
	fontTextureDesc.Width = fontWidth;
	fontTextureDesc.Height = fontHeight;
	fontTextureDesc.MipLevels = 1;
	fontTextureDesc.ArraySize = 1;
	fontTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	fontTextureDesc.SampleDesc.Count = 1;
	fontTextureDesc.SampleDesc.Quality = 0;
	fontTextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	fontTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	fontTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fontTextureDesc.MiscFlags = 0;

	HRESULT hr = GraphicManager::Instance().GetDevicePtr()->CreateTexture2D(&fontTextureDesc, NULL, &pFontTexture);
	if(FAILED(hr)){ return &s_NullTextureData_; }

	// フォント情報をテクスチャに書き込む部分
	D3D11_MAPPED_SUBRESOURCE hMappedResource;
	hr = GraphicManager::Instance().GetContextPtr()->Map(
		pFontTexture,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&hMappedResource);
	if(FAILED(hr)){ return &s_NullTextureData_; }

	BYTE* pBits = (BYTE*)hMappedResource.pData;
	int ofsX = glm.gmptGlyphOrigin.x;
	int ofsY = tm.tmAscent - glm.gmptGlyphOrigin.y;
	int bmpW = (glm.gmBlackBoxX + 3) / 4 * 4;
	int bmpH = glm.gmBlackBoxY;
	int x, y;
	DWORD alpha, color;
	memset(pBits, 0, hMappedResource.RowPitch * tm.tmHeight);
	for(y = ofsY; y < ofsY + bmpH; ++y)
	{
		for(x = ofsX; x < ofsX + bmpW && x < fontWidth; ++x)
		{
			alpha = (255 * pMono[x - ofsX + bmpW * (y - ofsY)]) / grad;
			color = 0x00ffffff | (alpha << 24);
			memcpy((BYTE*)pBits + hMappedResource.RowPitch * y + 4 * x, &color, sizeof(DWORD));
		}
	}
	GraphicManager::Instance().GetContextPtr()->Unmap(pFontTexture, 0);

	delete[] pMono;

	//シェーダで使うためのビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Format = fontTextureDesc.Format;
	viewDesc.Texture2D.MipLevels = fontTextureDesc.MipLevels;
	viewDesc.Texture2D.MostDetailedMip = 0;
	hr = GraphicManager::Instance().GetDevicePtr()->CreateShaderResourceView(pFontTexture, &viewDesc, &pTextureView);
	if(FAILED(hr)){ return &s_NullTextureData_; }

	if(code != L' ' && code != L'　'){
	} else{
		SafeRelease(pFontTexture);
		SafeRelease(pTextureView);
	}

	TextureData td;
	ZeroMemory(&td, sizeof(td));
	td.pFontTexture = pFontTexture;
	td.pTextureView = pTextureView;
	td.fontWidth = fontWidth;
	td.fontHeight = fontHeight;

	umTextureData_.insert(pair<UINT, TextureData>{ code, td });

	return &umTextureData_[code];	// テクスチャーインターフェイスのポインターを返す
}

bool Font::HasKeyTextureData(UINT code)
{
	return (umTextureData_.find(code) != umTextureData_.end());	// 検索
}
