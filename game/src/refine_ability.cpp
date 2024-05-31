#include "stdafx.h"
#include "char.h"
#include "refine_ability.h"

CRefineAbility::CRefineAbility()
{
}

int CRefineAbility::GetAddedPercentage(int skill_level)
{
	int value = 0;	
	value = skill_level*REFINE_PERCENTAGE_LEVEL_ADD;
	return value;
}

