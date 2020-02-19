#include "../../h/Archive/IPathManager.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <vector>

LRESULT IPathManager::NormalizePath(std::string& path)
{
	//	�p�X���R�s�[.
	std::string tmp = path;

	//	�S�Ă�'/'���A'\\'�ɒu�������邱�ƂŁA�p�X�L�@�ɓ��ꐫ����������.
	int n = 0;
	while(-1 != (n = (int)tmp.find('/', n)))
	{
		tmp = tmp.replace(n, 1, "\\");
	}

	//	�s�v��"."��".."�Ȃǂ̕������܂ރp�X�����A�����̕������܂܂Ȃ��p�X���ɕϊ�����.
	std::vector<char> p(tmp.size() + 1);
	if (!::PathCanonicalize(&p[0], tmp.c_str()))
	{
		return 1;
	}
	path = std::string(&p[0]);
	return 0;
}