#include "stdafx.h"
#include "constants.h"
#include "char.h"
#include "desc_client.h"
#include "guild.h"
#include "guild_manager.h"
#include "unique_item.h"
#include "utils.h"
#include "log.h"
#include "db.h"
#include "RedCoinManager.h"
#ifdef ENABLE_FATIH_SAHIN_REDCOIN_SYSTEM

CRedCoinManager::CRedCoinManager()
{
	
}

CRedCoinManager::~CRedCoinManager()
{
}

void CRedCoinManager::SetRedCoinCoure(double kur)
{
	m_coinVector.clear();
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("UPDATE web_core.svSettings SET rdc_Kur = '%f';", kur));
	m_coinVector.push_back(kur);
}

void CRedCoinManager::GetRedCoinCoureSelect(LPCHARACTER ch)
{
	if (NULL == ch)
		return;

	if (!ch || !ch->GetDesc() || !ch->IsPC())
		return;

	for (auto iter=m_coinVector.begin(); iter!=m_coinVector.end(); ++iter)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefreshRedCoinCour %f", *iter);
	}
}
#endif