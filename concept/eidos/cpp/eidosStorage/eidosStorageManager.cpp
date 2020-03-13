#include "../../h/eidosStorage/eidosStorageManager.h"
#include "../../h/Mesh/ObjModel.h"
#include "../../h/Mesh/PmdModel.h"
#include "../../h/Mesh/PmxModel.h"
#include "../../../idea/h/Utility/ideaUtility.h"

using namespace std;

// 静的メンバの実体
ObjModel eidosStorageManager::s_NullObjModel_;
PmdModel eidosStorageManager::s_NullPmdModel_;
PmxModel eidosStorageManager::s_NullPmxModel_;

eidosStorageManager::eidosStorageManager()
	: bUnLoadAll_(false)
{
	unordered_map<const char*, ObjModel*>().swap(umObjModelPtr_);
	vector<const char*>().swap(vecUnLoadObjModelList_);

	unordered_map<const char*, PmdModel*>().swap(umPmdModelPtr_);
	vector<const char*>().swap(vecUnLoadPmdModelList_);

	unordered_map<const char*, PmxModel*>().swap(umPmxModelPtr_);
	vector<const char*>().swap(vecUnLoadPmxModelList_);
}

bool eidosStorageManager::LoadObjMeshFromFile(const char * pFileName)
{
	if(HasKeyObjModel(pFileName)){ return false; }	// 既にキーが登録されていれば終了

	ObjModel* pMesh = new ObjModel;
	bool bRes = pMesh->LoadObjMeshFromFile(pFileName);

	if(bRes){
		umObjModelPtr_.insert(pair<const char*, ObjModel*>{ pFileName, pMesh });
	} else{
		SafeDelete(pMesh);
	}

	return bRes;
}

bool eidosStorageManager::LoadObjMeshFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
{
	if(HasKeyObjModel(pFileName)){ return false; }	// 既にキーが登録されていれば終了

	ObjModel* pMesh = new ObjModel;
	bool bRes = pMesh->LoadObjMeshFromArchiveFile(pArchiveFileName, pFileName);

	if(bRes){
		umObjModelPtr_.insert(pair<const char*, ObjModel*>{ pFileName, pMesh });
	} else{
		SafeDelete(pMesh);
	}

	return bRes;
}

ObjModel & eidosStorageManager::GetObjModel(const char * pFileName)
{
	if(!HasKeyObjModel(pFileName)
		|| !umObjModelPtr_.size()){
		SetDebugMessage("ObjMeshLoadError! [%s] はストレージに存在しません。ファイルから読み込みます。\n", pFileName);
		if(!LoadObjMeshFromFile(pFileName)){
			return s_NullObjModel_;
		}
	}

	return *umObjModelPtr_[pFileName];	// テクスチャーインターフェイスのポインターを返す
}

bool eidosStorageManager::HasKeyObjModel(const char * pFileName)
{
	if(!umObjModelPtr_.size()){ return false; }
	return (umObjModelPtr_.find(pFileName) != umObjModelPtr_.end());	// 検索
}

void eidosStorageManager::SetUnLoadObjModelRequest(const char * pFileName)
{
	vector<const char*>::iterator it = vecUnLoadObjModelList_.begin();
	vector<const char*>::iterator itEnd = vecUnLoadObjModelList_.end();

	for(; it != itEnd; ++it){
		if(*it == pFileName){
			return;
		}
	}

	vecUnLoadObjModelList_.push_back(pFileName);
}

void eidosStorageManager::UnLoadObjModel()
{
	if(umObjModelPtr_.size()){
		// イテレータを使って解放
		vector<const char*>::iterator it = vecUnLoadObjModelList_.begin();
		vector<const char*>::iterator itEnd = vecUnLoadObjModelList_.end();
		unordered_map<const char*, ObjModel*>::iterator itPmdEnd = umObjModelPtr_.end();

		for(; it != itEnd; ++it){
			if(umObjModelPtr_.find(*it) != itPmdEnd){
				unordered_map<const char*, ObjModel*>::iterator itPmd = umObjModelPtr_.find(*it);
				if(itPmd->second){
					itPmd->second->UnLoad();
					delete itPmd->second;
					itPmd->second = nullptr;
				}
				umObjModelPtr_.erase(itPmd);
			}
		}
	}

	vector<const char*>().swap(vecUnLoadObjModelList_);
}

