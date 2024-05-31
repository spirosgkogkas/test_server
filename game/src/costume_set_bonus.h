class CCostumeSetBonus : public singleton<CCostumeSetBonus>
{
	public:
		CCostumeSetBonus();
		
		void	CheckCostumeSet(LPCHARACTER ch);
		void	RemoveBonus(LPCHARACTER ch);
		
	public:
		void	AddBonus(LPCHARACTER ch, int set_id);
};