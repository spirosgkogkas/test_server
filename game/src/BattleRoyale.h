#if defined(BATTLE_ROYAL_SYSTEM)
extern int BATTLE_ROYALE_STATUS;
extern int BATTLE_ROYAL_GIFT_STATUS;

extern int HOUR_1;
extern int MINUTE_1;
extern int SECOND_1;

extern int HOUR_2;
extern int MINUTE_2;
extern int SECOND_2;

extern int HOUR_3;
extern int MINUTE_3;
extern int SECOND_3;

extern int ITEM_VNUM;
extern int ITEM_COUNT;

extern int RESET_RANK_HOUR;
extern int RESET_RANK_MINUTE;
extern int RESET_RANK_SECOND;
extern int RANK_DAY;

extern int DEFAULT_DAMAGE;
extern int COMMON_DAMAGE;
extern int RARE_DAMAGE;
extern int EPIC_DAMAGE;
extern int COMMON_ARMOR;
extern int RARE_ARMOR;
extern int EPIC_ARMOR;

extern int ORIGO_X;
extern int ORIGO_Y;
extern int ORIGO_X_2;
extern int ORIGO_Y_2;

enum EBoxVnum
{
	BOX_VNUM = 9020,
	SWORD_BOX = 9021,
	ARMOR_BOX = 9022,
};

enum EKillPerPoint
{
	KILL_PER_POINT = 1,
};

enum EHPSetting
{
	START_HP_ON_MAP = 1000,

	PLUS_HP = 50,
};

enum EStartRange_1TimeBegin
{
	RANGETIME_1_BEGIN = 05,
	RANGETIME_2_BEGIN = 05,
	RANGETIME_3_BEGIN = 05,
};

enum EStartRange_1TimeEnd
{
	RANGETIME_1_END = 05,
	RANGETIME_2_END = 05,
	RANGETIME_3_END = 05,
};

enum EStartRange_2TimeEnd
{
	RANGETIME2_1_END = 05,
	RANGETIME2_2_END = 05,
	RANGETIME2_3_END = 05,
};

enum EBattleRoyaleEndEvent
{
	EVENT_1_END = 05,
	EVENT_2_END = 05,
	EVENT_3_END = 05,
};

enum EItemWarriorWeapon
{
	WARRIOR_SWORD = 500,
	WARRIOR_SWORD_2 = 501,
	WARRIOR_SWORD_3 = 502,

	RARE_WARRIOR_SWORD = 503,
	RARE_WARRIOR_SWORD_2 = 504,
	RARE_WARRIOR_SWORD_3 = 505,

	EPIC_WARRIOR_SWORD = 506,
	EPIC_WARRIOR_SWORD_2 = 507,
	EPIC_WARRIOR_SWORD_3 = 508,


	WARROIR_TWOHAND = 510,
	WARROIR_TWOHAND_2 = 511,
	WARROIR_TWOHAND_3 = 512,

	RARE_WARROIR_TWOHAND = 513,
	RARE_WARROIR_TWOHAND_2 = 514,
	RARE_WARROIR_TWOHAND_3 = 515,

	EPIC_WARROIR_TWOHAND = 516,
	EPIC_WARROIR_TWOHAND_2 = 517,
	EPIC_WARROIR_TWOHAND_3 = 518
};

enum EItemSuraWeapon
{
	SURA_SWORD = WARRIOR_SWORD,
	SURA_SWORD_2 = WARRIOR_SWORD_2,
	SURA_SWORD_3 = WARRIOR_SWORD_3,

	RARE_SURA_SWORD = RARE_WARRIOR_SWORD,
	RARE_SURA_SWORD_2 = RARE_WARRIOR_SWORD_2,
	RARE_SURA_SWORD_3 = RARE_WARRIOR_SWORD_3,

	EPIC_SURA_SWORD = EPIC_WARRIOR_SWORD,
	EPIC_SURA_SWORD_2 = EPIC_WARRIOR_SWORD_2,
	EPIC_SURA_SWORD_3 = EPIC_WARRIOR_SWORD_3
};

enum EItemShamanWeapon
{
	SHAMAN_GLOCK = 520,
	SHAMAN_GLOCK_2 = 521,
	SHAMAN_GLOCK_3 = 522,

	RARE_SHAMAN_GLOCK = 523,
	RARE_SHAMAN_GLOCK_2 = 524,
	RARE_SHAMAN_GLOCK_3 = 525,

	EPIC_SHAMAN_GLOCK = 526,
	EPIC_SHAMAN_GLOCK_2 = 527,
	EPIC_SHAMAN_GLOCK_3 = 528,


	SHAMAN_FAN = 530,
	SHAMAN_FAN_2 = 531,
	SHAMAN_FAN_3 = 532,

	RARE_SHAMAN_FAN = 533,
	RARE_SHAMAN_FAN_2 = 534,
	RARE_SHAMAN_FAN_3 = 535,

	EPIC_SHAMAN_FAN = 536,
	EPIC_SHAMAN_FAN_2 = 537,
	EPIC_SHAMAN_FAN_3 = 538
};

enum EItemAssassinWeapon
{
	ASSASSIN_DAGGER = 540,
	ASSASSIN_DAGGER_2 = 541,
	ASSASSIN_DAGGER_3 = 542,

	RARE_ASSASSIN_DAGGER = 543,
	RARE_ASSASSIN_DAGGER_2 = 544,
	RARE_ASSASSIN_DAGGER_3 = 545,

