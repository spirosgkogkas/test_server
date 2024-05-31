#include "stdafx.h"
#ifdef __AUCTION_HOUSE__
#include "char.h"
#include "item.h"
#include "auction_house.h"
#include "db.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "item_manager.h"

CAuctionHouse::CAuctionHouse(){}
CAuctionHouse::~CAuctionHouse(){}

void CAuctionHouse::GetItems(LPCHARACTER ch, int cat, int next)
{
	if (!ch){return;}

	int limit = 0;
	limit = next*500;
	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),"SELECT * FROM player.auction_items WHERE category='%d' AND `show`='1' ORDER BY item_name ASC, price ASC LIMIT %d,%d",cat,limit,limit+500);
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiNumRows > 0){

		int id,owner_id,vnum,count,transmutation,category;
		long long price;  long Sockets[AUCTION_MAX_SOCKET];
		BYTE aAttrb[AUCTION_MAX_ATTRIBUTE]; short aAttrs[AUCTION_MAX_ATTRIBUTE];
		while (MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult))
		{
			int cur = 0;
	
			str_to_number(id, row[cur++]);
			str_to_number(owner_id, row[cur++]);
			str_to_number(vnum, row[cur++]);
			cur++;
			str_to_number(count, row[cur++]);
			str_to_number(category, row[cur++]);
			price = atoll(row[cur++]);
			str_to_number(transmutation, row[cur++]);

			for (int j = 0; j < AUCTION_MAX_SOCKET; j++)
			{
				str_to_number(Sockets[j], row[cur++]);
			}
			
			for (int j = 0; j < AUCTION_MAX_ATTRIBUTE; j++)
			{
				str_to_number(aAttrb[j], row[cur++]);
				str_to_number(aAttrs[j], row[cur++]);
			}
			ch->ChatPacket(CHAT_TYPE_COMMAND, "AUCTIONHOUSE GET_ITEMS#%d#%d#%d#%d#%d#%lld#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d",
			id,owner_id,vnum,count,category,price,transmutation,Sockets[0],Sockets[1],Sockets[2],Sockets[3],Sockets[4],Sockets[5],
			aAttrb[0],aAttrs[0],aAttrb[1],aAttrs[1],aAttrb[2],aAttrs[2],aAttrb[3],aAttrs[3],aAttrb[4],aAttrs[4],aAttrb[5],aAttrs[5],aAttrb[6],aAttrs[6]);
		}
	}
	ch->ChatPacket(CHAT_TYPE_COMMAND, "AUCTIONHOUSE REFRESH#1");
}

