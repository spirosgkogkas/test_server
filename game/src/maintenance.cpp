/*********************************************************************
* date        : 2016.07.20
* file        : maintenance.cpp
* author      : VegaS
* for         : Terabithia.pl
* description : 
*/
#include "stdafx.h"
#include "constants.h"
#include "desc.h"
#include "db.h"
#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "affect.h"
#include "pvp.h"
#include "cmd.h"
#include "start_position.h"
#include "party.h"
#include "guild_manager.h"
#include "p2p.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "war_map.h"
#include "questmanager.h"
#include "item_manager.h"
#include "monarch.h"
#include "mob_manager.h"
#include "dev_log.h"
#include "item.h"
#include "log.h"
#include "../../common/VnumHelper.h"
#include "../../common/billing.h"
#include "guild.h"
#include "empire_text_convert.h"
#include "castle.h"
#include "locale_service.h"
#include <string>
#include <boost/algorithm/string.hpp>
#include "maintenance.h"
#include "input.h"
extern long int global_time_maintenance = 0;

MaintenanceManager::MaintenanceManager()	{	}
MaintenanceManager::~MaintenanceManager()	{	}

EVENTINFO(maintenanceShutdown_event_data)
{
	int seconds;

	maintenanceShutdown_event_data()
		: seconds(0)
	{
	}
};
static LPEVENT vegas_maintenance_check = NULL;

/*********
* When the remaining 10 seconds of time put into reverse count will start the game for all players that the server will be closed 10,9,8 etc. and then the server will automatically stop.
*/
#define MAINTENANCE_CHECKTIME_SHUTDOWN	10

/*********
* Settings min/max character/second for check in maintenance
*/
#define MAINTENANCE_TEXT_MAX_CHAR 70 // Maximum characters that are allowed in reason maintenance
#define MAINTENANCE_TEXT_MIN_CHAR 5 // Minimum characters that are need in reason maintenance

#define MAINTENANCE_TIME_LEFT_MIN 15 // Seconds minimum how long they are allowed to start maintenance = 30 second
#define MAINTENANCE_TIME_LEFT_MAX 604800 // Seconds maximum how long they are allowed to start maintenance = 1 week

#define MAINTENANCE_TIME_DURATION_MIN 300 // Seconds minimum how long duration for back server online = 1 minute
#define MAINTENANCE_TIME_DURATION_MAX 86400 // Seconds maximum how long duration for back server online = 1 Day

#define MAINTENANCE_ADMIN_NAME	"Teaser" // Name for acces
#define MAINTENANCE_ADMIN_IP	"192.168.0.103" // Ip for acces

/*********
* Table with translate for all informations, have careful with %s or %u when you try to translate in other language.
*/
extern const char* maintenance_translate[] = {"-----------------------------------------------------------------------------------",
												"<Syntax> Wrong command! Use: /maintenance 2h 30m",
												"<Syntax> Example: d (day) | h (hour) | m (minutes) | s (second)",
												"<Syntax> Maximum %u second are allowed for left time!",
												"<Syntax> Maximum %u second are allowed for duration time!",
												"<Technical break> Was started successfully! Server will be offline in %u second!",
												"<Technical break> Estimated duration %u second!",											
												"<Syntax> Command wrong! Use: /m_text enable <reason>",	
												"<Syntax> Example: /m_text enable Hello player, need to make this maintenance for solve problem with spider dungeon 3.",	
												"<Syntax> Maximum %u characters are allowed for reason!",	
												"<Syntax> You must enter at least %u characters!",	
												"<Technical break> The reason was successfully removed!",	
												"<Technical break> The reason was successfully added!",	
												"<Technical break> Reason added: %s",
												"<Technical break> It was stopped succes!",
												"<Syntax> Minimum %u second need for left time!",
												"<Syntax> Minimum %u second need duration time!",
												"<Error> You must have name %s for acces on this command !",
												"<Error> You must have ip %s for acces on this command !"
											};

