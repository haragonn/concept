#ifndef INCLUDE_EIDOS_EIDOSSTORAGEMANAGER_H
#define INCLUDE_EIDOS_EIDOSSTORAGEMANAGER_H

#include <vector>
#include <unordered_map>

class ObjModel;
class PmdModel;
class PmxModel;

class eidosStorageManager{
public:
	static eidosStorageManager& Instance()
	{
		static eidosStorageManager s_Instance;
		return s_Instance;
	}

	bool LoadObjMeshFromFile(const char* pFileName);	// OBJメッシュの読み込み
	bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// アーカイブファイルからOBJメッシュを読み込む
	ObjModel& GetObjModel(const char* pFileName);	// OBJモデルの取得
	bool HasKeyObjModel(const char* pFileName);	// 指定したファイル名と関連付けられたOBJモデルが存在するか

	void SetUnLoadObjModelRequest(const char* pFileName);
	inline int GetUnLoadObjModelRequestSize()const{ return vecUnLoadObjModelList_.size(); }
	void UnLoadObjModel();

	bool LoadPmdMeshFromFile(const char* pFileName);	// PMDメッシュの読み込み
	//bool LoadPmdMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// アーカイブファイルからPMDメッシュを読み込む
	PmdModel& GetPmdModel(const char* pFileName);	// PMDモデルの取得
	bool HasKeyPmdModel(const char* pFileName);	// 指定したファイル名と関連付けられたPMDモデルが存在するか

	void SetUnLoadPmdModelRequest(const char* pFileName);
	inline int GetUnLoadPmdModelRequestSize()const{ return vecUnLoadPmdModelList_.size(); }
	void UnLoadPmdModel();

	bool LoadPmxMeshFromFile(const char* pFileName);	// PMXメッシュの読み込み
	//bool LoadPmxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// アーカイブファイルからPMXメッシュを読み込む
	PmxModel& GetPmxModel(const char* pFileName);	// PMDモデルの取得
	bool HasKeyPmxModel(const char* pFileName);	// 指定したファイル名と関連付けられたPMDモデルが存在するか

	void SetUnLoadPmxModelRequest(const char* pFileName);
	inline int GetUnLoadPmxModelRequestSize()const{ return vecUnLoadPmdModelList_.size(); }
	void UnLoadPmxModel();
	
	inline void SetUnLoadAllRequest(bool bRequest){ bUnLoadAll_ = bRequest; }
	inline bool IsUnLoadAllRequest()const{ return bUnLoadAll_; }
	void UnLoadAll();	// 全てのモデルの破棄

private:
	std::unordered_map<const char*, ObjModel*> umObjModelPtr_;
	std::vector<const char*> vecUnLoadObjModelList_;
	static ObjModel s_NullObjModel_;

	std::unordered_map<const char*, PmdModel*> umPmdModelPtr_;
	std::vector<const char*> vecUnLoadPmdModelList_;
	static PmdModel s_NullPmdModel_;

	std::unordered_map<const char*, PmxModel*> umPmxModelPtr_;
	std::vector<const char*> vecUnLoadPmxModelList_;
	static PmxModel s_NullPmxModel_;

	bool bUnLoadAll_;

	eidosStorageManager();					// コンストラクタ
	~eidosStorageManager(){ UnLoadAll(); }	// デストラクタ

	// コピーコンストラクタの禁止
	eidosStorageManager(const eidosStorageManager& src){}
	eidosStorageManager& operator=(const eidosStorageManager& src){}
};

#endif	// #ifndef INCLUDE_IDEA_STORAGEMANAGER_H
