#include "stdafx.h"
#include "Application.h"

volatile int	num_events_called = 0;
int             max_bytes_written = 0;
int             current_bytes_written = 0;
int             total_bytes_written = 0;

int g_shutdown_disconnect_pulse;
int g_shutdown_disconnect_force_pulse;
int g_shutdown_core_pulse;
bool g_bShutdown = false;

LPFDWATCH	main_fdw = nullptr;

std::array<std::uint32_t, PROF_MAX_NUM> CApplication::m_dwProfiler{};



CApplication::CApplication() : m_tcp_socket{ 0 }, m_p2p_socket{ 0 }, m_fdWatcher{ nullptr }, m_eventHandler{m_context}
{
	__InitDependencies();
	m_dwProfiler.fill(0);
}

CApplication::~CApplication()
{

}

void CApplication::__InitDependencies()
{
#ifdef DEBUG_ALLOC
	DebugAllocator::StaticSetUp();
#endif

	ilInit(); // DevIL Initialize

	{
		extern void WriteVersion();
		WriteVersion();
	}
	
#ifdef __ENABLE_NEW_OFFLINESHOP__
	// if(!Offlineshop_InitializeLibrary("asperity", "8u2y723g76gtwggywgy21g")){
		// std::fprintf(stderr, "Cannot initialize Offshop Library\n");
		// return;
	// }
#endif
}

