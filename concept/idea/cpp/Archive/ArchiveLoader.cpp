#include "../../h/Archive/ArchiveLoader.h"
#include "../../h/Archive/VirtualFileLoader.h"
#include "../../h/Archive/FileLoader.h"
#include "../../h/Archive/ArchiveData.h"
#include "../../h/Utility//ideaUtility.h"

bool ArchiveLoader::Load(const char* archiveFileName, const char* fileName)
{
	if(data_.size() != 0){ return false; }

	VirtualFileLoader loader;
	if(loader.PushLoader(SFileLoader(ArchiveData::CreateInstance(archiveFileName)), "") != 0){
		SetDebugMessage("\tArchiveLoadError! [%s] ÇÉtÉ@ÉCÉãÇ©ÇÁì«Ç›çûÇﬂÇ‹ÇπÇÒÇ≈ÇµÇΩ\n", archiveFileName);
		return false;
	}

	loader.Load(data_, fileName);

	if(data_.size() == 0){ return false; }

	return true;
}
