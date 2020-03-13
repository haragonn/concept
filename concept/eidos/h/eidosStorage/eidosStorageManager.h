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

	bool LoadObjMeshFromFile(const char* pFileName);	// OBJ���b�V���̓ǂݍ���
	bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// �A�[�J�C�u�t�@�C������OBJ���b�V����ǂݍ���
	ObjModel& GetObjModel(const char* pFileName);	// OBJ���f���̎擾
	bool HasKeyObjModel(const char* pFileName);	// �w�肵���t�@�C�����Ɗ֘A�t����ꂽOBJ���f�������݂��邩

	void SetUnLoadObjModelRequest(const char* pFileName);
	inline int GetUnLoadObjModelRequestSize()const{ return vecUnLoadObjModelList_.size(); }
	void UnLoadObjModel();

	bool LoadPmdMeshFromFile(const char* pFileName);	// PMD���b�V���̓ǂݍ���
	//bool LoadPmdMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// �A�[�J�C�u�t�@�C������PMD���b�V����ǂݍ���
	PmdModel& GetPmdModel(const char* pFileName);	// PMD���f���̎擾
	bool HasKeyPmdModel(const char* pFileName);	// �w�肵���t�@�C�����Ɗ֘A�t����ꂽPMD���f�������݂��邩

	void SetUnLoadPmdModelRequest(const char* pFileName);
	inline int GetUnLoadPmdModelRequestSize()const{ return vecUnLoadPmdModelList_.size(); }
	void UnLoadPmdModel();

	bool LoadPmxMeshFromFile(const char* pFileName);	// PMX���b�V���̓ǂݍ���
	//bool LoadPmxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// �A�[�J�C�u�t�@�C������PMX���b�V����ǂݍ���
	PmxModel& GetPmxModel(const char* pFileName);	// PMD���f���̎擾
	bool HasKeyPmxModel(const char* pFileName);	// �w�肵���t�@�C�����Ɗ֘A�t����ꂽPMD���f�������݂��邩

	void SetUnLoadPmxModelRequest(const char* pFileName);
	inline int GetUnLoadPmxModelRequestSize()const{ return vecUnLoadPmdModelList_.size(); }
	void UnLoadPmxModel();
	
	inline void SetUnLoadAllRequest(bool bRequest){ bUnLoadAll_ = bRequest; }
	inline bool IsUnLoadAllRequest()const{ return bUnLoadAll_; }
	void UnLoadAll();	// �S�Ẵ��f���̔j��

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

	eidosStorageManager();					// �R���X�g���N�^
	~eidosStorageManager(){ UnLoadAll(); }	// �f�X�g���N�^

	// �R�s�[�R���X�g���N�^�̋֎~
	eidosStorageManager(const eidosStorageManager& src){}
	eidosStorageManager& operator=(const eidosStorageManager& src){}
};

#endif	// #ifndef INCLUDE_IDEA_STORAGEMANAGER_H