bool CApplication::__Start(int argc, char** argv)
{
	std::string st_localeServiceName;

	bool bVerbose = false;
	char ch;

	//_malloc_options = "A";
#if defined(__FreeBSD__) && defined(DEBUG_ALLOC)
	_malloc_message = WriteMallocMessage;
#endif

#ifdef ENABLE_NEWSTUFF
	while ((ch = getopt(argc, argv, "npverltIC")) != -1)
#else
	while ((ch = getopt(argc, argv, "npverltI")) != -1)
#endif
	{
		char* ep = NULL;

		switch (ch)
		{
		case 'I': // IP
			strlcpy(g_szPublicIP, argv[optind], sizeof(g_szPublicIP));

			printf("IP %s\n", g_szPublicIP);

			optind++;
			optreset = 1;
			break;

		case 'p': // port
			mother_port = strtol(argv[optind], &ep, 10);

			if (mother_port <= 1024)
			{
				__Usage();
				return false;
			}

			printf("port %d\n", mother_port);

			optind++;
			optreset = 1;
			break;

		case 'l':
		{
			long l = strtol(argv[optind], &ep, 10);

			log_set_level(l);

			optind++;
			optreset = 1;
		}
		break;

		// LOCALE_SERVICE
		case 'n':
		{
			if (optind < argc)
			{
				st_localeServiceName = argv[optind++];
				optreset = 1;
			}
		}
		break;
		// END_OF_LOCALE_SERVICE

#ifdef ENABLE_NEWSTUFF
		case 'C': // checkpoint check
			bCheckpointCheck = strtol(argv[optind], &ep, 10);;
			printf("CHECKPOINT_CHECK %d\n", bCheckpointCheck);

			optind++;
			optreset = 1;
			break;
#endif

		case 'v': // verbose
			bVerbose = true;
			break;

		case 'r':
			g_bNoRegen = true;
			break;

			// TRAFFIC_PROFILER
		case 't':
			g_bTrafficProfileOn = true;
			break;
			// END_OF_TRAFFIC_PROFILER
		}
	}



	// LOCALE_SERVICE
	config_init(st_localeServiceName);
	// END_OF_LOCALE_SERVICE

#ifdef __WIN32__
	// In Windows dev mode, "verbose" option is [on] by default.
	bVerbose = true;
#endif
	if (!bVerbose)
		freopen("stdout", "a", stdout);

	bool is_thecore_initialized = thecore_init(25, CApplication::HeartBeat);

	if (!is_thecore_initialized)
	{
		fprintf(stderr, "Could not initialize thecore, check owner of pid, syslog\n");
		exit(0);
	}

	if (false == CThreeWayWar::instance().LoadSetting("forkedmapindex.txt"))
	{
		if (false == g_bAuthServer)
		{
			fprintf(stderr, "Could not Load ThreeWayWar Setting file");
			exit(0);
		}
	}

	signal_timer_disable();

	m_fdWatcher = fdwatch_new(4096);
	main_fdw = m_fdWatcher;

	if ((m_tcp_socket = socket_tcp_bind(g_szPublicIP, mother_port)) == INVALID_SOCKET)
	{
		perror("socket_tcp_bind: tcp_socket");
		return 0;
	}


#ifndef __UDP_BLOCK__
	if ((udp_socket = socket_udp_bind(g_szPublicIP, mother_port)) == INVALID_SOCKET)
	{
		perror("socket_udp_bind: udp_socket");
		return 0;
	}
#endif

	// if internal ip exists, p2p socket uses internal ip, if not use public ip
	//if ((p2p_socket = socket_tcp_bind(*g_szInternalIP ? g_szInternalIP : g_szPublicIP, p2p_port)) == INVALID_SOCKET)
	if ((m_p2p_socket = socket_tcp_bind(g_szPublicIP, p2p_port)) == INVALID_SOCKET)
	{
		perror("socket_tcp_bind: p2p_socket");
		return 0;
	}

	fdwatch_add_fd(m_fdWatcher, m_tcp_socket, NULL, FDW_READ, false);
#ifndef __UDP_BLOCK__
	fdwatch_add_fd(m_fdWatcher, m_udp_socket, NULL, FDW_READ, false);
#endif
	fdwatch_add_fd(m_fdWatcher, m_p2p_socket, NULL, FDW_READ, false);

	db_clientdesc = DESC_MANAGER::instance().CreateConnectionDesc(m_fdWatcher, db_addr, db_port, PHASE_DBCLIENT, true);
	if (!g_bAuthServer) {
		db_clientdesc->UpdateChannelStatus(0, true);
	}

	if (g_bAuthServer)
	{
		if (g_stAuthMasterIP.length() != 0)
		{
			fprintf(stderr, "SlaveAuth");
			g_pkAuthMasterDesc = DESC_MANAGER::instance().CreateConnectionDesc(m_fdWatcher, g_stAuthMasterIP.c_str(), g_wAuthMasterPort, PHASE_P2P, true);
			P2P_MANAGER::instance().RegisterConnector(g_pkAuthMasterDesc);
			g_pkAuthMasterDesc->SetP2P(g_stAuthMasterIP.c_str(), g_wAuthMasterPort, g_bChannel);

		}
		else
		{
			fprintf(stderr, "MasterAuth %d\n", LC_GetLocalType());
		}
	}
	/* game server to teen server */
	else
	{
		if (teen_addr[0] && teen_port)
			g_TeenDesc = DESC_MANAGER::instance().CreateConnectionDesc(m_fdWatcher, teen_addr, teen_port, PHASE_TEEN, true);

		sys_log(0, "SPAM_CONFIG: duration %u score %u reload cycle %u\n",
			g_uiSpamBlockDuration, g_uiSpamBlockScore, g_uiSpamReloadCycle);

		extern void LoadSpamDB();
		LoadSpamDB();
	}

	signal_timer_enable(30);

	return true;
}

