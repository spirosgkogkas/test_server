#include "stdafx.h"
#include "char.h"
#include "desc_client.h"
#include "utils.h"
#include "packet.h"
#include "distance_shopping.h"

CDistanceShopping::CDistanceShopping()
{
}

bool CDistanceShopping::CanUseDistanceShopping(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
		return false;
	
	for (unsigned int i=0; i < sizeof(DISTANCE_SHOP_EXCLUDED_MAPS)/sizeof(*DISTANCE_SHOP_EXCLUDED_MAPS); i++)
		if (ch->GetMapIndex() == DISTANCE_SHOP_EXCLUDED_MAPS[i])
			return false;
	
	return true;
}

void CDistanceShopping::OpenDistanceShoppingDialog(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
		return;

	if (!CanUseDistanceShopping(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("distance_shopping_excluded_map"));
		return;
	}
	
	TPacketGCDistanceShoppingOpen DistanceShoppingOpen;
	DistanceShoppingOpen.bHeader = HEADER_GC_OPEN_DISTANCE_SHOPPING;
	ch->GetDesc()->Packet(&DistanceShoppingOpen, sizeof(TPacketGCDistanceShoppingOpen));	
}
