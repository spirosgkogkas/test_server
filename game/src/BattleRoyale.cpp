#include "stdafx.h"
#include <iostream>
#if defined(BATTLE_ROYAL_SYSTEM)
#include "config.h"
#include "cmd.h"
#include "entity.h"
#include <ctime>
#include "char.h"
#include "item_manager.h"
#include "item.h"
#include "db.h"
#include "unique_item.h"
#include "desc.h"
#include "affect.h"
#include "questmanager.h"
#include "sectree_manager.h"
#include "char_manager.h"
#include "start_position.h"
#include "locale_service.h"
#include <fstream>
#include "utils.h"
#include "BattleRoyale.h"

#define MAX_RANDOM_HP 
// constans.cpp 36-44 random level hp (this only 44* level -1 if this komment (//) random 36-44 * level -1 )

static int ALL_BATTLE_ROYALE_ITEM[] =
{
	WARRIOR_SWORD, WARRIOR_SWORD_2, WARRIOR_SWORD_3, WARROIR_TWOHAND, WARROIR_TWOHAND_2, WARROIR_TWOHAND_3, SURA_SWORD, SURA_SWORD_2, SURA_SWORD_3, SHAMAN_GLOCK, SHAMAN_GLOCK_2, SHAMAN_GLOCK_3, SHAMAN_FAN, SHAMAN_FAN_2, SHAMAN_FAN_3, ASSASSIN_DAGGER, ASSASSIN_DAGGER_2, ASSASSIN_DAGGER_3, ASSASSIN_SWORD, ASSASSIN_SWORD_2, ASSASSIN_SWORD_3, ASSASSIN_BOW, ASSASSIN_BOW_2, ASSASSIN_BOW_3, ASSASSIN_ARROW,
	RARE_WARRIOR_SWORD, RARE_WARRIOR_SWORD_2, RARE_WARRIOR_SWORD_3, RARE_WARROIR_TWOHAND, RARE_WARROIR_TWOHAND_2, RARE_WARROIR_TWOHAND_3, RARE_SURA_SWORD, RARE_SURA_SWORD_2, RARE_SURA_SWORD_3, RARE_SHAMAN_GLOCK, RARE_SHAMAN_GLOCK_2, RARE_SHAMAN_GLOCK_3, RARE_SHAMAN_FAN, RARE_SHAMAN_FAN_2, RARE_SHAMAN_FAN_3, RARE_ASSASSIN_DAGGER, RARE_ASSASSIN_DAGGER_2, RARE_ASSASSIN_DAGGER_3, RARE_ASSASSIN_SWORD, RARE_ASSASSIN_SWORD_2, RARE_ASSASSIN_SWORD_3, RARE_ASSASSIN_BOW, RARE_ASSASSIN_BOW_2, RARE_ASSASSIN_BOW_3,
	EPIC_WARRIOR_SWORD, EPIC_WARRIOR_SWORD_2, EPIC_WARRIOR_SWORD_3, EPIC_WARROIR_TWOHAND, EPIC_WARROIR_TWOHAND_2, EPIC_WARROIR_TWOHAND_3, EPIC_SURA_SWORD, EPIC_SURA_SWORD_2, EPIC_SURA_SWORD_3, EPIC_SHAMAN_GLOCK, EPIC_SHAMAN_GLOCK_2, EPIC_SHAMAN_GLOCK_3, EPIC_SHAMAN_FAN, EPIC_SHAMAN_FAN_2, EPIC_SHAMAN_FAN_3, EPIC_ASSASSIN_DAGGER, EPIC_ASSASSIN_DAGGER_2, EPIC_ASSASSIN_DAGGER_3, EPIC_ASSASSIN_SWORD, EPIC_ASSASSIN_SWORD_2, EPIC_ASSASSIN_SWORD_3, EPIC_ASSASSIN_BOW, EPIC_ASSASSIN_BOW_2, EPIC_ASSASSIN_BOW_3,

	COMMON_BODY, COMMON_BODY_2, COMMON_BODY_3, RARE_BODY, RARE_BODY_2, RARE_BODY_3, EPIC_BODY, EPIC_BODY_2, EPIC_BODY_3
#ifdef ENABLE_WOLFMAN_CHARACTER										
	, WOLFMAN_WEAPON, WOLFMAN_WEAPON_2, WOLFMAN_WEAPON_3, RARE_WOLFMAN_WEAPON, RARE_WOLFMAN_WEAPON_2, RARE_WOLFMAN_WEAPON_3, EPIC_WOLFMAN_WEAPON, EPIC_WOLFMAN_WEAPON_2, EPIC_WOLFMAN_WEAPON_3
#endif
};

int BATTLE_ROYALE_STATUS;
int BATTLE_ROYAL_GIFT_STATUS;
int HOUR_1;
int MINUTE_1;
int SECOND_1;
int HOUR_2;
int MINUTE_2;
int SECOND_2;
int HOUR_3;
int MINUTE_3;
int SECOND_3;
int ITEM_VNUM;
int ITEM_COUNT;
int RESET_RANK_HOUR;
int RESET_RANK_MINUTE;
int RESET_RANK_SECOND;
int RANK_DAY;

