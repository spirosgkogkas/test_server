/*********************************************************************
 * starting	   : 02.07.2017
 * last_version: 16.09.2020
 * file        : GroupMatchManager.cpp
 * author      : blackdragonx61
 * description : GameForge Version: 17.3 (Summer) Group Search System
 */

#include "stdafx.h"

#if defined(ENABLE_PARTY_MATCH)
#include "GroupMatchManager.h"
#include "char.h"
#include "packet.h"
#include "desc.h"
#include "party.h"

CGroupMatchManager::CGroupMatchManager()
{
}

CGroupMatchManager::~CGroupMatchManager()
{
	SearchMap.clear();
}

static auto& Coordinates() {
	struct MapObject {
		int x, y, level, player_count;
		std::vector<std::pair<DWORD, BYTE>> items;
		MapObject(int m_x, int m_y, int m_level, int pcount, std::vector<std::pair<DWORD, BYTE>> m_items)
			: x(m_x), y(m_y), level(m_level), player_count(pcount), items(std::move(m_items))
		{}
	};

	static std::map<int, std::unique_ptr<MapObject>> Map;

	auto AddObject = [&](int index, int x, int y, int level, int pcount, std::vector<std::pair<DWORD, BYTE>> items) {
		auto p = std::make_unique<MapObject>(x, y, level, pcount, items);
		Map.emplace(index, std::move(p));
	};

	if (Map.empty()) {
		//AddObject(351, X, Y, LEVEL, COUNT, { {VNUM1, COUNT1}, {VNUM2, COUNT2}, {VNUM3, COUNT3} });
		//AddObject(352, 1234, 1234, 100, 2, {});
	}

	return Map;
};

void CGroupMatchManager::SendPacket(LPCHARACTER ch, BYTE SubHeader, BYTE MSG, DWORD index)
{
	if (!ch || !ch->GetDesc())
		return;

	TPacketGCPartyMatch p;

	p.Header = HEADER_GC_PARTY_MATCH;
	p.SubHeader = SubHeader;
	p.MSG = MSG;
	p.index = index;

	ch->GetDesc()->Packet(&p, sizeof(TPacketGCPartyMatch));
}

void CGroupMatchManager::AddSearcher(LPCHARACTER ch, int index)
{
	if (!ch)
		return;

	if (IsSearching(ch) != SearchMap.end()) {
		StopSearching(ch, eMSG::PARTY_MATCH_HOLD, index);
		return;
	}

	if (!index) {
		StopSearching(ch, eMSG::PARTY_MATCH_FAIL_NONE_MAP_INDEX, index);
		return;
	}

	const auto& TCoordinates = Coordinates();
	if (TCoordinates.find(index) == TCoordinates.end()) {
		StopSearching(ch, eMSG::PARTY_MATCH_FAIL_IMPOSSIBLE_MAP, index);
		return;
	}

	if (TCoordinates.at(index)->level > ch->GetLevel()) {
		StopSearching(ch, eMSG::PARTY_MATCH_FAIL_LEVEL, index);
		return;
	}

	const auto vnum = CheckItems(ch, index);
	if (vnum) {
		StopSearching(ch, eMSG::PARTY_MATCH_FAIL_NO_ITEM, vnum);
		return;
	}

	if (IsRestricted(ch->GetMapIndex())) {
		StopSearching(ch, eMSG::PARTY_MATCH_FAIL_IMPOSSIBLE_MAP, index);
		return;
	}

	if (ch->GetParty()) {
		StopSearching(ch, eMSG::PARTY_MATCH_FAIL, index);
		return;
	}

	SearchMap.emplace(index, ch);
	SendPacket(ch, eHeader::PARTY_MATCH_SEARCH, eMSG::PARTY_MATCH_INFO, index);
	CheckPlayers(index);
}

decltype(CGroupMatchManager::SearchMap)::const_iterator CGroupMatchManager::IsSearching(LPCHARACTER ch)
{
	if (ch)
		for (auto it = SearchMap.begin(); it != SearchMap.end(); ++it)
			if (it->second == ch)
				return it;
	return SearchMap.end();
}

void CGroupMatchManager::StopSearching(LPCHARACTER ch, BYTE MSG, int index)
{
	if (ch) {
		auto it = IsSearching(ch);
		if (it != SearchMap.end())
			SearchMap.erase(it);

		SendPacket(ch, eHeader::PARTY_MATCH_CANCEL, MSG, index);
	}
}

bool CGroupMatchManager::CheckParty(int index)
{
	bool Check = true;

	auto range = SearchMap.equal_range(index);

	std::unordered_set<LPCHARACTER> junk;
	for (auto i = range.first; i != range.second; ++i) {
		auto ch = i->second;
		if (ch && ch->GetParty()) {
			junk.insert(ch);
			Check = false;
		}
	}

	for (auto v : junk)
		StopSearching(v, eMSG::PARTY_MATCH_FAIL, index);

	return Check;
}

bool CGroupMatchManager::CheckItems(int index)
{
	bool check = true;
	auto range = SearchMap.equal_range(index);

	std::map<LPCHARACTER, int> junk;
	for (auto i = range.first; i != range.second; ++i) {
		auto ch = i->second;
		if (ch) {
			const auto vnum = CheckItems(ch, index);
			if (vnum) {
				junk.emplace(ch, vnum);
				check = false;
			}
		}
	}

	for (auto v : junk)
		StopSearching(v.first, eMSG::PARTY_MATCH_FAIL_NO_ITEM, v.second);

	return check;
}

DWORD CGroupMatchManager::CheckItems(LPCHARACTER ch, int index) const
{
	if (ch)
		for (const auto& v : Coordinates().at(index)->items)
			if (ch->CountSpecifyItem(v.first) < v.second)
				return v.first;

	return 0;
}

void CGroupMatchManager::EraseItems(LPCHARACTER ch, int index)
{
	if (ch)
		for (const auto& v : Coordinates().at(index)->items)
			ch->RemoveSpecifyItem(v.first, v.second);
}

void CGroupMatchManager::CheckPlayers(int index)
{
	const auto& TCoordinates = Coordinates();
	if (TCoordinates.find(index) == TCoordinates.end())
		return;

	if (static_cast<int>(GetPlayerCount(index)) < TCoordinates.at(index)->player_count)
		return;

	if (!CheckParty(index))
		return;

	if (!CheckItems(index))
		return;

	auto range = SearchMap.equal_range(index);

	LPPARTY party = nullptr;
	for (auto i = range.first; i != range.second; ++i) {
		auto ch = i->second;
		if (!ch)
			continue;

		EraseItems(ch, index);

		if (!party)
			party = CPartyManager::instance().CreateParty(ch);
		else {
			party->Join(ch->GetPlayerID());
			party->Link(ch);
		}

		party->SendPartyInfoAllToOne(party->GetLeaderCharacter());
		SendPacket(ch, eHeader::PARTY_MATCH_CANCEL, eMSG::PARTY_MATCH_SUCCESS, index);
	}

	for (auto i = range.first; i != range.second; ++i) {
		auto ch = i->second;
		if (ch)
			ch->WarpSet(TCoordinates.at(index)->x * 100, TCoordinates.at(index)->y * 100);
	}

	SearchMap.erase(index);
}

static std::unordered_set<int> s_RestrictedMaps;
void CGroupMatchManager::AddRestricted(int mapIndex)
{
	s_RestrictedMaps.emplace(mapIndex);
}
bool CGroupMatchManager::IsRestricted(int mapIndex) const
{
	return s_RestrictedMaps.find(mapIndex) != s_RestrictedMaps.end();
}

#endif