void CAuctionHouse::BuyItem(LPCHARACTER ch, int id, int amount, int posarray)
{
	if (!ch){return;}
	
	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),"SELECT * FROM player.auction_items WHERE id='%d' LIMIT 1",id);
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));

	if (pMsg->Get()->uiNumRows > 0){
		int id,owner_id,vnum,count,transmutation,category;
		long long price;  long Sockets[AUCTION_MAX_SOCKET];
		BYTE aAttrb[AUCTION_MAX_ATTRIBUTE]; short aAttrs[AUCTION_MAX_ATTRIBUTE];
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		{
			int cur = 0;

			str_to_number(id, row[cur++]);
			str_to_number(owner_id, row[cur++]);
			str_to_number(vnum, row[cur++]);
			cur++;
			str_to_number(count, row[cur++]);
			str_to_number(category, row[cur++]);
			price = atoll(row[cur++]);
			str_to_number(transmutation, row[cur++]);
			
			for (int j = 0; j < AUCTION_MAX_SOCKET; j++)
			{
				str_to_number(Sockets[j], row[cur++]);
			}
			
			for (int j = 0; j < AUCTION_MAX_ATTRIBUTE; j++)
			{
				str_to_number(aAttrb[j], row[cur++]);
				str_to_number(aAttrs[j], row[cur++]);
			}

			if (owner_id == ch->GetAID()){
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT( "<Auction House> You cannot buy an item yourself."));
				return;
			}
			
			if (ch->GetGold() >= (amount*price)){
				if (amount > count){
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT( "<Auction House> Someone has already bought the item."));
				}
				else{
					LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(vnum, amount);
					if (pkNewItem)
					{
#ifdef ENABLE_SPECIAL_INVENTORY
						int iEmptyPos = ch->GetEmptyInventory(pkNewItem);
#else
						int iEmptyPos = ch->GetEmptyInventory(pkNewItem->GetSize());
#endif
						if (iEmptyPos == -1)
						{
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("INVENTORY_FULL_ERROR"));
							return;
						}

						for (int i = 0; i < AUCTION_MIN_SOCKET; i++)
						{
							pkNewItem->SetSocket(i, Sockets[i]);
						}

						for (int i = 0; i < AUCTION_MIN_ATTRIBUTE; i++)
						{
							pkNewItem->SetForceAttribute(i, aAttrb[i], aAttrs[i]);
						}

#ifdef __CHANGELOOK_SYSTEM__
						if (transmutation > 0){pkNewItem->SetTransmutation(transmutation);}
#endif
						ch->AutoGiveItem(pkNewItem);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %s x%d"), pkNewItem->GetName(), amount);
						
						long long precio = price*amount;
						ch->PointChange(POINT_GOLD, -precio, true);
						
						char szQuery[1024];
						snprintf(szQuery, sizeof(szQuery),"UPDATE account.account SET bank=bank+%lld WHERE id = '%d'", precio, owner_id);
						DBManager::instance().DirectQuery(szQuery);

						char szQuery2[1024];
						
						if ((count - amount) > 0){
							snprintf(szQuery2, sizeof(szQuery2),"UPDATE player.auction_items SET count=count-%d WHERE id = %d", amount, id);
							DBManager::instance().DirectQuery(szQuery2);
							ch->ChatPacket(CHAT_TYPE_COMMAND, "AUCTIONHOUSE DELETE#1#%d#%d#%d",posarray,category, amount);
						}else{
							snprintf(szQuery2, sizeof(szQuery2),"DELETE FROM player.auction_items WHERE id='%d'", id);
							DBManager::instance().DirectQuery(szQuery2);
							ch->ChatPacket(CHAT_TYPE_COMMAND, "AUCTIONHOUSE DELETE#1#%d#%d#0",posarray,category);
						}

						DBManager::instance().DirectQuery("INSERT INTO log.auction_items (`who`,`toaccount`,`iditem`,`vnum`,`name`,`count`,`price`,`ip`) VALUES ('%d','%d','%d','%d','%s','%d','%lld','%s');", ch->GetPlayerID(), owner_id, pkNewItem->GetID(), vnum, pkNewItem->GetName(), amount, price, ch->GetDesc()->GetHostName());
						
						char szQuery1[1024];
						snprintf(szQuery1, sizeof(szQuery1),"INSERT INTO log.auction_items_selled (`owner_id`,`vnum`,`count`,`price`,`transmutation`,"
						"`socket0`,`socket1`,`socket2`,`socket3`,`socket4`,`socket5`,`attrtype0`,`attrvalue0`,`attrtype1`,`attrvalue1`,`attrtype2`,`attrvalue2`,"
						"`attrtype3`,`attrvalue3`,`attrtype4`,`attrvalue4`,`attrtype5`,`attrvalue5`,`attrtype6`,`attrvalue6`,`buyer`,`datetime`)"
						"VALUES ('%d','%d','%d','%lld','%d','%ld','%ld','%ld','%ld','%ld','%ld',"
						"'%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d', NOW());", owner_id, vnum, amount, price, transmutation, 
						Sockets[0], Sockets[1], Sockets[2], Sockets[3], Sockets[4], Sockets[5], aAttrb[0], aAttrs[0],aAttrb[1],aAttrs[1],aAttrb[2],aAttrs[2],
						aAttrb[3],aAttrs[3],aAttrb[4],aAttrs[4],aAttrb[5],aAttrs[5],aAttrb[6],aAttrs[6], ch->GetPlayerID());

						DBManager::Instance().DirectQuery(szQuery1);

					}else{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Auction House> Unexpected error, contact administration."));
					}
				}
			}else{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Auction House> You don't have enough Yang."));
			}
		}
	}else{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT( "<Auction House> Someone has already bought the item."));
	}
}

