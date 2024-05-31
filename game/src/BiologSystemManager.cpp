#include "stdafx.h"
#ifdef __ENABLE_BIOLOG_SYSTEM__
#include "BiologSystemManager.hpp"
#include "questmanager.h"
#include "char.h"
#include "packet.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "char_manager.h"
#include "p2p.h"
#include "item_manager.h"
#include "item.h"
#include "utils.h"

#include "mob_manager.h"

#include "EventFunctionHandler.hpp"

#define __DEBUG_PRINT
#ifdef __DEBUG_PRINT
#include <iostream>

using std::cerr;
using std::endl;
#endif

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
|||| CBiologSystem - CLASS
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/

/*******************************************************************\
| [PUBLIC] (De-)Initialize Functions
\*******************************************************************/

CBiologSystem::CBiologSystem(LPCHARACTER m_ch)
	: m_pkChar(m_ch)
{
	s_current_biolog_reminder = "BIOLOG_MANAGER_ALERT";
}

CBiologSystem::~CBiologSystem()
{
}

/*******************************************************************\
| [PUBLIC] Update Functions
\*******************************************************************/

void CBiologSystem::ResetMission()
{
	m_pkChar->SetBiologCollectedItems(0);
	m_pkChar->SetBiologCooldown(0);
	ActiveAlert(false);
}

/*******************************************************************\
| [PUBLIC] General Functions
\*******************************************************************/

void CBiologSystem::SendBiologInformation(bool bUpdate)
{
	if (!m_pkChar)
		return;

	auto bMission = m_pkChar->GetBiologMissions();
	auto iCount = m_pkChar->GetBiologCollectedItems();
	auto tWait = m_pkChar->GetBiologCooldown();
	auto tReminder = m_pkChar->GetBiologCooldownReminder();

	TBiologMissionsProto* pMission = CBiologSystemManager::instance().GetMission(bMission);
	if (!pMission)
	{
#ifdef __DEBUG_PRINT
		sys_err("SendBiologInformation -> cannot get mission %u by %s", bMission, m_pkChar->GetName());
#endif

		m_pkChar->ChatPacket(CHAT_TYPE_INFO, "You done every mission!");
		return;
	}

	TBiologRewardsProto* pReward = CBiologSystemManager::instance().GetReward(bMission);
	if (!pReward)
	{
#ifdef __DEBUG_PRINT
		sys_err("SendBiologInformation -> cannot get reward %u by %s", bMission, m_pkChar->GetName());
#endif
		return;
	}

	TPacketGCBiologManagerInfo kInfo;
	memset(&kInfo, 0, sizeof(kInfo));

	//copy data
	kInfo.bUpdate = bUpdate;
	kInfo.bRequiredLevel = pMission->bRequiredLevel;
	kInfo.iRequiredItem = pMission->iRequiredItem;
	kInfo.wGivenItems = iCount;
	kInfo.wRequiredItemCount = pMission->wRequiredItemCount;
	kInfo.iGlobalCooldown = pMission->iCooldown;
	kInfo.iCooldown = (tWait - get_global_time());
	kInfo.iCooldownReminder = tReminder;
	kInfo.bChance = pMission->bChance;
	kInfo.bApplyType = pReward->bApplyType;
	kInfo.lApplyValue = pReward->lApplyValue;

	SendClientPacket(m_pkChar->GetDesc(), GC_BIOLOG_OPEN, &kInfo, sizeof(kInfo));
}

