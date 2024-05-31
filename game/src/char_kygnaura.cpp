/***************************
* Yazılımcı: İbrahim Kıyağan
* Tarih: 09/08/2019
* Saat: 07:56
****************************/

#include "stdafx.h"
#include "char.h"
#include "desc_client.h"
#include "db.h"
#include "item.h"
#include "item_manager.h"
#include "../../common/item_length.h"


//const int MAX_NORM_ATTR_NUM = ITEM_MANAGER::MAX_NORM_ATTR_NUM;

void CHARACTER::AurayiSlotaEkleme(LPITEM item)
{
	if (GetAuraUpgradeGui() == true)
	{
		if (item == 0)
		{
			if (GetAura() && GetAura()->isLocked() == true)
				GetAura()->Lock(false);
			SetAura(0);
			TPacketGCKygnAura KygnAura;
			KygnAura.bHeader = HEADER_GC_KYGN_AURA;
			KygnAura.Pencere = GetWindow();
			KygnAura.AuraLevel = 0;
			KygnAura.AuraPos = 753;
			if (GetMaterial())
				KygnAura.MaterialPos = GetMaterial()->GetCell();
			else
				KygnAura.MaterialPos = 753;
			GetDesc()->Packet(&KygnAura, sizeof(TPacketGCKygnAura));
		}
		else if (GetWindow() == 3 && item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_AURA)
		{
			SetAura(item);
			if (GetAura()->GetAttributeCount() > 0)
			{
				GetAura()->Lock(false);
				SetAura(0);
				TPacketGCKygnAura KygnAura;
				KygnAura.bHeader = HEADER_GC_KYGN_AURA;
				KygnAura.Pencere = GetWindow();
				KygnAura.AuraLevel = 0;
				KygnAura.AuraPos = 753;
				if (GetMaterial())
					KygnAura.MaterialPos = GetMaterial()->GetCell();
				else
					KygnAura.MaterialPos = 753;
				GetDesc()->Packet(&KygnAura, sizeof(TPacketGCKygnAura));
			}
			else
			{
				GetAura()->Lock(true);
				TPacketGCKygnAura KygnAura;
				KygnAura.bHeader = HEADER_GC_KYGN_AURA;
				KygnAura.Pencere = GetWindow();
				KygnAura.AuraLevel = GetAura()->GetSocket(0) + 1;
				KygnAura.AuraPos = GetAura()->GetCell();
				if (GetMaterial())
					KygnAura.MaterialPos = GetMaterial()->GetCell();
				else
					KygnAura.MaterialPos = 753;
				GetDesc()->Packet(&KygnAura, sizeof(TPacketGCKygnAura));
			}
		}
		else if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_AURA)
		{
			SetAura(item);
			GetAura()->Lock(true);
			TPacketGCKygnAura KygnAura;
			KygnAura.bHeader = HEADER_GC_KYGN_AURA;
			KygnAura.Pencere = GetWindow();
			KygnAura.AuraLevel = GetAura()->GetSocket(0) + 1;
			KygnAura.AuraPos = GetAura()->GetCell();
			if (GetMaterial())
				KygnAura.MaterialPos = GetMaterial()->GetCell();
			else
				KygnAura.MaterialPos = 753;
			GetDesc()->Packet(&KygnAura, sizeof(TPacketGCKygnAura));
		}
	}
}