bool eidosStorageManager::LoadPmdMeshFromFile(const char * pFileName)
{
	if(HasKeyPmdModel(pFileName)){ return false; }	// 既にキーが登録されていれば終了

	PmdModel* pMesh = new PmdModel;
	bool bRes = pMesh->LoadPmdMeshFromFile(pFileName);

	if(bRes){
		umPmdModelPtr_.insert(pair<const char*, PmdModel*>{ pFileName, pMesh });
	} else{
		SafeDelete(pMesh);
	}

	return bRes;
}

//bool eidosStorageManager::LoadPmdMeshFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
//{
//	if(HasKeyPmdMesh(pFileName)){ return false; }	// 既にキーが登録されていれば終了
//
//	PmdModel* pMesh = new PmdModel;
//	bool bRes = pMesh->LoadPmdMeshFromArchiveFile(pArchiveFileName, pFileName);
//
//	if(bRes){
//		umPmdMeshPtr_.insert(pair<const char*, PmdModel*>{ pFileName, pMesh });
//	} else{
//		SafeDelete(pMesh);
//	}
//
//	return bRes;
//}

PmdModel & eidosStorageManager::GetPmdModel(const char * pFileName)
{
	if(!HasKeyPmdModel(pFileName)
		|| !umPmdModelPtr_.size()){
		SetDebugMessage("PmdMeshLoadError! [%s] はストレージに存在しません。ファイルから読み込みます。\n", pFileName);
		if(!LoadPmdMeshFromFile(pFileName)){
			return s_NullPmdModel_;
		}
	}

	return *umPmdModelPtr_[pFileName];	// テクスチャーインターフェイスのポインターを返す
}

bool eidosStorageManager::HasKeyPmdModel(const char * pFileName)
{
	if(!umPmdModelPtr_.size()){ return false; }
	return (umPmdModelPtr_.find(pFileName) != umPmdModelPtr_.end());	// 検索
}

void eidosStorageManager::SetUnLoadPmdModelRequest(const char * pFileName)
{
	vector<const char*>::iterator it = vecUnLoadPmdModelList_.begin();
	vector<const char*>::iterator itEnd = vecUnLoadPmdModelList_.end();

	for(; it != itEnd; ++it){
		if(*it == pFileName){
			return;
		}
	}

	vecUnLoadPmdModelList_.push_back(pFileName);
}

void eidosStorageManager::UnLoadPmdModel()
{
	if(umPmdModelPtr_.size()){
		// イテレータを使って解放
		vector<const char*>::iterator it = vecUnLoadPmdModelList_.begin();
		vector<const char*>::iterator itEnd = vecUnLoadPmdModelList_.end();
		unordered_map<const char*, PmdModel*>::iterator itPmdEnd = umPmdModelPtr_.end();

		for(; it != itEnd; ++it){
			if(umPmdModelPtr_.find(*it) != itPmdEnd){
				unordered_map<const char*, PmdModel*>::iterator itPmd = umPmdModelPtr_.find(*it);
				if(itPmd->second){
					itPmd->second->UnLoad();
					delete itPmd->second;
					itPmd->second = nullptr;
				}
				umPmdModelPtr_.erase(itPmd);
			}
		}
	}

	vector<const char*>().swap(vecUnLoadPmdModelList_);
}

//.pmx
bool eidosStorageManager::LoadPmxMeshFromFile(const char* pFileName)
{
	if(HasKeyPmxModel(pFileName)){ return false; }	// 既にキーが登録されていれば終了

	PmxModel* pMesh = new PmxModel;
	bool bRes = pMesh->LoadPmxMeshFromFile(pFileName);

	if(bRes){
		umPmxModelPtr_.insert(pair<const char*, PmxModel*>{ pFileName, pMesh });
	} else{
		SafeDelete(pMesh);
	}

	return bRes;
}

