#pragma once
#include <boost/asio.hpp>
#include "constants.h"
#include "config.h"
#include "event.h"
#include "minilzo.h"
#include "packet.h"
#include "desc_manager.h"
#include "item_manager.h"
#include "char.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "motion.h"
#include "sectree_manager.h"
#include "shop_manager.h"
#include "regen.h"
#include "text_file_loader.h"
#include "skill.h"
#include "pvp.h"
#include "party.h"
#include "questmanager.h"
#include "profiler.h"
#include "lzo_manager.h"
#include "messenger_manager.h"
#include "db.h"
#include "log.h"
#include "p2p.h"
#include "guild_manager.h"
#include "dungeon.h"
#include "cmd.h"
#include "refine.h"
#include "banword.h"
#include "priv_manager.h"
#include "war_map.h"
#include "building.h"
#include "login_sim.h"
#include "target.h"
#include "marriage.h"
#include "wedding.h"
#include "fishing.h"
#include "item_addon.h"
#include "TrafficProfiler.h"
#include "locale_service.h"
#include "arena.h"
#include "OXEvent.h"
#include "monarch.h"
#include "polymorph.h"
#include "blend_item.h"
#include "castle.h"
#include "passpod.h"
#include "ani.h"
#include "BattleArena.h"
#include "over9refine.h"
#include "horsename_manager.h"
#include "pcbang.h"
#include "MarkManager.h"
#include "spam.h"
#include "panama.h"
#include "threeway_war.h"
#include "auth_brazil.h"
#include "DragonLair.h"
#include "HackShield.h"
#include "skill_power.h"
#include "SpeedServer.h"
#include "XTrapManager.h"
#include "DragonSoul.h"
#ifdef __ENABLE_NEW_OFFLINESHOP__
#include "new_offlineshop.h"
#include "new_offlineshop_manager.h"
#include "Offlineshop.h"
// #include <offlineshop/Offlineshop.h>
#endif
#ifdef __INGAME_ITEMSHOP__
#include "ItemShop.h"
#endif
#ifdef ENABLE_PACKET_DUMPING
#include "packet_dumper.h"
#endif
#ifdef ENABLE_CSHIELD
#include "cshieldLib.h"
#endif

#ifdef ENABLE_SWITCHBOT
#include "switchbot.h"
#endif

#ifdef __NEW_EVENT_HANDLER__
#include "EventFunctionHandler.hpp"
#endif

#include "EventHandler.h"
#ifdef __ENABLE_PVP_ARENA__
#include "PVPArena.h"
#endif

#ifdef ENABLE_INGAME_ITEMSHOP
#include "ItemShopNewManager.h"
#endif

#ifdef __LOGIN_REWARD__
#include "login_reward.h"
#endif

#ifdef BATTLE_PASS_SYSTEM
#include "battle_pass_manager.h"
#endif

enum EProfile
{
	PROF_EVENT,
	PROF_CHR_UPDATE,
	PROF_IO,
	PROF_HEARTBEAT,
	PROF_MAX_NUM
};


class CApplication : public std::enable_shared_from_this<CApplication>, singleton<CApplication>
{
protected:
	socket_t	m_tcp_socket;
#ifndef __UDP_BLOCK__
	socket_t	udp_socket;
#endif
	socket_t	m_p2p_socket;
	LPFDWATCH m_fdWatcher;
	static std::array<std::uint32_t, PROF_MAX_NUM> m_dwProfiler;
private:
	boost::asio::io_service m_context;

protected:

#ifdef ENABLE_PACKET_DUMPING
	PacketDumper packet_dumper;
#endif

	SECTREE_MANAGER	m_sectree_manager;
	CHARACTER_MANAGER	m_char_manager;
	ITEM_MANAGER	m_item_manager;
	CShopManager	m_shop_manager;
	CMobManager		m_mob_manager;
	CMotionManager	m_motion_manager;
	CPartyManager	m_party_manager;
	CSkillManager	m_skill_manager;
	CPVPManager		m_pvp_manager;
	LZOManager		m_lzo_manager;
	DBManager		m_db_manager;
	AccountDB 		m_account_db;

	LogManager		m_log_manager;
	MessengerManager	m_messenger_manager;
	P2P_MANAGER		m_p2p_manager;
	CGuildManager	m_guild_manager;
	CGuildMarkManager m_mark_manager;
	CDungeonManager	m_dungeon_manager;
	CRefineManager	m_refine_manager;
	CBanwordManager	m_banword_manager;
	CPrivManager	m_priv_manager;
	CWarMapManager	m_war_map_manager;
	building::CManager	m_building_manager;
	CTargetManager	m_target_manager;
	marriage::CManager	m_marriage_manager;
	marriage::WeddingManager m_wedding_manager;
	CItemAddonManager	m_item_addon_manager;

	CArenaManager m_arena_manager;
	COXEventManager m_OXEvent_manager;
	CMonarch		m_Monarch;
	CHorseNameManager m_horsename_manager;
	CPCBangManager m_pcbang_manager;

	DESC_MANAGER	m_desc_manager;

	TrafficProfiler	m_trafficProfiler;
	CTableBySkill m_SkillPowerByLevel;
	CPolymorphUtils m_polymorph_utils;
	CProfiler		m_profiler;
	CPasspod		m_passpod;
	CBattleArena	m_ba;
	COver9RefineManager	m_o9r;
	SpamManager		m_spam_mgr;
	CThreeWayWar	m_threeway_war;
	CDragonLairManager	m_dl_manager;

	CHackShieldManager	m_HSManager;
	CXTrapManager		m_XTManager;

	CSpeedServerManager m_SSManager;
	DSManager m_dsManager;
#ifdef __ENABLE_BIOLOG_SYSTEM__
	CBiologSystemManager m_biologManager;
#endif
#ifdef ENABLE_SWITCHBOT
	CSwitchbotManager m_switchbot;
#endif

#ifdef __INGAME_ITEMSHOP__
	CItemShop m_itemShopManager;
#endif

#ifdef __AUCTION__
	AuctionManager m_auctionManager;
#endif

#ifdef __ENABLE_NEW_OFFLINESHOP__
	offlineshop::CShopManager m_offshopManager;
#endif
#ifdef BATTLE_PASS_SYSTEM
	CBattlePassManager bp_manager;
	CBattlePassMissionManager bp_mission_manager;
#endif

	quest::CQuestManager m_quest_manager;

#ifdef __NEW_EVENT_HANDLER__
	CEventFunctionHandler m_EventFunctionHandler;
#endif

	CEventHandler m_eventHandler;
#ifdef __ENABLE_PVP_ARENA__
	CPVPArenaManager pvp_arena_manager;
#endif

#ifdef __LOGIN_REWARD__
	CLoginRewardManager reward_manager;
#endif

public:
	CApplication(); 
	~CApplication();
	void Destroy();

	bool Run(int argc, char** argv);

	int __IoLoop();
	static void HeartBeat(LPHEART ht, int pulse);
private:
	bool __Start(int argc, char** argv);

	bool __Initialize();
	void __InitDependencies();
	void __CleanUpForEarlyExit();
	void __Usage();
	void __RunIdle();
	int __Idle();
};