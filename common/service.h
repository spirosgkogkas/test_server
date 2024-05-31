#ifndef __INC_SERVICE_H__
#define __INC_SERVICE_H__

///***///sajat///***///
#define __SASH_SYSTEM__

/* OFFLINE SHOPS ---------------------*/
#define OFFLINE_SHOP // Offline shops system
#define GIFT_SYSTEM // gift system enable
//#define FULL_YANG // Enable support for yang type long long
#define SHOP_TIME_REFRESH 1*60 // time for cycle checking older shops
#define SHOP_BLOCK_GAME99 //Blocking create shops on channel 99
//#define SHOP_DISTANCE // Show shops in pos distance like WoM2
#define SHOP_AUTO_CLOSE //Enable auto closing shop after sell last item
//#define SHOP_ONLY_ALLOWED_INDEX //Enable limiting for other map index which is not defined in player.shop_limit
//#define SHOP_HIDE_NAME // Enable hidding shop npc names like "Player's shop"
#define SHOP_GM_PRIVILEGES GM_IMPLEMENTOR //Minimum GM privileges to using Shop GM Panel
/* OFFLINE SHOPS ---------------------*/
#define BL_PRIVATESHOP_SEARCH_SYSTEM
#define __WEAPON_COSTUME_SYSTEM__
#define ENABLE_MOUNT_COSTUME_SYSTEM
#define ENABLE_CHEQUE_SYSTEM
#if defined(OFFLINE_SHOP) && defined(ENABLE_CHEQUE_SYSTEM)
#	define ENABLE_OFFLINE_SHOP_USE_CHEQUE
#else
#	define ENABLE_SHOP_USE_CHEQUE
#endif
#define __SEND_TARGET_INFO__
#define __VIEW_TARGET_PLAYER_HP__
#define __VIEW_TARGET_DECIMAL_HP__
#define ELEMENT_NEW_BONUSES
#define __WJ_SHOW_MOB_INFO__
#define ENABLE_DS_GRADE_MYTH























///***///sajat///***///

#define ENABLE_PROXY_IP
#define ENABLE_PORT_SECURITY
#define _IMPROVED_PACKET_ENCRYPTION_ // 패킷 암호화 개선
//#define __AUCTION__
#define __PET_SYSTEM__
#define __UDP_BLOCK__
#endif
