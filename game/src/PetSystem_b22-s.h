#ifndef	__HEADER_PET_SYSTEM__
#define	__HEADER_PET_SYSTEM__

#define INTERN_PET_MOD
#define PET_MAX_LEVEL 20


class CHARACTER;

// TODO: �����μ��� �ɷ�ġ? ����� ģ�е�, ����� ��Ÿ���... ��ġ
struct SPetAbility
{
};

/**
*/
class CPetActor //: public CHARACTER
{
public:
	enum EPetOptions
	{
		EPetOption_Followable		= 1 << 0,
		EPetOption_Mountable		= 1 << 1,
		EPetOption_Summonable		= 1 << 2,
		EPetOption_Combatable		= 1 << 3,		
	};


protected:
	friend class CPetSystem;

	CPetActor(LPCHARACTER owner, DWORD vnum, DWORD options = EPetOption_Followable | EPetOption_Summonable);
//	CPetActor(LPCHARACTER owner, DWORD vnum, const SPetAbility& petAbility, DWORD options = EPetOption_Followable | EPetOption_Summonable);

	virtual ~CPetActor();

	virtual bool	Update(DWORD deltaTime);

protected:
	virtual bool	_UpdateFollowAI();				///< ������ ����ٴϴ� AI ó��
	virtual bool	_UpdatAloneActionAI(float fMinDist, float fMaxDist);			///< ���� ��ó���� ȥ�� ��� AI ó��

	/// @TODO
	//virtual bool	_UpdateCombatAI();

private:
	bool Follow(float fMinDistance = 50.f);

public:
	LPCHARACTER		GetCharacter()	const					{ return m_pkChar; }
	LPCHARACTER		GetOwner()	const						{ return m_pkOwner; }
	DWORD			GetVID() const							{ return m_dwVID; }
	DWORD			GetVnum() const							{ return m_dwVnum; }
#ifdef INTERN_PET_MOD
	const char*		GetName() const							{ return m_name.c_str(); }
#endif
	bool			HasOption(EPetOptions option) const		{ return m_dwOptions & option; }

	void			SetName(const char* petName);

	bool			Mount();
	void			Unmount();

	DWORD			Summon(const char* petName, LPITEM pSummonItem, bool bSpawnFar = false);
	void			Unsummon();

	bool			IsSummoned() const			{ return 0 != m_pkChar; }
	void			SetSummonItem (LPITEM pItem);
	DWORD			GetSummonItemVID () { return m_dwSummonItemVID; }
	// ���� �ִ� �Լ��� �ŵδ� �Լ�.
	// �̰� �� �����Ѱ�, ������ ����, 
	// POINT_MOV_SPEED, POINT_ATT_SPEED, POINT_CAST_SPEED�� PointChange()�� �Լ��� �Ἥ ������ ���� �ҿ��� ���°�,
	// PointChange() ���Ŀ� ��𼱰� ComputePoints()�� �ϸ� �ϴ� �ʱ�ȭ�ǰ�, 
	// �� �����, ComputePoints()�� �θ��� ������ Ŭ���� POINT�� ���� ������ �ʴ´ٴ� �Ŵ�.
	// �׷��� ������ �ִ� ���� ComputePoints() ���ο��� petsystem->RefreshBuff()�� �θ����� �Ͽ���,
	// ������ ���� ���� ClearBuff()�� �θ���, ComputePoints�� �ϴ� ������ �Ѵ�.
	void			GiveBuff();
	void			ClearBuff();
#ifdef INTERN_PET_MOD
	void 			UpdateName() { m_pkUpdate = 1; }
#endif
private:
	DWORD			m_dwVnum;
	DWORD			m_dwVID;
	DWORD			m_dwOptions;
	DWORD			m_dwLastActionTime;
	DWORD			m_dwSummonItemVID;
	DWORD			m_dwSummonItemVnum;
#ifdef INTERN_PET_MOD
	DWORD			m_pkUpdate;
#endif
	short			m_originalMoveSpeed;

	std::string		m_name;

	LPCHARACTER		m_pkChar;					// Instance of pet(CHARACTER)
	LPCHARACTER		m_pkOwner;

//	SPetAbility		m_petAbility;				// �ɷ�ġ
};

/**
*/
class CPetSystem
{
public:
	typedef	boost::unordered_map<DWORD,	CPetActor*>		TPetActorMap;		/// <VNUM, PetActor> map. (�� ĳ���Ͱ� ���� vnum�� ���� ������ ���� ���� ������..??)

public:
	CPetSystem(LPCHARACTER owner);
	virtual ~CPetSystem();

	CPetActor*	GetByVID(DWORD vid) const;
	CPetActor*	GetByVnum(DWORD vnum) const;

	bool		Update(DWORD deltaTime);
	void		Destroy();

	size_t		CountSummoned() const;			///< ���� ��ȯ��(��üȭ �� ĳ���Ͱ� �ִ�) ���� ����

public:
	void		SetUpdatePeriod(DWORD ms);

	CPetActor*	Summon(DWORD mobVnum, LPITEM pSummonItem, const char* petName, bool bSpawnFar, DWORD options = CPetActor::EPetOption_Followable | CPetActor::EPetOption_Summonable);

	void		Unsummon(DWORD mobVnum, bool bDeleteFromList = false);
	void		Unsummon(CPetActor* petActor, bool bDeleteFromList = false);

	// TODO: ��¥ �� �ý����� �� �� ����. (ĳ���Ͱ� ������ ���� ������ �߰��� �� �����...)
	CPetActor*	AddPet(DWORD mobVnum, const char* petName, const SPetAbility& ability, DWORD options = CPetActor::EPetOption_Followable | CPetActor::EPetOption_Summonable | CPetActor::EPetOption_Combatable);

	void		DeletePet(DWORD mobVnum);
	void		DeletePet(CPetActor* petActor);
	void		RefreshBuff();
	TPetActorMap	m_petActorMap;
private:
	//TPetActorMap	m_petActorMap;
	LPCHARACTER		m_pkOwner;					///< �� �ý����� Owner
	DWORD			m_dwUpdatePeriod;			///< ������Ʈ �ֱ� (ms����)
	DWORD			m_dwLastUpdateTime;
	LPEVENT			m_pkPetSystemUpdateEvent;
};

/**
// Summon Pet
CPetSystem* petSystem = mainChar->GetPetSystem();
CPetActor* petActor = petSystem->Summon(~~~);

DWORD petVID = petActor->GetVID();
if (0 == petActor)
{
	ERROR_LOG(...)
};


// Unsummon Pet
petSystem->Unsummon(petVID);

// Mount Pet
petActor->Mount()..


CPetActor::Update(...)
{
	// AI : Follow, actions, etc...
}

*/



#endif	//__HEADER_PET_SYSTEM__