//bool eidosStorageManager::LoadPmxMeshFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
//{
//	if(HasKeyPmxMesh(pFileName)){ return false; }	// 既にキーが登録されていれば終了
//
//	PmdModel* pMesh = new PmxModel;
//	bool bRes = pMesh->LoadPmxMeshFromArchiveFile(pArchiveFileName, pFileName);
//
//	if(bRes){
//		umPmxMeshPtr_.insert(pair<const char*, PmxModel*>{ pFileName, pMesh });
//	} else{
//		SafeDelete(pMesh);
//	}
//
//	return bRes;
//}

PmxModel& eidosStorageManager::GetPmxModel(const char* pFileName)
{
	if(!HasKeyPmxModel(pFileName)
		|| !umPmxModelPtr_.size()){
		SetDebugMessage("PmxMeshLoadError! [%s] はストレージに存在しません。ファイルから読み込みます。\n", pFileName);

		if(!LoadPmxMeshFromFile(pFileName)){ return s_NullPmxModel_; }
	}

	return *umPmxModelPtr_[pFileName];	// テクスチャーインターフェイスのポインターを返す
}

bool eidosStorageManager::HasKeyPmxModel(const char* pFileName)
{
	if(!umPmxModelPtr_.size()){ return false; }

	return (umPmxModelPtr_.find(pFileName) != umPmxModelPtr_.end());	// 検索
}

void eidosStorageManager::SetUnLoadPmxModelRequest(const char* pFileName)
{
	auto it = vecUnLoadPmxModelList_.begin();
	auto itEnd = vecUnLoadPmxModelList_.end();

	for(; it != itEnd; ++it){
		if(*it == pFileName){
			return;
		}
	}

	vecUnLoadPmxModelList_.push_back(pFileName);
}

void eidosStorageManager::UnLoadPmxModel()
{
	if(umPmxModelPtr_.size()){
		// イテレータを使って解放
		auto it = vecUnLoadPmxModelList_.begin();
		auto itEnd = vecUnLoadPmxModelList_.end();
		auto itPmdEnd = umPmxModelPtr_.end();

		for(; it != itEnd; ++it){
			if(umPmxModelPtr_.find(*it) != itPmdEnd){
				auto itPmd = umPmxModelPtr_.find(*it);
				if(itPmd->second){
					itPmd->second->UnLoad();
					delete itPmd->second;
					itPmd->second = nullptr;
				}
				umPmxModelPtr_.erase(itPmd);
			}
		}
	}

	vector<const char*>().swap(vecUnLoadPmxModelList_);
}

void eidosStorageManager::UnLoadAll()
{
	if(umObjModelPtr_.size()){
		// イテレータを使って解放
		unordered_map<const char*, ObjModel*>::iterator itObj = umObjModelPtr_.begin();
		unordered_map<const char*, ObjModel*>::iterator itObjEnd = umObjModelPtr_.end();

		for(; itObj != itObjEnd; ++itObj){
			if(itObj->second){
				itObj->second->UnLoad();
				delete itObj->second;
				itObj->second = nullptr;
			}
		}
	}

	unordered_map<const char*, ObjModel*>().swap(umObjModelPtr_);

	vector<const char*>().swap(vecUnLoadObjModelList_);

	if(umPmdModelPtr_.size()){
		// イテレータを使って解放
		unordered_map<const char*, PmdModel*>::iterator itPmd = umPmdModelPtr_.begin();
		unordered_map<const char*, PmdModel*>::iterator itPmdEnd = umPmdModelPtr_.end();

		for(; itPmd != itPmdEnd; ++itPmd){
			if(itPmd->second){
				itPmd->second->UnLoad();
				delete itPmd->second;
				itPmd->second = nullptr;
			}
		}
	}
	unordered_map<const char*, PmdModel*>().swap(umPmdModelPtr_);

	vector<const char*>().swap(vecUnLoadPmdModelList_);

	if(umPmxModelPtr_.size()){
		// イテレータを使って解放
		auto itPmx  = umPmxModelPtr_.begin();
		auto itPmxEnd = umPmxModelPtr_.end();

		for(; itPmx != itPmxEnd; ++itPmx){
			if(itPmx->second){
				itPmx->second->UnLoad();
				delete itPmx->second;
				itPmx->second = nullptr;
			}
		}
	}
	unordered_map<const char*, PmxModel*>().swap(umPmxModelPtr_);

	vector<const char*>().swap(vecUnLoadPmxModelList_);

	bUnLoadAll_ = false;
}