void CAuctionHouse::SellItem(LPCHARACTER ch, int pos, int amount, long long price)
{
	if (!ch){return;}

	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),"SELECT count(id) FROM player.auction_items WHERE owner_id='%d' LIMIT 1",ch->GetAID());
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	
	int	items = 0;
	str_to_number(items, row[0]);

	int minimal_price = 1000;
	if (price < minimal_price){
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Auction House> The minimum price of an object has to be %d of Yang."), minimal_price);
		return;		
	}
	int max_item_in_sell = 50;
	if (items >= max_item_in_sell){
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Auction House> You have a limit of %d items."), max_item_in_sell);
		return;
	}

	LPITEM item = ch->GetItem(TItemPos(INVENTORY, pos));
	if (item)
	{
		if(IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE) || IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MYSHOP) || item->IsRealTimeItem()){
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Auction House> You cannot sell this item."));
			return;
		}
		
		if (amount > item->GetCount()){
			return;
		}
		int transmutation = 0;
#ifdef __CHANGELOOK_SYSTEM__
		transmutation = item->GetTransmutation();
#endif
		long Sockets[AUCTION_MAX_SOCKET];
		BYTE aAttrb[AUCTION_MAX_ATTRIBUTE]; short aAttrs[AUCTION_MAX_ATTRIBUTE];
		for (int i = 0; i < AUCTION_MAX_SOCKET; i++)
		{
			Sockets[i] = 0;
		}
		for (int i = 0; i < AUCTION_MAX_ATTRIBUTE; i++)
		{
			const TPlayerItemAttribute& attr = item->GetAttribute(i);
			aAttrb[i] = 0;
			aAttrs[i] = 0;
		}
		for (int i = 0; i < AUCTION_MIN_SOCKET; i++)
		{
			Sockets[i] = item->GetSocket(i);
		}
		for (int i = 0; i < AUCTION_MIN_ATTRIBUTE; i++)
		{
			const TPlayerItemAttribute& attr = item->GetAttribute(i);
			aAttrb[i] = attr.bType;
			aAttrs[i] = attr.sValue;
		}

		int category = 1; //default
		
		//Example
		// if(IS_SET(item->GetFlag(), ITEM_FLAG_SAFEBOX_MATERIAL)){
			// category = 1;
			// if (item->GetVnum() >= 95136 && item->GetVnum() <= 95139){category = 2;}
			// if (item->GetVnum() >= 95473 && item->GetVnum() <= 95496){category = 2;}
			// if (item->GetVnum() >= 95028 && item->GetVnum() <= 95031){category = 2;}
			// if (item->GetVnum() >= 95150 && item->GetVnum() <= 95153){category = 2;}
			// if (item->GetVnum() >= 95609 && item->GetVnum() <= 95613){category = 2;}
			// if (item->GetVnum() == 95235 || item->GetVnum() == 95346 || item->GetVnum() == 95354 || item->GetVnum() == 95363){category = 2;}
			// if (item->GetVnum() >= 95524 && item->GetVnum() <= 95527){category = 2;}
			// if (item->GetVnum() == 95358 || item->GetVnum() == 95361 || item->GetVnum() == 95362 || item->GetVnum() == 95382){category = 2;}
			// if (item->GetVnum() >= 95670 && item->GetVnum() <= 95676){category = 2;}
			
			// if (item->GetVnum() == 95310 || item->GetVnum() == 95316 || item->GetVnum() == 95319 || item->GetVnum() == 95324
				// || item->GetVnum() == 95325 || item->GetVnum() == 95326 || item->GetVnum() == 95328 || item->GetVnum() == 95336 || item->GetVnum() == 95356){category = 2;}
			
			// if (item->GetVnum() == 95251 || item->GetVnum() == 95253 || item->GetVnum() == 95268 || item->GetVnum() == 95270 || item->GetVnum() == 95272 
				// || item->GetVnum() == 95286 || item->GetVnum() == 95288 || item->GetVnum() == 95289 || item->GetVnum() == 95293){category = 2;}
				
			// if (item->GetVnum() >= 95518 && item->GetVnum() <= 95522){category = 2;}
			
			// if (item->GetVnum() == 95399 || item->GetVnum() == 95400 || item->GetVnum() == 95227){category = 3;}
			// if (item->GetVnum() == 95300 || item->GetVnum() == 95301 || item->GetVnum() == 95303){category = 3;}
			// if (item->GetVnum() == 95278 || item->GetVnum() == 95279 || item->GetVnum() == 95314){category = 3;}
			// if (item->GetVnum() == 95298 || item->GetVnum() == 95299 || item->GetVnum() == 95308){category = 3;}
			// if (item->GetVnum() == 95311 || item->GetVnum() == 95312 || item->GetVnum() == 95297){category = 3;}
			// if (item->GetVnum() == 95365 || item->GetVnum() == 95366 || item->GetVnum() == 95355){category = 3;}
			// if (item->GetVnum() == 95359 || item->GetVnum() == 95360 || item->GetVnum() == 95345){category = 3;}
			// if (item->GetVnum() == 95423 || item->GetVnum() == 95425 || item->GetVnum() == 95426){category = 3;}
			// if (item->GetVnum() == 95276 || item->GetVnum() == 95277 || item->GetVnum() == 95226){category = 3;}
			// if (item->GetVnum() == 95339 || item->GetVnum() == 95340 || item->GetVnum() == 95320){category = 3;}
			// if (item->GetVnum() >= 30220 && item->GetVnum() <= 30229){category = 3;}
			// if (item->GetVnum() >= 30220 && item->GetVnum() <= 30229){category = 3;}
			// if (item->GetVnum() >= 95690 && item->GetVnum() <= 95699){category = 3;}
			// if (item->GetVnum() >= 97564 && item->GetVnum() <= 97572){category = 3;}
			
			//PVP
			// if (item->GetVnum() == 97628 || item->GetVnum() == 97630 || item->GetVnum() == 97648 || item->GetVnum() == 97657
			 // || item->GetVnum() == 97679 || item->GetVnum() == 97682 || item->GetVnum() == 95564 || item->GetVnum() == 95569
			 // || item->GetVnum() == 97676 || item->GetVnum() == 97677){category = 4;}
			 
			// if (item->GetVnum() == 90027 || item->GetVnum() == 90028 || item->GetVnum() == 51001 || item->GetVnum() == 51002
			 // || item->GetVnum() == 51004){category = 1;}
		// }
		
		if (item->GetType() == ITEM_WEAPON) {category = 1;}									//weapons
