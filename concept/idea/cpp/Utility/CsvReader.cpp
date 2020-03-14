/*==============================================================================
	[CsvReader.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Utility/CsvReader.h"
#include "../../h/Utility/ideaUtility.h"
#include <sstream>
#include <fstream>

using namespace std;

namespace{
	const int MAX_READ_LINE = 2048;	// ��s�̃f�[�^�Ƃ��ēǂݍ��߂�ő啶����
}

// ���݂��Ȃ����̂��w�肳�ꂽ���ɕԂ�����
CsvReader::Variant CsvReader::s_NullValue;
CsvReader::Record CsvReader::s_NullRecord;

// int����string�֕ϊ�
string IntToString(int val)
{
	stringstream ss;
	ss << val;
	return ss.str();
}

// Context�N���X
// csv�̓ǂݍ��ݕ������߂�N���X
template<int N>class Context{
public:
	char str_[N];

private:
	int size_;

	int pos_;

	bool eol_;

public:
	Context() : size_(N), pos_(0),eol_(false){}

	// ���ڂ���ǂݐi�߂�
	string GetItem(){
		char c[N] = {};
		int j = 0;
		for(int i = pos_; i < size_; ++i, ++j, ++pos_){
			switch(str_[i]){
			// ���ڂ̋�؂�
			case ',':
				++pos_;
				return c;
				break;

			// �s�̏I���
			case '\0':
				++pos_;
				eol_ = true;
				return c;
				break;

			// �������镶��
			case ' ':
			case '\t':
				--j;
				break;

			default:
				c[j] = str_[i];
			}
		}

		eol_ = true;

		return c;
	}

	// �ǂވʒu�̃��Z�b�g
	void Reset(){
		pos_ = 0;

		eol_ = false;
	}

	// ��s�ǂݏI��������ǂ���
	bool EOL(){
		if(eol_){
			eol_ = false;

			return true;
		}

		return false;
	}
};

// Record�N���X
// csv�̃f�[�^��񕪂��i�[����N���X
CsvReader::Record::~Record()
{
	// map�̊J��
	map<string,Variant*>::iterator it, end;

	for(it = mapValue_.begin(), end = mapValue_.end(); it != end; ++it){ delete it->second; }
}

CsvReader::Variant& CsvReader::Record::operator[](const char* pKey)
{
	// �w�肳�ꂽKey�����݂���΂��̓��e��Ԃ�
	if(mapValue_.find(pKey) == mapValue_.end()){ return s_NullValue; }

	return *mapValue_[pKey];
}

CsvReader::Variant& CsvReader::Record::operator[](int col)
{
	// �ʂ��ԍ�����ID������o���Ă��̓��e��Ԃ�
	if((int)vecKey_.size() > col && col > -1){
		return *mapValue_[vecKey_[col]];
	}else{ return s_NullValue; }
}

void CsvReader::Record::Add(const char* pKey, Variant& val){
	// �w�肳�ꂽKey�����݂��Ȃ���ΐV�����쐬����
	if(mapValue_.find(pKey) == mapValue_.end()){
		mapValue_[pKey] = new Variant(val);
		vecKey_.push_back(pKey);
	}else{ (*mapValue_[pKey]) = val; }
}

// CsvReader�N���X
// �����s��Record����Ȃ�csv�̃f�[�^���i�[����N���X
CsvReader::CsvReader(const char* pFileName)
{
	OpenFile(pFileName);
}

CsvReader::~CsvReader()
{
	// map,vector�̉��
	map<string,Record*>::iterator it, end;

	for(it = mapRecord_.begin(), end = mapRecord_.end(); it != end; ++it){
		SafeDelete(it->second);
	}

	vector<string>().swap(vecId_);
}

bool CsvReader::OpenFile(const char* pFileName)
{
	ifstream ifs(pFileName);

	if(ifs.is_open() == true){
		// �f�[�^Key���̎擾
		vector<string> key;
		Context<MAX_READ_LINE> ct;
		string tStr;
		string tID;
		bool bID = false;

		do{
			ifs.getline(ct.str_, MAX_READ_LINE - 1);
			tStr = ct.str_;
		}while(tStr.empty() && !ifs.eof());
		
		colSize_ = 0;

		while(!ct.EOL()){
			key.push_back(ct.GetItem());
			++colSize_;
		}

		ct.Reset();
		
		// ID�̎g�p�m�F
		if(key[0] == "id"
		|| key[0] == "Id"
		|| key[0] == "ID"){ bID = true; }

		// �f�[�^�擾����ъi�[
		rowSize_ = 0;

		while(!ifs.eof()){
			ifs.getline(ct.str_,MAX_READ_LINE - 1);
			tStr = ct.str_;

			if(!tStr.empty()){
				Record* tRecord = new Record();

				// ID�̎g�p�m�F
				if(bID){
					tID = ct.GetItem();
					vecId_.push_back(tID);
				}else{
					tID = IntToString(rowSize_);
					vecId_.push_back(tID);
				}
				for(int i = 0;i < colSize_ && !ct.EOL();++i){
					if(i == 0 && bID){ continue; }

					Variant val = Variant(ct.GetItem());
					tRecord->Add(key[i].c_str(), val);
				}

				mapRecord_[tID] = tRecord;

				++rowSize_;
			}
			ct.Reset();
		}
		return true;
	}

	return false;
}

CsvReader::Variant& CsvReader::GetField(const char* pID, const char* pKey)
{
	// �w�肳�ꂽID�����݂���΂��̓��e��Ԃ�,Key�̔����operator[](pKey)�ɔC����
	if(mapRecord_.find(pID) == mapRecord_.end()){ return s_NullValue; }

	return mapRecord_[pID]->operator[](pKey);
}

CsvReader::Variant& CsvReader::GetField(int row, const char* pKey)
{
	// �ʂ��ԍ�����ID������o��
	if((int)vecId_.size() > row && row >= 0){
		return mapRecord_[vecId_[row]]->operator[](pKey);	// �w�肳�ꂽID�̓��e��Ԃ�,pKey�̔����operator[](pKey)�ɔC����
	}else{ return s_NullValue; }
}

CsvReader::Variant& CsvReader::GetField(int row, int col)
{
	// �ʂ��ԍ�����ID������o��
	if((int)vecId_.size() > row && row >= 0){
		return mapRecord_[vecId_[row]]->operator[](col);	// �w�肳�ꂽID,Key�̓��e��Ԃ�
	}else{ return s_NullValue; }
}

CsvReader::Record& CsvReader::operator[](const char* pID)
{
	// �w�肳�ꂽID�����݂���΂��̓��e��Ԃ�
	if(mapRecord_.find(pID) == mapRecord_.end()){ return s_NullRecord; }

	return *mapRecord_[pID];
}

CsvReader::Record& CsvReader::operator[](int row)
{
	// �ʂ��ԍ�����ID������o���Ă��̓��e��Ԃ�
	if((int)vecId_.size() > row && row > -1){
		return *mapRecord_[vecId_[row]];
	}else{ return s_NullRecord; }
}