void CHARACTER::MaterialiSlotaEkleme(LPITEM item)
{
	if (GetAuraUpgradeGui() == true)
	{
		if (item == 0)
		{
			if (GetMaterial() && GetMaterial()->isLocked() == true)
				GetMaterial()->Lock(false);
			SetMaterial(0);
			TPacketGCKygnAura KygnAura;
			KygnAura.bHeader = HEADER_GC_KYGN_AURA;
			KygnAura.Pencere = GetWindow();
			if (GetAura())
			{
				KygnAura.AuraLevel = GetAura()->GetSocket(0) + 1;
				KygnAura.AuraPos = GetAura()->GetCell();
			}
			else
			{
				KygnAura.AuraLevel = 0;
				KygnAura.AuraPos = 753;
			}
			KygnAura.MaterialPos = 753;
			GetDesc()->Packet(&KygnAura, sizeof(TPacketGCKygnAura));
		}
		else if (item->GetType() == ITEM_RESOURCE && item->GetSubType() == RESOURCE_AURA)
		{
			SetMaterial(item);
			GetMaterial()->Lock(true);
			TPacketGCKygnAura KygnAura;
			KygnAura.bHeader = HEADER_GC_KYGN_AURA;
			KygnAura.Pencere = GetWindow();
			if (GetAura())
			{
				KygnAura.AuraLevel = GetAura()->GetSocket(0) + 1;
				KygnAura.AuraPos = GetAura()->GetCell();
			}
			else
			{
				KygnAura.AuraLevel = 0;
				KygnAura.AuraPos = 753;
			}
			KygnAura.MaterialPos = GetMaterial()->GetCell();
			GetDesc()->Packet(&KygnAura, sizeof(TPacketGCKygnAura));
		}
		else if (GetWindow() == 3 && item->GetType() == ITEM_ARMOR && item->GetSubType() != ARMOR_BODY)
		{
			SetMaterial(item);
			GetMaterial()->Lock(true);
			TPacketGCKygnAura KygnAura;
			KygnAura.bHeader = HEADER_GC_KYGN_AURA;
			KygnAura.Pencere = GetWindow();
			if (GetAura())
			{
				KygnAura.AuraLevel = GetAura()->GetSocket(0) + 1;
				KygnAura.AuraPos = GetAura()->GetCell();
			}
			else
			{
				KygnAura.AuraLevel = 0;
				KygnAura.AuraPos = 753;
			}
			KygnAura.MaterialPos = GetMaterial()->GetCell();
			GetDesc()->Packet(&KygnAura, sizeof(TPacketGCKygnAura));
		}
	}
}

void CHARACTER::AuraYukselt()
{
	if (GetAura() != NULL && GetMaterial() != NULL)
	{
		if (GetAura()->GetSocket(0) == 48 || GetAura()->GetSocket(0) == 98 || GetAura()->GetSocket(0) == 148
		 || GetAura()->GetSocket(0) == 198 || GetAura()->GetSocket(0) == 248)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aurayi Daha Fazla Yukseltmek Icin Gelistirmen Gerek"));
			return;
		}
		BYTE bCell = GetAura()->GetCell();
		BYTE bCell2 = GetMaterial()->GetCell();
		if (MaterialSil(GetMaterial()->GetVnum(), 1, bCell2) == true)
		{
			GetAura()->SetSocket(0, GetAura()->GetSocket(0) + 1);
			if (CountSpecifyItem(GetMaterial()->GetVnum()) == 0)
			{
				GetMaterial()->Lock(false);
				SetMaterial(0);
				TPacketGCKygnAura KygnAura;
				KygnAura.bHeader = HEADER_GC_KYGN_AURA;
				KygnAura.Pencere = GetWindow();
				KygnAura.AuraLevel = GetAura()->GetSocket(0) + 1;
				KygnAura.AuraPos = GetAura()->GetCell();
				KygnAura.MaterialPos = 753;
				GetDesc()->Packet(&KygnAura, sizeof(TPacketGCKygnAura));
			}
			else
			{
				TPacketGCKygnAura KygnAura;
				KygnAura.bHeader = HEADER_GC_KYGN_AURA;
				KygnAura.Pencere = GetWindow();
				KygnAura.AuraLevel = GetAura()->GetSocket(0) + 1;
				KygnAura.AuraPos = GetAura()->GetCell();
				if (GetMaterial()->GetCell() == 0 && GetInventoryItem(0)->GetVnum() != GetMaterial()->GetVnum())
					KygnAura.MaterialPos = 753;
				else
					KygnAura.MaterialPos = GetMaterial()->GetCell();
				GetDesc()->Packet(&KygnAura, sizeof(TPacketGCKygnAura));
			}
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aura Yukseltme Basarili Yeni Seviye %d"), GetAura()->GetSocket(0) + 1);
		}
		else
			ChatPacket(CHAT_TYPE_INFO, "item yok");
	}
}

// void CHARACTER::AuraSil(BYTE pos)
// {
	// if (pos == NULL)
		// return;

	// LPITEM item2;
	// item2 = GetInventoryItem(pos);
	// ITEM_MANAGER::instance().RemoveItem(item2, "PURGE");
// }

bool CHARACTER::MaterialSil(int vnum, BYTE adet, BYTE pos)
{
	if (pos == 0)
		return false;

	LPITEM item2;
	item2 = GetInventoryItem(pos);

	if (item2->GetCount() >= adet && item2->GetCell() == pos)
	{
		item2->SetCount(item2->GetCount() - adet);
		return true;
	}

	return false;
}

