#pragma once
#include <list>
#include "SkillDef.h"
//#include "etc/Token.h"
#include "XRefObj.h"

XE_NAMESPACE_START( XSKILL )

struct EFFECT;

// 스킬하나에 대한 데이터 정의
class XSkillDat : public XRefObj
{
	static ID s_idGlobal;				// SkillDat m_idSkill제네레이트용 글로벌아이디
	static ID GenerateGlobalID( void ) { return ++s_idGlobal; }
public:
	static ID GetidGlobal() { return s_idGlobal; }
	static void sResetGlobalID( void ) { 
		s_idGlobal = 0; 
	}
private:
	_tstring m_strIdentifier;	// 고유식별자
	ID	m_idSkill;				// 스킬데이타고유 번호
	ID m_idName;				// 스킬이름
	ID m_idDesc;				// 스킬설명
	XSKILL::xCastMethod m_CastMethod;		// 시전방식
	float	m_fCoolTime;		// 쿨타임, 재사용대기시간
	_tstring m_strIcon;			// 아이콘 파일명
//	std::list<XSKILL::EFFECT*> m_listEffects;	// 하나의 스킬에는 여러개의 효과를 가질수 있다
	XList4<EFFECT*> m_listEffects;	// 하나의 스킬에는 여러개의 효과를 가질수 있다
	ID m_idCastMotion;			///< 시전동작
	xEffSfx m_CasterEff;		// 시전자 이펙트
// 	_tstring m_m_CasterEff.m_strSpr;	///< 시전자 이펙트 spr
// 	ID m_m_CasterEff.m_idAct;		///< 시전자 이펙트 ID
	xtWhenCasting m_whenCasting = xWC_BASE_TARGET_NEAR;	// 시전시점
	xtBaseTarget m_baseTarget;	///< 기준타겟
	xtFriendshipFilt m_bitBaseTarget;	///< 기준타겟우호
	xtBaseTargetCond m_condBaseTarget;	///< 기준타겟조건
	float m_rangeBaseTargetCond;		///< 기준타겟조건범위
	xEffSfx m_TargetEff;					// 타겟이펙트, 기준타겟에 생성되는 이펙트
// 	_tstring m_strTargetEffect;	// 타겟이펙트, 기준타겟에 생성되는 이펙트
// 	ID m_idTargetEffect;		// 타겟이펙트id
// 	xtPoint m_pointTargetEffect;		// 타겟이펙트생성지점
// 	xtAniLoop m_loopTargetEffect;		// 타겟이펙트반복
	xEffSfx m_ShootEff;				///< 슈팅이펙트
// 	_tstring m_strShootEffect;	///< 슈팅이펙트
// 	ID m_idShootEffect;			///< 슈팅이펙트id
// 	xtPoint m_pointShootEffect;	///< 슈팅이펙트생성위치
	xEffSfx m_ShootTargetEff;			///< 슈팅타겟이펙트
// 	_tstring m_strShootTargetEffect;	///< 슈팅타겟이펙트
// 	ID m_idShootTargetEffect;			///< 이펙트 아이디
// 	xtPoint m_pointShootTargetEffect;	///< 슈팅타겟이펙트생성위치
	_tstring m_strShootObj;				// 발사체
	ID m_idShootObj = 1;			// 발사체id
	float m_shootObjSpeed = 0.6f;		// 발사체속도
	int m_Debug = 0;
	void Init() {
		m_idSkill = GenerateGlobalID();
		m_CastMethod = XSKILL::xNONE_CAST;
		m_idName = 0;
		m_idDesc = 0;
		m_fCoolTime = 0;
//		m_UseType = xUST_NONE;
		m_idCastMotion = ACT_SKILL1;
//		m_m_CasterEff.m_idAct = 1;
		m_baseTarget = xBST_SELF;
		m_bitBaseTarget = xfNONESHIP;
		m_condBaseTarget = xBTC_NONE;
		m_rangeBaseTargetCond = 0;
// 		m_idTargetEffect = 1;
// 		m_pointTargetEffect = xPT_TARGET_POS;
// 		m_loopTargetEffect = xAL_ONCE;
// 		m_idShootEffect = 1;
		m_ShootEff.m_Point = xPT_ACTION_EVENT;
// 		m_pointShootEffect = xPT_ACTION_EVENT;
// 		m_idShootTargetEffect = 1;
// 		m_pointShootTargetEffect = xPT_TARGET_POS;
		// EFFECT구조체 초기화시킬때 memset으로 밀지말것. 스크립트클래스들어갈 가능성 있음
	}
	void Destroy ();
	BOOL ReplaceAbility( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut );
	BOOL ReplaceDuration( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut );
	BOOL ReplaceInvokeRatio( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut );
	BOOL ReplaceRadius( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut );
	void ReplaceToken( int idxEffect
									, const EFFECT* pEffect
									, int lvSkill
									, _tstring* pOut );
	void ReplaceTokenEach( int idParam
											, const _tstring& strToken
											, const EFFECT* pEffect
											, int lvSkill
											, _tstring* pOut );
	GET_SET_ACCESSOR( XSKILL::xCastMethod, CastMethod );
public:
	XSkillDat(void) { Init(); }
	virtual ~XSkillDat(void) { Destroy(); }
	// get/set
	GET_SET_ACCESSOR( const _tstring&, strIdentifier );
	GET_SET_ACCESSOR( ID, idSkill );
	GET_SET_ACCESSOR(ID, idName );
	GET_SET_ACCESSOR(ID, idDesc );
	GET_SET_ACCESSOR( float, fCoolTime );
	GET_ACCESSOR_CONST( const _tstring&, strIcon );
	LPCTSTR GetResIcon() {
		return XE::MakePath(DIR_IMG, m_strIcon );
	}
	GET_SET_ACCESSOR( ID, idCastMotion );
	GET_ACCESSOR_CONST( const xEffSfx&, CasterEff );
// 	GET_ACCESSOR( const _tstring&, m_CasterEff.m_strSpr );
// 	GET_ACCESSOR( ID, m_CasterEff.m_idAct );
	GET_ACCESSOR_CONST( const xEffSfx&, TargetEff );
// 	GET_ACCESSOR( const _tstring&, strTargetEffect );
// 	GET_ACCESSOR( ID, idTargetEffect );
// 	GET_ACCESSOR( xtPoint, pointTargetEffect );
	GET_ACCESSOR( xtBaseTarget, baseTarget );
	GET_ACCESSOR( xtFriendshipFilt, bitBaseTarget );
	GET_ACCESSOR( xtBaseTargetCond, condBaseTarget );
	GET_ACCESSOR_CONST( const xEffSfx&, ShootEff );
// 	GET_ACCESSOR( const _tstring&, strShootEffect );
// 	GET_ACCESSOR( ID, idShootEffect );
// 	GET_ACCESSOR( xtPoint, pointShootEffect );
// 	GET_ACCESSOR( xtAniLoop, loopTargetEffect );
	GET_ACCESSOR_CONST( const xEffSfx&, ShootTargetEff );
// 	GET_ACCESSOR( const _tstring&, strShootTargetEffect );
// 	GET_ACCESSOR( ID, idShootTargetEffect );
// 	GET_ACCESSOR( xtPoint, pointShootTargetEffect );
	GET_ACCESSOR( float, rangeBaseTargetCond );
	GET_ACCESSOR( const _tstring&, strShootObj );
	GET_ACCESSOR( ID, idShootObj );
	GET_ACCESSOR( float, shootObjSpeed );
	GET_ACCESSOR( xtWhenCasting, whenCasting );
	bool IsShootingType() {
		return !m_strShootObj.empty();
	}
	void SetszIcon( LPCTSTR szIcon ) {
		m_strIcon = szIcon;
	}
	LPCTSTR GetszIdentifier( void ) {
		return m_strIdentifier.c_str();
	}
	GET_ACCESSOR( XList4<XSKILL::EFFECT*>&, listEffects );
//	GET_SET_ACCESSOR( xtUseType, UseType );
	BOOL IsPassive( void ) { 
		return m_CastMethod == XSKILL::xPASSIVE; 
	}
	BOOL IsActive( void ) {
		return m_CastMethod == XSKILL::xACTIVE;
	}
	BOOL IsAbility( void ) {
		return m_CastMethod == XSKILL::xABILITY;
	}
	BOOL IsToggle( void ) {
		return m_CastMethod == XSKILL::xTOGGLE;
	}
	// 버프타입의 스킬인가
	BOOL IsBuff( const XSKILL::EFFECT *pEffect );
	// 자신이나 자신의 부대에게만 쓰는 버프인가.
	bool IsSelfBuff();
	/// 패시브형 스킬인가(패시브,특성)
	BOOL IsPassiveType() {
		return m_CastMethod == xPASSIVE || m_CastMethod == xABILITY;
	}
	//
	void AddEffect( XSKILL::EFFECT *pEffect );
	int GetNumEffect( void ) {
		return m_listEffects.size();
	}
	EFFECT* GetEffectByIdx( int idx ) {
		XBREAK( m_listEffects.size() == 0 );
		return *(m_listEffects.GetpByIndex( idx ));
	}
	void GetSkillDesc( _tstring *pOut, int level );
	void GetstrDesc( _tstring *pOut, int level ) {
		GetSkillDesc( pOut, level );
	}
	LPCTSTR GetstrName() {
		return XTEXT( m_idName );
	}
	LPCTSTR GetSkillName( void ) {
		return GetstrName();
	}
	BOOL IsSameCastMethod( xCastMethod castMethod ) {
		return m_CastMethod == castMethod;
	}
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
	// virtual
friend class XESkillMng;
};

XE_NAMESPACE_END;