int DEFAULT_DAMAGE;
int COMMON_DAMAGE;
int RARE_DAMAGE;
int EPIC_DAMAGE;
int COMMON_ARMOR;
int RARE_ARMOR;
int EPIC_ARMOR;

int ORIGO_X;
int ORIGO_Y;
int ORIGO_X_2;
int ORIGO_Y_2;

EVENTINFO(BattleRoyaleEventInfo)
{
	CBattleRoyale *InstanceBattleRoyale;

	BattleRoyaleEventInfo()
		: InstanceBattleRoyale(0)
	{
	}
};

EVENTFUNC(Begint_Battle_Royale_Event)
{
	if (!event || !event->info)
		return 0;

	BattleRoyaleEventInfo* info = dynamic_cast<BattleRoyaleEventInfo*>(event->info);

	if (!info)
		return 0;

	CBattleRoyale* InstanceBattleRoyale = info->InstanceBattleRoyale;

	if (!InstanceBattleRoyale)
		return 0;

	time_t currentTime;
	struct tm *localTime;

	time(&currentTime);
	localTime = localtime(&currentTime);

	int mResultHour = localTime->tm_hour, mResultMin = localTime->tm_min, mResultSec = localTime->tm_sec;
	int MINUS_TIME = 1;

	if (BATTLE_ROYALE_STATUS == 1)
	{
		if (((mResultHour == HOUR_1 - MINUS_TIME && mResultMin == MINUTE_1 && mResultSec == SECOND_1) || (mResultHour == HOUR_2 - MINUS_TIME && mResultMin == MINUTE_2 && mResultSec == SECOND_2) || (mResultHour == HOUR_3 - MINUS_TIME && mResultMin == MINUTE_3 && mResultSec == SECOND_3)))
		{
			int STARTING_TIME = mResultHour + MINUS_TIME;
			char BATTLE_ROYAL[256];
			snprintf(BATTLE_ROYAL, sizeof(BATTLE_ROYAL), LC_TEXT("Battle Royale Start Hour: %d Minute: %d Second: %d"), MINUS_TIME, mResultMin, mResultSec);
			SendNotice(BATTLE_ROYAL);
			CBattleRoyale::instance().DeleteMapLogout();
		}

		if (((mResultHour == HOUR_1 && mResultMin == MINUTE_1 && mResultSec == SECOND_1) || (mResultHour == HOUR_2 && mResultMin == MINUTE_2 && mResultSec == SECOND_2) || (mResultHour == HOUR_3 && mResultMin == MINUTE_3 && mResultSec == SECOND_3)))
		{
			SendNotice(LC_TEXT("Battle Royale event start"));
			quest::CQuestManager::instance().RequestSetEventFlag("battleroyale_event", 1);
			CBattleRoyale::instance().DeleteMapLogout();
		}

		if (((mResultHour == HOUR_1 && mResultMin == MINUTE_1 + RANGETIME_1_BEGIN) || (mResultHour == HOUR_2 && mResultMin == MINUTE_2 + RANGETIME_2_BEGIN) || (mResultHour == HOUR_3 && mResultMin == MINUTE_3 + RANGETIME_3_BEGIN)) && (mResultSec == 02))
		{
			quest::CQuestManager::instance().RequestSetEventFlag("battleroyale_event", 0);
			SendNotice(LC_TEXT("Battle Royale registration end"));
			CBattleRoyale::instance().ClearWinnerCounter();
		}

		int END_RANGE_1_1 = MINUTE_1 + RANGETIME_1_BEGIN + RANGETIME_1_END, END_RANGE_1_2 = MINUTE_2 + RANGETIME_2_BEGIN + RANGETIME_2_END, END_RANGE_1_3 = MINUTE_3 + RANGETIME_3_BEGIN + RANGETIME_3_END;
		int EVENT_END_1 = END_RANGE_1_1 + RANGETIME2_1_END + EVENT_1_END, EVENT_END_2 = END_RANGE_1_2 + RANGETIME2_2_END + EVENT_2_END, EVENT_END_3 = END_RANGE_1_3 + RANGETIME2_3_END + EVENT_3_END;

		if (((mResultHour == HOUR_1 && mResultMin == EVENT_END_1) || (mResultHour == HOUR_2 && mResultMin == EVENT_END_2) || (mResultHour == HOUR_3 && mResultMin == EVENT_END_3)) && (mResultSec == 00))
		{
			SendNotice(LC_TEXT("Battle Royale event end"));
			CBattleRoyale::instance().DeleteMapLogout();
		}

		if (((mResultHour == HOUR_1 && mResultMin == EVENT_END_1) || (mResultHour == HOUR_2 && mResultMin == EVENT_END_2) || (mResultHour == HOUR_3 && mResultMin == EVENT_END_3)) && (mResultSec == 20))
		{
			SECTREE_MANAGER::instance().PurgeNPCsInMap(BATTLE_ROYAL_MAP_INDEX);
			CBattleRoyale::instance().DeleteMapLogout();
		}

		int mResulDay = localTime->tm_wday;

		if (RANK_DAY == 10)
		{
			if (mResultHour == RESET_RANK_HOUR && mResultMin == RESET_RANK_MINUTE && mResultSec == RESET_RANK_SECOND)
			{
				CBattleRoyale::instance().ResetRanking();
			}
		}
		if (RANK_DAY != 10)
		{
			if (mResulDay == RANK_DAY)
			{
				if (mResultHour == RESET_RANK_HOUR && mResultMin == RESET_RANK_MINUTE && mResultSec == RESET_RANK_SECOND)
				{
					CBattleRoyale::instance().ResetRanking();
				}
			}
		}
	}

	return PASSES_PER_SEC(1);
}