bool CApplication::__Initialize()
{
	if (!m_quest_manager.Initialize()) {
		__CleanUpForEarlyExit();
		return false;
	}

	MessengerManager::instance().Initialize();
	CGuildManager::instance().Initialize();
	fishing::Initialize();
	m_OXEvent_manager.Initialize();
#ifdef ENABLE_CSHIELD
	if (!g_bAuthServer)
	{
		InitCShield();
	}
#endif
	if (speed_server)
		CSpeedServerManager::instance().Initialize();

#ifdef __LOGIN_REGISTER__
	if (!g_isRegisterServer)
#endif
	{
		Cube_init();
		Blend_Item_init();
		ani_init();
	}
	PanamaLoad();

	{
		// TRAFFIC_PROFILER
		static const DWORD	TRAFFIC_PROFILE_FLUSH_CYCLE = 3600;	///< TrafficProfiler �� Flush cycle. 1�ð� ����
		// END_OF_TRAFFIC_PROFILER

		if (g_bTrafficProfileOn)
			TrafficProfiler::instance().Initialize(TRAFFIC_PROFILE_FLUSH_CYCLE, "ProfileLog");
	}

	//TODO : make it config
	const std::string strPackageCryptInfoDir = "package/";
	if (!m_desc_manager.LoadClientPackageCryptInfo(strPackageCryptInfoDir.c_str()))
	{
		sys_err("Failed to Load ClientPackageCryptInfo File(%s)", strPackageCryptInfoDir.c_str());
	}

#ifdef __ENABLE_NEW_OFFLINESHOP__
	offlineshop::CShopManager::Instance().Start();
#endif


	return true;
}


bool CApplication::Run(int argc, char** argv)
{
	if (!__Start(argc, argv))
		return false;

	if (!__Initialize())
		return false;

	__RunIdle();

	m_context.run();

	return true;
}

void CApplication::Destroy()
{
	sys_log(0, "<shutdown> Destroying CArenaManager...");
	m_arena_manager.Destroy();
	sys_log(0, "<shutdown> Destroying COXEventManager...");
	m_OXEvent_manager.Destroy();

	sys_log(0, "<shutdown> Disabling signal timer...");
	signal_timer_disable();

	sys_log(0, "<shutdown> Shutting down CHARACTER_MANAGER...");
	m_char_manager.GracefulShutdown();
	sys_log(0, "<shutdown> Shutting down ITEM_MANAGER...");
	m_item_manager.GracefulShutdown();

	sys_log(0, "<shutdown> Flushing db_clientdesc...");
	db_clientdesc->FlushOutput();
	sys_log(0, "<shutdown> Flushing p2p_manager...");
	m_p2p_manager.FlushOutput();

	sys_log(0, "<shutdown> Destroying CShopManager...");
	m_shop_manager.Destroy();
	sys_log(0, "<shutdown> Destroying CHARACTER_MANAGER...");
	m_char_manager.Destroy();
	sys_log(0, "<shutdown> Destroying ITEM_MANAGER...");
	m_item_manager.Destroy();
	sys_log(0, "<shutdown> Destroying DESC_MANAGER...");
	m_desc_manager.Destroy();
	sys_log(0, "<shutdown> Destroying quest::CQuestManager...");
	m_quest_manager.Destroy();
	sys_log(0, "<shutdown> Destroying building::CManager...");
	m_building_manager.Destroy();
	
#ifdef __ENABLE_NEW_OFFLINESHOP__
	sys_log(0, "<shutdown> Destroying OfflineShopManager...");
	m_offshopManager.Destroy();
#endif

	if (!g_bAuthServer)
	{
		if (isHackShieldEnable)
		{
			sys_log(0, "<shutdown> Releasing HackShield manager...");
			m_HSManager.Release();
		}
	}

	sys_log(0, "<shutdown> Flushing TrafficProfiler...");
	m_trafficProfiler.Flush();

#ifdef DEBUG_ALLOC
	DebugAllocator::StaticTearDown();
#endif

#ifdef __NEW_EVENT_HANDLER__
	sys_log(0, "<shutdown> Destroying CEventFunctionHandler...");
	CEventFunctionHandler::instance().Destroy();
#endif

	sys_log(0, "<shutdown> Canceling ReloadSpamEvent...");
	__CleanUpForEarlyExit();

	sys_log(0, "<shutdown> regen_free()...");
	regen_free();

	sys_log(0, "<shutdown> Closing sockets...");
	socket_close(m_tcp_socket);
#ifndef __UDP_BLOCK__
	socket_close(m_udp_socket);
#endif
	socket_close(m_p2p_socket);

	sys_log(0, "<shutdown> fdwatch_delete()...");
	fdwatch_delete(m_fdWatcher);

	m_fdWatcher = nullptr;
	main_fdw = nullptr;

	sys_log(0, "<shutdown> event_destroy()...");
	event_destroy();

	sys_log(0, "<shutdown> CTextFileLoader::DestroySystem()...");
	CTextFileLoader::DestroySystem();

	sys_log(0, "<shutdown> thecore_destroy()...");
	thecore_destroy();

}

