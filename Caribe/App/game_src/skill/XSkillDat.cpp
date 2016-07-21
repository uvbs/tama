#include "stdafx.h"
#include "XSkillDat.h"
#include "XESkillMng.h"
#include "XEffect.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START( XSKILL )

ID XSkillDat::s_idGlobal = 0;		// SkillDat 제네레이트용 글로벌아이디
//////////////////////////////////////////////////////////////////////////

void XSkillDat::Destroy() 
{
	XLIST4_DESTROY( m_listEffects );
}

/**
 @brief #능력치# 부분을 abilityMin값으로 변경해서 스트링을 만든다
*/
void XSkillDat::GetSkillDesc( _tstring *pOut, int level )
{
	if( pOut->empty() )
		*pOut = XTEXT( m_idDesc );
	int idxEffect = 0;
	for( auto pEffect : m_listEffects ) {
		BOOL bReplacedAbil = FALSE;
		BOOL bReplacedDura = FALSE;
		BOOL bReplacedProb = FALSE;
		BOOL bReplacedRadius = FALSE;
		if( !pEffect->strInvokeSkill.empty() ) {
			auto pDat = XESkillMng::sGet()->FindByIdentifier( pEffect->strInvokeSkill );
			if( pDat )
				pDat->GetSkillDesc( pOut, level );
		}
// 		else {
			ReplaceToken( idxEffect, pEffect, level, pOut );
// 		}
		++idxEffect;
	}
}

void XSkillDat::ReplaceToken( int idxEffect
														, const EFFECT* pEffect
														, int lvSkill
														, _tstring* pOut )
{
	XVector<_tstring> aryToken(4);
	if( idxEffect == 0 ) {
		// 첫번째 효과에선 번호 없는 토큰도 검사해서 변환한다.
		aryToken[0] = _T( "#ability#" );
		aryToken[1] = _T( "#duration#" );
		aryToken[2] = _T( "#prob#" );
		aryToken[3] = _T( "#radius#" );
		for( int i = 0; i < (int)aryToken.size(); ++i ) {
			ReplaceTokenEach( i+1, aryToken[i], pEffect, lvSkill, pOut );
		}
	}
	const int idParam = idxEffect + 1;
	aryToken[0] = XE::Format( _T( "#ability%d#" ), idParam );
	aryToken[1] = XE::Format( _T( "#duration%d#" ), idParam );
	aryToken[2] = XE::Format( _T( "#prob%d#" ), idParam );
	aryToken[3] = XE::Format( _T( "#radius%d#" ), idParam );
	for( int i = 0; i < (int)aryToken.size(); ++i ) {
		ReplaceTokenEach( i+1, aryToken[i], pEffect, lvSkill, pOut );
	}
}

void XSkillDat::ReplaceTokenEach( int idParam
																, const _tstring& strToken
																, const EFFECT* pEffect
																, int lvSkill
																, _tstring* pOut )
{
	switch( idParam ) {
	case 1:
		ReplaceAbility( pEffect, strToken.c_str(), lvSkill, pOut );
		break;
	case 2:
		ReplaceDuration( pEffect, strToken.c_str(), lvSkill, pOut );
		break;
	case 3:
		ReplaceInvokeRatio( pEffect, strToken.c_str(), lvSkill, pOut );
		break;
	case 4:
		ReplaceRadius( pEffect, strToken.c_str(), lvSkill, pOut );
		break;
	default:
		XBREAK(1);
	}
}



BOOL XSkillDat::ReplaceAbility( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut )
{
	_tstring::size_type idxStart = pOut->find( szToken );	// #ability#의 시작인덱스
	if( pEffect->invokeAbilityMin.size() == 0 )
		return FALSE;
	if( pEffect->invokeAbilityMin.size() == 1 )
		level = 0;
	if( XBREAK( level > pEffect->invokeAbilityMin.size() ) )
		return FALSE;
	int len = _tcslen( szToken );				// 교체할 문자열 길이
	if( idxStart != _tstring::npos ) {
		auto valType = pEffect->valtypeInvokeAbility;
		if( XESkillMng::sGet()->IsRateParam( pEffect->invokeParameter ) )
			valType = xPERCENT;
		if( valType == xPERCENT ) {
			const auto val = pEffect->invokeAbilityMin[level];
			float fAbil;
			if( XESkillMng::sGet()->IsInverseParam(pEffect->invokeParameter) ) {
				fAbil = -(val / (val + 1));
			} else {
				fAbil = fabs( val );
			}
			LPCTSTR szReplace = XE::Format( _T( "%.0f%%" ), fAbil * 100.f );
			pOut->replace( idxStart, len, szReplace );
		} else {
			float fAbil = pEffect->invokeAbilityMin[level];
			if( fAbil < 1.f ) {
				// 1보다 작은값이면 하드코딩한 %라 본다.
				float ability = fabs( fAbil * 100.f );
				LPCTSTR szReplace = XE::Format( _T( "%.0f%%" ), ability );
				pOut->replace( idxStart, len, szReplace );
			} else {
				float ability = fabs( fAbil );
				LPCTSTR szReplace = XE::Format( _T( "%.0f" ), ability );
				pOut->replace( idxStart, len, szReplace );

			}
		}
	} else
		return FALSE;
	return TRUE;
}

