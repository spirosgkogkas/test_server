#include "stdafx.h"
#include "../../common/stl.h"
#include "config.h"
#include "vector.h"
#include "char.h"
#include "sectree_manager.h"
#include "char_manager.h"
#include "mob_manager.h"
#ifdef ENABLE_LOTTERY_SYSTEM
#include "utils.h"
#include "LotteryManager.h"
#include "affect.h"
#include "desc.h"
#include "desc_manager.h"
#include "desc_client.h"
#include "questmanager.h"
#include "packet.h"
#include "locale_service.h"
#include "p2p.h"
#include "buffer_manager.h"

EVENTINFO(roll_start)
{

	roll_start()
	{
	}
};

EVENTFUNC(lottery_roll_event)
{
	roll_start* info = dynamic_cast<roll_start*>(event->info);

	if (info == NULL)
	{
		sys_err("lottery_roll_event> <Factor> NULL pointer");
		return 0;
	}

	CLotteryManager::instance().StartRollEvent();
	return 0;
}

CLotteryManager::CLotteryManager()
{
}

CLotteryManager::~CLotteryManager()
{
	Destroy();
}

void CLotteryManager::Destroy()
{
	kumarbaz_map.clear();
	statistic_kumar.clear();
	destroyEvent = NULL;
	event_cancel(&destroyEvent);
}

void CLotteryManager::StartRollEvent()
{
	BYTE randNumber;
	BYTE randNumber2 = number(0, 100);
	
	if(randNumber2 <= 2)
		randNumber = 0;
	else if(randNumber2 > 2 && randNumber2 < 49)
		randNumber = 1;
	else
		randNumber = 2;
	
	for (auto it = kumarbaz_map.begin(); it != kumarbaz_map.end(); ++it)
	{
		LPCHARACTER ch = it->first;
		DWORD miktar = it->second.first;
		BYTE color = it->second.second;
		
		if(!ch || !ch->GetDesc())
			return;

		if(color == randNumber)
		{
			char msg[CHAT_MAX_LEN + 1];
			if(color == 0)
			{
				snprintf(msg, sizeof(msg), LC_TEXT("%s_%d_T_LOTTERYMESAJ_%d"),ch->GetName(),miktar,miktar*14);
				ch->SetDragonCoin(ch->GetDragonCoin() + miktar*14);
				CLotteryManager::instance().StatisticAdd(ch,miktar,1);
			}
			else
			{
				snprintf(msg, sizeof(msg), LC_TEXT("%s_%d_T_LOTTERYMESAJ_%d"),ch->GetName(),miktar,miktar*2);
				ch->SetDragonCoin(ch->GetDragonCoin() + miktar*2);
				CLotteryManager::instance().StatisticAdd(ch,miktar,1);
			}
			
			// TPacketGCWhisper pack;
			// int len = MIN(CHAT_MAX_LEN, strlen(msg) + 1);
			// pack.bHeader = HEADER_GC_WHISPER;
			// pack.wSize = sizeof(TPacketGCWhisper) + len;
			// pack.bType = WHISPER_TYPE_NORMAL;
			// strlcpy(pack.szNameFrom, LC_TEXT("LOTTERYBASLIK"), sizeof(pack.szNameFrom));
			// TEMP_BUFFER buf;
			// buf.write(&pack, sizeof(TPacketGCWhisper));
			// buf.write(msg, len);
			// ch->GetDesc()->Packet(buf.read_peek(), buf.size());
			// sys_err("Roll atan kiþi: %s , Miktar %d, Rakam %d",ch->GetName(),miktar,color);
		}
		else
		{
			CLotteryManager::instance().StatisticAdd(ch,miktar,0);
		}
	}
	
	CLotteryManager::instance().Roll(randNumber);
	
	roll_start* info = AllocEventInfo<roll_start>();
	destroyEvent = event_create(lottery_roll_event, info, PASSES_PER_SEC(CLotteryManager::instance().LOTTERY_ROLL_TIME));
}

struct notice_roll_map_packet_func
{
	int m_mapIndex;
	BYTE m_randNumber;

	notice_roll_map_packet_func(int idx, BYTE randNumber) : m_mapIndex(idx), m_randNumber(randNumber)
	{
	}

	void operator() (LPDESC d)
	{
		if (d->GetCharacter() == NULL) return;
		if (d->GetCharacter()->GetMapIndex() != m_mapIndex) return;
		
		d->GetCharacter()->ChatPacket(CHAT_TYPE_BIG_NOTICE, "Kumar Turu baþladý!");
		
		d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "LotteryRegisterReward %d", m_randNumber);

		if(m_randNumber == 0)
			sys_err("Kumar turunda bu raund Yeþil Geldi!");
		if(m_randNumber == 1)
			sys_err("Kumar turunda bu raund Kýrmýzý Geldi!");
		if(m_randNumber == 2)
			sys_err("Kumar turunda bu raund Mavi Geldi!");

		CLotteryManager::instance().ClearAllInfo();
		