//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-/
		if (item->GetType() == ITEM_ARMOR)
		{
			if (item->GetSubType() == ARMOR_BODY)											//armors
			{
				category = 2;
			}
			else if (item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD
				/*item->GetVnum() >= 18000 && item->GetVnum() <= 18089*/
				)																			//helmet+shield
			{
				category = 3;
			}
			else if (item->GetVnum() >= 18000 && item->GetVnum() <= 18089)											//armors
			{
				category = 1;
			}
			// else 
			// {
				// category = 13;
			// }
		}
//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-/
		if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_WEAPON){category = 4;}
		if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_BODY){category = 5;}
		if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_HAIR){category = 6;}
//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-/
		if (item->GetVnum() >= 50300 && item->GetVnum() <= 50511 ||/*normal books*/
			item->GetVnum() >= 58001 && item->GetVnum() <= 58008/*pet books*/
			
			)
		{
			category = 7;															//books
		}
//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-/
		if (item->GetVnum() >= 53001 && item->GetVnum() <= 53200)
		{
			category = 8;															//pets
		}
//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-/
		if (item->GetVnum() == 71244)
		{
			category = 9;															//mounts
		}
//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-/
		if (item->GetVnum() >= 28030 && item->GetVnum() <= 28543){category = 10;}	//stones
//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-/
		if (item->GetVnum() >= 85001 && item->GetVnum() <= 86500){category = 11;}	//sashes
//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-/
		if (item->GetVnum() >= 20201 && item->GetVnum() <= 20214 || /*infinite potions*/
			item->GetVnum() >= 72723 && item->GetVnum() <= 72730  /*auto potions*/

			)
		{
			category = 12;															//poti/harmat/si
		}
//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-/
		if (item->GetVnum() == 79150){category = 13;}								//címek_title
//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-/
		if (item->GetVnum() >= 71084 && item->GetVnum() <= 71085 ||
			item->GetVnum() >= 70065 && item->GetVnum() <= 70066 ||
			item->GetVnum() == 71175 || item->GetVnum() >= 71178 && item->GetVnum() <= 71179 ||
			item->GetVnum() == 80039
			//item->GetVnum() >= 71084 && item->GetVnum() <= 71085
			)
		{
			category = 14;															//forgato/hozzaado
		}
//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-/
		if (item->GetVnum() >= 50070 && item->GetVnum() <= 50082)
		{
			category = 15;															//chests
		}
