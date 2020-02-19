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
	bool LoadObjMeshFromFile(const char* pFileName);	// �摜�̓ǂݍ���
	bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// �A�[�J�C�u�t�@�C������摜��ǂݍ���
	ObjModel& GetObjModel(const char* pFileName);	// �摜�̎擾
	bool HasKeyObjModel(const char* pFileName);	// �w�肵���t�@�C�����Ɗ֘A�t����ꂽ�摜�����݂��邩

	bool LoadPmdMeshFromFile(const char* pFileName);	// �摜�̓ǂݍ���
	//bool LoadPmdMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// �A�[�J�C�u�t�@�C������摜��ǂݍ���
	PmdModel& GetPmdModel(const char* pFileName);	// �摜�̎擾
	bool HasKeyPmdModel(const char* pFileName);	// �w�肵���t�@�C�����Ɗ֘A�t����ꂽ�摜�����݂��邩

	void SetUnLoadObjModelRequest(const char* pFileName);
	inline int GetUnLoadObjModelRequestSize()const{ return vecUnLoadObjModelList_.size(); }
	void UnLoadObjModel();

	void SetUnLoadPmdModelRequest(const char* pFileName);
	inline int GetUnLoadPmdModelRequestSize()const{ return vecUnLoadPmdModelList_.size(); }
	void UnLoadPmdModel();
	
	inline void SetUnLoadAllRequest(bool bRequest){ bUnLoadAll_ = bRequest; }
	inline bool IsUnLoadAllRequest()const{ return bUnLoadAll_; }
	void UnLoadAll();	// �S�Ẳ摜�ƃ\�[�X�{�C�X�̔j��

private:
	std::unordered_map<const char*, ObjModel*> umObjModelPtr_;
	std::vector<const char*> vecUnLoadObjModelList_;
	static ObjModel s_NullObjModel_;

	std::unordered_map<const char*, PmdModel*> umPmdModelPtr_;
	std::vector<const char*> vecUnLoadPmdModelList_;
	static PmdModel s_NullPmdModel_;

	bool bUnLoadAll_;

	eidosStorageManager();					// �R���X�g���N�^
	~eidosStorageManager(){ UnLoadAll(); }	// �f�X�g���N�^
	// �R�s�[�R���X�g���N�^�̋֎~
	eidosStorageManager(const eidosStorageManager& src){}
	eidosStorageManager& operator=(const eidosStorageManager& src){}
};

#endif	// #ifndef INCLUDE_IDEA_STORAGEMANAGER_H
