#include "stdafx.h"
#include "desc_client.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "log.h"
#ifdef __ITEM_SHOP__
#include "itemshop_manager.h"
CItemShopManager::CItemShopManager()
{
}

CItemShopManager::~CItemShopManager()
{
}

void CItemShopManager::Initialize()
{
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEMSHOP_ITEMS_REQUEST, 0, 0);
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEMSHOP_EDITORS_REQUEST, 0, 0);
}

void CItemShopManager::LoadItems(BYTE bType, DWORD dwCount, TItemShopItem * pItems)
{
	//loading when startup
	if (bType == 0)
	{
		for (DWORD i = 0; i < dwCount; ++i)
		{
			m_vec_itemShopItems.push_back(pItems[i]);
			sys_log(0, "Loaded data: vnum [%d], count [%d], price [%d], category [%d], id [%d] ", pItems[i].vnum, pItems[i].count, pItems[i].price, pItems[i].category, pItems[i].id);
		}
	}

	//loading when item added
	else if (bType == 1)
	{
		BYTE i = 0;
		char szChat[CHAT_MAX_LEN];

		m_vec_itemShopItems.push_back(pItems[i]);

		snprintf(szChat, sizeof(szChat), "SetItemShopItems %d|%d|%d|%d|%d", pItems[i].vnum, pItems[i].count, pItems[i].price, pItems[i].category, pItems[i].id);
		ViewerChatPacket(CHAT_TYPE_COMMAND, szChat);

		snprintf(szChat, sizeof(szChat), "RefreshItemShopPage");
		ViewerChatPacket(CHAT_TYPE_COMMAND, szChat);
	}

	//loading when item edited
	else if (bType == 2)
	{
		BYTE i = 0;
		char szChat[CHAT_MAX_LEN];

		m_vec_itemShopItems.push_back(pItems[i]);

		snprintf(szChat, sizeof(szChat), "UpdateItemShopItem %d %d %d %d %d", pItems[i].vnum, pItems[i].count, pItems[i].price, pItems[i].category, pItems[i].id);
		ViewerChatPacket(CHAT_TYPE_COMMAND, szChat);

		snprintf(szChat, sizeof(szChat), "RefreshItemShopPage");
		ViewerChatPacket(CHAT_TYPE_COMMAND, szChat);
	}
}

void CItemShopManager::LoadEditors(DWORD dwCount, TItemShopEditor * pEditors)
{
	for (DWORD i = 0; i < dwCount; ++i)
	{
		char * szName = &pEditors[i].name[0];
		AddEditor(szName);
		sys_log(0, "Loaded editor name[%s] ", szName);
	}
}

TItemShopItem CItemShopManager::FindItemByID(DWORD id, DWORD category)
{
	itertype(m_vec_itemShopItems) it = m_vec_itemShopItems.begin();
	while (it != m_vec_itemShopItems.end()) 
	{
		TItemShopItem item = *(it++);

		if (item.id == id && item.category == category)
			return item;
	}
}

void CItemShopManager::OpenItemShop(LPCHARACTER ch)
{
	char szChat[CHAT_MAX_LEN];

	snprintf(szChat, sizeof(szChat), "ClearItemShopItems");
	ch->ChatPacket(CHAT_TYPE_COMMAND, szChat);

	snprintf(szChat, sizeof(szChat), "ItemShopEditor %d", IsEditor(ch) ? 1 : 0 );

	itertype(m_vec_itemShopItems) it = m_vec_itemShopItems.begin();
	while (it != m_vec_itemShopItems.end()) 
	{	
		TItemShopItem item = *(it++);
		
		snprintf(szChat, sizeof(szChat), "SetItemShopItems %d|%d|%d|%d|%d", item.vnum, item.count, item.price, item.category, item.id);
		ch->ChatPacket(CHAT_TYPE_COMMAND, szChat);
	}

	snprintf(szChat, sizeof(szChat), "SetItemShopCoins %d", ch->GetCoins());
	ch->ChatPacket(CHAT_TYPE_COMMAND, szChat);

	snprintf(szChat, sizeof(szChat), "OpenShopWindow");
	ch->ChatPacket(CHAT_TYPE_COMMAND, szChat);

	AddViewer(ch);
}

void CItemShopManager::BuyItem(LPCHARACTER ch, DWORD id, DWORD category)
{
	if (!IsViewer(ch))
	{
		sys_err("no viewer character (%d %s) try to buy item id[%d] category[%d]",
			ch->GetPlayerID(), ch->GetName(), id, category);
		return;
	}

	if (category < 0 || category > 12)
		return;

	char szChat[CHAT_MAX_LEN];

	TItemShopItem item = FindItemByID(id, category);
	if (item.price < 0)
		return;

	if (ch->GetCoins() < item.price)
		return;

	 TItemTable * pTable = ITEM_MANAGER::instance().GetTable(item.vnum);

	if (!pTable)
		return;

	if (ch->GetEmptyInventory(pTable->bSize) == -1)
		return;

	LogManager::instance().ItemShopBuyLog(item.vnum, item.count, id, category, ch->GetDesc()->GetAccountTable().id, item.price);

	ch->SetCoins(ch->GetCoins() - item.price);
	ch->AutoGiveItem(item.vnum, item.count);

	snprintf(szChat, sizeof(szChat), "SetItemShopCoins %d", ch->GetCoins());
	ch->ChatPacket(CHAT_TYPE_COMMAND, szChat);
}