void CHARACTER::AuraEfsunlariniKopyala(LPITEM eskiitem, LPITEM yeniitem)
{
	if (eskiitem == NULL || yeniitem == NULL)
		return;
	BYTE *efsuntype;
	int *efsundegeri;
	efsuntype = new BYTE;
	efsundegeri = new int;
	for (BYTE i = 0;i < MAX_NORM_ATTR_NUM;i++)
	{
		*efsuntype = eskiitem->GetAttributeType(i);
		*efsundegeri = eskiitem->GetAttributeValue(i);
		if (*efsuntype != 0 && *efsundegeri != 0)
			yeniitem->SetForceAttribute(i, *efsuntype, *efsundegeri);
		else
			return;
	}
	delete efsuntype;
	delete efsundegeri;
}

void CHARACTER::AuraBonusEmisteEfsunlariAktarma(LPITEM Material, LPITEM Aura, BYTE AuraLevel)
{
	if (Material == NULL || Aura == NULL)
		return;
	BYTE *efsuntype;
	int *efsundegeri;

	efsuntype = new BYTE;
	efsundegeri = new int;

	for (BYTE i = 0;i < MAX_NORM_ATTR_NUM;i++)
	{
		*efsuntype = Material->GetAttributeType(i);
		*efsundegeri = (Material->GetAttributeValue(i) * AuraLevel) / 1000;
		if (*efsuntype != 0 && *efsundegeri <= 0)
			*efsundegeri = 1;
		if (*efsuntype != 0 && *efsundegeri != 0)
			Aura->SetForceAttribute(i, *efsuntype, *efsundegeri);
	}

	delete efsuntype;
	delete efsundegeri;
}

void CHARACTER::AuraEmis()
{
	if (GetAura() == NULL || GetMaterial() == NULL)
		return;

	if (GetMaterial()->GetType() != ITEM_ARMOR || GetMaterial()->GetSubType() == ARMOR_BODY)
	{
		ChatPacket(CHAT_TYPE_INFO, "ItemType Uyumsuz.");
		return;
	}
	if (GetAura()->GetAttributeCount() > 0)
	{
		ChatPacket(CHAT_TYPE_INFO, "Zaten Bir Bonusa Sahip!!!");
		return;
	}
	GetAura()->SetSocket(1, GetMaterial()->GetVnum());
	GetAura()->Lock(false);
	GetMaterial()->Lock(false);
	AuraBonusEmisteEfsunlariAktarma(GetMaterial(), GetAura(), GetAura()->GetSocket(0) + 1);
	ITEM_MANAGER::instance().RemoveItem(GetMaterial(), "PURGE");
	TPacketGCKygnAura KygnAura;
	KygnAura.bHeader = HEADER_GC_KYGN_AURA;
	KygnAura.Pencere = GetWindow();
	KygnAura.AuraLevel = 0;
	KygnAura.AuraPos = 753;
	KygnAura.MaterialPos = 753;
	GetDesc()->Packet(&KygnAura, sizeof(TPacketGCKygnAura));
}