void CApplication::HeartBeat(LPHEART ht, int pulse)
{
	DWORD t;

	t = get_dword_time();
	num_events_called += event_process(pulse);
	CApplication::m_dwProfiler[PROF_EVENT] += (get_dword_time() - t);

	t = get_dword_time();

	// 1�ʸ���
	if (!(pulse % ht->passes_per_sec))
	{
#ifdef __LOGIN_REWARD__
		CLoginRewardManager::instance().Update();
#endif

#ifdef ENABLE_BRAZIL_AUTH_FEATURE // @warme006
		if (g_bAuthServer && !test_server)
			auth_brazil_log();
#endif

		if (!g_bAuthServer)
		{
			TPlayerCountPacket pack;
			pack.dwCount = DESC_MANAGER::instance().GetLocalUserCount();
			db_clientdesc->DBPacket(HEADER_GD_PLAYER_COUNT, 0, &pack, sizeof(TPlayerCountPacket));
		}
		else
		{
			DESC_MANAGER::instance().ProcessExpiredLoginKey();
			DBManager::instance().FlushBilling();
			/*
			   if (!(pulse % (ht->passes_per_sec * 600)))
			   DBManager::instance().CheckBilling();
			 */
		}
	}

	//
	// 25 PPS(Pulse per second) ��� ������ ��
	//

	// �� 1.16�ʸ���
	if (!(pulse % (passes_per_sec + 4)))
		CHARACTER_MANAGER::instance().ProcessDelayedSave();

	//4�� ����
#if defined (__FreeBSD__) && defined(__FILEMONITOR__)
	if (!(pulse % (passes_per_sec * 5)))
	{
		FileMonitorFreeBSD::Instance().Update(pulse);
	}
#endif

	// �� 5.08�ʸ���
	if (!(pulse % (passes_per_sec * 5 + 2)))
	{
		ITEM_MANAGER::instance().Update();
		DESC_MANAGER::instance().UpdateLocalUserCount();
	}

	if (!(ht->pulse % (ht->passes_per_sec * (5 * 60))))
	{
		sys_log(0, "FlushSusConnections ...");
		DESC_MANAGER::instance().FlushSusConnections();
	}

	CApplication::m_dwProfiler[PROF_HEARTBEAT] += (get_dword_time() - t);

	DBManager::instance().Process();
	AccountDB::instance().Process();
	CPVPManager::instance().Process();

	if (g_bShutdown)
	{
		if (thecore_pulse() > g_shutdown_disconnect_pulse)
		{
			const DESC_MANAGER::DESC_SET& c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), [](LPDESC d) {
				if (!d) return;
				if (auto ch = d->GetCharacter(); ch)
				{
					if (ch->GetGMLevel() == GM_PLAYER)
						ch->ChatPacket(CHAT_TYPE_COMMAND, "quit Shutdown(SendDisconnectFunc)");
				}
				});
			g_shutdown_disconnect_pulse = INT_MAX;
		}
		else if (thecore_pulse() > g_shutdown_disconnect_force_pulse)
		{
			const DESC_MANAGER::DESC_SET& c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), [](LPDESC d) {
				if (d->GetType() == DESC_TYPE_CONNECTOR)
					return;

				if (d->IsPhase(PHASE_P2P))
					return;

				d->SetPhase(PHASE_CLOSE);
				});
		}
		else if (thecore_pulse() > g_shutdown_disconnect_force_pulse + PASSES_PER_SEC(5))
		{
			thecore_shutdown();
		}
	}
}

