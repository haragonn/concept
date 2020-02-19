/*==============================================================================
	[CsvReader.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_CSVREADER_H
#define INCLUDE_IDEA_CSVREADER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <map>

//------------------------------------------------------------------------------
// �N���X���@�FCsvReader
// �N���X�T�v�Fcsv�t�@�C���̓ǂݍ��݂��s���N���X
//------------------------------------------------------------------------------
class CsvReader{
public:
	// Variant�\����
	// �����̌^�Ƃ��Ĉ�����\����
	struct Variant{
		int asInt;				// int�Ƃ��Ă̒l
		float asFloat;			// float�Ƃ��Ă̒l
		std::string asString;	// std::string�Ƃ��Ă̒l
		Variant():asInt(0),asFloat(0){}	// �R���X�g���N�^
		Variant(const std::string& str):asInt(atoi(str.c_str())), asFloat((float)atof(str.c_str())), asString(str){}	// �R���X�g���N�^
		operator int(){ return asInt; }				// int�Ƃ��Ď󂯎�����ꍇ
		operator float(){ return asFloat; }			// float�Ƃ��Ď󂯎�����ꍇ
		operator std::string(){ return asString; }	// string�Ƃ��Ď󂯎�����ꍇ
	};
	// Record�N���X
	// csv�̃f�[�^��񕪂��i�[����N���X
	class Record{
	public:
		~Record();	// �f�X�g���N�^
		Variant& operator[](const char* pKey);	// operator[]
		Variant& operator[](int col);			///
		void Add(const char* pKey, Variant& val);	// �l�̒ǉ�
	private:
		std::map<std::string, Variant*> value_;	// �l
		std::vector<std::string> key_;	// �L�[
	};

	CsvReader():maxRow_(0), maxCol_(0){}	// �R���X�g���N�^
	CsvReader(const char* pFileName);		///
	~CsvReader();	// �f�X�g���N�^
	bool OpenFile(const char* pFileName);	// csv�t�@�C���̓ǂݍ���
	int GetMaxrow()const{ return maxRow_; }	// csv�t�@�C���̍s���̎擾
	int GetMaxcol()const{ return maxCol_; }	// csv�t�@�C���̗񐔂̎擾
	Variant& GetField(const char* pID, const char* pKey);	// csv�t�@�C���̃t�B�[���h���擾
	Variant& GetField(int row, const char* pKey);			///
	Variant& GetField(int row, int col);					///
	Record& operator[](const char* pID);					///
	Record& operator[](int row);							///

private:
	int maxRow_;	// �ő�s��
	int maxCol_;	// �ő��
	std::vector<std::string> id_;	// ID���X�g
	std::map<std::string, Record*> record_;	// Record
	static Variant NullValue;	// ��̒l
	static Record NullRecord;	// ���Record
};

#endif	// #ifndef INCLUDE_IDEA_CSVREADER_H