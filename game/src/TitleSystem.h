#pragma once
#include <string>

#ifdef __TITLE_SYSTEM_YUMA__
class CTitleSystem : public singleton<CTitleSystem>
{
	public:
		CTitleSystem();
		virtual ~CTitleSystem();

		std::pair<std::string, DWORD> 	GetTitleString(LPCHARACTER pkChar);
		void							UseTitleItem(bool bEquip, int iValue0);

	private:
		std::string m_stTitle;

		DWORD		GetColorFromDatabase(int iValue0) const;
		std::string		GetTitleFromDatabase(int iValue0) const;
		void			SetTitleString(const std::string& title);
	};
#endif