//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-/
		if (item->GetVnum() >= 30000 && item->GetVnum() <= 31337)
		{
			category = 16;															//uppitems
		}
//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-/
		if (item->GetVnum() >= 9600 && item->GetVnum() <= 10950 || 
			item->GetVnum() >= 55040 && item->GetVnum() <= 55055
			)
		{
			category = 17;															//talismans
		}
//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-//-/
		char szQuery1[1024];
		snprintf(szQuery1, sizeof(szQuery1),"INSERT INTO player.auction_items (`owner_id`,`vnum`,`item_name`,`count`,`category`,`price`,`transmutation`,"
		"`socket0`,`socket1`,`socket2`,`socket3`,`socket4`,`socket5`,`attrtype0`,`attrvalue0`,`attrtype1`,`attrvalue1`,`attrtype2`,`attrvalue2`,"
		"`attrtype3`,`attrvalue3`,`attrtype4`,`attrvalue4`,`attrtype5`,`attrvalue5`,`attrtype6`,`attrvalue6`,`date`)"
		"VALUES ('%d','%d','%s','%d','%d','%lld','%d','%ld','%ld','%ld','%ld','%ld','%ld',"
		"'%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d',NOW());", ch->GetAID(), item->GetVnum(), item->GetName(), amount, category, price, transmutation, 
		Sockets[0], Sockets[1], Sockets[2], Sockets[3], Sockets[4], Sockets[5], aAttrb[0], aAttrs[0],aAttrb[1],aAttrs[1],aAttrb[2],aAttrs[2],
		aAttrb[3],aAttrs[3],aAttrb[4],aAttrs[4],aAttrb[5],aAttrs[5],aAttrb[6],aAttrs[6]);

		std::unique_ptr<SQLMsg> pmsg (DBManager::Instance().DirectQuery(szQuery1));

		if (pmsg->Get()->uiInsertID != 0){
			if (amount == item->GetCount()){
				ITEM_MANAGER::Instance().RemoveItem(item, "AUCTIONHOUSE");
			}else if (amount < item->GetCount()){
				item->SetCount(item->GetCount() - amount);
			}
		}else{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Auction House> Unexpected error, contact administration."));
		}
	}	
}

void CAuctionHouse::GetItemSelled(LPCHARACTER ch)
{
	if (!ch){return;}

	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),"SELECT * FROM log.auction_items_selled WHERE owner_id='%d' ORDER BY id desc LIMIT 500",ch->GetAID());
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiNumRows > 0){
		int vnum,count,transmutation;
		long long price;  long Sockets[AUCTION_MAX_SOCKET];
		BYTE aAttrb[AUCTION_MAX_ATTRIBUTE]; short aAttrs[AUCTION_MAX_ATTRIBUTE];
		while (MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult))
		{
			int cur = 2;

			str_to_number(vnum, row[cur++]);
			str_to_number(count, row[cur++]);
			price = atoll(row[cur++]);
			str_to_number(transmutation, row[cur++]);

			for (int j = 0; j < AUCTION_MAX_SOCKET; j++)
			{
				str_to_number(Sockets[j], row[cur++]);
			}
			
			for (int j = 0; j < AUCTION_MAX_ATTRIBUTE; j++)
			{
				str_to_number(aAttrb[j], row[cur++]);
				str_to_number(aAttrs[j], row[cur++]);
			}
			
			char date[11]; char time[8]; char aChar[19];
			strcpy(aChar, row[cur++]);
			for (int i = 0; i < 9; i++) 
				date[i] = aChar[i]; 
			for (int i = 0; i < 7; i++) 
				time[i] = aChar[i+10]; 
		
			ch->ChatPacket(CHAT_TYPE_COMMAND, "AUCTIONHOUSE ITEMS_SELLED#%d#%d#%lld#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%s#%s",
			vnum,count,price,transmutation,Sockets[0],Sockets[1],Sockets[2],Sockets[3],Sockets[4],Sockets[5],
			aAttrb[0],aAttrs[0],aAttrb[1],aAttrs[1],aAttrb[2],aAttrs[2],aAttrb[3],aAttrs[3],aAttrb[4],aAttrs[4],aAttrb[5],aAttrs[5],aAttrb[6],aAttrs[6],date,time);
		}
	}
	ch->ChatPacket(CHAT_TYPE_COMMAND, "AUCTIONHOUSE REFRESH#3");	
}