EVENTFUNC(maintenanceDown_event)
{
	maintenanceShutdown_event_data* info = dynamic_cast<maintenanceShutdown_event_data*>(event->info);

	if (info == NULL)
	{
		sys_err("maintenanceDown_event> <Factor> Time 0 - Error");
		return 0;
	}

	int * pSecondMaintenance = &(info->seconds);

	if (*pSecondMaintenance == MAINTENANCE_CHECKTIME_SHUTDOWN)
	{	
		char sTime[128];
		char sDuration[128];
		char sReason[128];
		
		snprintf(sTime, sizeof(sTime), "UPDATE player.maintenance SET time = %u", 0);
		snprintf(sDuration, sizeof(sDuration), "UPDATE player.maintenance SET duration = %u", 0);
		snprintf(sReason, sizeof(sReason), "UPDATE player.maintenance SET reason = 'no_reason'");
		
		std::unique_ptr<SQLMsg> pTime(DBManager::instance().DirectQuery(sTime));	
		std::unique_ptr<SQLMsg> pDuration(DBManager::instance().DirectQuery(sDuration));
		std::unique_ptr<SQLMsg> pReason(DBManager::instance().DirectQuery(sReason));	
		
		TPacketGGShutdown p;
		p.bHeader = HEADER_GG_SHUTDOWN;
		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShutdown));
		g_bNoMoreClient = true;		
		Shutdown(global_time_maintenance);		
	}
	else
	{
/*		const DESC_MANAGER::DESC_SET & setMaintenance = DESC_MANAGER::instance().GetClientSet();
		DESC_MANAGER::DESC_SET::const_iterator pMaintenance = setMaintenance.begin();

		while (pMaintenance != setMaintenance.end())
		{
			LPDESC d = *(pMaintenance++);

			SQLMsg * pTableMaintenance = DBManager::instance().DirectQuery("SELECT time,duration,reason from player.maintenance");
			
			if (pTableMaintenance->Get()->uiNumRows > 0)
			{
				MYSQL_ROW row = mysql_fetch_row(pTableMaintenance->Get()->pSQLResult);
				d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_Update_Maintenance %s %s %s", row[0], row[1], row[2]);
				delete pTableMaintenance;
			}
		}	
*/
			char sTime[128];
			snprintf(sTime, sizeof(sTime), "UPDATE player.maintenance SET time = %u", *pSecondMaintenance);
			std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery(sTime));
			
			--*pSecondMaintenance;
			return passes_per_sec;
		}

	vegas_maintenance_check = NULL;
	return 0;
}

void StartMaintenance(LPCHARACTER ch, int iSec)
{
	if (g_bNoMoreClient)
	{
		thecore_shutdown();
		return;
	}

	CWarMapManager::instance().OnShutdown();

	maintenanceShutdown_event_data* info = AllocEventInfo<maintenanceShutdown_event_data>();
	info->seconds = iSec;
	vegas_maintenance_check = event_create(maintenanceDown_event, info, 1);
}

void MaintenanceManager::Send_DisableSecurity(LPCHARACTER ch)
{
	const std::string & szName = ch->GetName();
	const std::string & szIp = ch->GetDesc()->GetHostName();
	
	if (szName != MAINTENANCE_ADMIN_NAME)
	{	
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[17], MAINTENANCE_ADMIN_NAME);	
		return;	
	}
	
	if (szIp != MAINTENANCE_ADMIN_IP)
	{	
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[18], MAINTENANCE_ADMIN_IP);
		return;	
	}
	
	if (vegas_maintenance_check)
	{
		event_cancel(&vegas_maintenance_check);
		vegas_maintenance_check = NULL;
	}	
		char sTime[128];
		char sDuration[128];
		char sReason[128];
		
		snprintf(sTime, sizeof(sTime), "UPDATE player.maintenance SET time = %u", 0);
		snprintf(sDuration, sizeof(sDuration), "UPDATE player.maintenance SET duration = %u", 0);
		snprintf(sReason, sizeof(sReason), "UPDATE player.maintenance SET reason = 'no_reason'");
		
		std::unique_ptr<SQLMsg> pTime(DBManager::instance().DirectQuery(sTime));	
		std::unique_ptr<SQLMsg> pDuration(DBManager::instance().DirectQuery(sDuration));
		std::unique_ptr<SQLMsg> pReason(DBManager::instance().DirectQuery(sReason));	
		
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[14]);
}
void MaintenanceManager::Send_ActiveMaintenance(LPCHARACTER ch, long int time_maintenance, long int duration_maintenance)
{
	if (NULL == ch)
		return;

	if (!ch->IsPC())
		return;
	
	const std::string & szName = ch->GetName();
	const std::string & szIp = ch->GetDesc()->GetHostName();
	
	if (szName != MAINTENANCE_ADMIN_NAME)
	{	
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[17], MAINTENANCE_ADMIN_NAME);	
		return;	
	}
	
	if (szIp != MAINTENANCE_ADMIN_IP)
	{	
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[18], MAINTENANCE_ADMIN_IP);
		return;	
	}

	if (!time_maintenance || time_maintenance < 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[0]);	
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[1]);
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[2]);
		return;
	}
	
	else if (!duration_maintenance || duration_maintenance < 1)
	{	
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[0]);	
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[1]);
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[2]);
		return;		
	}
	
	else if (time_maintenance < MAINTENANCE_TIME_LEFT_MIN)
	{	
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[0]);	
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[15], MAINTENANCE_TIME_LEFT_MIN);
		return;	
	}	
	
	else if (time_maintenance > MAINTENANCE_TIME_LEFT_MAX)
	{	
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[0]);	
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[3], MAINTENANCE_TIME_LEFT_MAX);
		return;	
	}	
	
	else if (duration_maintenance < MAINTENANCE_TIME_DURATION_MIN)
	{	
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[0]);	
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[16], MAINTENANCE_TIME_DURATION_MIN);
		return;	
	}	
	
	else if (duration_maintenance > MAINTENANCE_TIME_DURATION_MAX)
	{	
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[0]);	
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[4], MAINTENANCE_TIME_DURATION_MAX);
		return;	
	}	
	else
	{	
		char sDuration[128];
		snprintf(sDuration, sizeof(sDuration), "UPDATE player.maintenance SET duration = %u", duration_maintenance);
		std::unique_ptr<SQLMsg> pDuration(DBManager::instance().DirectQuery(sDuration));	
				
		global_time_maintenance = time_maintenance;
		
		StartMaintenance(ch, time_maintenance);
		
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[0]);	
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[5], time_maintenance);
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[6], duration_maintenance);
	}	
}