bool CBattleRoyale::Initialize()
{
	BattleRoyaleEventInfo* info = AllocEventInfo<BattleRoyaleEventInfo>();
	info->InstanceBattleRoyale = this;
	m_Begin_Battle_royale = event_create(Begint_Battle_Royale_Event, info, PASSES_PER_SEC(10));

	return true;
}

void CBattleRoyale::OnLogin(LPCHARACTER ch)
{
	if (!ch)
		return;

	if ((ch->GetMapIndex() == BATTLE_ROYAL_MAP_INDEX) && (ch->GetGMLevel() == GM_PLAYER))
	{
		EnterAttender(ch);
		AddToMaLLBattleRoyaleMap(ch);
		HPSetBattleroyale(ch);
		ch->SetKillerMode(true);
		ch->SetMaxHP(START_HP_ON_MAP);
		CHARACTER_MANAGER::instance().SpawnMobRandomPosition(BOX_VNUM, BATTLE_ROYAL_MAP_INDEX);

		ch->StartCheckBattleRoyaleRange();

		if (ch->IsRiding())
		{
			ch->StopRiding();
		}

		if (BATTLE_ROYAL_GIFT_STATUS == 1)
		{
			ch->SetQuestFlag("BattleRoyale.battle_royale_gift", 1);
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your gift in the mall."));
		}
		if (ch->GetQuestFlag("BattleRoyale.battleroyalemap") == 0)
		{
			int X_LOGIN = ch->GetX() + number(1500, 2500), Y_LOGIN = ch->GetY() + number(2000, 2500);
			switch (number(1,3))
			{
				case 1:
				{
					CHARACTER_MANAGER::instance().SpawnMob(BOX_VNUM, BATTLE_ROYAL_MAP_INDEX, X_LOGIN, Y_LOGIN, 0);
				}
				break;
				case 2:
				{
					CHARACTER_MANAGER::instance().SpawnMob(SWORD_BOX, BATTLE_ROYAL_MAP_INDEX, X_LOGIN, Y_LOGIN, 0);
				}
				break;
				case 3:
				{
					CHARACTER_MANAGER::instance().SpawnMob(ARMOR_BOX, BATTLE_ROYAL_MAP_INDEX, X_LOGIN, Y_LOGIN, 0);
				}
				break;
			}

			DeleteTakenTime(ch);
		}
		ch->ChatPacket(CHAT_TYPE_COMMAND, "SOW_BATTLE_ROYALE_WINDOW");
		for (int i = 0; i < WEAR_MAX; i++)
		{
			LPITEM item = ch->GetWear(i);
			if (item != NULL)
			{
				ch->UnequipItem(item);
			}
		}

		if (ch->GetQuestFlag("BattleRoyale.battleroyalemap") == 1)
		{
			//ch->DestroyRange();
			ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
		}
	}
}

void CBattleRoyale::OnLogout(LPCHARACTER ch)
{
	if (!ch)
		return;

	if ((ch->GetMapIndex() == BATTLE_ROYAL_MAP_INDEX) && (ch->GetGMLevel() == GM_PLAYER))
	{
		EnterAudience(ch);
		//ch->DestroyRange();
		DeleteBattleRoyalItem(ch);
		ch->SetQuestFlag("BattleRoyale.battleroyalemap", 1);
		HPResetBattleroyale(ch);
		if (ch->IsAffectFlag(AFF_BATTLEROYALE_XY) != NULL)
		{
			ch->ClearAffect(AFF_BATTLEROYALE_XY);
		}
	}
}

void CBattleRoyale::DeleteMapLogout()
{
	DBManager::instance().DirectQuery("DELETE FROM player.quest WHERE lValue > 0 and szState = 'battleroyalemap'");

	if (BATTLE_ROYAL_GIFT_STATUS == 1)
	{
		DBManager::instance().DirectQuery("DELETE FROM player.quest WHERE lValue > 0 and szState = 'battle_royale_gift'");
	}
}

bool CBattleRoyale::EnterAttender(LPCHARACTER ch)
{
	DWORD pid = ch->GetPlayerID();

	m_map_char.insert(std::make_pair(pid, pid));
	m_map_attender.insert(std::make_pair(pid, pid));

	return true;
}

bool CBattleRoyale::EnterAudience(LPCHARACTER ch)
{
	DWORD pid = ch->GetPlayerID();
	m_map_char.erase(ch->GetPlayerID());
	m_map_attender.erase(ch->GetPlayerID());

	return true;
}