void CBiologSystem::SendBiologItem(bool bAdditionalChance, bool bAdditionalTime)
{
	if (!m_pkChar)
		return;

	auto bMission = m_pkChar->GetBiologMissions();
	auto tWait = m_pkChar->GetBiologCooldown();
	auto iCount = m_pkChar->GetBiologCollectedItems();

	// Getting actual biolog mission
	TBiologMissionsProto* pMission = CBiologSystemManager::instance().GetMission(bMission);
	if (!pMission || bMission == CBiologSystemManager::instance().m_mapMission_Proto.size())
	{
		//We have to inform players they've done every mission!
#ifdef __DEBUG_PRINT
		cerr << "SendBiologItem -> cannot get mission of level " << bMission << " by " << m_pkChar->GetName() << endl;
#endif
		return;
	}
	
	// Checking required level
	if (m_pkChar->GetLevel() < pMission->bRequiredLevel)
	{
		m_pkChar->ChatPacket(CHAT_TYPE_INFO, "You doesn't have enought level");
		return;
	}

	// Counting required item
	if (m_pkChar->CountSpecifyItem(pMission->iRequiredItem) < 1)
	{
		m_pkChar->ChatPacket(CHAT_TYPE_INFO, "You doesn't have required item!");
		return;
	}

	// Checking time
	if (get_global_time() < tWait && bAdditionalTime)
	{
		if (m_pkChar->CountSpecifyItem(BIOLOG_TIME_ITEM) < 1)
		{
			m_pkChar->ChatPacket(CHAT_TYPE_INFO, "You doesn't have required item for remove cooldown!");
			return;
		}
		else
			m_pkChar->RemoveSpecifyItem(BIOLOG_TIME_ITEM, 1);
#ifdef __DEBUG_PRINT
		cerr << "SendBiologItem: Doesn't check time and remove item!" << endl;
#endif
	}

	if (get_global_time() < tWait && !bAdditionalTime)
	{
		m_pkChar->ChatPacket(CHAT_TYPE_INFO, "You have to wait, you can try next time at: %s", GetFullDateFromTime(tWait).c_str());
		return;
	}

	// Define variables
	BYTE chance = pMission->bChance;

	if (bAdditionalChance)
	{
		LPITEM biologChanceItem = m_pkChar->FindSpecifyItem(BIOLOG_CHANCE_ITEM);
		if (biologChanceItem)
		{
			const int BIOLOG_CHANCE = biologChanceItem->GetValue(0);

			if (BIOLOG_CHANCE > 0)
			{
				if (biologChanceItem->GetCount() > 1)
				{
					biologChanceItem->SetCount(biologChanceItem->GetCount() - 1);
					chance += BIOLOG_CHANCE;

#ifdef __DEBUG_PRINT
					cerr << "SendBiologItem: Intercase chance SUCCEED! percent before: " << chance - BIOLOG_CHANCE << " after " << chance << endl;
#endif
				}
				else
				{
					m_pkChar->ChatPacket(CHAT_TYPE_INFO, "You doesn't have required item for intercase chance!");
					return;
				}
			}
			else {
#ifdef __DEBUG_PRINT
				cerr << "SendBiologItem: Intercase chance FAILED! item: " << BIOLOG_CHANCE_ITEM << " value0 == 0" << endl;
#endif
				return;
			}
		}
	}

	// Checking count
	if (iCount < pMission->wRequiredItemCount) // it means you haven't give all items
	{
		m_pkChar->RemoveSpecifyItem(pMission->iRequiredItem, 1);

		if (chance >= number(1, 100))
		{
			// Update count
			m_pkChar->SetBiologCollectedItems(iCount + 1);
			m_pkChar->ChatPacket(CHAT_TYPE_INFO, "This item is good!");
		} else 
			m_pkChar->ChatPacket(CHAT_TYPE_INFO, "This item is not good, try next time later.");

		// Update cooldown
		m_pkChar->SetBiologCooldown(get_global_time() + pMission->iCooldown);
		
		// Active Alert
		if (m_pkChar->GetBiologCooldownReminder())
			ActiveAlert(true);

		bool bSendInformation = true;
		// Compare current count with required count
		if (m_pkChar->GetBiologCollectedItems() == pMission->wRequiredItemCount)
		{
#ifdef __DEBUG_PRINT
			cerr << "You give me all required items, right now you have to get reward!" << endl;
#endif

			// We have to give reward
			GiveRewardByMission(bMission);

			// We have to reset mission
			ResetMission();

			BYTE nextMission = bMission + 1;
			m_pkChar->SetBiologMissions(nextMission);

			TBiologMissionsProto* pNextMission = CBiologSystemManager::instance().GetMission(nextMission);
			if (!pNextMission)
			{
#ifdef __DEBUG_PRINT
				cerr << "You done every mission from biolog." << endl;
#endif
				SendClientPacket(m_pkChar->GetDesc(), GC_BIOLOG_CLOSE, NULL, 0);
				bSendInformation = false;
			}
		}

		// Update client!
		if (bSendInformation)
			SendBiologInformation(true);

#ifdef __DEBUG_PRINT
		cerr << "SendBiologItem: Level actual: " << m_pkChar->GetBiologMissions() << endl;
		cerr << "SendBiologItem: Counts: *" << m_pkChar->GetBiologCollectedItems() << "* : *" << pMission->wRequiredItemCount << "*" << endl;
		cerr << "SendBiologItem: Current time: " << GetFullDateFromTime(get_global_time()).c_str() << endl;
		cerr << "SendBiologItem: Next possible try: " << GetFullDateFromTime(get_global_time() + pMission->iCooldown).c_str() << endl;
#endif
	}
}

