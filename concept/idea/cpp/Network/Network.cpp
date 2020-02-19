/*==============================================================================
[Network.cpp]
Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Network/Network.h"
#include "../../h/Network/NetworkManager.h"
#include "../../h/Utility/Timer.h"

Timer tmr;

Network::Network():serial_(0U)
{
	NetworkManager::Instance().Flash();
}
bool Network::SetUpServer(unsigned short portNum)
{
	return NetworkManager::Instance().SetUpServer(portNum);
}

bool Network::SetUpClient(unsigned short IPAddrA, unsigned short IPAddrB, unsigned short IPAddrC, unsigned short IPAddrD, unsigned short portNum)
{
	return NetworkManager::Instance().SetUpClient(IPAddrA, IPAddrB, IPAddrC, IPAddrD, portNum);
}

void Network::Close()
{
	NetworkManager::Instance().Close();
}

bool Network::IsSetUp()const
{
	return NetworkManager::Instance().IsSetUp();
}

bool Network::IsServer()const
{
	return NetworkManager::Instance().IsServer();
}

bool Network::IsConnect()const
{
	return NetworkManager::Instance().IsConnect();
}

bool Network::WaitConnect(unsigned int timeOut)
{
	NetworkManager::Instance().SetSkipFlag(false);

	if(!NetworkManager::Instance().IsSetUp() || !NetworkManager::Instance().IsConnect()){ return false; }

	++NetworkManager::Instance().GetSendData()->Serial;

	NetworkManager::Instance().WritingEnd();
	if(timeOut == WAIT_INFINITY){
		while(NetworkManager::Instance().IsSetUp() && NetworkManager::Instance().IsConnect() && (NetworkManager::Instance().IsCommunication() || NetworkManager::Instance().GetSendData()->Serial % 120 == 1 && (NetworkManager::Instance().GetSendData()->Serial > NetworkManager::Instance().GetReceiveData()->Serial || NetworkManager::Instance().GetSendData()->Serial == 1U && 60U < NetworkManager::Instance().GetReceiveData()->Serial))){
			Sleep(1);
		}
	}else{
		tmr.SetStartTime();
		while(NetworkManager::Instance().IsSetUp() && NetworkManager::Instance().IsConnect() && tmr.GetElapsed() < timeOut && (NetworkManager::Instance().IsCommunication() || NetworkManager::Instance().GetSendData()->Serial % 120 == 1 && (NetworkManager::Instance().GetSendData()->Serial > NetworkManager::Instance().GetReceiveData()->Serial || NetworkManager::Instance().GetSendData()->Serial == 1U && 60U < NetworkManager::Instance().GetReceiveData()->Serial))){
			Sleep(1);
		}
	}
	NetworkManager::Instance().WritingBegin();
	
	if(NetworkManager::Instance().GetSendData()->Serial + 1U < NetworkManager::Instance().GetReceiveData()->Serial){
		NetworkManager::Instance().SetSkipFlag(true);
	}

	return NetworkManager::Instance().GetSendData()->Serial % 120 == 1;
}

char Network::GetChar(unsigned int section, unsigned int key)const
{
	if(!NetworkManager::Instance().IsConnect() || section >= 100U || key >= 8U){ return 0; }
	return NetworkManager::Instance().GetReceiveData()->Value[section].cw[key];
}

short Network::GetShort(unsigned int section, unsigned int key)const
{
	if(!NetworkManager::Instance().IsConnect() || section >= 100U || key >= 4U){ return 0; }
	return NetworkManager::Instance().GetReceiveData()->Value[section].sw[key];
}

int Network::GetInt(unsigned int section, unsigned int key)const
{
	if(!NetworkManager::Instance().IsConnect() || section >= 100U || key >= 2U){ return 0; }
	return NetworkManager::Instance().GetReceiveData()->Value[section].iw[key];
}

float Network::GetFloat(unsigned int section, unsigned int key)const
{
	if(!NetworkManager::Instance().IsConnect() || section >= 100U || key >= 2U){ return 0.0f; }
	return NetworkManager::Instance().GetReceiveData()->Value[section].fw[key];
}

double Network::GetDouble(unsigned int section)const
{
	if(!NetworkManager::Instance().IsConnect() || section >= 100U){ return 0.0; }
	return NetworkManager::Instance().GetReceiveData()->Value[section].dw;
}

unsigned long Network::GetSeed()
{
	if(!NetworkManager::Instance().IsConnect()){ return 0U; }
	return NetworkManager::Instance().GetSeed();
}

bool Network::SetChar(unsigned int section, unsigned int key, char Value)
{
	if(section >= 100U || key >= 8U){ return false; }
	NetworkManager::Instance().GetSendData()->Value[section].cw[key] = Value;
	return true;
}

bool Network::SetShort(unsigned int section, unsigned int key, short Value)
{
	if(section >= 100U || key >= 4U){ return false; }
	NetworkManager::Instance().GetSendData()->Value[section].sw[key] = Value;
	return true;
}

bool Network::SetInt(unsigned int section, unsigned int key, int Value)
{
	if(section >= 100U || key >= 2U){ return false; }
	NetworkManager::Instance().GetSendData()->Value[section].iw[key] = Value;
	return true;
}

bool Network::SetFloat(unsigned int section, unsigned int key, float Value)
{
	if(section >= 100U || key >= 2U){ return false; }
	NetworkManager::Instance().GetSendData()->Value[section].fw[key] = Value;
	return true;
}

bool Network::SetDouble(unsigned int section, double Value)
{
	if(section >= 100U){ return false; }
	NetworkManager::Instance().GetSendData()->Value[section].dw = Value;
	return true;
}

void Network::Flash()
{
	NetworkManager::Instance().Flash();
	serial_ = 0U;
	NetworkManager::Instance().GetSendData()->Serial = serial_;
}

unsigned int Network::GetFrequency()
{
	return NetworkManager::Instance().GetFrequency();
}

int Network::GetLag()
{
	return (int)NetworkManager::Instance().GetSendData()->Serial - (int)NetworkManager::Instance().GetReceiveData()->Serial;
}