void CBattleRoyale::SpawnBattleroyale(LPCHARACTER ch)
{
	int X_SPAWN = ch->GetX() + number(2000, 10000), Y_SPAWN = ch->GetY() + number(2000, 10000);
	switch (number(1,3))
	{
		case 1:
		{
			CHARACTER_MANAGER::instance().SpawnMob(BOX_VNUM, BATTLE_ROYAL_MAP_INDEX, X_SPAWN, Y_SPAWN, 0);
		}
		break;
		case 2:
		{
			CHARACTER_MANAGER::instance().SpawnMob(SWORD_BOX, BATTLE_ROYAL_MAP_INDEX, X_SPAWN, Y_SPAWN, 0);
		}
		break;
		case 3:
		{
			CHARACTER_MANAGER::instance().SpawnMob(ARMOR_BOX, BATTLE_ROYAL_MAP_INDEX, X_SPAWN, Y_SPAWN, 0);
		}
		break;
	}
}

void CBattleRoyale::CreateBattleRoyaleID(LPCHARACTER ch, DWORD ITEM_ID)
{
	LPITEM all_id = ITEM_MANAGER::instance().CreateItem(ITEM_ID);
	if (!all_id || !all_id->EquipTo(ch, all_id->FindEquipCell(ch)))
	{
		ITEM_MANAGER::instance().RemoveItem(all_id);
	}
}

void CBattleRoyale::SwitchBattleRoyaleItem(LPCHARACTER ch, DWORD SWITCH_ID1, DWORD SWITCH_ID2, DWORD SWITCH_ID3)
{
	switch (number(1, 3))
	{
		case 1:
		{
			CreateBattleRoyaleID(ch, SWITCH_ID1);
		}
		break;
		case 2:
		{
			CreateBattleRoyaleID(ch, SWITCH_ID2);
		}
		break;
		case 3:
		{
			CreateBattleRoyaleID(ch, SWITCH_ID3);
		}
		break;
	}
}

void CBattleRoyale::SwitchBattleRoyaleTierItem(LPCHARACTER ch, DWORD SWITCH_ID1, DWORD SWITCH_ID2, DWORD SWITCH_ID3, DWORD SWITCH_ID4, DWORD SWITCH_ID5, DWORD SWITCH_ID6, DWORD SWITCH_ID7, DWORD SWITCH_ID8, DWORD SWITCH_ID9)
{
	switch (number(1, 3))
	{
	case 1:
	{
		SwitchBattleRoyaleItem(ch, SWITCH_ID1, SWITCH_ID2, SWITCH_ID3);
	}
	break;
	case 2:
	{
		SwitchBattleRoyaleItem(ch, SWITCH_ID4, SWITCH_ID5, SWITCH_ID6);
	}
	break;
	case 3:
	{
		SwitchBattleRoyaleItem(ch, SWITCH_ID7, SWITCH_ID8, SWITCH_ID9);
	}
	break;
	}
}