	EPIC_ASSASSIN_DAGGER = 546,
	EPIC_ASSASSIN_DAGGER_2 = 547,
	EPIC_ASSASSIN_DAGGER_3 = 548,


	ASSASSIN_SWORD = WARRIOR_SWORD,
	ASSASSIN_SWORD_2 = WARRIOR_SWORD_2,
	ASSASSIN_SWORD_3 = WARRIOR_SWORD_3,

	RARE_ASSASSIN_SWORD = RARE_WARRIOR_SWORD,
	RARE_ASSASSIN_SWORD_2 = RARE_WARRIOR_SWORD_2,
	RARE_ASSASSIN_SWORD_3 = RARE_WARRIOR_SWORD_3,

	EPIC_ASSASSIN_SWORD = EPIC_WARRIOR_SWORD,
	EPIC_ASSASSIN_SWORD_2 = EPIC_WARRIOR_SWORD_2,
	EPIC_ASSASSIN_SWORD_3 = EPIC_WARRIOR_SWORD_3,


	ASSASSIN_BOW = 550,
	ASSASSIN_BOW_2 = 551,
	ASSASSIN_BOW_3 = 552,

	RARE_ASSASSIN_BOW = 553,
	RARE_ASSASSIN_BOW_2 = 554,
	RARE_ASSASSIN_BOW_3 = 555,

	EPIC_ASSASSIN_BOW = 556,
	EPIC_ASSASSIN_BOW_2 = 557,
	EPIC_ASSASSIN_BOW_3 = 558,

	ASSASSIN_ARROW = 559,
	ARROW_COUNT = 10
};

#ifdef ENABLE_WOLFMAN_CHARACTER	
enum eItemWolfmanWeapon
{
	WOLFMAN_WEAPON = 560,
	WOLFMAN_WEAPON_2 = 561,
	WOLFMAN_WEAPON_3 = 562,

	RARE_WOLFMAN_WEAPON = 563,
	RARE_WOLFMAN_WEAPON_2 = 564,
	RARE_WOLFMAN_WEAPON_3 = 565,

	EPIC_WOLFMAN_WEAPON = 566,
	EPIC_WOLFMAN_WEAPON_2 = 567,
	EPIC_WOLFMAN_WEAPON_3 = 568,
};
#endif

enum BodyItem
{
	COMMON_BODY = 21000,
	COMMON_BODY_2 = 21001,
	COMMON_BODY_3 = 21002,

	RARE_BODY = 21003,
	RARE_BODY_2 = 21004,
	RARE_BODY_3 = 21005,

	EPIC_BODY = 21006,
	EPIC_BODY_2 = 21007,
	EPIC_BODY_3 = 21008
};

class CBattleRoyale : public singleton<CBattleRoyale>
{
private:
	std::map<DWORD, DWORD> m_map_attender;
	std::map<DWORD, DWORD> m_map_char;	
	
public:
	bool Initialize();

	LPEVENT		m_Begin_Battle_royale;

	void OnLogin(LPCHARACTER ch);
	void OnLogout(LPCHARACTER ch);

	void SpawnBattleroyale(LPCHARACTER ch);
	void BattleRoyaleKill(LPCHARACTER pChar, LPCHARACTER pkKiller);
	void DeleteBattleRoyalItem(LPCHARACTER ch);
	void BlockAutoPotion(LPCHARACTER ch);

	void HPSettingsBattleRoyale(LPCHARACTER ch);
	void HPSetBattleroyale(LPCHARACTER ch);
	void HPResetBattleroyale(LPCHARACTER ch);

	DWORD Get_Battle_Royale_HTPoint(LPCHARACTER ch);
	void Set_Battle_Royale_HTPoint(DWORD BATTLE_ROYALE_HT, LPCHARACTER ch);

	void DeleteTakenTime(LPCHARACTER ch);
	void AddToMaLLBattleRoyaleMap(LPCHARACTER ch);

	void DeleteMapLogout();
	void SwordBox(LPCHARACTER ch);
	void ArmorBox(LPCHARACTER ch);
	void CreateBattleRoyaleID(LPCHARACTER ch, DWORD ITEM_ID);
	void SwitchBattleRoyaleItem(LPCHARACTER ch, DWORD SWITCH_ID1, DWORD SWITCH_ID2, DWORD SWITCH_ID3);
	void SwitchBattleRoyaleTierItem(LPCHARACTER ch, DWORD SWITCH_ID1, DWORD SWITCH_ID2, DWORD SWITCH_ID3, DWORD SWITCH_ID4, DWORD SWITCH_ID5, DWORD SWITCH_ID6, DWORD SWITCH_ID7, DWORD SWITCH_ID8, DWORD SWITCH_ID9);
	DWORD GetAttenderCount() { return m_map_attender.size(); }
	bool EnterAttender(LPCHARACTER ch);
	bool EnterAudience(LPCHARACTER ch);
	int WinnerCounter;
	void ClearWinnerCounter(void)       { WinnerCounter = 0; }
	void SetWinnerCounter(void)       { WinnerCounter = 1; }
	int GetWinnerCounter(void) const { return WinnerCounter; }

	void ResetRanking();
	void UpdateKillPoint(DWORD ID, const char* NAME, BYTE EMPIRE, DWORD KILL);

	void LoadBattleRoyalConfig();

};

#endif
