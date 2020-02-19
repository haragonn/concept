#ifndef INCLUDE_IDEA_SHARDER_H
#define INCLUDE_IDEA_SHARDER_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct ID3D11PixelShader;

inline int ReadShader(const char* pFileName, BYTE** byteArray);

class PixelShader{
public:
	PixelShader();
	~PixelShader();
	bool LoadPixelShaderFromCSO(const char* pFileName);
	bool LoadPixelShaderFromArchiveFile(const char* pArchiveFileName, const char* pFileName);
	ID3D11PixelShader* GetPixelShaderPtr()const{ return pPixelShader_; }

private:
	ID3D11PixelShader* pPixelShader_;
};

#endif	// #ifndef INCLUDE_IDEA_SHARDER_H