void CBattleRoyale::SwordBox(LPCHARACTER ch)
{
	LPITEM WEAPONDESTROY = ch->GetWear(WEAR_WEAPON);
	if (WEAPONDESTROY != NULL)
	{
		ITEM_MANAGER::instance().RemoveItem(WEAPONDESTROY);
	}

	switch (ch->GetJob())
	{
#ifdef ENABLE_WOLFMAN_CHARACTER	
		case JOB_WOLFMAN:
		{		
			SwitchBattleRoyaleTierItem(ch, WOLFMAN_WEAPON, WOLFMAN_WEAPON_2, WOLFMAN_WEAPON_3, RARE_WOLFMAN_WEAPON, RARE_WOLFMAN_WEAPON_2, RARE_WOLFMAN_WEAPON_3, EPIC_WOLFMAN_WEAPON, EPIC_WOLFMAN_WEAPON_2, EPIC_WOLFMAN_WEAPON_3);
		}
		break;
#endif
		case JOB_SURA:
		{
			SwitchBattleRoyaleTierItem(ch, SURA_SWORD, SURA_SWORD_2, SURA_SWORD_3, RARE_SURA_SWORD, RARE_SURA_SWORD_2, RARE_SURA_SWORD_3, EPIC_SURA_SWORD, EPIC_SURA_SWORD_2, EPIC_SURA_SWORD_3);
		}
		break;
		case JOB_WARRIOR:
		{
			switch (number(1,2))
			{
				case 1:
				{
					SwitchBattleRoyaleTierItem(ch, WARRIOR_SWORD, WARRIOR_SWORD_2, WARRIOR_SWORD_3, RARE_WARRIOR_SWORD, RARE_WARRIOR_SWORD_2, RARE_WARRIOR_SWORD_3, EPIC_WARRIOR_SWORD, EPIC_WARRIOR_SWORD_2, EPIC_WARRIOR_SWORD_3);
				}
				break;
				case 2:
				{
					SwitchBattleRoyaleTierItem(ch, WARROIR_TWOHAND, WARROIR_TWOHAND_2, WARROIR_TWOHAND_3, RARE_WARROIR_TWOHAND, RARE_WARROIR_TWOHAND_2, RARE_WARROIR_TWOHAND_3, EPIC_WARROIR_TWOHAND, EPIC_WARROIR_TWOHAND_2, EPIC_WARROIR_TWOHAND_3);
				}
				break;
			}
		}
		break;
		case JOB_SHAMAN:
		{
			switch (number(1, 2))
			{
				case 1:
				{
					SwitchBattleRoyaleTierItem(ch, SHAMAN_GLOCK, SHAMAN_GLOCK_2, SHAMAN_GLOCK_3, RARE_SHAMAN_GLOCK, RARE_SHAMAN_GLOCK_2, RARE_SHAMAN_GLOCK_3, EPIC_SHAMAN_GLOCK, EPIC_SHAMAN_GLOCK_2, EPIC_SHAMAN_GLOCK_3);			
				}
				break;
				case 2:
				{
					SwitchBattleRoyaleTierItem(ch, SHAMAN_FAN, SHAMAN_FAN_2, SHAMAN_FAN_3, RARE_SHAMAN_FAN, RARE_SHAMAN_FAN_2, RARE_SHAMAN_FAN_3, EPIC_SHAMAN_FAN, EPIC_SHAMAN_FAN_2, EPIC_SHAMAN_FAN_3);			
				}
				break;
			}
		}
		break;
		case JOB_ASSASSIN:
		{
			switch (number(1, 3))
			{
			case 1:
			{
				SwitchBattleRoyaleTierItem(ch, ASSASSIN_DAGGER, ASSASSIN_DAGGER_2, ASSASSIN_DAGGER_3, RARE_ASSASSIN_DAGGER, RARE_ASSASSIN_DAGGER_2, RARE_ASSASSIN_DAGGER_3, EPIC_ASSASSIN_DAGGER, EPIC_ASSASSIN_DAGGER_2, EPIC_ASSASSIN_DAGGER_3);			
			}
			break;
			case 2:
			{
				SwitchBattleRoyaleTierItem(ch, ASSASSIN_SWORD, ASSASSIN_SWORD_2, ASSASSIN_SWORD_3, RARE_ASSASSIN_SWORD, RARE_ASSASSIN_SWORD_2, RARE_ASSASSIN_SWORD_3, EPIC_ASSASSIN_SWORD, EPIC_ASSASSIN_SWORD_2, EPIC_ASSASSIN_SWORD_3);			
			}
			break;
			case 3:
			{
				SwitchBattleRoyaleTierItem(ch, ASSASSIN_BOW, ASSASSIN_BOW_2, ASSASSIN_BOW_3, RARE_ASSASSIN_BOW, RARE_ASSASSIN_BOW_2, RARE_ASSASSIN_BOW_3, EPIC_ASSASSIN_BOW, EPIC_ASSASSIN_BOW_2, EPIC_ASSASSIN_BOW_3);						

				LPITEM Arrow = ITEM_MANAGER::instance().CreateItem(ASSASSIN_ARROW, ARROW_COUNT);
				if (Arrow)
				{
					ch->EquipItem(Arrow);
				}				
			}
			break;			
			}
		}
		break;
	}
}

void CBattleRoyale::ArmorBox(LPCHARACTER ch)
{
	LPITEM BODYDESTROY = ch->GetWear(WEAR_BODY);
	if (BODYDESTROY != NULL)
	{
		ITEM_MANAGER::instance().RemoveItem(BODYDESTROY);
	}

	SwitchBattleRoyaleTierItem(ch, COMMON_BODY, COMMON_BODY_2, COMMON_BODY_3, RARE_BODY, RARE_BODY_2, RARE_BODY_3, EPIC_BODY, EPIC_BODY_2, EPIC_BODY_3);
}

void CBattleRoyale::BattleRoyaleKill(LPCHARACTER pChar, LPCHARACTER pkKiller)
{
	if (NULL == pChar || NULL == pkKiller)
		return;

	if (false == pChar->IsPC() || false == pkKiller->IsPC())
		return;

	if (pChar->GetMapIndex() != BATTLE_ROYAL_MAP_INDEX || pkKiller->GetMapIndex() != BATTLE_ROYAL_MAP_INDEX)
		return;

	pChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You killed : %s"), pkKiller->GetName());
	CBattleRoyale::instance().HPSettingsBattleRoyale(pChar);
	UpdateKillPoint(pChar->GetPlayerID(), pChar->GetName(), pChar->GetEmpire(), KILL_PER_POINT);
	pkKiller->WarpSet(EMPIRE_START_X(pkKiller->GetEmpire()), EMPIRE_START_Y(pkKiller->GetEmpire()));
	char BATTLE_ROYAL[256];
	snprintf(BATTLE_ROYAL, sizeof(BATTLE_ROYAL), LC_TEXT("He fell out of the game %s"), pkKiller->GetName());
	SendNoticeMap(BATTLE_ROYAL, BATTLE_ROYAL_MAP_INDEX, true);
}

