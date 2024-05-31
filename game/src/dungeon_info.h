/*
* Date : 2021.01.12
* File : DungeonInfoManager.h
* Author : Owsap
* Description : Dungeon Information System
*/

#if !defined(_DUNGEON_INFO_MANAGER_H_)
#define _DUNGEON_INFO_MANAGER_H_

#define DUNGEON_TOKEN(string) if (!str_cmp(szTokenString, string))

enum EQuestType
{
	QUEST_FLAG_PC,
	QUEST_FLAG_GLOBAL
};

struct SDungeonLimits
{
	UINT iMin, iMax;
	bool operator == (const SDungeonLimits& c_sRef)
	{
		return (this->iMin == c_sRef.iMin)
			&& (this->iMax == c_sRef.iMax);
	}
};

struct SDungeonEntryPosition
{
	long lBaseX, lBaseY;
	bool operator == (const SDungeonEntryPosition& c_sRef)
	{
		return (this->lBaseX == c_sRef.lBaseX)
			&& (this->lBaseY == c_sRef.lBaseY);
	}
};

struct SDungeonBonus
{
	BYTE bAttBonus, bDefBonus;
	bool operator == (const SDungeonBonus& c_sRef)
	{
		return (this->bAttBonus == c_sRef.bAttBonus)
			&& (this->bDefBonus == c_sRef.bDefBonus);
	}
};

struct SDungeonItem
{
	DWORD dwVnum;
	BYTE bCount;

	bool operator == (const SDungeonItem& c_sRef)
	{
		return (this->bCount == c_sRef.bCount)
			&& (this->dwVnum == c_sRef.dwVnum);
	}
};

struct SDungeonQuest
{
	std::string strQuest;
	std::string strQuestFlag;
	BYTE bType;

	bool operator == (const SDungeonQuest& c_sRef)
	{
		return (this->bType == c_sRef.bType) &&
			(this->strQuestFlag == c_sRef.strQuestFlag)
			&& (this->strQuest == c_sRef.strQuest);
	}
};

struct SDungeonData
{
	BYTE bType = 0;

	long lMapIndex = 0;
	long lEntryMapIndex = 0;

	std::vector<SDungeonEntryPosition> vecEntryPosition;

	DWORD dwBossVnum = 0;

	std::vector<SDungeonLimits> vecLevelLimit;
	std::vector<SDungeonLimits> vecMemberLimit;
	std::vector<SDungeonItem> vecRequiredItem;

	DWORD dwDuration = 0;
	BYTE bElement = 0;

	std::vector<SDungeonBonus> vecBonus;
	std::vector<SDungeonItem> vecBossDropItem;

	std::vector<SDungeonQuest> vecQuest;

	SDungeonData();
};

class CDungeonInfoManager : public singleton<CDungeonInfoManager>
{
public:

	enum EDungeonInfo
	{
		MAX_REQUIRED_ITEMS = 3,
		MAX_BOSS_DROP_ITEMS = 255,
		MAX_DUNGEONS = 100,
	};

	enum EDungeonInfoActions
	{
		CLOSE,
		OPEN,
		WARP,
		RANK
	};

	enum EDungeonRanking
	{
		MAX_RANKING_LINES = 10,
		MAX_RANKING_COUNT = 10,
	};

	enum EDungeonRankingTypes
	{
		RANKING_TYPE_COMPLETED = 1,
		RANKING_TYPE_TIME = 2,
		RANKING_TYPE_DAMAGE = 3,

		MAX_RANKING_TYPE,
	};

	CDungeonInfoManager();
	virtual ~CDungeonInfoManager();

	void Destroy();

	void Initialize();
	bool LoadFile(const char* c_szFileName);

	void Reload();

	void SendInfo(LPCHARACTER pkCh, BOOL bReset = FALSE);
	void Warp(LPCHARACTER pkCh, BYTE bIndex);
	UINT GetResult(LPCHARACTER pkCh, std::string strQuest, std::string strFlag);
	void Ranking(LPCHARACTER pkCh, BYTE bIndex, BYTE bRankType = 0);
	void UpdateRanking(LPCHARACTER pkCh, const std::string c_strQuestName);

};

#endif /* _DUNGEON_INFO_MANAGER_H_ */