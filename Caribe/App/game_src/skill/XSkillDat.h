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
	static ID GenerateGlobalID() { return ++s_idGlobal; }
public:
	static ID GetidGlobal() { return s_idGlobal; }
	static void sResetGlobalID() { 
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
	XList4<EFFECT*> m_listEffects;	// 하나의 스킬에는 여러개의 효과를 가질수 있다
	ID m_idCastMotion;			///< 시전동작
	xEffSfx m_CasterEff;		// 시전자 이펙트
	xtWhenCasting m_whenCasting = xWC_BASE_TARGET_NEAR;	// 시전시점
	xtBaseTarget m_baseTarget;	///< 기준타겟
	xtFriendshipFilt m_bitBaseTarget;	///< 기준타겟우호
	xtBaseTargetCond m_condBaseTarget;	///< 기준타겟조건
	float m_rangeBaseTargetCond;		///< 기준타겟조건범위
	xEffSfx m_TargetEff;					// 타겟이펙트, 기준타겟에 생성되는 이펙트
	xEffSfx m_ShootEff;				///< 슈팅이펙트
	xEffSfx m_ShootTargetEff;			///< 슈팅타겟이펙트
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
		m_idCastMotion = ACT_SKILL1;
		m_baseTarget = xBST_SELF;
		m_bitBaseTarget = xfNONESHIP;
		m_condBaseTarget = xBTC_NONE;
		m_rangeBaseTargetCond = 0;
		m_ShootEff.m_Point = xPT_ACTION_EVENT;
		// EFFECT구조체 초기화시킬때 memset으로 밀지말것. 스크립트클래스들어갈 가능성 있음
	}
	void Destroy ();
	BOOL ReplaceAbility( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut );
	BOOL ReplaceDuration( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut );
	BOOL ReplaceInvokeRatio( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut );
	bool ReplaceInvokeApplyRatio( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut );
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
	GET_SET_ACCESSOR_CONST( XSKILL::xCastMethod, CastMethod );
public:
	XSkillDat(void) { Init(); }
	virtual ~XSkillDat(void) { Destroy(); }
	// get/set
	GET_SET_ACCESSOR_CONST( const _tstring&, strIdentifier );
	GET_SET_ACCESSOR_CONST( ID, idSkill );
	GET_SET_ACCESSOR_CONST(ID, idName );
	GET_SET_ACCESSOR_CONST(ID, idDesc );
	GET_SET_ACCESSOR_CONST( float, fCoolTime );
	GET_ACCESSOR_CONST( const _tstring&, strIcon );
	LPCTSTR GetResIcon() const {
		return XE::MakePath(DIR_IMG, m_strIcon );
	}
	GET_SET_ACCESSOR_CONST( ID, idCastMotion );
	GET_ACCESSOR_CONST( const xEffSfx&, CasterEff );
	GET_ACCESSOR_CONST( const xEffSfx&, TargetEff );
	GET_ACCESSOR_CONST( xtBaseTarget, baseTarget );
	GET_ACCESSOR_CONST( xtFriendshipFilt, bitBaseTarget );
	GET_ACCESSOR_CONST( xtBaseTargetCond, condBaseTarget );
	GET_ACCESSOR_CONST( const xEffSfx&, ShootEff );
	GET_ACCESSOR_CONST( const xEffSfx&, ShootTargetEff );
	GET_ACCESSOR_CONST( float, rangeBaseTargetCond );
	GET_ACCESSOR_CONST( const _tstring&, strShootObj );
	GET_ACCESSOR_CONST( ID, idShootObj );
	GET_ACCESSOR_CONST( float, shootObjSpeed );
	GET_ACCESSOR_CONST( xtWhenCasting, whenCasting );
	inline bool IsShootingType() const {
		return !m_strShootObj.empty();
	}
	inline void SetszIcon( LPCTSTR szIcon ) {
		m_strIcon = szIcon;
	}
	inline LPCTSTR GetszIdentifier() const {
		return m_strIdentifier.c_str();
	}
	GET_ACCESSOR_CONST( const XList4<XSKILL::EFFECT*>&, listEffects );
//	GET_SET_ACCESSOR( xtUseType, UseType );
	inline BOOL IsPassive() const {
		return m_CastMethod == XSKILL::xPASSIVE; 
	}
	inline BOOL IsActive() const {
		return m_CastMethod == XSKILL::xACTIVE;
	}
	inline BOOL IsAbility() const {
		return m_CastMethod == XSKILL::xABILITY;
	}
	inline BOOL IsToggle() const {
		return m_CastMethod == XSKILL::xTOGGLE;
	}
	// 버프타입의 스킬인가
	BOOL IsBuff( const XSKILL::EFFECT *pEffect ) const;
	bool IsBuffShort() const;
	// 자신이나 자신의 부대에게만 쓰는 버프인가.
	bool IsSelfBuff() const;
	/// 패시브형 스킬인가(패시브,특성)
	BOOL IsPassiveType() const {
		return m_CastMethod == xPASSIVE || m_CastMethod == xABILITY;
	}
	//
	void AddEffect( XSKILL::EFFECT *pEffect );
	inline int GetNumEffect() const {
		return m_listEffects.size();
	}
	inline const EFFECT* GetEffectByIdx( int idx ) const {
		XBREAK( m_listEffects.size() == 0 );
		return *(m_listEffects.GetpByIndexConst( idx ));
	}
	void GetSkillDesc( _tstring *pOut, int level );
	inline void GetstrDesc( _tstring *pOut, int level ) {
		GetSkillDesc( pOut, level );
	}
	inline LPCTSTR GetstrName() const {
		return XTEXT( m_idName );
	}
	inline LPCTSTR GetSkillName() const {
		return GetstrName();
	}
	inline BOOL IsSameCastMethod( xCastMethod castMethod ) const {
		return m_CastMethod == castMethod;
	}
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
private:
	bool ReplaceParam( const EFFECT *pEffect, int idxParam, LPCTSTR szToken, _tstring *pOut );
	// virtual
friend class XESkillMng;
};

XE_NAMESPACE_END;