void CAuctionHouse::CheckItemSell(LPCHARACTER ch)
{
	if (!ch){return;}

	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),"SELECT * FROM player.auction_items WHERE owner_id='%d' AND `show` = 0 LIMIT 1",ch->GetAID());
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiNumRows > 0){
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Auction House> You have items that have passed the sales time limit."));
	}	
}

void CAuctionHouse::GetItemSell(LPCHARACTER ch)
{
	if (!ch){return;}

	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),"SELECT * FROM player.auction_items WHERE owner_id='%d' LIMIT 100",ch->GetAID());
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiNumRows > 0){
		int id,vnum,count,transmutation,show;
		long long price;  long Sockets[AUCTION_MAX_SOCKET];
		BYTE aAttrb[AUCTION_MAX_ATTRIBUTE]; short aAttrs[AUCTION_MAX_ATTRIBUTE];
		while (MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult))
		{
			int cur = 0;

			str_to_number(id, row[cur++]);
			cur++; //owner_id
			str_to_number(vnum, row[cur++]);
			cur++; //item_name
			str_to_number(count, row[cur++]);
			cur++; //cat
			price = atoll(row[cur++]);
			str_to_number(transmutation, row[cur++]);

			for (int j = 0; j < AUCTION_MAX_SOCKET; j++)
			{
				str_to_number(Sockets[j], row[cur++]);
			}
			
			for (int j = 0; j < AUCTION_MAX_ATTRIBUTE; j++)
			{
				str_to_number(aAttrb[j], row[cur++]);
				str_to_number(aAttrs[j], row[cur++]);
			}
			cur++; //date
			str_to_number(show, row[cur++]);
			
			ch->ChatPacket(CHAT_TYPE_COMMAND, "AUCTIONHOUSE ITEMS_SELL#%d#%d#%d#%lld#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d",
			id,vnum,count,price,transmutation,Sockets[0],Sockets[1],Sockets[2],Sockets[3],Sockets[4],Sockets[5],
			aAttrb[0],aAttrs[0],aAttrb[1],aAttrs[1],aAttrb[2],aAttrs[2],aAttrb[3],aAttrs[3],aAttrb[4],aAttrs[4],aAttrb[5],aAttrs[5],aAttrb[6],aAttrs[6],show);
		}
	}
	ch->ChatPacket(CHAT_TYPE_COMMAND, "AUCTIONHOUSE REFRESH#2");	
}

void CAuctionHouse::UpdateItem(LPCHARACTER ch, int id, long long precio, int posarray)
{
	if (!ch){return;}

	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),"SELECT * FROM player.auction_items WHERE id='%d' LIMIT 1",id);
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	
	if (pMsg->Get()->uiNumRows > 0){
		int owner_id,vnum,count,transmutation,category;
		long long price;  long Sockets[AUCTION_MAX_SOCKET];
		BYTE aAttrb[AUCTION_MAX_ATTRIBUTE]; short aAttrs[AUCTION_MAX_ATTRIBUTE];
		
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		{
			int cur = 1;

			str_to_number(owner_id, row[cur++]);
			str_to_number(vnum, row[cur++]);
			cur++; //item_name
			str_to_number(count, row[cur++]);
			str_to_number(category, row[cur++]);
			price = atoll(row[cur++]);
			str_to_number(transmutation, row[cur++]);

			for (int j = 0; j < AUCTION_MAX_SOCKET; j++)
			{
				str_to_number(Sockets[j], row[cur++]);
			}
			
			for (int j = 0; j < AUCTION_MAX_ATTRIBUTE; j++)
			{
				str_to_number(aAttrb[j], row[cur++]);
				str_to_number(aAttrs[j], row[cur++]);
			}

			char szQuery1[1024];
			snprintf(szQuery1, sizeof(szQuery1),"INSERT INTO player.auction_items (`owner_id`,`vnum`,`item_name`,`count`,`category`,`price`,`transmutation`,"
			"`socket0`,`socket1`,`socket2`,`socket3`,`socket4`,`socket5`,`attrtype0`,`attrvalue0`,`attrtype1`,`attrvalue1`,`attrtype2`,`attrvalue2`,"
			"`attrtype3`,`attrvalue3`,`attrtype4`,`attrvalue4`,`attrtype5`,`attrvalue5`,`attrtype6`,`attrvalue6`)"
			"VALUES ('%d','%d','%s','%d','%d','%lld','%d','%ld','%ld','%ld','%ld','%ld','%ld',"
			"'%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d');", owner_id, vnum, row[3], count, category, precio, transmutation, 
			Sockets[0], Sockets[1], Sockets[2], Sockets[3], Sockets[4], Sockets[5], aAttrb[0], aAttrs[0],aAttrb[1],aAttrs[1],aAttrb[2],aAttrs[2],
			aAttrb[3],aAttrs[3],aAttrb[4],aAttrs[4],aAttrb[5],aAttrs[5],aAttrb[6],aAttrs[6]);
			std::unique_ptr<SQLMsg> pmsg (DBManager::Instance().DirectQuery(szQuery1));

			if (pmsg->Get()->uiInsertID != 0){
				char szQuery2[1024];
				snprintf(szQuery2, sizeof(szQuery2), "DELETE FROM player.auction_items WHERE id='%d'", id);
				DBManager::instance().DirectQuery(szQuery2);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "AUCTIONHOUSE DELETE#2#%d",posarray);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "AUCTIONHOUSE ITEMS_SELL#%d#%d#%d#%lld#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d",
				pmsg->Get()->uiInsertID,vnum,count,precio,transmutation,Sockets[0],Sockets[1],Sockets[2],Sockets[3],Sockets[4],
				aAttrb[0],aAttrs[0],aAttrb[1],aAttrs[1],aAttrb[2],aAttrs[2],aAttrb[3],aAttrs[3],aAttrb[4],aAttrs[4],aAttrb[5],aAttrs[5],aAttrb[6],aAttrs[6]);
			}else{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Auction House> Unexpected error, contact administration."));
			}
		}
	}else{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT( "<Auction House> Someone has already bought the item."));
	}
}

