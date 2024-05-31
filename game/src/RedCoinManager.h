#ifndef __INC_REDCOIN_MANAGER_H
#define __INC_REDCOIN_MANAGER_H

#include <vector>

#include "../../common/service.h"
#ifdef ENABLE_FATIH_SAHIN_REDCOIN_SYSTEM
class CRedCoinManager : public singleton<CRedCoinManager>
{
	public: 
	CRedCoinManager();
	virtual ~CRedCoinManager();

	void SetRedCoinCoure(double kur);

	void GetRedCoinCoureSelect(LPCHARACTER ch);
	
	protected:
		std::vector<double>		m_coinVector;
};
#endif
#endif