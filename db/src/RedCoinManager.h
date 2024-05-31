#ifndef __INC_REDCOIN_MANAGER_H
#define __INC_REDCOIN_MANAGER_H

#include "Peer.h"
#include "../../common/service.h"

struct TPrivCharDatas
{
    double kur;
    TPrivCharDatas(double kur)
	: kur(kur)
    {}
};

#if defined(ENABLE_FATIH_SAHIN_REDCOIN_SYSTEM)
class CRedCoinManager : public singleton<CRedCoinManager>
{
	public: 
	CRedCoinManager();
	virtual ~CRedCoinManager();

	void AddCoinCoure(double kur);
	
	void SendChangeCoinCoure(double kur);

	void SendRDCSetup(CPeer* peer);
	
	private:
		std::vector<double>		m_coinVector;
		std::map<double, TPrivCharDatas*> m_aCoin[1];
};
#endif
#endif