void CBiologSystem::GiveRewardByMission(BYTE bMission)
{
	if (!m_pkChar)
		return;

	TBiologRewardsProto* pReward = CBiologSystemManager::instance().GetReward(bMission);
	if (!pReward)
	{
		sys_err("GiveRewardByMission -> cannot get reward %u by %s", bMission, m_pkChar->GetName());
		return;
	}

#ifdef __DEBUG_PRINT
	cerr << "GiveRewardByMission -> Reward for mission: " << bMission << " by " << m_pkChar->GetName()
		<< " type: " << pReward->bApplyType << " value: " << pReward->lApplyValue << endl;
#endif

	m_pkChar->AddAffect(AFFECTS[bMission], aApplyInfo[pReward->bApplyType].bPointType, pReward->lApplyValue, 0, BIOLOG_AFF_TIME, 0, true);
}

bool CBiologSystem::GetBiologItemByMobVnum(LPCHARACTER pkKiller, WORD MonsterVnum, DWORD& ItemVnum, BYTE& bChance)
{
	if (!pkKiller || !MonsterVnum)
		return false;

	const CMob* pkMob = CMobManager::instance().Get(MonsterVnum);
	if (!pkMob)
		return false;

	auto bMission = pkKiller->GetBiologMissions();

#ifdef __DEBUG_PRINT
	cerr << "GetBiologItemByMobVnum: #1 " << bMission << " by " << pkKiller->GetName() << endl;
#endif

	TBiologMissionsProto* pMission = CBiologSystemManager::instance().GetMission(bMission);
	if (!pMission)
		return false;

#ifdef __DEBUG_PRINT
	cerr << "GetBiologItemByMobVnum: #2 " << bMission << " by " << pkKiller->GetName() << endl;
#endif

	if (pkKiller->GetLevel() < pMission->bRequiredLevel)
		return false;

#ifdef __DEBUG_PRINT
	cerr << "GetBiologItemByMobVnum: #3 " << bMission << " by " << pkKiller->GetName() << endl;
#endif

	std::vector<TBiologMonstersProto*> monsters;
	if(!CBiologSystemManager::instance().GetMonsters(bMission, monsters))
		return false;

	auto monster = std::find_if(monsters.begin(), monsters.end(), [MonsterVnum](TBiologMonstersProto* m) {
		return m->dwMonsterVnum == MonsterVnum;
	});
	if (monster == monsters.end())
		return false;

	ItemVnum = pMission->iRequiredItem;
	bChance = static_cast<TBiologMonstersProto*>(*monster)->bChance;

	return true;
}

/*******************************************************************\
| [PUBLIC] Alert Functions
\*******************************************************************/

void CBiologSystem::ActiveAlert(bool bReminder)
{
	if (bReminder)
	{
		m_pkChar->SetBiologCooldownReminder(1);

		if (m_pkChar->GetBiologCooldown() < get_global_time())
			return;

		if (!CEventFunctionHandler::Instance().FindEvent(s_current_biolog_reminder))
		{
#ifdef __DEBUG_PRINT
			cerr << "ActiveAlert -> bCheck: " << m_pkChar->GetBiologCooldown() - get_global_time() << " by " << m_pkChar->GetName() << endl;
#endif

			CEventFunctionHandler::instance().AddEvent([this](SArgumentSupportImpl*)
				{
					BroadcastAlert();
				},
				(s_current_biolog_reminder), m_pkChar->GetBiologCooldown() - get_global_time());

#ifdef __DEBUG_PRINT
			cerr << "ActiveAlert -> when: " << m_pkChar->GetBiologCooldown() - get_global_time() << " by " << m_pkChar->GetName() << endl;
#endif
		} else
			CEventFunctionHandler::instance().DelayEvent((s_current_biolog_reminder), m_pkChar->GetBiologCooldown() - get_global_time());

		return;
	}

	m_pkChar->SetBiologCooldownReminder(0);
	if (CEventFunctionHandler::Instance().FindEvent(s_current_biolog_reminder))
	{
		CEventFunctionHandler::Instance().RemoveEvent((s_current_biolog_reminder));
	}
}

void CBiologSystem::BroadcastAlert()
{
	SendClientPacket(m_pkChar->GetDesc(), GC_BIOLOG_ALERT, NULL, 0);
}

/*******************************************************************\
| [PUBLIC] Incoming Packet Functions
\*******************************************************************/

