#include "stdafx.h"
#include "RedCoinManager.h"
#if defined(ENABLE_FATIH_SAHIN_REDCOIN_SYSTEM)
#include "ClientManager.h"

CRedCoinManager::CRedCoinManager()
{
	
}

CRedCoinManager::~CRedCoinManager()
{
}

struct FSendChangeRedCoinCoure
{
	FSendChangeRedCoinCoure(double kur)
	{
		p.kur = kur;
	}

	void operator()(CPeer* peer)
	{
		peer->EncodeHeader(HEADER_DG_CHANGE_REDCOIN_COURE, 0, sizeof(TPacketDGChangeRDCCoure));
		peer->Encode(&p, sizeof(TPacketDGChangeRDCCoure));
	}

	TPacketDGChangeRDCCoure p;
};

void CRedCoinManager::AddCoinCoure(double kur)
{
	SendChangeCoinCoure(kur);
}

void CRedCoinManager::SendChangeCoinCoure(double kur)
{
	TPrivCharDatas* p = new TPrivCharDatas(kur);
	
	m_aCoin[0].insert(std::make_pair(kur, p));
	
	CClientManager::instance().for_each_peer(FSendChangeRedCoinCoure(kur));
	sys_err("SendChangeCoinCoure");
}

void CRedCoinManager::SendRDCSetup(CPeer* peer)
{
	for (__typeof(m_aCoin[0].begin()) it = m_aCoin[0].begin(); it != m_aCoin[0].end(); ++it)
	{
		FSendChangeRedCoinCoure(it->first)(peer);
	}
}
#endif