void CApplication::__CleanUpForEarlyExit()
{
	extern void CancelReloadSpamEvent();
	CancelReloadSpamEvent();
}

void CApplication::__Usage()
{
	std::printf("Option list\n"
		"-p <port>    : bind port number (port must be over 1024)\n"
		"-l <level>   : sets log level\n"
		"-n <locale>  : sets locale name\n"
#ifdef ENABLE_NEWSTUFF
		"-C <on-off>  : checkpointing check on/off\n"
#endif
		"-v           : log to stdout\n"
		"-r           : do not load regen tables\n"
		"-t           : traffic profile on\n");
}

void CApplication::__RunIdle()
{
	m_context.post([self = shared_from_this()]() {
		auto r = self->__Idle();
		if (!r)
		{
			g_bShutdown = true;
			g_bNoMoreClient = true;

			if (g_bAuthServer)
			{
				DBManager::instance().FlushBilling(true);

				int iLimit = DBManager::instance().CountQuery() / 50;
				int i = 0;

				do
				{
					DWORD dwCount = DBManager::instance().CountQuery();
					sys_log(0, "Queries %u", dwCount);

					if (dwCount == 0)
						break;

					usleep(500000);

					if (++i >= iLimit)
						if (dwCount == DBManager::instance().CountQuery())
							break;
				} while (1);
			}

			sys_err("Idle failed! err id %d", r);
		}
		else
		{
			self->__RunIdle();
		}
	});
}