void CAuctionHouse::DeleteItem(LPCHARACTER ch, int id, int posarray)
{
	if (!ch){return;}
	
	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),"SELECT * FROM player.auction_items WHERE id='%d' LIMIT 1",id);
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));

	if (pMsg->Get()->uiNumRows > 0){
		int id,owner_id,vnum,count,transmutation,category;
		long long price;  long Sockets[AUCTION_MAX_SOCKET];
		BYTE aAttrb[AUCTION_MAX_ATTRIBUTE]; short aAttrs[AUCTION_MAX_ATTRIBUTE];
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		{
			int cur = 0;

			str_to_number(id, row[cur++]);
			str_to_number(owner_id, row[cur++]);
			str_to_number(vnum, row[cur++]);
			cur++;
			str_to_number(count, row[cur++]);
			str_to_number(category, row[cur++]);
			price = atoll(row[cur++]);
			str_to_number(transmutation, row[cur++]);

			for (int j = 0; j < AUCTION_MAX_SOCKET; j++)
			{
				str_to_number(Sockets[j], row[cur++]);
			}
			
			for (int j = 0; j < AUCTION_MAX_ATTRIBUTE; j++)
			{
				str_to_number(aAttrb[j], row[cur++]);
				str_to_number(aAttrs[j], row[cur++]);
			}

			LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(vnum, count);
			if (pkNewItem)
			{
#ifdef ENABLE_SPECIAL_INVENTORY
				int iEmptyPos = ch->GetEmptyInventory(pkNewItem);
#else
				int iEmptyPos = ch->GetEmptyInventory(pkNewItem->GetSize());
#endif
				if (iEmptyPos == -1)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("INVENTORY_FULL_ERROR"));
					return;
				}

				for (int i = 0; i < AUCTION_MIN_SOCKET; i++)
				{
					pkNewItem->SetSocket(i, Sockets[i]);
				}

				for (int i = 0; i < AUCTION_MIN_ATTRIBUTE; i++)
				{
					pkNewItem->SetForceAttribute(i, aAttrb[i], aAttrs[i]);
				}

#ifdef __CHANGELOOK_SYSTEM__
				if (transmutation > 0){pkNewItem->SetTransmutation(transmutation);}
#endif
				pkNewItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %s x%d"), pkNewItem->GetName(), count);

				DBManager::instance().DirectQuery("DELETE FROM player.auction_items WHERE id='%d'", id);

				DBManager::instance().DirectQuery("INSERT INTO log.auction_items_get (`who`,`account`,`iditem`,`vnum`,`name`,`count`,`price`,`ip`) VALUES ('%d','%d','%d','%d','%s','%d','%lld','%s');", ch->GetPlayerID(), owner_id, pkNewItem->GetID(), vnum, pkNewItem->GetName(), count, price, ch->GetDesc()->GetHostName());

				ch->ChatPacket(CHAT_TYPE_COMMAND, "AUCTIONHOUSE DELETE#2#%d",posarray);
				//pkNewItem end
			}else{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Auction House> Unexpected error, contact administration."));
			}
		}
	}else{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT( "<Auction House> Someone has already bought the item."));
	}	
}