void CBattleRoyale::DeleteBattleRoyalItem(LPCHARACTER ch)
{
	LPITEM item = ch->GetWear(WEAR_WEAPON);
	LPITEM item2 = ch->GetWear(WEAR_ARROW);
	LPITEM item3 = ch->GetWear(WEAR_BODY);
	if (item2 != NULL)
	{
		ITEM_MANAGER::instance().RemoveItem(item2);
	}

	if (item3 != NULL)
	{
		ITEM_MANAGER::instance().RemoveItem(item3);
	}

	if (item != NULL)
	{
		ITEM_MANAGER::instance().RemoveItem(item);
	}

	for (int i = 0; i < _countof(ALL_BATTLE_ROYALE_ITEM); i++)
	{
		for (int j = 0; j < INVENTORY_AND_EQUIP_SLOT_MAX; ++j)
		{
			if (ch->CountSpecifyItem(ALL_BATTLE_ROYALE_ITEM[i]))
			{
				ch->RemoveSpecifyItem(ALL_BATTLE_ROYALE_ITEM[i]);
			}
		}
	}
}

void CBattleRoyale::BlockAutoPotion(LPCHARACTER ch)
{
	int blocked_items[] = { ITEM_AUTO_HP_RECOVERY_S, ITEM_AUTO_HP_RECOVERY_M, ITEM_AUTO_HP_RECOVERY_L, ITEM_AUTO_HP_RECOVERY_X, ITEM_AUTO_SP_RECOVERY_S, ITEM_AUTO_SP_RECOVERY_M, ITEM_AUTO_SP_RECOVERY_L, ITEM_AUTO_SP_RECOVERY_X, REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS, REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S, REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS, REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S, FUCKING_BRAZIL_ITEM_AUTO_SP_RECOVERY_S, FUCKING_BRAZIL_ITEM_AUTO_HP_RECOVERY_S
	};

	for (int i = 0; i < INVENTORY_MAX_NUM; i++)
	{
		for (int id = 0; id < _countof(blocked_items); id++)
		{
			if (ch->GetInventoryItem(i) > 0 && ch->GetInventoryItem(i)->GetSocket(0) == 1 && ch->GetInventoryItem(i)->GetVnum() == blocked_items[id])
			{
				ch->GetInventoryItem(i)->SetSocket(0, false);
				ch->GetInventoryItem(i)->Lock(false);
			}
		}
	}
}

void CBattleRoyale::HPSettingsBattleRoyale(LPCHARACTER ch)
{
	if (ch->GetHP() >= (START_HP_ON_MAP / 2))
	{
		ch->PointChange(POINT_HP_RECOVERY, PLUS_HP);
		ch->EffectPacket(SE_HPUP_RED);
	}
	if (ch->GetHP() < (START_HP_ON_MAP / 2))
	{
		switch (number(1, 2))
		{
			case 1:
				{
					ch->PointChange(POINT_HP_RECOVERY, PLUS_HP);
					ch->EffectPacket(SE_HPUP_RED);
				}
				break;
			case 2:
				{
					ch->PointChange(POINT_HP_RECOVERY, (ch->GetMaxHP() - ch->GetHP()) / number(2, 4));
					ch->EffectPacket(SE_HPUP_RED);
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("HP BOOST"));
				}
				break;
		}
	}
	ch->UpdatePacket();
}

DWORD CBattleRoyale::Get_Battle_Royale_HTPoint(LPCHARACTER ch)
{
	std::auto_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT set_ht_battleroyal FROM player.player WHERE id = %u", ch->GetPlayerID()));
	if (pMsg->Get()->uiNumRows == 0)
		return 0;

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	DWORD BATTLE_ROYALE_HT2 = 0;
	str_to_number(BATTLE_ROYALE_HT2, row[0]);
	return BATTLE_ROYALE_HT2;
}

void CBattleRoyale::Set_Battle_Royale_HTPoint(DWORD BATTLE_ROYALE_HT, LPCHARACTER ch)
{
	DBManager::instance().Query("UPDATE player.player SET set_ht_battleroyal = %d WHERE id = %u", BATTLE_ROYALE_HT, ch->GetPlayerID());
}

void CBattleRoyale::HPSetBattleroyale(LPCHARACTER ch)
{
	if (ch->GetMapIndex() == BATTLE_ROYAL_MAP_INDEX)
	{
		Set_Battle_Royale_HTPoint(ch->GetRealPoint(POINT_HT), ch);
		ch->SetRealPoint(POINT_HT, 0);
		ch->SetRandomHP(START_HP_ON_MAP - JobInitialPoints[ch->GetJob()].max_hp);
		ch->ComputePoints();
		ch->PointsPacket();
	}
}

void CBattleRoyale::HPResetBattleroyale(LPCHARACTER ch)
{
	ch->SetRealPoint(POINT_HT, Get_Battle_Royale_HTPoint(ch));

#if defined(MAX_RANDOM_HP)
	ch->SetRandomHP((ch->GetLevel() - 1) * JobInitialPoints[ch->GetJob()].hp_per_lv_end);
#else
	ch->SetRandomHP((ch->GetLevel() - 1) * number(JobInitialPoints[ch->GetJob()].hp_per_lv_begin, JobInitialPoints[ch->GetJob()].hp_per_lv_end));
#endif
	ch->ComputePoints();
	ch->PointsPacket();
}

