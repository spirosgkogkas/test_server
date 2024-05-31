#pragma once

#if defined(ENABLE_PARTY_MATCH)
#include "char.h"

class CGroupMatchManager : public singleton<CGroupMatchManager>
{
private:
	std::unordered_multimap<int, LPCHARACTER> SearchMap;

public:
	CGroupMatchManager();
	~CGroupMatchManager();

	enum eHeader { PARTY_MATCH_SEARCH, PARTY_MATCH_CANCEL };
	enum eMSG {
		PARTY_MATCH_INFO = PARTY_MATCH_SEARCH, PARTY_MATCH_FAIL, PARTY_MATCH_SUCCESS,
		PARTY_MATCH_START, PARTY_MATCH_CANCEL_SUCCESS, PARTY_MATCH_FAIL_NO_ITEM, PARTY_MATCH_FAIL_LEVEL,
		PARTY_MATCH_FAIL_NOT_LEADER, PARTY_MATCH_FAIL_MEMBER_NOT_CONDITION, PARTY_MATCH_FAIL_NONE_MAP_INDEX,
		PARTY_MATCH_FAIL_IMPOSSIBLE_MAP, PARTY_MATCH_HOLD, PARTY_MATCH_FAIL_FULL_MEMBER,
	};

	void AddSearcher(LPCHARACTER ch, int index);
	void CheckPlayers(int index);

	DWORD CheckItems(LPCHARACTER ch, int index) const;
	bool CheckItems(int index);
	bool CheckParty(int index);
	void EraseItems(LPCHARACTER ch, int index);

	void StopSearching(LPCHARACTER ch, BYTE MSG, int index);
	decltype(SearchMap)::const_iterator IsSearching(LPCHARACTER ch);

	void SendPacket(LPCHARACTER ch, BYTE SubHeader, BYTE MSG, DWORD index);

	bool IsRestricted(int mapIndex) const;
	void AddRestricted(int mapIndex);
	size_t GetPlayerCount(int mapIndex) const { return SearchMap.count(mapIndex); }
	
};
#endif