void CHARACTER::AuraGelistir()
{
	if (GetAura() != NULL && GetMaterial() != NULL)
	{
		if (GetAura()->GetSocket(0) == 48 || GetAura()->GetSocket(0) == 98 || GetAura()->GetSocket(0) == 148
		 || GetAura()->GetSocket(0) == 198 || GetAura()->GetSocket(0) == 248)
		{
			LPITEM YeniItem;
			GetAura()->Lock(false);
			GetMaterial()->Lock(false);
			BYTE *bCell2;
			bCell2 = new BYTE;
			*bCell2 = GetMaterial()->GetCell();
			if (MaterialSil(GetMaterial()->GetVnum(), 1, *bCell2) == true)
			{
				if (GetAura()->GetVnum() == 49001)
				{
					LPITEM YeniItem = AutoGiveItem(49002);
					AuraEfsunlariniKopyala(GetAura(), YeniItem);
					// AuraSil(bCell);
					ITEM_MANAGER::instance().RemoveItem(GetAura(), "PURGE");
					SetAura(YeniItem);
					GetAura()->SetSocket(0, 49);
				}
				else if (GetAura()->GetVnum() == 49002)
				{
					LPITEM YeniItem = AutoGiveItem(49003);
					AuraEfsunlariniKopyala(GetAura(), YeniItem);
					// AuraSil(bCell);
					ITEM_MANAGER::instance().RemoveItem(GetAura(), "PURGE");
					SetAura(YeniItem);
					GetAura()->SetSocket(0, 99);
				}
				else if (GetAura()->GetVnum() == 49003)
				{
					LPITEM YeniItem = AutoGiveItem(49004);
					AuraEfsunlariniKopyala(GetAura(), YeniItem);
					// AuraSil(bCell);
					ITEM_MANAGER::instance().RemoveItem(GetAura(), "PURGE");
					SetAura(YeniItem);
					GetAura()->SetSocket(0, 149);
				}
				else if (GetAura()->GetVnum() == 49004)
				{
					LPITEM YeniItem = AutoGiveItem(49005);
					AuraEfsunlariniKopyala(GetAura(), YeniItem);
					// AuraSil(bCell);
					ITEM_MANAGER::instance().RemoveItem(GetAura(), "PURGE");
					SetAura(YeniItem);
					GetAura()->SetSocket(0, 199);
				}
				else if (GetAura()->GetVnum() == 49005)
				{
					LPITEM YeniItem = AutoGiveItem(49006);
					AuraEfsunlariniKopyala(GetAura(), YeniItem);
					// AuraSil(bCell);
					ITEM_MANAGER::instance().RemoveItem(GetAura(), "PURGE");
					SetAura(YeniItem);
					GetAura()->SetSocket(0, 249);
				}
			}
			else
			{
				TPacketGCKygnAura KygnAura;
				KygnAura.bHeader = HEADER_GC_KYGN_AURA;
				KygnAura.Pencere = GetWindow();
				KygnAura.AuraLevel = 0;
				KygnAura.AuraPos = 753;
				KygnAura.MaterialPos = 753;
				GetDesc()->Packet(&KygnAura, sizeof(TPacketGCKygnAura));
				SetAura(0);
				SetMaterial(0);
			}
			delete bCell2;
		}
		else
			ChatPacket(CHAT_TYPE_INFO, "Aurayi Daha Fazla Gelistiremem.");
	}
}

void CHARACTER::PaneliKapat()
{
	if (GetAura())
	{
		GetAura()->Lock(false);
		SetAura(0);
	}
	if (GetMaterial())
	{
		GetMaterial()->Lock(false);
		SetMaterial(0);
	}
	SetAuraUpgradeGui(false);
}

void CHARACTER::YukseltmePaneliniAc()
{
	if (GetAura())
	{
		GetAura()->Lock(false);
		SetAura(0);
	}
	if (GetMaterial())
	{
		GetMaterial()->Lock(false);
		SetMaterial(0);
	}

	SetAuraUpgradeGui(true);
	SetWindow(1);
	TPacketGCKygnAura KygnAura;
	KygnAura.bHeader = HEADER_GC_KYGN_AURA;
	KygnAura.Pencere = GetWindow();
	KygnAura.AuraLevel = 0;
	KygnAura.AuraPos = 753;
	KygnAura.MaterialPos = 753;
	GetDesc()->Packet(&KygnAura, sizeof(TPacketGCKygnAura));
}

void CHARACTER::GelistirmePaneliniAc()
{
	if (GetAura())
	{
		GetAura()->Lock(false);
		SetAura(0);
	}
	if (GetMaterial())
	{
		GetMaterial()->Lock(false);
		SetMaterial(0);
	}
	SetAuraUpgradeGui(true);
	SetWindow(2);
	TPacketGCKygnAura KygnAura;
	KygnAura.bHeader = HEADER_GC_KYGN_AURA;
	KygnAura.Pencere = GetWindow();
	KygnAura.AuraLevel = 0;
	KygnAura.AuraPos = 753;
	KygnAura.MaterialPos = 753;
	GetDesc()->Packet(&KygnAura, sizeof(TPacketGCKygnAura));
}

void CHARACTER::BonusEmisPaneliniAc()
{
	if (GetAura())
	{
		GetAura()->Lock(false);
		SetAura(0);
	}
	if (GetMaterial())
	{
		GetMaterial()->Lock(false);
		SetMaterial(0);
	}
	SetAuraUpgradeGui(true);
	SetWindow(3);
	TPacketGCKygnAura KygnAura;
	KygnAura.bHeader = HEADER_GC_KYGN_AURA;
	KygnAura.Pencere = GetWindow();
	KygnAura.AuraLevel = 0;
	KygnAura.AuraPos = 753;
	KygnAura.MaterialPos = 753;
	GetDesc()->Packet(&KygnAura, sizeof(TPacketGCKygnAura));
}