BOOL XSkillDat::ReplaceDuration( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut )
{
	_tstring::size_type idxStart = pOut->find( szToken );	// #ability#의 시작인덱스
	if( pEffect->arySecDuration.size() == 0 )
		return FALSE;
	if( pEffect->arySecDuration.size() == 1 )
		level = 0;
	if( XBREAK(level > pEffect->arySecDuration.size()) )
		return FALSE;
	int len = _tcslen( szToken );				// 교체할 문자열 길이
	if( idxStart != _tstring::npos )
	{
		float ability = fabs( pEffect->GetDuration( level ) );
		LPCTSTR szReplace = XE::Format( _T( "%.0f" ), ability );
		pOut->replace( idxStart, len, szReplace );
	} else
		return FALSE;
	return TRUE;
}

BOOL XSkillDat::ReplaceInvokeRatio( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut )
{
	_tstring::size_type idxStart = pOut->find( szToken );	// #ability#의 시작인덱스
	if( pEffect->aryInvokeRatio.size() == 0 )
		return FALSE;
	if( pEffect->aryInvokeRatio.size() == 1 )
		level = 0;
	if( XBREAK( level > pEffect->aryInvokeRatio.size() ) )
		return FALSE;
	int len = _tcslen( szToken );				// 교체할 문자열 길이
	if( idxStart != _tstring::npos ) {
		float ability = fabs( pEffect->aryInvokeRatio[level] * 100.f );
		LPCTSTR szReplace = XE::Format( _T( "%.0f%%" ), ability );
		pOut->replace( idxStart, len, szReplace );
	} else
		return FALSE;
	return TRUE;
}

BOOL XSkillDat::ReplaceRadius( const EFFECT *pEffect, LPCTSTR szToken, int level, _tstring *pOut )
{
	_tstring::size_type idxStart = pOut->find( szToken );	// #ability#의 시작인덱스
	float radius = pEffect->GetInvokeSize( level );
	if( radius == 0 )
		radius = pEffect->castSize.w;
	if( radius == 0 ) {
		if( pEffect->attrAmplify == xEA_CAST_RADIUS )		// 증폭파라메터:시전범위
			radius = pEffect->GetAbilityMin(level);
	}
	if( radius > 0 ) {
		int len = _tcslen( szToken );				// 교체할 문자열 길이
		if( idxStart != _tstring::npos ) {
			LPCTSTR szReplace = XE::Format( _T( "%.0f" ), fabs( radius ) );
			pOut->replace( idxStart, len, szReplace );
		} else
			return FALSE;
	}
	return TRUE;
}

void EFFECT::Serialize( XArchive& ar ) const {
	ar << (char)castTarget;
	ar << (char)castTargetRange;
	ar << (char)castTargetCond;
	ar << (char)castfiltFriendship;

	ar << (char)castfiltPlayerType;
// 	ar << (char)m_CasterEff.m_Point;
// 	ar << (char)m_CastTargetEff.m_Point;
// 	ar << (char)m_PersistEff.m_Point;
	XBREAK( numOverlap > 0xff );
	XBREAK( invokeNumApply > 0xff );
	ar << (char)bDuplicate;
	ar << (char)numOverlap;
	ar << (char)invokeNumApply;

	ar << m_CasterEff;
	ar << m_CastTargetEff;
// 	ar << m_CasterEff.m_strSpr;
// 	ar << m_CasterEff.m_idAct;
// 	ar << m_CastTargetEff.m_strSpr;
// 	ar << m_CastTargetEff.m_idAct;
	ar << castSize;
	ar << arySecDuration;
	ar << m_PersistEff;
// 	ar << m_PersistEff.m_strSpr;
// 	ar << m_PersistEff.m_idAct;
	ar << idCastSound;
	ar << (char)invokeTarget;
	ar << (char)invokefiltFriendship;
	ar << (char)invokefiltPlayerType;
	ar << (char)invokeJuncture;

	ar << (char)invokeTargetCondition;
	ar << (char)valtypeInvokeAbility;
	ar << (char)liveTarget;
	ar << (char)attrAmplify;
	ar << aryInvokeCondition;
	ar << secDurationInvoke;
	ar << idInvokeSkill;
	ar << strInvokeSkill;
	ar << strInvokeTimeSkill;
	ar << strInvokeIfHaveBuff;
	ar << aryInvokeRatio;
	ar << invokeParameter;
	ar << invokeAbilityMin;
	ar << invokeState;
	ar << _invokeSize;
	ar << aryInvokeSize;
	ar << secInvokeDOT;
//	ar << invokeNumApply;
	ar << m_invokeTargetEff;
	ar << m_invokerEff;
// 	ar << m_invokeTargetEff.m_strSpr;
// 	ar << m_invokeTargetEff.m_idAct;
	ar << idInvokeSound;
//	ar << bDuplicate;
//	ar << numOverlap;
	ar << strCreateObj;
	ar << idCreateObj;
	for( int i = 0; i < XNUM_ARRAY(createObjParam); ++i ) {
		ar << createObjParam[i];
	}
	// script시리즈는 모두 생략
	ar << bImmunity;
	ar << invokeAddAbility;
	ar << idAddAbilityToClass;
	ar << idAddAbilityToTribe;
	for( int i = 0; i < XNUM_ARRAY( dwParam ); ++i ) {
		ar << dwParam[i];
	}
//	ar << (char)m_invokeTargetEff.m_Point;
// 	ar << (char)0;
// 	ar << (char)0;
// 	ar << (short)0;
}


