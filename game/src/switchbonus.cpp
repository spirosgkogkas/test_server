#include "stdafx.h"
#include "switchbonus.h"
#include "char.h"
#include "locale_service.h"
#include "packet.h"
#include "desc_client.h"
#include "item_manager.h"
#include "item.h"
#include "item_manager.h"
#include "locale_service.h"

CSwitchBonus::CSwitchBonus()
{
}

CSwitchBonus::~CSwitchBonus()
{
	
}

bool CSwitchBonus::CheckElementsSB(LPCHARACTER ch){
	if (ch->GetExchange() || ch->GetShop() || ch->GetMyShop() || ch->IsOpenSafebox() || ch->IsCubeOpen())
		return true;
	return false;
}

bool CSwitchBonus::CanMoveIntoSB(LPITEM item)
{
	if(!item){return false;}
	if(item->GetType()==ITEM_WEAPON && item->GetSubType() != WEAPON_ARROW){return true;}
	if(item->GetType()==ITEM_ARMOR){return true;}
	return false;
}

void CSwitchBonus::OpenSB(LPCHARACTER ch)
{
	if (ch->IsSBOpen()){
		return;
	}

	ch->SetSBOpen(true);
	SendSBPackets(ch,SWITCH_RECEIVE_OPEN);

	LPITEM item = ch->GetSwitchBotItem(0);
	if(item){
		SendSBItem(ch,item);
	}
}

void CSwitchBonus::CloseSB(LPCHARACTER ch)
{
	ch->SetSBOpen(false);
}

void CSwitchBonus::ClearSB(LPCHARACTER ch)
{
	SendSBPackets(ch,SWITCH_RECEIVE_CLEAR_INFO);
}

void CSwitchBonus::SendSBItem(LPCHARACTER ch,LPITEM item)
{
	if(!item)
		return;
	
	ClearSB(ch);
	int iAttributeSet = item->GetAttributeSetIndex();
	for (int b = 0; b < MAX_APPLY_NUM; ++b)	
	{
		const TItemAttrTable & r = g_map_itemAttr[b];
		if (r.bMaxLevelBySet[iAttributeSet]){
			SendSBPackets(ch,SWITCH_RECEIVE_INFO_BONUS_ITEM, b, r.lValues[0], r.lValues[1], r.lValues[2], r.lValues[3], r.lValues[4]);
		}
	}

	TItemTable const * pProto = item->GetProto();
	if(pProto && pProto->sAddonType)
	{
		SendSBPackets(ch,SWITCH_RECEIVE_INFO_EXTRA);
	}
	
}

void CSwitchBonus::ChangeSB(LPCHARACTER ch,const TPlayerItemAttribute* aAttr)
{
	LPITEM item = ch->GetSwitchBotItem(0);

	if(!item)
		return;

	if (!ch->IsSBOpen()){
		return;
	}

	if (!item->CheckBonusSwitchBot(aAttr))
	{
		ch->ChatPacket(CHAT_TYPE_INFO,"[SB]Change Bonus Succefull.");
		return;
	}

	int count_change = ch->CountSpecifyItem(VNUM_CHANGE_ITEM);
	int remove_total = 0;
	int max_changes = COUNT_CHANGE_ITEM;
	bool change_succefull = false;

	if (count_change > 0)
	{
		if(count_change < max_changes){
			remove_total = count_change;
		}else{
			remove_total = max_changes;
		}

		int result = item->ChangeAttributeSwtichBot(remove_total,aAttr,&change_succefull);
		item->UpdatePacket();
		item->Save();

		if(result > 0)
		{
			if(change_succefull)
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "SWITCHBOT_SUCCEFULL %d",result);
			}else{
				ch->ChatPacket(CHAT_TYPE_INFO,"[SB]Change Bonus Fail.");
			}

			ch->RemoveSpecifyItem(VNUM_CHANGE_ITEM,result);

		}else{

			ch->ChatPacket(CHAT_TYPE_INFO,"[SB]Change Bonus Fail.");
		}
	}else{
		ch->ChatPacket(CHAT_TYPE_INFO,"[SB]Not enough switch items.");
	}
}


void CSwitchBonus::SendSBPackets(LPCHARACTER ch,BYTE subheader, int id_bonus, int value_bonus_0, int value_bonus_1, int value_bonus_2, int value_bonus_3, int value_bonus_4)
{
	TPacketGCSwitchBotReceive pack;
	pack.bSubType = subheader;


	if(subheader == SWITCH_RECEIVE_INFO_BONUS_ITEM)
	{
		pack.id_bonus = id_bonus;
		pack.value_bonus_0 = value_bonus_0;
		pack.value_bonus_1 = value_bonus_1;
		pack.value_bonus_2 = value_bonus_2;
		pack.value_bonus_3 = value_bonus_3;
		pack.value_bonus_4 = value_bonus_4;

	}

	LPDESC d = ch->GetDesc();

	if (NULL == d)
	{
		sys_err ("User CSwitchBonus::SendSBPackets (%s)'s DESC is NULL POINT.", ch->GetName());
		return ;
	}

	d->Packet(&pack, sizeof(pack));
}
