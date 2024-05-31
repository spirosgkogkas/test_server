#ifndef __INC_METIN_II_GAME_LOTTERY_MANAGER_H__
#define __INC_METIN_II_GAME_LOTTERY_MANAGER_H__

#ifdef ENABLE_LOTTERY_SYSTEM
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

class CLotteryManager : public singleton<CLotteryManager>
{
	public: 
		CLotteryManager();
		virtual ~CLotteryManager();

		bool		RegisterEvent(LPCHARACTER ch,DWORD betMiktar,BYTE color);
		void		RefreshRegisterList();
		void		SetBroadCastBigNotice(char* bufInfo);
		void		StartRollEvent();
		void		Initialize();
		void		Roll(BYTE randNumber);
		void		Destroy();
		bool		ClearAllInfo();
		void		StatisticAdd(LPCHARACTER ch,DWORD miktar,BYTE addMode);
		void		StatisticBilgiGonder(LPCHARACTER ch);
		
		enum LotteryConfig
		{
			LOTTERY_MAP_INDEX	= 200, // LotteryConfig
			LOTTERY_BASE_X	= 1041, // LotteryConfig
			LOTTERY_BASE_Y	= 97, // LotteryConfig
			LOTTERY_ROLL_TIME	= 60, // LotteryConfig
		};
		
		typedef struct TStatisticKumar
		{
			long	dwTotalEp;
			int	dwTotalWin;
			int	dwTotalLoss;
			
			TStatisticKumar(long totEp, int totWin, int totLoss)
				: dwTotalEp(totEp), dwTotalWin(totWin), dwTotalLoss(totLoss)
				{}
		};
		
		typedef std::map<LPCHARACTER, TStatisticKumar> TKumarciMap;

	protected:
		boost::unordered_multimap<LPCHARACTER, std::pair<DWORD, BYTE>> kumarbaz_map;
		
		TKumarciMap	statistic_kumar;
		
		LPEVENT destroyEvent;
};
#endif

#endif