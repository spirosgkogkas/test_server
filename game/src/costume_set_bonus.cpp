#include "stdafx.h"
#include "constants.h"
#include "char.h"
#include "config.h"
#include "costume_set_bonus.h"
#include "item.h"
#include "affect.h"

CCostumeSetBonus::CCostumeSetBonus()
{
}

static const int CostumeSetsData[][9][6] = { 
	//
	//INFORMATION
	//Insert 0 to vnums if other costumes do not not mind.
	//Insert -1 if costume slot for that type should be empty.
	//To create set only for one char, you must input for example value 1 to all other chars excluding bonus type and value.
	//

	//Costume set 0
	{
		//bonus type, bonus value, body, hair, weapon
		{113, 20, 41766, 45438, 42155}, //WARRIOR (m)	tested
		//{113, 20, 41766, 45438, 42158}, //WARRIOR 2hand (m)	
		{113, 20, 41767, 45439, 42156}, //ASSASIN (f)	tested
		{113, 20, 41766, 45438, 42161}, //SURA (m)		tested
		{113, 20, 41767, 45439, 42159}, //SHAMAN (f)	tested
		{113, 20, 41767, 45439, 42155}, //WARRIOR (f)	tested
		{113, 20, 41766, 45438, 42156}, //ASSASIN (m)	tested
		{113, 20, 41767, 45439, 42161}, //SURA (f)		tested
		{113, 20, 41766, 45438, 42159}, //SHAMAN (m)	tested
	//	{113, 100, 41139, 45076, 40121} //WOLFMAN
	},
	//Costume set 1
	{
		//bonus type, bonus value, body, hair, weapon
		{54, 200, 41137, 45075, 0}, //WARRIOR (m)
		{54, 200, 41138, 45075, 0}, //ASSASIN (f)
		{54, 200, 41137, 45075, 0}, //SURA (m)
		{54, 200, 41138, 45075, 0}, //SHAMAN (f)
		{54, 200, 41138, 45075, 0}, //WARRIOR (f)
		{54, 200, 41137, 45075, 0}, //ASSASIN (m)
		{54, 200, 41138, 45075, 0}, //SURA (f)
		{54, 200, 41137, 45075, 0}, //SHAMAN (m)
		{54, 200, 41137, 45075, 0} //WOLFMAN
	},
};

void CCostumeSetBonus::CheckCostumeSet(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc() || !ch->IsPC())
		return;
	
	LPITEM costumeBody = ch->GetWear(WEAR_COSTUME_BODY);
	LPITEM costumeHair = ch->GetWear(WEAR_COSTUME_HAIR);
	LPITEM costumeWeapon = ch->GetWear(WEAR_COSTUME_WEAPON);
	//LPITEM costumeSash = ch->GetWear(WEAR_COSTUME_SASH);
	
	int cBodyVnum = 0;
	int cHairVnum = 0;
	int cWeaponVnum = 0;
	//int cSashVnum = 0;
	
	if (costumeBody)
		cBodyVnum = costumeBody->GetVnum();
	if (costumeHair)
		cHairVnum = costumeHair->GetVnum();
	if (costumeWeapon)
		cWeaponVnum = costumeWeapon->GetVnum();
	// if (costumeSash)
		// cSashVnum = costumeSash->GetVnum();
				
	for (unsigned int i = 0; i < (sizeof(CostumeSetsData)/sizeof(*CostumeSetsData)); i++)
	{
		if ((int(CostumeSetsData[i][ch->GetRaceNum()][2]) == -1 && cBodyVnum > 0) || 
			(int(CostumeSetsData[i][ch->GetRaceNum()][3]) == -1 && cHairVnum > 0) ||
			(int(CostumeSetsData[i][ch->GetRaceNum()][4]) == -1 && cWeaponVnum > 0))
			{
				RemoveBonus(ch);
				return;
			}
		
		if (cBodyVnum == int(CostumeSetsData[i][ch->GetRaceNum()][2]) || int(CostumeSetsData[i][ch->GetRaceNum()][2]) <= 0)
			if (cHairVnum == int(CostumeSetsData[i][ch->GetRaceNum()][3]) || int(CostumeSetsData[i][ch->GetRaceNum()][3]) <= 0)
				if (cWeaponVnum == int(CostumeSetsData[i][ch->GetRaceNum()][4]) || int(CostumeSetsData[i][ch->GetRaceNum()][4]) <= 0)
				{
					AddBonus(ch, i);
					return;
				}
	}
	
	RemoveBonus(ch);
}

void CCostumeSetBonus::AddBonus(LPCHARACTER ch, int set_id)
{
	if (!ch || !ch->GetDesc() || !ch->IsPC())
		return;	
		
	if(!ch->FindAffect(AFFECT_COSTUME_SET_BONUS))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("costume_set_bonus_applied"));
		ch->AddAffect(AFFECT_COSTUME_SET_BONUS, aApplyInfo[CostumeSetsData[set_id][ch->GetRaceNum()][0]].bPointType, CostumeSetsData[set_id][ch->GetRaceNum()][1], AFF_COSTUME_SET, INFINITE_AFFECT_DURATION, 0, false);
		return;
	}
}

void CCostumeSetBonus::RemoveBonus(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc() || !ch->IsPC())
		return;	
	
	if(ch->FindAffect(AFFECT_COSTUME_SET_BONUS))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("costume_set_bonus_removed"));
		ch->RemoveAffect(AFFECT_COSTUME_SET_BONUS);
		return;
	}
}