		d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "SaniyeBaslat %d", CLotteryManager::instance().LOTTERY_ROLL_TIME);
		
		CLotteryManager::instance().StatisticBilgiGonder(d->GetCharacter());
		
		sys_err("Roll Bitti");
	}
};

void CLotteryManager::StatisticAdd(LPCHARACTER ch,DWORD miktar,BYTE addMode)
{
	if(!ch || !ch->GetDesc())
		return;
	
	auto it = statistic_kumar.find(ch);
	
	if(it != statistic_kumar.end())
	{
		it->second.dwTotalEp=it->second.dwTotalEp+miktar;
		if(addMode == 0)
			it->second.dwTotalLoss = it->second.dwTotalLoss + 1;
		else
			it->second.dwTotalWin = it->second.dwTotalWin + 1;
	}
	else
	{
		if(addMode == 0) // Lose Ekle
			statistic_kumar.insert(TKumarciMap::value_type(ch, TStatisticKumar(miktar, 0, 1)));
		else
			statistic_kumar.insert(TKumarciMap::value_type(ch, TStatisticKumar(miktar, 1, 0)));
	}
	
}

void CLotteryManager::StatisticBilgiGonder(LPCHARACTER ch)
{
	if(!ch || !ch->GetDesc())
		return;
		
	auto it = statistic_kumar.find(ch);

	if(it != statistic_kumar.end())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "StatisticGonder %ld %d %d", it->second.dwTotalEp,it->second.dwTotalWin,it->second.dwTotalLoss);
	}
}

// Roll After Clean Map
void CLotteryManager::Roll(BYTE randNumber)
{
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), notice_roll_map_packet_func(CLotteryManager::instance().LOTTERY_MAP_INDEX, randNumber));

}

bool CLotteryManager::ClearAllInfo()
{
	kumarbaz_map.clear();
	// statistic_kumar.clear();
	return true;
}

void CLotteryManager::Initialize()
{
	kumarbaz_map.clear();
	statistic_kumar.clear();
	
	// Eventi Baþlat
	
	roll_start* info = AllocEventInfo<roll_start>();
	destroyEvent = event_create(lottery_roll_event, info, PASSES_PER_SEC(CLotteryManager::instance().LOTTERY_ROLL_TIME));
}

bool CLotteryManager::RegisterEvent(LPCHARACTER ch,DWORD betMiktar,BYTE color)
{
	// color 1 ise yeþil
	// color 2 ise kýrmýzý
	// color 3 ise mavi
	if(!ch || !ch->GetDesc())
		return false;
	
	if(ch->GetMapIndex() != LOTTERY_MAP_INDEX)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "NoMatchMapIndex_Alert");
		return false;
	}

	bool bRegisterStatus = false;

	auto getMapInfo = kumarbaz_map.equal_range(ch);

	for (auto i = getMapInfo.first; i != getMapInfo.second; ++i)
	{
		bRegisterStatus = true;
	}
	
	if(bRegisterStatus == false)
	{
		kumarbaz_map.emplace(std::make_pair(ch, std::make_pair(betMiktar, color)));
		CLotteryManager::instance().RefreshRegisterList();
		return true;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO,"Zaten tura kayýtlýsýnýz.");
		return false;
	}
}

struct map_packet_func
{
	int m_mapIndex;
	TPacketGCRefreshKumarList m_list;

	map_packet_func(int idx, TPacketGCRefreshKumarList list) : m_mapIndex(idx), m_list(list)
	{
	}

	void operator() (LPDESC d)
	{
		if (d->GetCharacter() == NULL) return;
		if (d->GetCharacter()->GetMapIndex() != m_mapIndex) return;

		d->GetCharacter()->GetDesc()->Packet(&m_list, sizeof(m_list));

		sys_err("map_packet_func Bitti");
	}
};

void CLotteryManager::RefreshRegisterList()
{

	TPacketGCRefreshKumarList pKumarList;
	pKumarList.header = HEADER_GC_LOTTERY_LIST_SYSTEM;
	BYTE sayi = 0;
	for (auto it = kumarbaz_map.begin(); it != kumarbaz_map.end(); ++it)
	{
		if(sayi > 10)
			return;

		LPCHARACTER chKumarci = it->first;
		DWORD miktar = it->second.first;
		BYTE color = it->second.second;
		
		LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().FindByPID(chKumarci->GetPlayerID());
		
		if(pkVictim && pkVictim->GetDesc())
		{
			strlcpy(pKumarList.isim, pkVictim->GetName(), sizeof(pKumarList.isim));
			pKumarList.miktar = miktar;
			pKumarList.renk = color;
			
			sys_err("Ýsim %s,miktar %d,renk %d",pkVictim->GetName(),miktar,color);
		}
		else
		{
			strlcpy(pKumarList.isim, "Bilinmeyen Yatýrýmcý", sizeof(pKumarList.isim));
			pKumarList.miktar = miktar;
			pKumarList.renk = color;
		}
		
		const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_ref_set.begin(), c_ref_set.end(), map_packet_func(CLotteryManager::instance().LOTTERY_MAP_INDEX, pKumarList));

		sayi++;
	}
}

#endif

