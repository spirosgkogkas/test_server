#ifndef __HEADER__AUCTION_HOUSE__
#define __HEADER__AUCTION_HOUSE__
#ifdef __AUCTION_HOUSE__

constexpr int minimal(int a, int b) { return  a < b ? a : b;}

enum EAuctionHouse
{
	AUCTION_MAX_SOCKET = 6,
	AUCTION_MIN_SOCKET = minimal(AUCTION_MAX_SOCKET, ITEM_SOCKET_MAX_NUM),
	AUCTION_MAX_ATTRIBUTE = 7,
	AUCTION_MIN_ATTRIBUTE = minimal(AUCTION_MAX_ATTRIBUTE, ITEM_ATTRIBUTE_MAX_NUM),
};

class CAuctionHouse : public singleton<CAuctionHouse>
{
public:	
	CAuctionHouse();
	~CAuctionHouse();
	void GetItems(LPCHARACTER ch, int cat, int next = 0);
	void BuyItem(LPCHARACTER ch, int id, int amount, int posarray);
	void SellItem(LPCHARACTER ch, int pos, int amount, long long price);
	void GetItemSelled(LPCHARACTER ch);
	void GetItemSell(LPCHARACTER ch);
	void CheckItemSell(LPCHARACTER ch);
	void UpdateItem(LPCHARACTER ch, int id, long long precio, int posarray);
	void DeleteItem(LPCHARACTER ch, int id, int posarray);
	void GetItemSearch(LPCHARACTER ch, const char* arg1);
	void GetBank(LPCHARACTER ch);
	void GetMoneyBank(LPCHARACTER ch);
};
#endif // __AUCTION_HOUSE__
#endif