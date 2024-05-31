#include "stdafx.h"
 
#include "questlua.h"
#include "questmanager.h"
#include "horsename_manager.h"
#include "char.h"
#include "affect.h"
#include "config.h"
#include "utils.h"
#include "../../common/service.h"
#include "SupportSystem.h"
#undef sys_err
#ifndef __WIN32__
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

#ifdef ENABLE_SUPPORT_SYSTEM

namespace quest
{
	int support_summon(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPITEM pItem = CQuestManager::instance().GetCurrentItem();
		if (!ch || !pItem)
			return 0;
		CSupportSystem* supportSystem = ch->GetSupportSystem();
		if (!supportSystem)
			return 0;

		DWORD mobVnum= lua_isnumber(L, 1) ? lua_tonumber(L, 1) : 0;
		const char* supportName = lua_isstring(L, 2) ? lua_tostring(L, 2) : 0;
		bool bFromFar = lua_isboolean(L, 3) ? lua_toboolean(L, 3) : false;
		supportSystem->Summon(mobVnum, pItem, supportName, bFromFar);
		return 1;
	}

	int support_unsummon(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;
		CSupportSystem* supportSystem = ch->GetSupportSystem();
		if (!supportSystem)
			return 0;
		DWORD mobVnum= lua_isnumber(L, 1) ? lua_tonumber(L, 1) : 0;
		supportSystem->Unsummon(mobVnum,true,true);
		return 1;
	}

	int support_count_summoned(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;
		CSupportSystem* supportSystem = ch->GetSupportSystem();
		lua_Number count = 0;
		if (0 != supportSystem)
			count = (lua_Number)supportSystem->CountSummoned();
		lua_pushnumber(L, count);
		return 1;
	}

	int support_is_summon(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;
		CSupportSystem* supportSystem = ch->GetSupportSystem();
		if (!supportSystem)
			return 0;
		DWORD mobVnum= lua_isnumber(L, 1) ? lua_tonumber(L, 1) : 0;
		CSupportActor* supportActor = supportSystem->GetByVnum(mobVnum);
		if (supportActor)
			lua_pushboolean(L,true);
		else
			lua_pushboolean(L, false);
		return 1;
	}

	void RegisterSupportFunctionTable()
	{
		luaL_reg support_functions[] =
		{
			{ "summon",			support_summon			},
			{ "unsummon",		support_unsummon		},
			{ "is_summon",		support_is_summon		},
			{ "count_summoned",	support_count_summoned	},
			{ NULL,				NULL				}
		};

		CQuestManager::instance().AddLuaFunctionTable("supports", support_functions);
	}
#endif

}

