#include "../../h/Shader/Shader.h"
#include "../../../idea/h/Framework/GraphicManager.h"
#include "../../../idea/h/Archive/ArchiveLoader.h"
#include "../../../idea/h/Utility/ideaUtility.h"
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <stdio.h>

int ReadShader(const char* pFileName, BYTE** byteArray)
{
	FILE* fp;
	int ret = fopen_s(&fp, pFileName, "rb");
	if(ret != 0){ return 0; }
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	BYTE* temp = new BYTE[size];
	fseek(fp, 0, SEEK_SET);

	fread(temp, size, 1, fp);
	*byteArray = temp;
	fclose(fp);

	return size;
}

PixelShader::PixelShader():
	pPixelShader_(nullptr)
{
}

PixelShader::~PixelShader()
{
	SafeRelease(pPixelShader_);
}

bool PixelShader::LoadPixelShaderFromCSO(const char * pFileName)
{
	if(pPixelShader_){ return false; }

	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetDevicePtr()){ return false; }

	HRESULT hr;

	BYTE* pData;
	int size = ReadShader(pFileName, &pData);
	if(size == 0){ return false; }

	hr = gm.GetDevicePtr()->CreatePixelShader(pData, size, NULL, &pPixelShader_);
	delete[] pData;
	if(FAILED(hr)){ return false; }

	return true;
}

bool PixelShader::LoadPixelShaderFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
{
	if(pPixelShader_){ return false; }

	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetDevicePtr()){ return false; }

	ArchiveLoader loader;
	if(!loader.Load(pArchiveFileName, pFileName)){ return false; }

	HRESULT hr;

	hr = gm.GetDevicePtr()->CreatePixelShader(&loader.GetData()[0], loader.GetSize(), NULL, &pPixelShader_);
	if(FAILED(hr)){ return false; }

	return true;
}