void CBattleRoyale::DeleteTakenTime(LPCHARACTER ch)
{
	DBManager::instance().DirectQuery("DELETE FROM player.item_award WHERE taken_time > 0 and item_id > 0 and why = 'BTR' and login = '%s' ", ch->GetDesc()->GetAccountTable().login);
}

void CBattleRoyale::AddToMaLLBattleRoyaleMap(LPCHARACTER ch)
{
	if (ch->GetQuestFlag("BattleRoyale.battleroyalemap") < 1)
	{
		if (BATTLE_ROYAL_GIFT_STATUS == 1)
		{
			if (ch->GetQuestFlag("BattleRoyale.battle_royale_gift") != 1)
			{
				DBManager::instance().Query("INSERT INTO player.item_award (login, vnum, count, given_time, why, mall) VALUES('%s', '%d', '%d', NOW(), 'BTR', '1') "
					, ch->GetDesc()->GetAccountTable().login, ITEM_VNUM, ITEM_COUNT);
			}
		}	
	}
}

void CBattleRoyale::ResetRanking()
{
	DBManager::instance().DirectQuery("UPDATE player.player SET battle_royale_rank = 0 WHERE battle_royale_rank > 0");

	std::auto_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT * FROM player.battle_royale ORDER BY kill_count DESC, date ASC LIMIT 3"));
	if (pMsg->Get()->uiNumRows == 3)
	{
		MYSQL_ROW row;
		int NameRank = 1;
		char szQuery[512 + 1];
		while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
		{
			sprintf(szQuery, "UPDATE player.player SET battle_royale_rank = '%d' WHERE name = '%s'", NameRank, row[1]);
			DBManager::instance().DirectQuery(szQuery);

			char BATTLE_ROYAL[256];
			snprintf(BATTLE_ROYAL, sizeof(BATTLE_ROYAL), LC_TEXT("The giving ranking medals succeful. %d : %s"), NameRank, row[1]);
			SendNotice(BATTLE_ROYAL);
			NameRank++;
		}
	}
	else {
		
		SendNotice(LC_TEXT("The giving ranking medals not was possible because not was exist 3 players on ranking."));
	}

	DBManager::instance().DirectQuery("TRUNCATE TABLE player.battle_royale");
}

void CBattleRoyale::UpdateKillPoint(DWORD ID, const char* NAME, BYTE EMPIRE, DWORD KILL)
{
	std::auto_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery("SELECT player_name, player_empire, kill_count FROM player.battle_royale%s WHERE player_id=%u;", get_table_postfix(), ID));
	if (pMsg->Get()->uiNumRows == 0)
	{
		DBManager::instance().DirectQuery("INSERT INTO battle_royale%s (player_id, player_name, player_empire, kill_count, date) VALUES(%u, '%s', %u, %u, NOW())", get_table_postfix(), ID, NAME, EMPIRE, KILL_PER_POINT);
	}
	else
	{
		const char* szPlayerNameR;
		DWORD dwPlayerEmpireR = 0;
		DWORD dwKillCountR = 0;
		MYSQL_ROW mRow;
		while (NULL != (mRow = mysql_fetch_row(pMsg->Get()->pSQLResult)))
		{
			int iCur = 0;
			szPlayerNameR = mRow[iCur++];
			str_to_number(dwPlayerEmpireR, mRow[iCur++]);
			str_to_number(dwKillCountR, mRow[iCur++]);
		}

		if ((dwKillCountR > 0))
		{
			DBManager::instance().DirectQuery("UPDATE battle_royale%s SET player_empire=%u, kill_count=%u, date=NOW() WHERE player_id=%u;", get_table_postfix(), EMPIRE, KILL_PER_POINT + dwKillCountR, ID);
		}
	}

}

