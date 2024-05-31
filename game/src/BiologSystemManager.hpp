#pragma once
#include "stdafx.h"

#ifdef __ENABLE_BIOLOG_SYSTEM__
#include "../../common/tables.h"
#include "affect.h"

class CBiologSystem {
	const DWORD BIOLOG_CHANCE_ITEM = 71035;
	const DWORD BIOLOG_TIME_ITEM = 172001;
	const int BIOLOG_AFF_TIME = 60 * 60 * 60 * 365;
	const int AFFECTS[20] = { AFF_BIO_1, AFF_BIO_2, AFF_BIO_3, AFF_BIO_4, AFF_BIO_5, AFF_BIO_6, AFF_BIO_7, AFF_BIO_8, AFF_BIO_9, AFF_BIO_10, AFF_BIO_11, AFF_BIO_12, AFF_BIO_13, AFF_BIO_14, AFF_BIO_15, AFF_BIO_16, AFF_BIO_17, AFF_BIO_18, AFF_BIO_19, AFF_BIO_20 };

	public:
		enum ECGPackets {
			CG_BIOLOG_OPEN,
			CG_BIOLOG_SEND,
			CG_BIOLOG_TIMER,
		};

		enum EGCPackets {
			GC_BIOLOG_OPEN,
			GC_BIOLOG_ALERT,
			GC_BIOLOG_CLOSE,
		};

	public:
		CBiologSystem(LPCHARACTER m_pkChar);
		~CBiologSystem();

		// update functions
		void	ResetMission();

		// general functions
		void	SendBiologInformation(bool bUpdate = false);
		void	SendBiologItem(bool bAdditionalChance, bool bAdditionalTime);

		void	GiveRewardByMission(BYTE bMission);

		bool	GetBiologItemByMobVnum(LPCHARACTER pkKiller, WORD MonsterVnum, DWORD& ItemVnum, BYTE& bChance);
		
		// timer functions
		void	ActiveAlert(bool bReminder);
		void	BroadcastAlert();

		// incoming packet functions
		int		RecvClientPacket(BYTE bSubHeader, const char* c_pData, size_t uiBytes);

		// outgoing packet functions
		void	SendClientPacket(LPDESC pkDesc, BYTE bSubHeader, const void* c_pvData, size_t iSize);
		void	SendClientPacket(DWORD dwPID, BYTE bSubHeader, const void* c_pvData, size_t iSize);

	private:
		LPCHARACTER m_pkChar;
		std::string s_current_biolog_reminder;
};

class CBiologSystemManager : public singleton<CBiologSystemManager> {


	public:
		typedef std::map<BYTE, TBiologMissionsProto> TMissionProtoMap;
		typedef std::map<BYTE, TBiologRewardsProto> TRewardProtoMap;
		typedef std::map<BYTE, std::vector<TBiologMonstersProto*>> TMonstersProtoMap;

	public:
		CBiologSystemManager();
		~CBiologSystemManager();

		void	InitializeMissions(TBiologMissionsProto* pData, size_t size);
		TBiologMissionsProto* GetMission(BYTE bMission);

		void	InitializeRewards(TBiologRewardsProto* pData, size_t size);
		TBiologRewardsProto* GetReward(BYTE bMission);

		void	InitializeMonsters(TBiologMonstersProto* pData, size_t size);
		bool	GetMonsters(BYTE bMission, std::vector<TBiologMonstersProto*>& monsters);

		friend void CBiologSystem::SendBiologItem(bool bAdditionalChance, bool bAdditionalTime);

	private:
		TMissionProtoMap	m_mapMission_Proto;
		TRewardProtoMap		m_mapReward_Proto;
		TMonstersProtoMap	m_mapMonsters_Proto;
};
#endif
