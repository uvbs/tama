/********************************************************************
	@date:	2015/12/23 15:18
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XPropLegion.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

XE_NAMESPACE_START( XGAME )
//////////////////////////////////////////////////////////////////////////
/// "부대"를 정의한다. xml에선 "squad" element로 표현된다.
struct xSquad {
	int idxPos = -1;	///< 부대위치. 분대번호
	ID idHero = 0;
	xtAttack atkType = xAT_NONE;		// 영웅병과
	xtUnit unit = XGAME::xUNIT_NONE;
	xtSize sizeUnit = XGAME::xSIZE_NONE;
	int lvSquad = 0;		///< 부대레벨
	int lvSkill = 0;		///< 영웅의 스킬레벨
	int lvHero = 0;		///< 영웅레벨
	int adjLvHero = 0x7f;	// 영웅레벨 보정
	XGAME::xtGrade grade = XGAME::xGD_NONE;
	//	int numUnit = 0;			// 유닛수
	float mulAtk = 1.f;	// 공격력 보정
	float mulHp = 1.f;		// 체력보정
#ifdef _XSINGLE
	bool m_bShow = true;	// 부대hp정보 보이기
#endif // _XSINGLE
#ifdef _DEBUG
	int debug = 0;			// 디버깅용
#endif // _DEBUG
											//
	int Serialize( XArchive& ar ) const;
	int DeSerialize( XArchive& ar, int ver );
	bool LoadFromXML( XEXmlNode& node, LPCTSTR szTag );
	bool SaveXML( XEXmlNode& nodeLegion, const std::string& strName );
};
//////////////////////////////////////////////////////////////////////////
struct xLegion {
private:
	ID m_idProp = 0;			///< "legion"블럭이 있었으면 고유번호가 들어간다. 이것은 save에 활용된다.
public:
	std::string strIds;		// 식별자
												//	ID snLegion = 0;	///< "legion"블럭이 있었으면 고유번호가 들어간다. 이것은 save에 활용된다.
	ID idName = 0;		///< 군단이름
	int lvLegion = 0;	///< 군단레벨(제네레이트 목적)
	XGAME::xtGradeLegion gradeLegion = XGAME::xGL_NORMAL;	///< 정예인가
	int lvLimit = 0;	///< 최소 도전레벨
	int adjLvLegion = 0x7f;	///< 기준레벨대비 보정값. levelLegion이 0이어야 한다.
	int numSquad = 0;	///< 만들어야하는 부대수. 따로지정하지 않으면 levelLegion에 의해 자동계산된다.
	ID idBoss = 0;		///< 군단장 영웅 아이디
	float mulAtk = 1.f;	// 공격력 보정
	float mulHp = 1.f;		// 체력보정
	xSquad squadDefault;	// 디폴트 분대
	XVector<xSquad> arySquads;		///< 수동으로 지정해야 하는 부대리스트
	xLegion() {}		// Deserial용
	xLegion( const char* cIdentifier ) : strIds( cIdentifier ) {}
	xLegion( const std::string& strIdentifier ) : xLegion( strIdentifier.c_str() ) {}
	inline void _SetidProp( const ID idProp ) {
		m_idProp = idProp;
	}
	GET_ACCESSOR_CONST( ID, idProp );
	// 수동으로 지정해야할 idxPos의 수동데이타가 있는가.
	const XGAME::xSquad* GetManualSquad( const int idxPos ) const {
		for( auto& squad : arySquads ) {
			if( squad.idxPos == idxPos )
				return &squad;
		}
		return nullptr;
	}
	// idxPos부대가 메뉴얼 지정이라면 부대프로퍼티를 꺼낸다.
	const xSquad* GetPropSquadWhenManual( const int idxPos ) const {
		return GetManualSquad( idxPos );
	}
	inline int GetNumSquad() {
		return numSquad;
	}
	bool IsEmpty() const {
		return m_idProp == 0;
	}
	int Serialize( XArchive& ar ) const;
	int DeSerialize( XArchive& ar, int ver );
	bool LoadFromXML( XEXmlNode& node, LPCTSTR szTag );
	inline bool LoadFromXML( XEXmlNode& node, const _tstring& strTag ) {
		return LoadFromXML( node, strTag.c_str() );
	}
	bool SaveXML( XEXmlNode& nodeRoot );
};

XE_NAMESPACE_END
