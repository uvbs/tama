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
	xtJuncture m_castJuncture = xJC_NONE;			// 시전시점
	xtWhenUse m_whenUse = xWC_NONE;	// 시전시점(스킬버튼(혹은AI)버튼을 눌렀을때 언제 UseSkill을 하는지에 대한. 0이면 버튼눌러사용하는스킬(=액티브)이 아니다.
	xtBaseTarget m_baseTarget = xBST_SELF;	///< 기준타겟
//	xtCastTarget m_castTarget = xCST_NONE;		///< 시전대상-자신,타겟,지역,반경내,부채꼴,일직선,주변,파티,유닛전체,장군
	xtFriendshipFilt m_bitBaseTarget;	///< 기준타겟우호
	xtBaseTargetCond m_condBaseTarget;	///< 기준타겟조건
	float m_rangeBaseTargetCond;		///< 기준타겟조건범위
	xEffSfx m_TargetEff;					// 타겟이펙트, 기준타겟에 생성되는 이펙트
	xEffSfx m_ShootEff;				///< 슈팅이펙트
	xEffSfx m_ShootTargetEff;			///< 슈팅타겟이펙트
//	xEffSfx m_CastTargetEff;			// 시전대상이펙트, 시전대상에 하나씩 이펙트가 생긴다.(효과가 여러개 있을때 효과마다 이펙트가 생길필요는 없을거 같아 이쪽으로 옮김)
//	xEffSfx m_invokeTargetEff;		// 발동대상이펙트, 발동대상에 하나씩 이펙트가 생긴다.(효과가 여러개 있을때 효과마다 이펙트가 생길필요는 없을거 같아 이쪽으로 옮김)
	_tstring m_strShootObj;				// 발사체
	xtMoving m_MoveType = xMT_STRAIGHT;
	ID m_idShootObj = 1;			// 발사체id
	float m_shootObjSpeed = 0;		// 발사체속도(프레임당이동픽셀)
	int m_Debug = 0;
	XVector<std::string> m_aryTag;
	void Init() {
		m_idSkill = GenerateGlobalID();
		m_CastMethod = XSKILL::xNONE_CAST;
		m_idName = 0;
		m_idDesc = 0;
		m_fCoolTime = 0;
		m_idCastMotion = ACT_SKILL1;
//		m_baseTarget = xBST_SELF;
		m_bitBaseTarget = xfNONESHIP;
		m_condBaseTarget = xBTC_NONE;
		m_rangeBaseTargetCond = 0;
		m_ShootEff.m_Point = xPT_ACTION_EVENT;
		// EFFECT구조체 초기화시킬때 memset으로 밀지말것. 스크립트클래스들어갈 가능성 있음
	}
	void Destroy ();
	BOOL ReplaceAbility( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut ) const;
	BOOL ReplaceDuration( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut ) const;
	BOOL ReplaceInvokeRatio( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut ) const;
	bool ReplaceInvokeApplyRatio( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut ) const;
	BOOL ReplaceRadius( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut ) const;
	void ReplaceToken( int idxEffect
									, const EFFECT* pEffect
									, int lvSkill
									, _tstring* pOut ) const;
	void ReplaceTokenEach( int idParam
											, const _tstring& strToken
											, const EFFECT* pEffect
											, int lvSkill
											, _tstring* pOut ) const;
	SET_ACCESSOR( XSKILL::xCastMethod, CastMethod );
public:
	XSkillDat(void) { Init(); }
	virtual ~XSkillDat(void) { Destroy(); }
	// get/set
	GET_SET_ACCESSOR_CONST( const _tstring&, strIdentifier );
	inline const _tstring& GetIds() const {
		return m_strIdentifier;
	}
	GET_SET_ACCESSOR_CONST( ID, idSkill );
	inline ID getid() const {
		return m_idSkill;
	}
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
//	GET_ACCESSOR_CONST( const xEffSfx&, CastTargetEff );
//	GET_ACCESSOR_CONST( const xEffSfx&, invokeTargetEff );
	GET_ACCESSOR_CONST( xtBaseTarget, baseTarget );
//	GET_ACCESSOR_CONST( xtCastTarget, castTarget );
	GET_ACCESSOR_CONST( xtFriendshipFilt, bitBaseTarget );
	GET_ACCESSOR_CONST( xtBaseTargetCond, condBaseTarget );
	GET_ACCESSOR_CONST( const xEffSfx&, ShootEff );
	GET_ACCESSOR_CONST( const xEffSfx&, ShootTargetEff );
	GET_ACCESSOR_CONST( float, rangeBaseTargetCond );
	GET_ACCESSOR_CONST( const _tstring&, strShootObj );
	GET_ACCESSOR_CONST( ID, idShootObj );
//	GET_ACCESSOR_CONST( float, shootObjSpeed );
	// 프레임당 이동픽셀
	inline float GetMovePixelPerFrame() const {
		return m_shootObjSpeed;
	}
	GET_ACCESSOR_CONST( xtWhenUse, whenUse );
	GET_ACCESSOR_CONST( xtMoving, MoveType );
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
	// 이제 이것은 단순히 스킬종류의 분류를 뜻한다. 지속시간이 무한이냐라는 의미로 이것을 사용해선 안된다.
	// 발동스킬과 비발동스킬도 하나의 카테고리에 들어간다.
	inline bool IsPassiveCategory() const {
		return m_CastMethod == XSKILL::xPASSIVE; 
	}
	inline bool IsActiveCategory() const {
		return m_CastMethod == XSKILL::xACTIVE;
	}
	inline bool IsAbilityCategory() const {
		return m_CastMethod == XSKILL::xABILITY;
	}
	inline bool IsToggleCategory() const {
		return m_CastMethod == XSKILL::xTOGGLE;
	}
	// 액티브사용타입의 스킬인가?(일반적으로 스킬버튼 눌러서 사용하는 스킬을 의미)
	inline bool IsActiveUseType() const {
		return m_whenUse != xWC_NONE;
	}
	inline bool IsInvoke() const {
		return !IsTag( "invoke" );
	}
	// 버프타입의 스킬인가
//	BOOL IsBuff( const XSKILL::EFFECT *pEffect ) const;
	bool IsBuffFinite() const;
	// 자신이나 자신의 부대에게만 쓰는 버프인가.
	bool IsSelfBuff() const;
	/// 패시브형 스킬인가(패시브,특성)
// 	BOOL IsPassiveType() const {
// 		return m_CastMethod == xPASSIVE || m_CastMethod == xABILITY;
// 	}
	//
	void AddEffect( XSKILL::EFFECT *pEffect );
	inline int GetNumEffect() const {
		return m_listEffects.size();
	}
	inline const EFFECT* GetEffectByIdx( int idx ) const {
		XBREAK( m_listEffects.size() == 0 );
		return *(m_listEffects.GetpByIndexConst( idx ));
	}
	void GetSkillDesc( _tstring *pOut, int level ) const;
	inline void GetstrDesc( _tstring *pOut, int level ) const {
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
	inline void AddTag( const std::string& strTag ) {
		if( !m_aryTag.IsExist( strTag ) )
			m_aryTag.push_back( strTag );
	}
	inline bool IsTag( const std::string& strTag ) const {
		return m_aryTag.IsExist( strTag );
	}
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
	float GetDuration( int lv );
private:
	bool ReplaceParam( const EFFECT *pEffect, int idxParam, LPCTSTR szToken, _tstring *pOut ) const;
	// virtual
friend class XESkillMng;
};

XE_NAMESPACE_END;


