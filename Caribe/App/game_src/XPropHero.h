#pragma once
#include "XFramework/XEBaseProp.h"

const float HERO_STAT_MULTIPLY = 100.f;
const int VER_PROP_HERO =	1;

class XPropHero : public XEBaseProp
{
public:
	struct xPROP {
	private:
	public:
		ID idProp = 0;					///</< 캐릭터 아이디
		_tstring strIdentifier;		///< 식별 아이디 문자열
		ID idName = 0;

		XGAME::xtAttack typeAtk = XGAME::xAT_NONE;			///< 지휘타입
		XGAME::xtClass classJob = XGAME::xCLS_NONE;	///< 클래스
		XGAME::xtTribe tribe = XGAME::xTB_NONE;		///< 종족
		XGAME::xtGet getType = XGAME::xGET_NONE;	///< 영웅 입수방법
		int priority = 0;					///< 영웅의전장 배치 우선순위(숫자가 작을수록 앞에 배치)
		int lvSpawn = 0;					///< 등장 레벨(군단레벨이 lvSpawn이상이 되어야 등장한다)
		///< 기본 스탯(백분율)
		float atkMelee = 0.f;				///< 근접공격력
		float atkRange = 0.f;				///< 원거리 공격력
		float defense = 0.f;				///< 방어력
		float hpMax = 0.f;					///< 체력
		float attackSpeed = 0.f;			///< 공격 속도
		float moveSpeed = 0.f;			///< 이동 속도
		float radiusAtk = 0.f;		///< 공격사거리
//		int leadPowerMax = 0;			///< 최대 지휘할수 있는 병사인구수
		///< 레벨업 증가치
		float atkMeleeAdd = 0.f;			///< 공격력
		float atkRangeAdd = 0.f;			///< 공격력
		float defenseAdd = 0.f;				///< 방어력
		float hpAdd = 0.f;					///< 체력
		float attackSpeedAdd = 0.f;			///< 공격 속도
		float moveSpeedAdd = 0.f;			///< 이동 속도
//		int leadPowerAdd = 0;				///< 지휘력
		///<
		_tstring strPassive;		///< 스킬
		_tstring strActive;			///< 

		_tstring strFace;			///< 초상화 png파일명
		_tstring strSpr;			///< 스프라이트 파일명
		int movSpeedNormal = 0;		///< 1배속일때 이동속도
		float scale = 1.f;			///< 크기조절
		XE::VEC3 vHSL;				///< HSL조절값.(float형이지만 소수점 사용하지 않음)
		_tstring strName;			// cache용
		const _tstring& GetstrName() const {
			return TEXT_TBL->GetstrText( idName );
		}
		bool IsTanker() const {
			return typeAtk == XGAME::xAT_TANKER;
		}
		BOOL IsRange() const {
			return typeAtk == XGAME::xAT_RANGE;
// 			return classJob == XGAME::xCLS_RANGE ||
// 					classJob == XGAME::xCLS_WIZARD ||
// 					classJob == XGAME::xCLS_HEAL;
		}
		BOOL IsSpeed() const {
//			return classJob == XGAME::xCLS_HORSE;
			return typeAtk == XGAME::xAT_SPEED;
		}
		bool IsEventHero() const {
			return idProp >= 10000;
		}
		const _tstring& GetsidSkill( XGAME::xtIdxSkilltype idxType ) const {
			if( idxType == XGAME::xPASSIVE )
				return strPassive;
			return strActive;
		}
// 		XGAME::xtGrade GetGradeMax() {
// 			return gradeMax;
// 		}
		float _GetStat( int idx ) const {
			switch( idx ) {
			case 0:	return atkMelee;
			case 1: return atkRange;
			case 2: return defense;
			case 3: return hpMax;
			case 4: return attackSpeed;
			case 5: return moveSpeed;
			default: XBREAK( 1 ); break;
			}
			return 0.f;
		}
		float GetStatBase( XGAME::xtStat stat ) const {
			switch( stat ) {
			case XGAME::xSTAT_ATK_MELEE:	return atkMelee;
			case XGAME::xSTAT_ATK_RANGE:	return atkRange;
			case XGAME::xSTAT_DEF:				return defense;
			case XGAME::xSTAT_HP:					return hpMax;
			case XGAME::xSTAT_SPEED_ATK:	return attackSpeed;
			case XGAME::xSTAT_SPEED_MOV:	return moveSpeed;
			default:
				XBREAK(1);
				break;
			}
			return 0;
		}
		// 스탯 증가치
		float GetStatAdd( XGAME::xtStat stat ) const {
			switch( stat ) {
			case XGAME::xSTAT_ATK_MELEE:	return atkMeleeAdd;
			case XGAME::xSTAT_ATK_RANGE:	return atkRangeAdd;
			case XGAME::xSTAT_DEF:				return defenseAdd;
			case XGAME::xSTAT_HP:					return hpAdd;
			case XGAME::xSTAT_SPEED_ATK:	return attackSpeedAdd;
			case XGAME::xSTAT_SPEED_MOV:	return moveSpeedAdd;
			default:
				XBREAK( 1 );
				break;
			}
			return 0;
		}
		// 이 영웅이 grade등급에 lvHero레벨일경우 idStat의 스탯값을 얻는다.
		float GetStat2( XGAME::xtStat idStat, XGAME::xtGrade grade, int lvHero, bool bForShow = false ) const {
			const float addGrade = grade * 0.1f;		// 등급당 10%씩 증가
			const float base = GetStatBase( idStat ) + addGrade;	// 기본스탯에 등급스탯을 더함.
			const auto statVal = base + ( lvHero * GetStatAdd( idStat ) );
			if( bForShow ) {
				if( idStat == XGAME::xSTAT_ATK_RANGE && (IsTanker() || IsSpeed()) )	// 근접영웅의 원거리능력은 무조건 0이다.
					return statVal * 0;
				else
					return statVal * HERO_STAT_MULTIPLY;
			}
			return statVal;
		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
		friend class XPropHero;
	}; // xPROP
private:
	std::map<_tstring, xPROP*> m_mapData;			// 프로퍼티 맵(식별자)
	std::map<ID, xPROP*> m_mapID;					// 프로퍼티 맵(ID)
//	std::map<ID, xPROP*>::iterator m_itorID;
	