void EFFECT::DeSerialize( XArchive& ar, int ) {
	char c0;
//	short s0;
	ar >> c0;    castTarget = (xtCastTarget)c0;
	ar >> c0;    castTargetRange = (xtTargetRangeType)c0;
	ar >> c0;    castTargetCond = (xtTargetCond)c0;
	ar >> c0;    castfiltFriendship = (xtFriendshipFilt)c0;

	ar >> c0;    castfiltPlayerType = (xtPlayerTypeFilt)c0;
// 	ar >> c0;    m_CasterEff.m_Point = (xtPoint)c0;
// 	ar >> c0;    m_CastTargetEff.m_Point = (xtPoint)c0;
// 	ar >> c0;    m_PersistEff.m_Point = (xtPoint)c0;
	ar >> c0;		bDuplicate = (c0 != 0);
	ar >> c0;		numOverlap = c0;
	ar >> c0;		invokeNumApply = c0;

	ar >> m_CasterEff;
	ar >> m_CastTargetEff;
// 	ar >> m_CasterEff.m_strSpr;
// 	ar >> m_CasterEff.m_idAct;
// 	ar >> m_CastTargetEff.m_strSpr;
// 	ar >> m_CastTargetEff.m_idAct;
	ar >> castSize;
	ar >> arySecDuration;
	ar >> m_PersistEff;
// 	ar >> m_PersistEff.m_strSpr;
// 	ar >> m_PersistEff.m_idAct;
	ar >> idCastSound;
	ar >> c0;    invokeTarget = (xtInvokeTarget)c0;
	ar >> c0;    invokefiltFriendship = (xtFriendshipFilt)c0;
	ar >> c0;    invokefiltPlayerType = (xtPlayerTypeFilt)c0;
	ar >> c0;    invokeJuncture = (xtJuncture)c0;

	ar >> c0;    invokeTargetCondition = (xtCondition)c0;
	ar >> c0;    valtypeInvokeAbility = (xtValType)c0;
	ar >> c0;    liveTarget = (xtTargetLive)c0;
	ar >> c0;    attrAmplify = (xtEffectAttr)c0;
	ar >> aryInvokeCondition;
	ar >> secDurationInvoke;
	ar >> idInvokeSkill;
	ar >> strInvokeSkill;
	ar >> strInvokeTimeSkill;
	ar >> strInvokeIfHaveBuff;
	ar >> aryInvokeRatio;
	ar >> invokeParameter;
	ar >> invokeAbilityMin;
	ar >> invokeState;
	ar >> _invokeSize;
	ar >> aryInvokeSize;
	ar >> secInvokeDOT;
//	ar >> invokeNumApply;
	ar >> m_invokeTargetEff;
// 	ar >> m_invokeTargetEff.m_strSpr;
// 	ar >> m_invokeTargetEff.m_idAct;
	ar >> idInvokeSound;
//	ar >> bDuplicate;
//	ar >> numOverlap;
	ar >> strCreateObj;
	ar >> idCreateObj;
	for( int i = 0; i < XNUM_ARRAY( createObjParam ); ++i ) {
		ar >> createObjParam[i];
	}
	// script시리즈는 모두 생략
	ar >> bImmunity;
	ar >> invokeAddAbility;
	ar >> idAddAbilityToClass;
	ar >> idAddAbilityToTribe;
	for( int i = 0; i < XNUM_ARRAY( dwParam ); ++i ) {
		ar >> dwParam[i];
	}
// 	ar >> c0;    m_invokeTargetEff.m_Point = (xtPoint)c0;
// 	ar >> c0;    
// 	ar >> s0;
}

