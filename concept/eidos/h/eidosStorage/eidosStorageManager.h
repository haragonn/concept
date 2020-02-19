#ifndef INCLUDE_EIDOS_EIDOSSTORAGEMANAGER_H
#define INCLUDE_EIDOS_EIDOSSTORAGEMANAGER_H

#include <vector>
#include <unordered_map>

class ObjModel;
class PmdModel;

class eidosStorageManager{
public:
	static eidosStorageManager& Instance()
	{
		static eidosStorageManager s_Instance;
		return s_Instance;
	}
	bool LoadObjMeshFromFile(const char* pFileName);	// 画像の読み込み
	bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// アーカイブファイルから画像を読み込む
	ObjModel& GetObjModel(const char* pFileName);	// 画像の取得
	bool HasKeyObjModel(const char* pFileName);	// 指定したファイル名と関連付けられた画像が存在するか

	bool LoadPmdMeshFromFile(const char* pFileName);	// 画像の読み込み
	//bool LoadPmdMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// アーカイブファイルから画像を読み込む
	PmdModel& GetPmdModel(const char* pFileName);	// 画像の取得
	bool HasKeyPmdModel(const char* pFileName);	// 指定したファイル名と関連付けられた画像が存在するか

	void SetUnLoadObjModelRequest(const char* pFileName);
	inline int GetUnLoadObjModelRequestSize()const{ return vecUnLoadObjModelList_.size(); }
	void UnLoadObjModel();

	void SetUnLoadPmdModelRequest(const char* pFileName);
	inline int GetUnLoadPmdModelRequestSize()const{ return vecUnLoadPmdModelList_.size(); }
	void UnLoadPmdModel();
	
	inline void SetUnLoadAllRequest(bool bRequest){ bUnLoadAll_ = bRequest; }
	inline bool IsUnLoadAllRequest()const{ return bUnLoadAll_; }
	void UnLoadAll();	// 全ての画像とソースボイスの破棄

private:
	std::unordered_map<const char*, ObjModel*> umObjModelPtr_;
	std::vector<const char*> vecUnLoadObjModelList_;
	static ObjModel s_NullObjModel_;

	std::unordered_map<const char*, PmdModel*> umPmdModelPtr_;
	std::vector<const char*> vecUnLoadPmdModelList_;
	static PmdModel s_NullPmdModel_;

	bool bUnLoadAll_;

	eidosStorageManager();					// コンストラクタ
	~eidosStorageManager(){ UnLoadAll(); }	// デストラクタ
	// コピーコンストラクタの禁止
	eidosStorageManager(const eidosStorageManager& src){}
	eidosStorageManager& operator=(const eidosStorageManager& src){}
};

#endif	// #ifndef INCLUDE_IDEA_STORAGEMANAGER_H
