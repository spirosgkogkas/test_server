#include "../../common/length.h"
#include <boost/unordered_map.hpp>

class CHARACTER;
class CItem;

class CStackAttr : public singleton<CStackAttr>
{
	public:
		CStackAttr();
		~CStackAttr();

		void	Initialize();
		void	AddProgressToAttr(LPCHARACTER ch, int iType, LPITEM iWear, int item = 1, int iValue = 1);
		void	DoStackAttr(LPCHARACTER ch, LPITEM test, int iType, int iBonus, int iValue = 1);

		void	SendInfoItems(LPCHARACTER ch);

	protected:
		typedef struct SInfoItem
		{
			int ItemVnum;
			int	TypeBonus[3];
			int	BonusType[3];
			int	BonusRewardProgress[3];
			int	BonusMaxStack[3];
			int	GetBonusVnum[3];
		};

		std::map<int, SInfoItem> map_stack_attr;
};