void CItemShopManager::DeleteItem(LPCHARACTER ch, DWORD id, DWORD category, bool deleteFromDB)
{
	if (!IsViewer(ch))
	{
		sys_err("no viewer character (%d %s) try to delete item id[%d] category[%d]",
			ch->GetPlayerID(), ch->GetName(), id, category);
		return;
	}

	if (!IsEditor(ch))
	{
		sys_err("non editor character (%d %s) try to delete item id[%d] category[%d]",
			ch->GetPlayerID(), ch->GetName(), id, category);
		return;
	}

	if (category < 0 || category > 12)
		return;

	if (id == 0)
		return;

	for (DWORD i = 0; i < m_vec_itemShopItems.size(); ++i)
		if (m_vec_itemShopItems[i].id == id && m_vec_itemShopItems[i].category == category)
			m_vec_itemShopItems.erase(m_vec_itemShopItems.begin() + i);

	if (deleteFromDB)
	{
		TItemShopDeleteItem p;
		p.id = id;
		p.category = category;
		db_clientdesc->DBPacket(HEADER_GD_ITEMSHOP_ITEM_DELETE, 0, &p, sizeof(TItemShopDeleteItem));
	}

	char szChat[CHAT_MAX_LEN];
	
	snprintf(szChat, sizeof(szChat), "DeleteItemShopItem %d %d", category, id);
	ViewerChatPacket(CHAT_TYPE_COMMAND, szChat);

	snprintf(szChat, sizeof(szChat), "RefreshItemShopPage");
	ViewerChatPacket(CHAT_TYPE_COMMAND, szChat);
}

void CItemShopManager::AddItem(LPCHARACTER ch, DWORD vnum, DWORD count, DWORD price, DWORD category)
{
	if (!IsViewer(ch))
	{
		sys_err("no viewer character (%d %s) try to add item vnum[%d] count[%d] price[%d] category[%d]",
			ch->GetPlayerID(), ch->GetName(), vnum, count, price, category);
		return;
	}

	if (!IsEditor(ch))
	{
		sys_err("non editor character (%d %s) try to add item vnum[%d] count[%d] price[%d] category[%d]",
			ch->GetPlayerID(), ch->GetName(), vnum, count, price, category);
		return;
	}

	if (category < 0 || category > 12)
		return;

	if (vnum == 0 || count == 0 || price == 0)
		return;

	TItemTable * pTable = ITEM_MANAGER::instance().GetTable(vnum);

	if (!pTable)
		return;

	TItemShopItem p;
	p.vnum = vnum;
	p.count = count;
	p.price = price;
	p.category = category;
	p.id = -1;

	db_clientdesc->DBPacket(HEADER_GD_ITEMSHOP_ITEM_ADD, 0, &p, sizeof(TItemShopItem));
}

void CItemShopManager::EditItem(LPCHARACTER ch, DWORD vnum, DWORD count, DWORD price, DWORD category, DWORD id)
{
	if (!IsViewer(ch))
	{
		sys_err("no viewer character (%d %s) try to add item vnum[%d] count[%d] price[%d] category[%d]",
			ch->GetPlayerID(), ch->GetName(), vnum, count, price, category);
		return;
	}

	if (!IsEditor(ch))
	{
		sys_err("non editor character (%d %s) try to add item vnum[%d] count[%d] price[%d] category[%d]",
			ch->GetPlayerID(), ch->GetName(), vnum, count, price, category);
		return;
	}

	if (category < 0 || category > 12)
		return;

	if (vnum == 0 || count == 0 || price == 0 || id == 0)
		return;
	
	TItemTable * pTable = ITEM_MANAGER::instance().GetTable(vnum);

	if (!pTable)
		return;

	TItemShopItem p;
	p.vnum = vnum;
	p.count = count;
	p.price = price;
	p.category = category;
	p.id = id;

	DeleteItem(ch, id, category, false);

	db_clientdesc->DBPacket(HEADER_GD_ITEMSHOP_ITEM_EDIT, 0, &p, sizeof(TItemShopItem));
}

void CItemShopManager::AddEditor(char* szName)
{
	m_vec_pkEditors.push_back(szName);
}

bool CItemShopManager::IsEditor(LPCHARACTER ch)
{
	//for (auto it = m_vec_pkEditors.begin(); it!= m_vec_pkEditors.end(); ++it )
	for (itertype(m_vec_pkEditors) it = m_vec_pkEditors.begin(); it!= m_vec_pkEditors.end(); ++it )
	{
		char* szEditorName = *(it);

		if (strcmp(ch->GetName(), szEditorName) == 0)
			return true;

	}

	return false;
}

bool CItemShopManager::IsViewer(LPCHARACTER ch)
{
	return m_set_pkCurrentViewer.find(ch) != m_set_pkCurrentViewer.end();
}

void CItemShopManager::AddViewer(LPCHARACTER ch)
{
	if (!IsViewer(ch))
		m_set_pkCurrentViewer.insert(ch);
}

void CItemShopManager::RemoveViewer(LPCHARACTER ch)
{
	m_set_pkCurrentViewer.erase(ch);
}

void CItemShopManager::ViewerChatPacket(BYTE bType, const char * szChat)
{
	for (std::set<LPCHARACTER>::iterator it = m_set_pkCurrentViewer.begin(); it != m_set_pkCurrentViewer.end(); ++it)
	{
		if((*it)->GetDesc())
			(*it)->GetDesc()->ChatPacket(bType, szChat);
		else
			RemoveViewer(*it);
	}
}
#endif
