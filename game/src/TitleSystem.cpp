#include "stdafx.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "db.h"
#include "item.h"
#include "TitleSystem.h"

#ifdef __TITLE_SYSTEM_YUMA__
CTitleSystem::CTitleSystem()
{
}

CTitleSystem::~CTitleSystem()
{
	m_stTitle = "";
}

void CTitleSystem::SetTitleString(const std::string& title)
{
	m_stTitle = title;
}

void CTitleSystem::UseTitleItem(bool bEquip, int iValue0)
{
	if (bEquip)
		SetTitleString(GetTitleFromDatabase(iValue0));
	else
		SetTitleString("");
}

DWORD CTitleSystem::GetColorFromDatabase(int iValue0) const
{
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT szColorHEX FROM player.titlesystem WHERE id='%d'", iValue0));

	if (pMsg->uiSQLErrno != 0)
		return 0;

	if (pMsg->Get()->uiNumRows == 0)
		return 0;

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	return strtoul(row[0], NULL, 0);
}

std::string CTitleSystem::GetTitleFromDatabase(int iValue0) const
{
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT szTitle FROM player.titlesystem WHERE id='%d'", iValue0));

	if (pMsg->uiSQLErrno != 0)
		return "";

	if (pMsg->Get()->uiNumRows == 0)
		return "";

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	return row[0];
}

std::pair<std::string, DWORD> CTitleSystem::GetTitleString(LPCHARACTER pkChar)
{
	if (!pkChar->IsPC())
		return std::make_pair("", 0);

	LPITEM pkTitleItem = pkChar->FindTitleItem();
	if (pkTitleItem == NULL)
		return std::make_pair("", 0);

	int iTitleID = pkTitleItem->GetValue(0);
	std::string szTitle = GetTitleFromDatabase(iTitleID);
		
	return std::make_pair(szTitle, GetColorFromDatabase(iTitleID));
}
#endif