int CApplication::__IoLoop()
{
	LPDESC	d;
	int		num_events, event_idx;

	DESC_MANAGER::instance().DestroyClosed(); // PHASE_CLOSE�� ���ӵ��� �����ش�.
	DESC_MANAGER::instance().TryConnect();

	if ((num_events = fdwatch(m_fdWatcher, 0)) < 0)
		return 0;

	for (event_idx = 0; event_idx < num_events; ++event_idx)
	{
		d = (LPDESC)fdwatch_get_client_data(m_fdWatcher, event_idx);

		if (!d)
		{
			if (FDW_READ == fdwatch_check_event(m_fdWatcher, m_tcp_socket, event_idx))
			{
				DESC_MANAGER::instance().AcceptDesc(m_fdWatcher, m_tcp_socket);
				fdwatch_clear_event(m_fdWatcher, m_tcp_socket, event_idx);
			}
			else if (FDW_READ == fdwatch_check_event(m_fdWatcher, m_p2p_socket, event_idx))
			{
				DESC_MANAGER::instance().AcceptP2PDesc(m_fdWatcher, m_p2p_socket);
				fdwatch_clear_event(m_fdWatcher, m_p2p_socket, event_idx);
			}
			/*
			else if (FDW_READ == fdwatch_check_event(fdw, udp_socket, event_idx))
			{
				char			buf[256];
				struct sockaddr_in	cliaddr;
				socklen_t		socklen = sizeof(cliaddr);

				int iBytesRead;

				if ((iBytesRead = socket_udp_read(udp_socket, buf, 256, (struct sockaddr *) &cliaddr, &socklen)) > 0)
				{
					static CInputUDP s_inputUDP;

					s_inputUDP.SetSockAddr(cliaddr);

					int iBytesProceed;
					s_inputUDP.Process(NULL, buf, iBytesRead, iBytesProceed);
				}

				fdwatch_clear_event(fdw, udp_socket, event_idx);
			}
			*/
			continue;
		}

		int iRet = fdwatch_check_event(m_fdWatcher, d->GetSocket(), event_idx);

		switch (iRet)
		{
		case FDW_READ:
			if (db_clientdesc == d)
			{
				int size = d->ProcessInput();

				if (size)
					sys_log(1, "DB_BYTES_READ: %d", size);

				if (size < 0)
				{
					d->SetPhase(PHASE_CLOSE);
				}
			}
			else if (d->ProcessInput() < 0)
			{
				d->SetPhase(PHASE_CLOSE);
			}
			break;

		case FDW_WRITE:
			if (db_clientdesc == d)
			{
				int buf_size = buffer_size(d->GetOutputBuffer());
				int sock_buf_size = fdwatch_get_buffer_size(m_fdWatcher, d->GetSocket());

				int ret = d->ProcessOutput();

				if (ret < 0)
				{
					d->SetPhase(PHASE_CLOSE);
				}

				if (buf_size)
					sys_log(1, "DB_BYTES_WRITE: size %d sock_buf %d ret %d", buf_size, sock_buf_size, ret);
			}
			else if (d->ProcessOutput() < 0)
			{
				d->SetPhase(PHASE_CLOSE);
			}
			else if (g_TeenDesc == d)
			{
				int buf_size = buffer_size(d->GetOutputBuffer());
				int sock_buf_size = fdwatch_get_buffer_size(m_fdWatcher, d->GetSocket());

				int ret = d->ProcessOutput();

				if (ret < 0)
				{
					d->SetPhase(PHASE_CLOSE);
				}

				if (buf_size)
					sys_log(0, "TEEN::Send(size %d sock_buf %d ret %d)", buf_size, sock_buf_size, ret);
			}
			break;

		case FDW_EOF:
		{
			d->SetPhase(PHASE_CLOSE);
		}
		break;

		default:
			sys_err("fdwatch_check_event returned unknown %d", iRet);
			d->SetPhase(PHASE_CLOSE);
			break;
		}
	}

	return 1;
}
int CApplication::__Idle()
{
	static struct timeval	pta = { 0, 0 };
	static int			process_time_count = 0;
	struct timeval		now;

	if (pta.tv_sec == 0)
		gettimeofday(&pta, (struct timezone*)0);

	int passed_pulses;

	if (!(passed_pulses = thecore_idle()))
		return 0;

	assert(passed_pulses > 0);

	DWORD t;

	while (passed_pulses--) {
		CApplication::HeartBeat(thecore_heart, ++thecore_heart->pulse);

		// To reduce the possibility of abort() in checkpointing
		thecore_tick();
	}

	t = get_dword_time();
	CHARACTER_MANAGER::instance().Update(thecore_heart->pulse);
	db_clientdesc->Update(t);
	m_dwProfiler[PROF_CHR_UPDATE] += (get_dword_time() - t);

	t = get_dword_time();
	if (!__IoLoop()) return 0;
	m_dwProfiler[PROF_IO] += (get_dword_time() - t);

	log_rotate();

	gettimeofday(&now, (struct timezone*)0);
	++process_time_count;

	if (now.tv_sec - pta.tv_sec > 0)
	{
		pt_log("[%3d] event %5d/%-5d idle %-4ld event %-4ld heartbeat %-4ld I/O %-4ld chrUpate %-4ld | WRITE: %-7d | PULSE: %d",
			process_time_count,
			num_events_called,
			event_count(),
			thecore_profiler[PF_IDLE],
			m_dwProfiler[PROF_EVENT],
			m_dwProfiler[PROF_HEARTBEAT],
			m_dwProfiler[PROF_IO],
			m_dwProfiler[PROF_CHR_UPDATE],
			current_bytes_written,
			thecore_pulse());

		num_events_called = 0;
		current_bytes_written = 0;

		process_time_count = 0;
		gettimeofday(&pta, (struct timezone*)0);

		memset(&thecore_profiler[0], 0, sizeof(thecore_profiler));
		memset(&m_dwProfiler[0], 0, sizeof(m_dwProfiler));
	}

#ifdef __WIN32__
	if (_kbhit()) {
		int c = _getch();
		switch (c) {
		case 0x1b: // Esc
			return 0; // shutdown
			break;
		default:
			break;
		}
	}
#endif

#ifdef __NEW_EVENT_HANDLER__
	CEventFunctionHandler::instance().Process();
#endif

	return 1;
}