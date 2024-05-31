const int REFINE_ABILITY_SKILL_ID = 132;
const int REFINE_MANUAL_ITEM_VNUM = 50069; //Item vnum of refine manual
const int REFINE_MANUAL_READ_DELAY_MIN = 3600;
const int REFINE_MANUAL_READ_DELAY_MAX = 7200;
const int REFINE_PERCENTAGE_LEVEL_ADD = 1;

class CRefineAbility : public singleton<CRefineAbility>
{
	public:
		CRefineAbility();
		
		int	GetAddedPercentage(int skill_level);
};