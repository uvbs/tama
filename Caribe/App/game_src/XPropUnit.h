#pragma once
#include "XFramework/XEBaseProp.h"

class XHero;

#define VER_PROP_UNIT		1

class XPropUnit : public XEBaseProp
{
public:
	struct xPROP {
		ID idProp;					///</< 캐릭터 아이디(XGAME::xtUnit과 매치됨)
		_tstring strIdentifier;		///< 식별 아이디 문자열
		_tstring strName;			///< 이름
		ID idDesc = 0;				///< 설명
		
		XGAME::xtAttack typeAtk;		///< 공격타입
		XGAME::xtSize size;			///< 유닛크기
		///< 기본 스탯
		float atkMelee;				///< 근접공격력
		float atkRange;				///< 원거리공격력
		float def;				///< 방어력
		float hpMax;					///< 체력
		float atkSpeed;			///< 공격 속도
		float movSpeedPerSec;			///< 초당 이동속도(단위 픽셀), 엑셀에선 초당 미터단위로 쓰고 읽을때 픽셀단위로 변환한다.
		float radiusAtkByPixel;		///< 공격사거리(픽셀). 엑셀에선 미터로 입력한다.
		XGAME::xtTribe tribe;		///< 종족
		XGAME::xtResource resNeed[2];	///<필요자원
		int numRes[2];				///< 필요개수
		_tstring strFace;			///< 초상화 파일
		_tstring strSpr;			///< 스프라이트 파일명
		int movSpeedNormal;		///< 1배속일때 이동속도
		float scale;			///< 크기조절
		float scale_factor = 1.f;		// 유닛에 붙는 이펙트등의 상대적 크기
		XE::VEC3 vHSL;				///< HSL조절값.(float형이지만 소수점 사용하지 않음)
		xPROP() {
			idProp = 0;
			typeAtk = XGAME::xAT_NONE;
			size = XGAME::xSIZE_NONE;
			atkMelee = 0;
			atkRange = 0;
			def = 0;
			hpMax = 0;
			atkSpeed = 0;
			movSpeedPerSec = 0;
			movSpeedNormal = 0;
			radiusAtkByPixel = XGAME::ONE_METER;
			tribe = XGAME::xTB_NONE;
			XCLEAR_ARRAY( resNeed );
			XCLEAR_ARRAY( numRes );
			scale = 1.f;
		}
		~xPROP() { }
		XGAME::xtUnit GetTypeUnit() const {
			return (XGAME::xtUnit) idProp;
		}
		bool IsTanker() {
			return typeAtk == XGAME::xAT_TANKER;
		}
		BOOL IsRange() {
			return typeAtk == XGAME::xAT_RANGE;
		}
		BOOL IsSpeed() {
			return typeAtk == XGAME::xAT_SPEED;
		}
		BOOL IsSmall() {
			return size == XGAME::xSIZE_SMALL;
		}
		BOOL IsMiddle() {
			return size == XGAME::xSIZE_MIDDLE;
		}
		BOOL IsBig() {
			return size == XGAME::xSIZE_BIG;
		}
		float GetStat( int idx ) {
			switch( idx ) {
			case 0:	return atkMelee;
			case 1: return atkRange;
			case 2: return def;
			case 3: return hpMax;
			case 4: return atkSpeed;
			case 5: return movSpeedPerSec;
			default: XBREAK(1); break;
			}
			return 0.f;
		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int ver );
	}; // struct xPROP {
private:
	std::map<_tstring, xPROP*> m_mapData;			// 프로퍼티 맵(식별자)
	std::map<ID, xPROP*> m_mapID;					// 프로퍼티 맵(ID)
//	std::map<ID, xPROP*>::iterator m_itorID;
	
	XVector<xPROP*> m_aryProp;				// 전체 프로퍼티 배열

	void Init() {}
	void Destroy();

public:
	XPropUnit();
	virtual ~XPropUnit() { Destroy(); }

	GET_ACCESSOR_CONST( const XVector<xPROP*>&, aryProp );
	
//	void GetNextClear( void ) { m_itorID = m_mapID.begin(); }		// 반복자 초기화
	BOOL ReadProp( CToken& token, DWORD dwParam=0 ) override;					// txt 파싱
	
	xPROP* GetpProp( LPCTSTR szIdentifier );
	// strIdentifier는 모두 소문자여야 함.
	xPROP* GetpProp( const _tstring& strIdentifier ) { 
		XBREAK( strIdentifier.empty() == TRUE ); 
		return GetpProp( strIdentifier.c_str() ); 
	}
	xPROP* GetpProp( ID idType );
	xPROP* GetpProp( XGAME::xtUnit unit ) {
		return GetpProp( (ID)unit );
	}
	
	void Add( const _tstring& strIdentifier, xPROP *pProp );
	void Add( xPROP *pProp );

	virtual int GetSize( void ) { return m_mapData.size(); }

	LPCTSTR GetszSpr( ID idProp ) {
		XPropUnit::xPROP *pProp = GetpProp( idProp );
		if( pProp == nullptr )
			return nullptr;
		return pProp->strSpr.c_str();
	}

	void OnDidBeforeReadProp( CToken& token ) override;
	void OnDidFinishReadProp( CToken& token ) override;

	xPROP* GetpPropFromName( LPCTSTR szName );
	xPROP* GetPropRandom( void ) {
		return m_aryProp.GetFromRandom();
	}
	//////////////////////////////////////////////////////////////////////////
	// 스탯
// 	float GetSquadPower( XHero *pHero, float stat, XGAME::xtSize size, int levelSquad );
// 	float GetAttackMeleePowerSquad( XHero *pHero, xPROP *pProp, int levelSquad ) {
// 		return GetSquadPower( pHero, pProp->atkMelee, pProp->size, levelSquad );
// 	}
// 	float GetAttackMeleePowerSquad( XHero *pHero, int levelSuqad );
// 	float GetAttackRangePowerSquad( XHero *pHero, xPROP *pProp, int levelSquad ) {
// 		return GetSquadPower( pHero, pProp->atkRange, pProp->size, levelSquad );
// 	}
// 	float GetAttackRangePowerSquad( XHero *pHero, int levelSquad );
// 	int GetMaxHpSquad( XHero *pHero, xPROP *pProp, int levelSquad ) {
// 		return (int)GetSquadPower( pHero, pProp->hpMax, pProp->size, levelSquad );
// 	}
// 	int GetMaxHpSquad( XHero *pHero, int levelSquad );
// 	float GetDefensePowerSquad( XHero *pHero, xPROP *pProp, int levelSquad ) {
// 		return GetSquadPower( pHero, pProp->def, pProp->size, levelSquad );
// 	}
// 	float GetDefensePowerSquad( XHero *pHero, int levelSquad );
	float GetAttackSpeed( ID idProp ) {
		xPROP *pProp = GetpProp( idProp );
		if( XBREAK( pProp == nullptr ) )
			return 0.f;
		return pProp->atkSpeed;
	}
	float GetMoveSpeedPerSec( ID idProp ) {
		xPROP *pProp = GetpProp( idProp );
		if( XBREAK( pProp == nullptr ) )
			return 0.f;
		return pProp->movSpeedPerSec;
	}
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar );
	// 스탯
	//////////////////////////////////////////////////////////////////////////
};

extern XPropUnit *PROP_UNIT;