int CBiologSystem::RecvClientPacket(BYTE bSubHeader, const char* c_pData, size_t uiBytes)
{
	switch (bSubHeader)
	{
		case CG_BIOLOG_OPEN:
		{
			SendBiologInformation();
			return 0;
		}
		break;

		case CG_BIOLOG_SEND:
		{
			if (uiBytes < sizeof(bool) + sizeof(bool))
				return -1;

			bool bAdditionalChance = *(bool*)c_pData;
			c_pData += sizeof(bool);
			uiBytes -= sizeof(bool);

			bool bAdditionalTime = *(bool*)c_pData;
			c_pData += sizeof(bool);
			uiBytes -= sizeof(bool);

#ifdef __DEBUG_PRINT
			cerr << "RecvClientPacket -> bSubHeader: " << bSubHeader << " by " << m_pkChar->GetName() << " bAdditionalChance: " << bAdditionalChance << " bAdditionalTime: " << bAdditionalTime << endl;
#endif
			SendBiologItem(bAdditionalChance, bAdditionalTime);
			return sizeof(bool) + sizeof(bool);
		}
		break;

		case CG_BIOLOG_TIMER:
		{
			if (uiBytes < sizeof(bool))
				return -1;

			bool bReminder = *(bool*)c_pData;
			c_pData += sizeof(bool);
			uiBytes -= sizeof(bool);

			ActiveAlert(bReminder);
			return sizeof(bool);
		}
		break;
	}

	sys_err("invalid subheader %u", bSubHeader);
	return -1;
}

/*******************************************************************\
| [PUBLIC] Outgoing Packet Functions
\*******************************************************************/

void CBiologSystem::SendClientPacket(LPDESC pkDesc, BYTE bSubHeader, const void* c_pvData, size_t iSize)
{
	TPacketGCBiologManager packet;
	packet.bHeader = HEADER_GC_BIOLOG_MANAGER;
	packet.wSize = sizeof(packet) + iSize;
	packet.bSubHeader = bSubHeader;

	TEMP_BUFFER buf;
	buf.write(&packet, sizeof(packet));
	if (iSize)
		buf.write(c_pvData, iSize);

	pkDesc->Packet(buf.read_peek(), buf.size());
}

void CBiologSystem::SendClientPacket(DWORD dwPID, BYTE bSubHeader, const void* c_pvData, size_t iSize)
{
	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindByPID(dwPID);

	if (pkChr)
	{
		SendClientPacket(pkChr->GetDesc(), bSubHeader, c_pvData, iSize);
	}
	else
	{
		CCI* pkCCI = P2P_MANAGER::instance().FindByPID(dwPID);
		if (pkCCI)
		{
			pkCCI->pkDesc->SetRelay(pkCCI->szName);
			SendClientPacket(pkCCI->pkDesc, bSubHeader, c_pvData, iSize);
		}
		else
			sys_err("cannot send client packet to pid %u subheader %hu [cannot find player]", dwPID, bSubHeader);
	}
}

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
|||| CBiologSystemManager - CLASS
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/

/*******************************************************************\
| [PUBLIC] (De-)Initialize Functions
\*******************************************************************/

CBiologSystemManager::CBiologSystemManager()
{
}

CBiologSystemManager::~CBiologSystemManager()
{
}

void CBiologSystemManager::InitializeMissions(TBiologMissionsProto* pData, size_t size)
{
	for (size_t i{ 0 }; i < size; ++i, ++pData)
	{
		thecore_memcpy(&m_mapMission_Proto[pData->bMission], pData, sizeof(TBiologMissionsProto));
	}
}

TBiologMissionsProto* CBiologSystemManager::GetMission(BYTE bMission)
{
	TMissionProtoMap::iterator it = m_mapMission_Proto.find(bMission);
	if (it == m_mapMission_Proto.end())
		return NULL;

	return &it->second;
}

void CBiologSystemManager::InitializeRewards(TBiologRewardsProto* pData, size_t size)
{
	for (size_t i{ 0 }; i < size; ++i, ++pData)
	{
		thecore_memcpy(&m_mapReward_Proto[pData->bMission], pData, sizeof(TBiologRewardsProto));
	}
}

TBiologRewardsProto* CBiologSystemManager::GetReward(BYTE bMission)
{
	TRewardProtoMap::iterator it = m_mapReward_Proto.find(bMission);
	if (it == m_mapReward_Proto.end())
		return NULL;

	return &it->second;
}

void CBiologSystemManager::InitializeMonsters(TBiologMonstersProto* pData, size_t size)
{
	for (size_t i{ 0 }; i < size; ++i, ++pData)
	{
		m_mapMonsters_Proto[pData->bMission].push_back(pData);
	}
}

bool CBiologSystemManager::GetMonsters(BYTE bMission, std::vector<TBiologMonstersProto*>& monsters)
{
	auto it = m_mapMonsters_Proto.find(bMission);
	if (it == m_mapMonsters_Proto.end())
		return false;

	monsters = it->second;

	return true;
}

#endif