void XSkillDat::Serialize( XArchive& ar ) const
{
	ar << s_idGlobal;
	ar << m_strIdentifier << m_idSkill << m_idName << m_idDesc;
	ar << (char)m_CastMethod;
	ar << (char)m_whenCasting;
	ar << (char)m_baseTarget;
	ar << (char)m_condBaseTarget;
	ar << m_fCoolTime;
	ar << m_strIcon;
	ar << (char)m_listEffects.size();
	ar << (char)m_bitBaseTarget;
	ar << (char)m_Debug;
	ar << (char)0;
	for( auto pEffect : m_listEffects ) {
		pEffect->Serialize( ar );
	}
	ar << m_idCastMotion;
	ar << m_CasterEff;
// 	ar << m_m_CasterEff.m_strSpr;
// 	ar << m_m_CasterEff.m_idAct;
	ar << m_rangeBaseTargetCond;
	ar << m_TargetEff;
// 	ar << m_strTargetEffect;
// 	ar << m_idTargetEffect;
// 	ar << (char)m_pointTargetEffect;
// 	ar << (char)m_loopTargetEffect;
	ar << m_ShootEff;
	ar << m_ShootTargetEff;
// 	ar << (char)m_pointShootEffect;
// 	ar << (char)m_pointShootTargetEffect;
// 	ar << m_strShootEffect;
// 	ar << m_idShootEffect;
// 	ar << m_strShootTargetEffect;
// 	ar << m_idShootTargetEffect;
	ar << m_strShootObj;
	ar << m_idShootObj;
	ar << m_shootObjSpeed;

}
void XSkillDat::DeSerialize( XArchive& ar, int ver )
{
	char c0;
	ar >> s_idGlobal;
	ar >> m_strIdentifier >> m_idSkill >> m_idName >> m_idDesc;
	ar >> c0;    m_CastMethod = (xCastMethod)c0;
	ar >> c0;    m_whenCasting = (xtWhenCasting)c0;
	ar >> c0;    m_baseTarget = (xtBaseTarget)c0;
	ar >> c0;    m_condBaseTarget = (xtBaseTargetCond)c0;
	ar >> m_fCoolTime;
	ar >> m_strIcon;
	
	ar >> c0;		int num = c0;
	ar >> c0;		m_bitBaseTarget = (xtFriendshipFilt)c0;
	ar >> c0;		m_Debug = c0;
	ar >> c0;
	for( int i = 0; i < num; ++i ) {
		auto pEffect = new EFFECT();
		pEffect->DeSerialize( ar, ver );
		m_listEffects.Add( pEffect );
	}
	ar >> m_idCastMotion;
	ar >> m_CasterEff;
// 	ar >> m_m_CasterEff.m_strSpr;
// 	ar >> m_m_CasterEff.m_idAct;
	ar >> m_rangeBaseTargetCond;
	ar >> m_TargetEff;
// 	ar >> m_strTargetEffect;
// 	ar >> m_idTargetEffect;
	ar >> m_ShootEff;
	ar >> m_ShootTargetEff;
// 	ar >> c0;    m_pointTargetEffect = (xtPoint)c0;
// 	ar >> c0;    m_loopTargetEffect = (xtAniLoop)c0;
// 	ar >> c0;    m_pointShootEffect = (xtPoint)c0;
// 	ar >> c0;		 m_pointShootTargetEffect = (xtPoint)c0;
// 	ar >> m_strShootEffect;
// 	ar >> m_idShootEffect;
// 	ar >> m_strShootTargetEffect;
// 	ar >> m_idShootTargetEffect;
	ar >> m_strShootObj;
	ar >> m_idShootObj;
	ar >> m_shootObjSpeed;
}

BOOL XSkillDat::IsBuff( const EFFECT *pEffect ) 
{
	return (pEffect->IsDuration()
					 || pEffect->secInvokeDOT > 0
					 || IsPassive() || IsAbility()) ? TRUE : FALSE;
}
// 자신이나 자신의 부대에게만 쓰는 버프인가.
bool XSkillDat::IsSelfBuff() 
{
	if( m_baseTarget != xBST_SELF )
		return false;
	for( auto pEffect : m_listEffects ) {
		if( pEffect->IsDuration() )
			return true;
	}
	return false;
}

void XSkillDat::AddEffect( EFFECT *pEffect ) 
{
	pEffect->m_snEffect = XE::GenerateID();
	m_listEffects.push_back( pEffect );
}


XE_NAMESPACE_END;


