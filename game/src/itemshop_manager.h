#pragma once

#include "stdafx.h"

#ifdef __ITEM_SHOP__
class CItemShopManager : public singleton<CItemShopManager>
{
public:
	CItemShopManager();
	~CItemShopManager();
	
	void						Initialize();
	void						LoadItems(BYTE bType, DWORD dwCount, TItemShopItem * pItems);
	void						LoadEditors(DWORD dwCount, TItemShopEditor* pEditors);
	TItemShopItem				FindItemByID(DWORD id, DWORD category);
	void						OpenItemShop(LPCHARACTER ch);
	void						BuyItem(LPCHARACTER ch, DWORD id, DWORD category);
	void						DeleteItem(LPCHARACTER ch, DWORD id, DWORD category, bool deleteFromDB = true);
	void						AddItem(LPCHARACTER ch, DWORD vnum, DWORD count, DWORD price, DWORD category);
	void						EditItem(LPCHARACTER ch, DWORD vnum, DWORD count, DWORD price, DWORD category, DWORD id);
private:
	std::vector<TItemShopItem>	m_vec_itemShopItems;
	std::set<LPCHARACTER>		m_set_pkCurrentViewer;
	std::vector<char*>			m_vec_pkEditors;
	void						AddEditor(char* szName);
	bool						IsEditor(LPCHARACTER ch);
	bool						IsViewer(LPCHARACTER ch);
	void						AddViewer(LPCHARACTER ch);
	void						RemoveViewer(LPCHARACTER ch);
	void						ViewerChatPacket(BYTE bType, const char * szChat);
};
#endif
