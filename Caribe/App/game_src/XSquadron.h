﻿#pragma once
#include "XPropHero.h"
class XArchive;
class XAccount;
class XSquadron;
class XHero;
////////////////////////////////////////////////////////////////
class XSquadron
{
	ID m_snSquad = 0;					/// 부대 고유번호
	int m_idxPos = -99;			/// 전장에서 부대의 위치인덱스 0 ~ 15. -1은 특별히 정해진 위치가 없다. 99는 초기값
	bool m_bCreateHero = false;			// NPC부대인가?
	XSPHero m_pHero = nullptr;			// m_bNPC가 TRUE면 여기서 m_pHero를 삭제하지 않는다.
	//ID m_snHero = 0;
	float m_mulAtk = 1.f;
	float m_mulHp = 1.f;
	void Init() {
		m_snSquad = XE::GenerateID();
	}
	void Destroy();
public:
	XSquadron() { Init(); }
	XSquadron( XSPHero pHero );
#if defined(_XSINGLE) || !defined(_CLIENT)
// 	XSquadron( XSPAccConst spAcc, XPropHero::xPROP *pPropHero, int levelHero, XGAME::xtUnit unit, int levelSquad, bool bCreateHero = true );
	XSquadron( XPropHero::xPROP *pPropHero, int levelHero, XGAME::xtUnit unit, int levelSquad );
#endif // defined(_XSINGLE) || !defined(_CLIENT)
	~XSquadron() { Destroy(); }
	//
	GET_ACCESSOR_CONST( ID, snSquad );
	GET_SET_ACCESSOR_CONST( int, idxPos );
	GET_ACCESSOR_CONST( XSPHero, pHero );
	GET_SET_ACCESSOR( bool, bCreateHero );
	GET_SET_ACCESSOR_CONST( float, mulAtk );
	GET_SET_ACCESSOR_CONST( float, mulHp );
//	GET_SET_ACCESSOR( bool, bResourceSquad );
	inline bool IsNpc() const {
		return m_bCreateHero != false;
	}
	inline bool IsPc() const {
		return !IsNpc();
	}

	//
	void Clear() {
		// 정적객체로 만들었을때 m_pHero를 삭제하지 않게 하기위한 궁여지책..-_-;;
		Init();
	}
	ID GetsnHero() const;
	ID GetidPropHero() const;
	int GetLevelHero() const;
	XGAME::xtUnit GetUnitType() const;
	XGAME::xtAttack GetAtkType() const;
	int GetnumUnit() const;
	//
	inline bool IsEmpty() const {
		//return m_snHero == 0;
		return m_pHero == nullptr;
	}
	inline bool IsFill() const {
		return !IsEmpty();
	}
	void Serialize( XArchive& ar );
	void SerializeFull( XArchive& ar );
	BOOL DeSerialize( XArchive& ar, XSPAccConst spAcc, int verLegion );
private:
	BOOL DeSerializeFull( XArchive& ar, int verLegion );
	// 영웅을 교체
	void ChangeHero( XSPHero pHeroNew ) {
		XBREAK( m_bCreateHero );		// 이런경우가 있음?
		m_pHero = pHeroNew;
		//m_snHero = pHeroNew->GetsnHero();
	}
public:
	//
	friend class XLegion;
};