void MaintenanceManager::Send_Text(LPCHARACTER ch, const char* reason)
{
	if (NULL == ch)
		return;

	if (!ch->IsPC())
		return;
	
	const std::string & szName = ch->GetName();
	const std::string & szIp = ch->GetDesc()->GetHostName();
	
	if (szName != MAINTENANCE_ADMIN_NAME)
	{	
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[17], MAINTENANCE_ADMIN_NAME);	
		return;	
	}
	
	if (szIp != MAINTENANCE_ADMIN_IP)
	{	
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[18], MAINTENANCE_ADMIN_IP);
		return;	
	}

	if (!*reason)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[0]);	
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[7]);
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[8]);
		return;
	}	
	
	if (strlen(reason) > MAINTENANCE_TEXT_MAX_CHAR)
	{	
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[0]);	
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[9], MAINTENANCE_TEXT_MAX_CHAR);
		return;
	}
	
	if (strlen(reason) < MAINTENANCE_TEXT_MIN_CHAR && !!strcmp(reason, "rmf"))
	{	
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[0]);	
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[10], MAINTENANCE_TEXT_MIN_CHAR);
		return;
	}
	
	if (!strcmp(reason, "rmf")) 
	{
		char sReason[128];
		snprintf(sReason, sizeof(sReason), "UPDATE player.maintenance SET reason = 'no_reason'");
		std::unique_ptr<SQLMsg> pReason(DBManager::instance().DirectQuery(sReason));	
		
		ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[0]);	
		ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[11]);
		return;
	}	
			char sReason[128];
			snprintf(sReason, sizeof(sReason), "UPDATE player.maintenance SET `reason` = replace(\"%s\",' ','//')", reason);
			std::unique_ptr<SQLMsg> reasonReplace(DBManager::instance().DirectQuery(sReason));	
		
			ch->ChatPacket(CHAT_TYPE_INFO, maintenance_translate[0]);	
			ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[12]);	
			ch->ChatPacket(CHAT_TYPE_NOTICE, maintenance_translate[13], reason);	
}

void MaintenanceManager::Send_UpdateBinary(LPCHARACTER ch)
{
	if (NULL == ch)
		return;

	if (!ch->IsPC())
		return;
	
	if (ch)
	{
		SQLMsg * pMsg = DBManager::instance().DirectQuery("SELECT time,duration,reason from player.maintenance");
		
		if (pMsg->Get()->uiNumRows > 0)
		{
			MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_Update_Maintenance %s %s %s", row[0], row[1], row[2]);
			delete pMsg;
		}
	}
}

void MaintenanceManager::Send_CheckTable(LPCHARACTER ch)
{
	if (NULL == ch)
		return;

	if (!ch->IsPC())
		return;
	
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT time,duration FROM player.maintenance LIMIT 1"));	
	
	if (pMsg->Get()->uiNumRows == 0)	
		return;
		
	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	
	int sTime = 0;
	int sDuration = 0;
	
	str_to_number(sTime, row[0]);
	str_to_number(sDuration, row[1]);
			
	if (sTime > 0 && sDuration > 0)	
	{
		Send_UpdateBinary(ch);
	}
}