void CAuctionHouse::GetItemSearch(LPCHARACTER ch, const char* arg1)
{
	if (!ch){return;}

	char szQuery[1024];const char* porcentaje = "%%";

	std::string cadena = DBManager::Instance().EscapeString(arg1);
	snprintf(szQuery, sizeof(szQuery),"SELECT * FROM player.auction_items WHERE item_name like '%s%s%s' AND `show`='1' ORDER BY item_name ASC, price ASC LIMIT 500",porcentaje,cadena.c_str(),porcentaje);

	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiNumRows > 0){

		int id,owner_id,vnum,count,transmutation,category;
		long long price;  long Sockets[AUCTION_MAX_SOCKET];
		BYTE aAttrb[AUCTION_MAX_ATTRIBUTE]; short aAttrs[AUCTION_MAX_ATTRIBUTE];
		while (MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult))
		{
			int cur = 0;
	
			str_to_number(id, row[cur++]);
			str_to_number(owner_id, row[cur++]);
			str_to_number(vnum, row[cur++]);
			cur++;
			str_to_number(count, row[cur++]);
			str_to_number(category, row[cur++]);
			price = atoll(row[cur++]);
			str_to_number(transmutation, row[cur++]);

			for (int j = 0; j < AUCTION_MAX_SOCKET; j++)
			{
				str_to_number(Sockets[j], row[cur++]);
			}
			
			for (int j = 0; j < AUCTION_MAX_ATTRIBUTE; j++)
			{
				str_to_number(aAttrb[j], row[cur++]);
				str_to_number(aAttrs[j], row[cur++]);
			}
			ch->ChatPacket(CHAT_TYPE_COMMAND, "AUCTIONHOUSE GET_ITEMS_SEARCH#%d#%d#%d#%d#%lld#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d#%d",
			id,owner_id,vnum,count,price,transmutation,Sockets[0],Sockets[1],Sockets[2],Sockets[3],Sockets[4],Sockets[5],
			aAttrb[0],aAttrs[0],aAttrb[1],aAttrs[1],aAttrb[2],aAttrs[2],aAttrb[3],aAttrs[3],aAttrb[4],aAttrs[4],aAttrb[5],aAttrs[5],aAttrb[6],aAttrs[6]);
		}
	}
	ch->ChatPacket(CHAT_TYPE_COMMAND, "AUCTIONHOUSE REFRESH#1");
}

void CAuctionHouse::GetBank(LPCHARACTER ch)
{
	if (!ch){return;}

	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),"SELECT bank FROM account.account WHERE id ='%d' LIMIT 1",ch->GetAID());
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	
	if (pMsg->Get()->uiNumRows > 0){
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		long long bank;
		bank = atoll(row[0]);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "AUCTIONHOUSE SET_YANG#%lld",bank);
	}
}

void CAuctionHouse::GetMoneyBank(LPCHARACTER ch)
{
	if (!ch){return;}

	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),"SELECT bank FROM account.account WHERE id ='%d' LIMIT 1",ch->GetAID());
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	
	if (pMsg->Get()->uiNumRows > 0){
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		long long bank;
		bank = atoll(row[0]);

		if(ch->GetGold() + bank > GOLD_MAX){
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You exceed the limit of Yang."));
		}else{
			char szQuery[1024];
			snprintf(szQuery, sizeof(szQuery), "UPDATE account.account SET bank=bank-%lld WHERE id = %d", bank, ch->GetAID());
			DBManager::instance().DirectQuery(szQuery);

			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Auction House> You have received %lld from Yang."),bank);

			ch->PointChange(POINT_GOLD, bank, true);
			GetBank(ch);
		}
	}	
}

#endif // __AUCTION_HOUSE__