void CBattleRoyale::LoadBattleRoyalConfig()
{
	char szFileName[256];
	char buf[256];
	char token_string[256];
	char value_string[256];
	snprintf(szFileName, sizeof(szFileName), "%s/BATTLE_ROYALE_CONFIG", LocaleService_GetBasePath().c_str());

	FILE* file = NULL;
	file = fopen(szFileName, "r");

	if (!file)
	{
		sys_err("Error %s", szFileName);
		return;
	}

	while (fgets(buf, 256, file))
	{
		parse_token(buf, token_string, value_string);

		TOKEN("BATTLE_ROYALE_STATUS")
		{
			str_to_number(BATTLE_ROYALE_STATUS, value_string);
			fprintf(stderr, "BATTLE_ROYALE_STATUS: %d\n", BATTLE_ROYALE_STATUS);
		}	
		TOKEN("BATTLE_ROYAL_GIFT_STATUS")
		{
			str_to_number(BATTLE_ROYAL_GIFT_STATUS, value_string);
			fprintf(stderr, "BATTLE_ROYAL_GIFT_STATUS: %d\n", BATTLE_ROYAL_GIFT_STATUS);
		}
		TOKEN("ITEM_VNUM")
		{
			str_to_number(ITEM_VNUM, value_string);
			fprintf(stderr, "ITEM_VNUM: %d\n", ITEM_VNUM);
		}
		TOKEN("ITEM_COUNT")
		{
			str_to_number(ITEM_COUNT, value_string);
			fprintf(stderr, "ITEM_COUNT: %d\n", ITEM_COUNT);
		}
		TOKEN("HOUR_1")
		{
			str_to_number(HOUR_1, value_string);
			fprintf(stderr, "HOUR_1: %d\n", HOUR_1);
		}
		TOKEN("HOUR_2")
		{
			str_to_number(HOUR_2, value_string);
			fprintf(stderr, "HOUR_2: %d\n", HOUR_2);
		}
		TOKEN("HOUR_3")
		{
			str_to_number(HOUR_3, value_string);
			fprintf(stderr, "HOUR_3: %d\n", HOUR_3);
		}
		TOKEN("MINUTE_1")
		{
			str_to_number(MINUTE_1, value_string);
			fprintf(stderr, "MINUTE_1: %d\n", MINUTE_1);
		}
		TOKEN("MINUTE_2")
		{
			str_to_number(MINUTE_2, value_string);
			fprintf(stderr, "MINUTE_2: %d\n", MINUTE_2);
		}
		TOKEN("MINUTE_3")
		{
			str_to_number(MINUTE_3, value_string);
			fprintf(stderr, "MINUTE_3: %d\n", MINUTE_3);
		}
		TOKEN("SECOND_1")
		{
			str_to_number(SECOND_1, value_string);
			fprintf(stderr, "SECOND_1: %d\n", SECOND_1);
		}
		TOKEN("SECOND_2")
		{
			str_to_number(SECOND_2, value_string);
			fprintf(stderr, "SECOND_2: %d\n", SECOND_2);
		}
		TOKEN("SECOND_3")
		{
			str_to_number(SECOND_3, value_string);
			fprintf(stderr, "SECOND_3: %d\n", SECOND_3);
		}
		TOKEN("RESET_RANK_HOUR")
		{
			str_to_number(RESET_RANK_HOUR, value_string);
			fprintf(stderr, "RESET_RANK_HOUR: %d\n", RESET_RANK_HOUR);
		}
		TOKEN("RESET_RANK_MINUTE")
		{
			str_to_number(RESET_RANK_MINUTE, value_string);
			fprintf(stderr, "RESET_RANK_MINUTE: %d\n", RESET_RANK_MINUTE);
		}
		TOKEN("RESET_RANK_SECOND")
		{
			str_to_number(RESET_RANK_SECOND, value_string);
			fprintf(stderr, "RESET_RANK_SECOND: %d\n", RESET_RANK_SECOND);
		}
		TOKEN("RANK_DAY")
		{
			str_to_number(RANK_DAY, value_string);
			fprintf(stderr, "RANK_DAY: %d\n", RANK_DAY);
		}
		TOKEN("DEFAULT_DAMAGE")
		{
			str_to_number(DEFAULT_DAMAGE, value_string);
			fprintf(stderr, "DEFAULT_DAMAGE: %d\n", DEFAULT_DAMAGE);
		}	
		TOKEN("COMMON_DAMAGE")
		{
			str_to_number(COMMON_DAMAGE, value_string);
			fprintf(stderr, "COMMON_DAMAGE: %d\n", COMMON_DAMAGE);
		}
		TOKEN("RARE_DAMAGE")
		{
			str_to_number(RARE_DAMAGE, value_string);
			fprintf(stderr, "RARE_DAMAGE: %d\n", RARE_DAMAGE);
		}
		TOKEN("EPIC_DAMAGE")
		{
			str_to_number(EPIC_DAMAGE, value_string);
			fprintf(stderr, "EPIC_DAMAGE: %d\n", EPIC_DAMAGE);
		}
		TOKEN("COMMON_ARMOR")
		{
			str_to_number(COMMON_ARMOR, value_string);
			fprintf(stderr, "COMMON_ARMOR: %d\n", COMMON_ARMOR);
		}
		TOKEN("RARE_ARMOR")
		{
			str_to_number(RARE_ARMOR, value_string);
			fprintf(stderr, "RARE_ARMOR: %d\n", RARE_ARMOR);
		}
		TOKEN("EPIC_ARMOR")
		{
			str_to_number(EPIC_ARMOR, value_string);
			fprintf(stderr, "EPIC_ARMOR: %d\n", EPIC_ARMOR);
		}
		TOKEN("ORIGO_X")
		{
			str_to_number(ORIGO_X, value_string);
			fprintf(stderr, "ORIGO_X: %d\n", ORIGO_X);
		}
		TOKEN("ORIGO_Y")
		{
			str_to_number(ORIGO_Y, value_string);
			fprintf(stderr, "ORIGO_Y: %d\n", ORIGO_Y);
		}
		TOKEN("ORIGO_X_2")
		{
			str_to_number(ORIGO_X_2, value_string);
			fprintf(stderr, "ORIGO_X_2: %d\n", ORIGO_X_2);
		}
		TOKEN("ORIGO_Y_2")
		{
			str_to_number(ORIGO_Y_2, value_string);
			fprintf(stderr, "ORIGO_Y_2: %d\n", ORIGO_Y_2);
		}
	}
	fclose(file);
}
#endif