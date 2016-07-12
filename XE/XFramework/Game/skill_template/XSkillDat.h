#pragma once
#include <list>
using namespace std;
#include "SkillDef.h"
//#include "etc/Token.h"
#include "XRefObj.h"

NAMESPACE_XSKILL_START

// 스킬하나에 대한 데이터 정의
class XSkillDat : public XRefObj
{
	static ID s_idGlobal;				// SkillDat m_idSkill제네레이트용 글로벌아이디
	static ID GenerateGlobalID( void ) { return ++s_idGlobal; }
public:
	static ID GetidGlobal() { return s_idGlobal; }
private:
//	int		m_nRefCnt;
	_tstring m_strIdentifier;	// 고유식별자
	ID	m_idSkill;				// 스킬데이타고유 번호
	ID m_idName;				// 스킬이름
	ID m_idDesc;				// 스킬설명
	XSKILL::xCastMethod m_CastMethod;		// 시전방식
	XSKILL::xtTargetMethod m_TargetMethod;	// 타겟방법
	XSKILL::xtTargetCond m_TargetCond;		// 타겟조건
	XSKILL::xtTargetType m_TargetType;		// 타겟종류
	float	m_fCoolTime;		// 쿨타임, 재사용대기시간
	_tstring m_strIcon;			// 아이콘 파일명
	float m_UseMP;				// 소모(필요) 마나
	list<XSKILL::EFFECT*> m_listEffects;	// 하나의 스킬에는 여러개의 효과를 가질수 있다
	xtUseType m_UseType;		// 스킬 사용방식
	ID m_idCastMotion;			///< 시전동작
	void Init() {
//		m_nRefCnt = 0;
		m_idSkill = GenerateGlobalID();
		m_CastMethod = XSKILL::xNONE_CAST;
//		m_szDesc = NULL;
//		m_szName = NULL;
		m_idName = 0;
		m_idDesc = 0;
		m_fCoolTime = 0;
		m_UseMP = 0;
		m_UseType = xUST_NONE;
		m_idCastMotion = 0;
		// EFFECT구조체 초기화시킬때 memset으로 밀지말것. 스크립트클래스들어갈 가능성 있음
	}
	void Destroy () {
		LIST_DESTROY( m_listEffects, XSKILL::EFFECT* );
	}
public:
/*	void operator = ( XSkillDat& rhs ) {
		m_CastMethod = rhs.m_CastMethod;
		m_fCoolTime = rhs.m_fCoolTime;
		m_UseMP = rhs.m_UseMP;
	} */
	XSkillDat(void) { Init(); }
	virtual ~XSkillDat(void) { Destroy(); }
	// get/set
	GET_SET_ACCESSOR( const _tstring&, strIdentifier );
	GET_SET_ACCESSOR( ID, idSkill );
//	GET_SET_ACCESSOR( TCHAR*, szName );
//	GET_SET_ACCESSOR( TCHAR*, szDesc );
	GET_SET_ACCESSOR(ID, idName );
	GET_SET_ACCESSOR(ID, idDesc );
	GET_SET_ACCESSOR( XSKILL::xCastMethod, CastMethod );
	GET_SET_ACCESSOR( float, fCoolTime );
	GET_SET_ACCESSOR( float, UseMP );
	GET_ACCESSOR( const _tstring&, strIcon );
	GET_SET_ACCESSOR( ID, idCastMotion );
	void SetszIcon( LPCTSTR szIcon ) {
		m_strIcon = szIcon;
	}
	LPCTSTR GetszIdentifier( void ) {
		return m_strIdentifier.c_str();
	}
	GET_ACCESSOR( list<XSKILL::EFFECT*>&, listEffects );
	GET_SET_ACCESSOR( xtUseType, UseType );
//	void SetszName( LPCTSTR name ) { _tcscpy_s( m_szName, name ); }
//	void SetszDesc( LPCTSTR desc ) { _tcscpy_s( m_szDesc, desc ); } 
//	void SetszIconSpr( LPCTSTR szIcon ) { _tcscpy_s( m_szIconSpr, szIcon ); } 
	BOOL IsPassive( void ) { return m_CastMethod == XSKILL::xPASSIVE; }
	// 버프타입의 스킬인가
	BOOL IsBuff( const XSKILL::EFFECT *pEffect ) { 
		return (IsPassive() || 
				pEffect->secDuration != 0 || 
				pEffect->secInvokeDOT > 0 )? TRUE : FALSE; 
	}
	//
	void AddEffect( XSKILL::EFFECT *pEffect ) { 
		m_listEffects.push_back( pEffect ); 
	}
	int GetNumEffect( void ) {
		return m_listEffects.size();
	}
	// virtual
//	virtual XSKILL::EFFECT* CreateEffect( void ) { return new XSKILL::EFFECT; }	// 지금 XSkillMng에 이게 들어있는데 XSkillDat에 있는게 맞는거 같다
};

//typedef list<XSkillDat *>				XSkillDat_List;
//typedef list<XSkillDat *>::iterator	XSkillDat_Itor;

NAMESPACE_XSKILL_END;