	XVector<xPROP*> m_aryProp;				// 전체 프로퍼티 배열

	void Init() {}
	void Destroy();

public:
	XPropHero();
	virtual ~XPropHero() { Destroy(); }

	GET_ACCESSOR_CONST( const XVector<xPROP*>&, aryProp );
	
//	void GetNextClear() { m_itorID = m_mapID.begin(); }		// 반복자 초기화
	BOOL ReadProp( CToken& token, DWORD dwParam ) override;					// txt 파싱
	
	xPROP* GetpProp( LPCTSTR szIdentifier ) const;
	inline const xPROP* GetpPropConst( LPCTSTR szIdentifier ) const {
		return GetpProp( szIdentifier );
	}
	inline const xPROP* GetpPropConst( const _tstring& strIdentifier ) const {
		return GetpProp( strIdentifier.c_str() );
	}
		// strIdentifier는 모두 소문자여야 함.
	xPROP* GetpProp( const _tstring& strIdentifier ) const { 
		XBREAK( strIdentifier.empty() == TRUE ); 
		return GetpProp( strIdentifier.c_str() ); 
	}
	xPROP* GetpProp( ID idProp ) const;
	inline const xPROP* GetpPropConst( ID idProp ) const {
		return GetpProp( idProp );
	}

	void Add( const _tstring& strIdentifier, xPROP *pProp );
	void Add( xPROP *pProp );
	
	int GetSize() override { return m_mapData.size(); }

	LPCTSTR GetszSpr( ID idProp ) {
		auto pProp = GetpProp( idProp );
		if( pProp == nullptr )
			return nullptr;
		return pProp->strSpr.c_str();
	}

	void OnDidFinishReadProp( CToken& token ) override;
	void OnDidBeforeReadProp( CToken& token ) override;

	xPROP* GetpPropFromName( LPCTSTR szName );
	xPROP* GetPropRandom() {
		return m_aryProp.GetFromRandom();
	}
	xPROP* GetPropRandomTemp();
	int GetpPropBySizeToAry( XArrayLinearN<XPropHero::xPROP*,256>& ary, 
														XGAME::xtAttack typeAtk, int lvLimit = 0);
//							XGAME::xtGrade gradeMax=XGAME::xGD_NONE );
// 	xPROP* GetPropRandomByAtkType( XGAME::xtAttack typeAtk, XGAME::xtGrade gradeMax=XGAME::xGD_NONE );
	xPROP* GetPropRandomByAtkType( XGAME::xtAttack typeAtk, int lvLimit );
//	xPROP* GetPropRandomGrade( XGAME::xtGrade gradeMax ) ;
	int GetpPropByGetToAry( XVector<xPROP*> *pOutAry, XGAME::xtGet bitGet, XGAME::xtAttack typeAtk = XGAME::xAT_NONE );
	int GetpPropByGetToAryWithExclude( XVector<xPROP*> *pOutAry
																	, XGAME::xtGet typeGet
																	, const XVector<ID>& aryExclude
																	, XGAME::xtAttack typeAtk = XGAME::xAT_NONE );
	xPROP* GetpPropRandomByGetType( XGAME::xtGet bitGet, XGAME::xtAttack typeAtk = XGAME::xAT_NONE );
	ID GetidPropByIds( LPCTSTR idsHero );
	ID GetidPropByIds( const _tstring& idsHero ) {
		return GetidPropByIds( idsHero.c_str() );
	}
	int GetnumPropByGetType( XGAME::xtGet typeGet );
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
	void DestroyProps();
};

extern XPropHero *PROP_HERO;




