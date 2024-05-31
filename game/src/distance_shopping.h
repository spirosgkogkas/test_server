const int DISTANCE_SHOP_EXCLUDED_MAPS[] = {99, 100, 101, 103, 105, 111, 113};

class CDistanceShopping : public singleton<CDistanceShopping>
{
	public:
		CDistanceShopping();
		
		bool	CanUseDistanceShopping(LPCHARACTER ch);
		void	OpenDistanceShoppingDialog(LPCHARACTER ch);
};