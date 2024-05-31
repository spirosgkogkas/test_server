#include <boost/algorithm/string.hpp>
#include "stdafx.h"
#include "constants.h"
#include "item.h"
#include "item_manager.h"
#include "unique_item.h"
#include "packet.h"
#include "desc.h"
#include "char.h"
#include "config.h"
#include "StackAttr.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char_manager.h"

CStackAttr::CStackAttr()
{
}

CStackAttr::~CStackAttr()
{
	map_stack_attr.clear();
}

void CStackAttr::Initialize()
{
	if (g_bAuthServer)
		return;

	map_stack_attr.clear();

	// Initalize' File
	FILE 	*fp;
	char	one_line[256];
	int		value1, value2, value3, value4, value5, value6, value7, value8, value9, value10, value11, value12,value13,value14,value15,value16;
	const char	*delimit = " \t\r\n";
	char	*v, *token_string;
	char file_name[256+1];
	int pos = 0;
	snprintf(file_name, sizeof(file_name), "locale/hungary/stack_attr_config.txt");
	fp = fopen(file_name, "r");

	while (fgets(one_line, 256, fp))
	{
		value1 = value2 = value3 = value4 = value5 = value6 = value7 = value8 = value9 = value10= value11 = value12 = value13 = value14 = value15 = value16 = 0;

		if (one_line[0] == '#')
			continue;
		
		token_string = strtok(one_line, delimit);
		if (NULL == token_string)
			continue;

		if ((v = strtok(NULL, delimit)))
			str_to_number(value1, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value2, v);

		if ((v = strtok(NULL, delimit)))
			str_to_number(value3, v);
	
		if ((v = strtok(NULL, delimit)))
			str_to_number(value4, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value5, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value6, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value7, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value8, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value9, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value10, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value11, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value12, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value13, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value14, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value15, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value16, v);

		TOKEN("DO_ITEM")
		{
			SInfoItem cache;
			cache.ItemVnum = value1;
			cache.TypeBonus[0] = value2;
			cache.GetBonusVnum[0] = value3;
			cache.BonusType[0] = value4;
			cache.BonusRewardProgress[0] = value5;
			cache.BonusMaxStack[0] = value6;
			cache.TypeBonus[1] = value7;
			cache.GetBonusVnum[1] = value8;
			cache.BonusType[1] = value9;
			cache.BonusRewardProgress[1] = value10;
			cache.BonusMaxStack[1] = value11;
			cache.TypeBonus[2] = value12;
			cache.GetBonusVnum[2] = value13;
			cache.BonusType[2] = value14;
			cache.BonusRewardProgress[2] = value15;
			cache.BonusMaxStack[2] = value16;

			map_stack_attr[value1] = cache;
		}

	}
	fclose(fp);
}

void CStackAttr::AddProgressToAttr(LPCHARACTER ch, int iType, LPITEM iWear, int item, int iValue)
{
	if (iWear == NULL || ch == NULL)
		return;

	if (iWear)
	{
		if (map_stack_attr.find(iWear->GetVnum()) == map_stack_attr.end())
			return;
		
		if (iWear->GetVnum() == map_stack_attr[iWear->GetVnum()].ItemVnum)
		{
			if (map_stack_attr[iWear->GetVnum()].GetBonusVnum[0] == item || map_stack_attr[iWear->GetVnum()].GetBonusVnum[0] == 0)
				DoStackAttr(ch, iWear, iType, 1, iValue);
			
			if (map_stack_attr[iWear->GetVnum()].GetBonusVnum[1] == item || map_stack_attr[iWear->GetVnum()].GetBonusVnum[1] == 0)
				DoStackAttr(ch, iWear, iType, 2, iValue);
			
			if (map_stack_attr[iWear->GetVnum()].GetBonusVnum[2] == item || map_stack_attr[iWear->GetVnum()].GetBonusVnum[2] == 0)
				DoStackAttr(ch, iWear, iType, 3, iValue);
		}
	}
}

void CStackAttr::DoStackAttr(LPCHARACTER ch, LPITEM test, int iType, int iBonus, int iValue)
{
	if (test == NULL || ch == NULL)
		return;
	
	if (map_stack_attr.find(test->GetVnum()) == map_stack_attr.end())
		return;

	if (map_stack_attr[test->GetVnum()].TypeBonus[0] == iType && iBonus == 1 && test->GetAttributeValue(1) < map_stack_attr[test->GetVnum()].BonusMaxStack[0])
	{	
		test->SetForceAttribute(1, 0, test->GetAttributeValue(1) + iValue);
	
		int iWantVal = test->GetAttributeValue(0); 

		test->SetForceAttribute(0, map_stack_attr[test->GetVnum()].BonusType[0], test->GetAttributeValue(1) / map_stack_attr[test->GetVnum()].BonusRewardProgress[0]);

		int iNewVal = test->GetAttributeValue(0); 
		if (iWantVal != iNewVal)
			ch->ApplyPoint(map_stack_attr[test->GetVnum()].BonusType[0], 1);
	}
	
	else if (map_stack_attr[test->GetVnum()].TypeBonus[1] == iType && iBonus == 2 && test->GetAttributeValue(3) < map_stack_attr[test->GetVnum()].BonusMaxStack[1])
	{
		test->SetForceAttribute(3, 0, test->GetAttributeValue(3) + iValue);
		
		int iWantVal = test->GetAttributeValue(2); 
				
		test->SetForceAttribute(2, map_stack_attr[test->GetVnum()].BonusType[1], test->GetAttributeValue(3) / map_stack_attr[test->GetVnum()].BonusRewardProgress[1]);
	
		int iNewVal = test->GetAttributeValue(2); 
		if (iWantVal != iNewVal)
			ch->ApplyPoint(map_stack_attr[test->GetVnum()].BonusType[1], 1);
	}
	
	else if (map_stack_attr[test->GetVnum()].TypeBonus[2] == iType && iBonus == 3 && test->GetAttributeValue(5) < map_stack_attr[test->GetVnum()].BonusMaxStack[2])
	{
		test->SetForceAttribute(5, 0, test->GetAttributeValue(5) + iValue);
		
		int iWantVal = test->GetAttributeValue(4); 

		test->SetForceAttribute(4, map_stack_attr[test->GetVnum()].BonusType[2], test->GetAttributeValue(5) / map_stack_attr[test->GetVnum()].BonusRewardProgress[2]);
		
		int iNewVal = test->GetAttributeValue(4); 
		if (iWantVal != iNewVal)
			ch->ApplyPoint(map_stack_attr[test->GetVnum()].BonusType[2], 1);
	}
}

void CStackAttr::SendInfoItems(LPCHARACTER ch)
{
	std::map<int, SInfoItem>::iterator it = map_stack_attr.begin();
	
	while (it != map_stack_attr.end())
	{
		TPacketGCStackAttrItem packet;
		packet.header = HEADER_GC_STACK_ATTR_ITEM;
		packet.ItemVnum = it->second.ItemVnum;
		
		for (int i = 0; i < 3; ++i)
		{
			packet.BonusType[i] = it->second.BonusType[i];
			packet.BonusRewardProgress[i] = it->second.BonusRewardProgress[i];
			packet.BonusMaxStack[i] = it->second.BonusMaxStack[i];
			packet.TypeBonus[i] = it->second.TypeBonus[i];
			packet.GetBonusVnum[i] = it->second.GetBonusVnum[i];
		}

		LPDESC d = ch->GetDesc();

		if (!d)
			continue;
	
		d->LargePacket(&packet, sizeof(TPacketGCStackAttrItem));

		it++;
	}
}
