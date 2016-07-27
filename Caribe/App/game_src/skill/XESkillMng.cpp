#include "stdafx.h"
#include "XESkillMng.h"
#include "XSkillDat.h"
#include "SkillDef.h"
#include "XFramework/XTextTableUTF8.h"
#include "etc/ConvertString.h"
#include "XEffect.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
#ifdef _DEBUG
#define _XDEBUG_SKILL
#endif

XE_NAMESPACE_START( XSKILL )

XESkillMng *XESkillMng::s_pInstance = NULL;
// XAutoPtr XSkillMng::s_pDestroyer;		///< 프로그램 종료될때 자동 파괴될수 있게 사용

////////////////////////////////////////////////////////////////
XESkillMng::XESkillMng()
{
	Init();
	XSkillDat::sResetGlobalID();
}

void XESkillMng::Destroy()
{
	LIST_RELEASE( m_listSkillDat, XSkillDat* );
//	LIST_DESTROY( m_listSkillDat, XSkillDat* );
	SAFE_DELETE( m_pTextTable );
}



BOOL XESkillMng::Load( LPCTSTR szXml )
{
	m_strProp = szXml;
	// 파일은 utf16이지만 테이블엔 utf8로 보관
	m_pTextTable = new XTextTableUTF8;	
	if( m_pTextTable->Load( XE::MakePath( DIR_PROP, _T("text_skill.txt") ) ) == FALSE )
	{
		XLOG( "text_skill.txt load failed!" );
		return FALSE;
	}
	XXMLDoc doc;
	if( doc.Load( XE::MakePath( DIR_PROP, szXml ) ) == FALSE )
		return FALSE;
	// 루트노드를 찾는다.
	TiXmlElement *pRoot = doc.GetDoc().FirstChildElement( "skill" );
	XBREAKF( pRoot == NULL, "%s: <skill>root node not found" );
	// 
	AddConstant();
	//
	LoadSkill( pRoot, NULL, NULL );
	return TRUE;
}

#if defined(_XDEBUG_SKILL) && defined(WIN32)

#define XU8LOG( UTF8 )	{ \
		_tstring __str = U82SZ( UTF8 ); \
		XTRACE( "%s\n", __str.c_str() ); \
}
#else
#define XU8LOG( UTF8 )	{}
#endif 

// pRoot의 모든 스킬을 읽는다.
XSkillDat* XESkillMng::LoadSkill( TiXmlElement *pRoot, 
							XSkillDat *pParentDat,
							EFFECT *pParentEffect )
{
	XSkillDat *pSkillDat = NULL;
	EFFECT *pEffect = NULL;
	//////////////////////////////////////////////////////////////////////////
	// 먼저 Attribute(폴더가 아닌것)가 있는지 살펴본다.
	TiXmlAttribute *pAttr = pRoot->FirstAttribute();
	if( pAttr )
	{
		const char *cAttrName = pAttr->Name();
		if( cAttrName )
		{
			// 속성이 하나라도 있으면 skillDat객체를 생성한다.
			// Attribute가 있으면 일단 스킬로 인식하고 Dat객체를 생성한다.
			pSkillDat = CreateSkillDat();
			if( pParentDat )
			{
				ID idSkill = pSkillDat->GetidSkill();
				*pSkillDat = *pParentDat;		// 부모의 데이타를 상속받는다.
				pSkillDat->SetidSkill( idSkill );
				_tstring strIdentifier = U82SZ( pRoot->Value() );
				pSkillDat->SetstrIdentifier( strIdentifier );
			}
			// 디폴트 이펙트버퍼도 하나 받는다.
			pEffect = new EFFECT;		
			if( pParentEffect )
				*pEffect = *pParentEffect;
			// 루프 시작
			do 
			{
				const char *cAttrName = pAttr->Name();
				const char *cParam = pAttr->Value();
				if( cAttrName && cAttrName[ 0 ] != '_' )
				{
					XU8LOG( cAttrName );
					XU8LOG( cParam );
					// 변수명과 값을 파싱해서 pSkillDat에 넣는다.
					ParsingAttr( pAttr, cAttrName, cParam, pSkillDat, pEffect );
				}
			} while (( pAttr = pAttr->Next() ));
		}
	}
	
	// pRoot에 폴더가 있는지 찾는다.
	TiXmlElement *pElemChild = pRoot->FirstChildElement();
	if( pElemChild  )
	{
		do 
		{
			// pRoot하의 모든 폴더를 하나씩 꺼낸다.
			const char *cFolderName = pElemChild->Value();
			if( cFolderName && cFolderName[0] != '_' ) {
				XU8LOG( cFolderName );
				//////////////////////////////////////////////////////////////////////////
				// "효과"블럭은 따로 처리
				if( XSAME( pElemChild->Value(), 96 ) ||
					XSAME( pElemChild->Value(), 226 ) )	{ // 효과/발동효과
					if( pSkillDat == NULL ) {
						pSkillDat = CreateSkillDat();
						if( pParentDat ) {
							ID idSkill = pSkillDat->GetidSkill();
							*pSkillDat = *pParentDat;		// 부모의 데이타를 상속받는다.
							pSkillDat->SetidSkill( idSkill );
							_tstring strIdentifier = U82SZ( pRoot->Value() );
							pSkillDat->SetstrIdentifier( strIdentifier );
						}
					}
					EFFECT *pEffBlock = new EFFECT;
					if( pEffect )
						*pEffBlock = *pEffect;	// 하위상속을 위해 내용 복사.
					else
						*pEffBlock = *pParentEffect;		// 스킬블럭에 디폴트 파라메터가 없으면 부모것을 디폴트로 쓴다.

					int numAttr = LoadEffect( pElemChild, pSkillDat, pEffBlock );
					// 효과블럭안에 아무것도 없었으면 지운다.
					if( numAttr == 0 )
						SAFE_DELETE( pEffBlock );
					if( pEffBlock )
						pSkillDat->AddEffect( pEffBlock );
				} else
				if( XSAME( pElemChild->Value(), 235 ) )	{	// 발동조건
					if( pEffect == nullptr ) {
						pEffect = new EFFECT;
						if( pParentEffect )
							*pEffect = *pParentEffect;
					}
					LoadCond( pElemChild, pEffect );
				} else
					if( cFolderName[0] != '_' )		// 스킬이름이 _로 시작되면 읽지 않는다.
					{
						if( XE::IsSame( cFolderName, "chill_blast_arrow" ) ) {
							int a = 0;
						}
						// 그외 폴더는 일단 스킬로 인식한다.
						XSkillDat* pNewSkillDat = NULL;
						pNewSkillDat = LoadSkill( pElemChild,
							( pSkillDat ) ? pSkillDat : pParentDat,
							( pEffect ) ? pEffect : pParentEffect );
						if( pNewSkillDat ) {
							Add( pNewSkillDat );
						}
					}
			}
		} while (( pElemChild = pElemChild->NextSiblingElement() ));
	}
	if( pSkillDat )
	{
		if( pSkillDat->GetstrIdentifier() == _T("chill_blast_arrow") )
		{
			int a = 0;
		}
		// "효과"블럭이 추가된게 있었으면 디폴트용으로 생성되었던 이펙트 블럭은 필요없으므로 지운다.
		if( pSkillDat->GetNumEffect() > 0 )
		{
			SAFE_DELETE( pEffect );
		}
		else
		// 효과폴더가 없고 발동파라메터가 지정되지 않은 폴더는 스킬이 아니라고 보고 지운다.
		if( pSkillDat->GetNumEffect() == 0 )
		{
			if( pEffect->invokeParameter == 0 && 
				pEffect->invokeState == 0 &&
				pEffect->strInvokeSkill.empty() && 
				pEffect->idInvokeSkill == 0 &&
				pEffect->invokeAbilityMin.size() == 0 &&
				pEffect->invokeJuncture != 99 )		// 발동시점:하드코딩
			{
				SAFE_RELEASE_REF( pSkillDat );
				SAFE_DELETE( pEffect );
			}
			else
				// "효과"블럭으로 추가된게 없고 발동파라메터는 지정되었으면 디폴트 이펙트를 이펙트로 추가한다.
				pSkillDat->AddEffect( pEffect );
		} else
		{
			XBREAK(1);
		}
		if( pSkillDat )
		{
			if( pSkillDat->m_strShootObj.empty() == false && pSkillDat->m_idShootObj == 0 )
				pSkillDat->m_idShootObj = 1;
			// 파라메터 보정
			LIST_LOOP( pSkillDat->GetlistEffects(), EFFECT*, itor, pEffect )
			{
				// 디폴트 파라메터일때.
				if( pEffect->invokeJuncture == xJC_FIRST )
				{
					if( pSkillDat->IsPassiveType() )
						pEffect->invokeJuncture = xJC_PERSIST;
					else
					if( pEffect->IsDuration() )
						// 지속시간 타입이 발동시점 "최초"로 되어있으면 지속형으로 바꾼다.
						pEffect->invokeJuncture = xJC_PERSIST;		
				}
				// 발동시점이 스킬발동시 일때 발동시점스킬이 지정되어 있지 않다면 에러.
// 				if( pEffect->invokeJuncture == xJC_INVOKE_SKILL )
// 				{
// 					XBREAK( pEffect->strInvokeTimeSkill.empty() );
// 				}
				// 발동시점스킬이 지정되어 있다면 발동시점은 무조건 스킬발동시가 된다.
				if( pEffect->strInvokeTimeSkill.empty() == false )
				{
					pEffect->invokeJuncture = xJC_INVOKE_SKILL;	// 이걸하지않으면 지속형 스킬이 되어버림.
				}
				// 발동대상우호가 지정되지 않았으면 시전대상우호를 가져다 쓴다.
				if( pEffect->invokefiltFriendship == xfNONESHIP )
					pEffect->invokefiltFriendship = pEffect->castfiltFriendship;
				// 시전거리는 정해졌는데 시전범위타입이 지정되지 않았으면 디폴트로 원형이 됨
				if( pEffect->castSize.w > 0.f && pEffect->castSize.h > 0.f &&
					pEffect->castTargetRange == xTR_ONE )
					pEffect->castTargetRange = xTR_LINE;
				if( pEffect->castSize.w > 0.f && 
					pEffect->castTargetRange == xTR_ONE )
					pEffect->castTargetRange = xTR_CIRCLE;
				if( pEffect->invokeTarget == xIVT_NONE )
					pEffect->invokeTarget = xIVT_CAST_TARGET;
				if( pEffect->invokeTarget == xIVT_CAST_TARGET_RADIUS ||
					pEffect->invokeTarget == xIVT_CAST_TARGET_SURROUND )
				{
					if( pEffect->IsHaveInvokeSize() == false )
						XALERT("스킬\"%s\":발동범위가 지정되지 않음", pSkillDat->GetstrIdentifier().c_str() );
				}
				AdjustEffectParam( pSkillDat, pEffect );	// virtual
			} END_LOOP;
		}
	} else
		SAFE_DELETE( pEffect );
	return pSkillDat;
}

LPCTSTR XESkillMng::GetText( ID idText )
{
	return XSKTEXT( idText );
}

//#define _XU8(STR)	XStringUTF8(STR)

XSkillDat* XESkillMng::CreateSkillDat( void ) 
{ 
	return new XSkillDat; 
}

// 변수이름 cAttrName을 분석해서 pSkillDat에 넣는다.
BOOL XESkillMng::ParsingAttr( TiXmlAttribute *pAttr, 
							const char *cAttrName,
							const char *cParam,
							XSkillDat* pSkillDat,
							EFFECT *pEffect )
{
	const std::string strAttrName = (cAttrName)? cAttrName : "";
	if( XSAME( cAttrName, 4 ) )		// 이름
	{
		int idName = pAttr->IntValue();
		pSkillDat->SetidName( idName );
	} else
	if( XSAME( cAttrName, 5 ) )		// 설명
	{
		int idDesc = pAttr->IntValue();
		pSkillDat->SetidDesc( idDesc );
	} else
	if( XSAME( cAttrName, 1 ) )		// 시전방식
	{
		xCastMethod cast = (xCastMethod)ParsingParam( cParam );
		pSkillDat->SetCastMethod( cast );
	} else
	if( XSAME( cAttrName, 200 ) )	// 시전동작
	{
		pSkillDat->SetidCastMotion( (ID)pAttr->IntValue() );
	} else
	if( XSAME( cAttrName, 11 ) || strAttrName == "icon" )	// 아이콘
	{
		pSkillDat->SetszIcon( U82SZ(cParam) );
	} else
	if( XSAME( cAttrName, 249) ) {	// 시전시점
		pSkillDat->m_whenCasting = (xtWhenCasting)ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 57) )		// 기준타겟
	{
		pSkillDat->m_baseTarget = (xtBaseTarget) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 61 ) )		// 기준타겟우호
	{
		pSkillDat->m_bitBaseTarget = (xtFriendshipFilt)ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 195 ) )	// 기준타겟조건
	{
		pSkillDat->m_condBaseTarget = (xtBaseTargetCond) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 196 ) )	// 기준타겟범위
	{
		pSkillDat->m_rangeBaseTargetCond = (float)pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 164 ) )	// 타겟이펙트
	{
		pSkillDat->m_TargetEff.m_strSpr = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 167 ) )	// 타겟이펙트생성지점
	{
		pSkillDat->m_TargetEff.m_Point = (xtPoint)ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 165 ) )	// 타겟이펙트id
	{
		pSkillDat->m_TargetEff.m_idAct = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 166 ) )	// 타겟이펙트반복
	{
		pSkillDat->m_TargetEff.m_Loop = (xtAniLoop)ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 135 ) )	// 시전자이펙트
	{	
		pSkillDat->m_CasterEff.m_strSpr = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 137 ) )	// 시전자이펙트생성지점
	{
		pSkillDat->m_CasterEff.m_Point = (xtPoint) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 136 ) )	// 시전자이펙트id
	{	
		pSkillDat->m_CasterEff.m_idAct = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 64 ) )	// 슈팅이펙트
	{
		pSkillDat->m_ShootEff.m_strSpr = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 98 ) )	// 슈팅이펙트생성지점
	{
		pSkillDat->m_ShootEff.m_Point = (xtPoint)ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 97 ) )	// 슈팅이펙트id
	{
		pSkillDat->m_ShootEff.m_idAct = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 192 ) )
	{
		pSkillDat->m_ShootTargetEff.m_strSpr = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 194 ) )	// 슈팅타겟이펙트생성지점
	{
		pSkillDat->m_ShootTargetEff.m_strSpr = (xtPoint)ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 193 ) )	// 슈팅타겟이펙트id
	{
		pSkillDat->m_ShootTargetEff.m_idAct = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 36 ) )	// 발사체
	{	
		pSkillDat->m_strShootObj = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 37 ) )	// 발사체id
	{	
		pSkillDat->m_idShootObj = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 38 ) )	// 발사체속도
	{	
		pSkillDat->m_shootObjSpeed = (float)pAttr->DoubleValue();
	} else
	if( XE::IsSame(cAttrName, "debug") ) {
		int a = pAttr->IntValue();
		pSkillDat->m_Debug = a;
	} else
	if( CustomParsingAttr( pAttr, cAttrName, cParam, pSkillDat, pEffect ) == FALSE )
	{
		// 하위상속클래스에서도 분석실패한 변수명은 이펙트 파싱으로 넘겨본다.
		if( ParsingEffect( pAttr, cAttrName, cParam, pSkillDat, pEffect ) == FALSE )
		{
			// 이펙트 파싱에서도 모른다면 알수없는 변수
			return FALSE;
		}
	} 
	return TRUE;
}


BOOL XESkillMng::ParsingEffect( TiXmlAttribute *pAttr,
								const char *cAttrName,
								const char *cParam,
								XSkillDat* pSkillDat,
								EFFECT *pEffect )
{
#ifdef _DEBUG
	_tstring strUTF16 = U82SZ(cAttrName);
#endif
	if( XSAME( cAttrName, 2 ) ) {	// 시전대상
		pEffect->castTarget = (xtCastTarget) ParsingParam( cParam );
		//
// 		if( pEffect->castTarget == xCST_OTHER || 
// 			pEffect->castTarget == xCST_TARGET_GROUND )
// 				pSkillDat->SetUseType( xUST_TARGETTING );
// 		else
// 		if( pEffect->castTarget == xCST_GROUND ||
// 			pEffect->castTarget == xCST_GROUND_RADIUS_OBJ )
// 				pSkillDat->SetUseType( xUST_GROUND );
// 		else
// 			pSkillDat->SetUseType( xUST_IMMEDIATELY );	// 즉시사용
	} else
	if( XSAME( cAttrName, 15 ) )	// 시전대상 우호
	{
		pEffect->castfiltFriendship = (xtFriendshipFilt) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 199 ) )	// 시전조건
	{
		pEffect->castTargetCond = (xtTargetCond) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 17 ) )	// 시전대상플레이어
	{
		pEffect->castfiltPlayerType = (xtPlayerTypeFilt) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 126 ) )	// 시전범위타입
	{
		pEffect->castTargetRange = (xtTargetRangeType) ParsingParam( cParam );
	} else
// 	if( XSAME( cAttrName, 19 ) )	// 시전시간
// 	{	
// 		pEffect->castTime = (float)pAttr->DoubleValue();
// 	} else
// 	if( XSAME( cAttrName, 135 ) )	// 시전자이펙트
// 	{	
// 		pEffect->m_CasterEff.m_strSpr = U82SZ( cParam );
// 	} else
// 	if( XSAME( cAttrName, 137 ) )	// 시전자이펙트생성지점
// 	{
// 		pEffect->m_CasterEff.m_Point = (xtPoint) ParsingParam( cParam );
// 	} else
// // 	if( XSAME( cAttrName, 155 ) )	// 시전자이펙트반복
// // 	{
// // 		pEffect->casterEffectPlayMode = (xtAniLoop) ParsingParam( cParam );
// // 	} else
// 	if( XSAME( cAttrName, 136 ) )	// 시전자이펙트id
// 	{	
// 		pEffect->m_CasterEff.m_idAct = (int)pAttr->IntValue();
// 	} else
	if( XSAME( cAttrName, 20 ) )	// 시전대상이펙트
	{	
		pEffect->m_CastTargetEff.m_strSpr = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 21 ) )	// 시전대상이펙트id
	{	
		pEffect->m_CastTargetEff.m_idAct = (int)pAttr->IntValue();
	} else
// 	if( XSAME( cAttrName, 97 ) )	// 시전대상이펙트반복
// 	{
// 		pEffect->castTargetEffectPlayMode = (xtAniLoop)ParsingParam( cParam );
// 	} else
	if( XSAME( cAttrName, 156 ) )	// 시전대상이펙트생성지점
	{
		pEffect->m_CastTargetEff.m_Point = (xtPoint) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 60 ) || XSAME( cAttrName, 127 ) )	// /시전범위 시전길이
	{	
		pEffect->castSize.w = (float)pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 128 ) )	// 시전폭
	{
		pEffect->castSize.h = (float)pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 22 ) )	// 지속시간
	{	
//		pEffect->secDuration = (float)pAttr->DoubleValue();
		_tstring strParam = U82SZ( cParam );
		_tstring strAttrName = U82SZ( cAttrName );
		ReadTableAry( strAttrName.c_str(),
			pSkillDat->GetstrIdentifier().c_str(),
			&pEffect->arySecDuration, strParam.c_str(), xVAL );
	} else
	if( XSAME( cAttrName, 233 ) )	// 발동지속시간
	{	
		pEffect->secDurationInvoke = (float)pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 23 ) )	// 시전사운드
	{	
		pEffect->idCastSound = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 3 ) )	// 발동대상
	{
		pEffect->invokeTarget = (xtInvokeTarget) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 10 ) )	// 발동대상 우호
	{
		pEffect->invokefiltFriendship = (xtFriendshipFilt) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 24 ) )	// 발동대상플레이어
	{
		pEffect->invokefiltPlayerType = (xtPlayerTypeFilt) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 122 ) )	// 발동시점
	{
		if( pSkillDat->GetstrIdentifier() == _T("chill_blast_arrow") )
		{
			int a = 0;
		}
		pEffect->invokeJuncture = (xtJuncture) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 25 ) )	// 발동대상조건
	{
		pEffect->invokeTargetCondition = (xtCondition) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 99 ) )	// 발동스킬id
	{	
		pEffect->idInvokeSkill = (ID)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 26 ) )	// 발동스킬
	{	
		pEffect->strInvokeSkill = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 232 ) )	// 발동시점스킬
	{
		pEffect->strInvokeTimeSkill = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 239 ) )	// 발동조건스킬
	{
		pEffect->strInvokeIfHaveBuff = U82SZ( cParam ); 
	} else
	if( XSAME( cAttrName, 116 ) )	// 발동확률
	{
		const _tstring strParam = U82SZ( cParam ); 
		const _tstring strAttrName = U82SZ( cAttrName );
		ReadTableAry( strAttrName.c_str(), 
					pSkillDat->GetstrIdentifier().c_str(), 
					&pEffect->aryInvokeRatio, strParam.c_str(), xPERCENT );
	} else
	if( XSAME( cAttrName, 257 ) )	{// 발동적용확률
		const _tstring strParam = U82SZ( cParam ); 
		const _tstring strAttrName = U82SZ( cAttrName );
		ReadTableAry2( strAttrName.c_str(), 
					pSkillDat->GetstrIdentifier().c_str(), 
					&pEffect->m_aryInvokeApplyRatio, strParam.c_str(), xPERCENT );
	} else
	if( XSAME( cAttrName, 7 ) || XSAME( cAttrName, 161 ) )	// 발동파라메터/효과인덱스.
	{	
		pEffect->invokeParameter = (int)ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 228) )	// 증폭파라메터
	{
		pEffect->attrAmplify = (xtEffectAttr)ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 8 ) )	// 능력치
	{	
		CToken token;
		token.LoadString( U82SZ( cParam ) );
		token.GetToken();
		pEffect->valtypeInvokeAbility = GetValType( token.m_Token[0] );
		if( pEffect->valtypeInvokeAbility == xNONE_VALTYPE )
		{	
			XALERT( "skill %s:unknown valtype.", pSkillDat->GetszIdentifier() );
			return FALSE;
		}
		// %#기호뒤에 숫자가 더 있을때만 읽는다.

		if( token.IsEof() == FALSE )
		{
			float ability = token.GetNumberF();
			if( pEffect->invokeAbilityMin.size() != 0 )
			{
				XALERT( "skill %s(%s):이미 능력치값이 존재합니다.",
					pSkillDat->GetszIdentifier(),
					U82SZ( cAttrName ) );
				return FALSE;
			}
			// "능력치"만 단독으로 쓰일때는 index 0을 사용한다.
			if( pEffect->valtypeInvokeAbility == xPERCENT )
				pEffect->invokeAbilityMin.Add( ability / 100.f );
			else
				pEffect->invokeAbilityMin.Add( ability );
		}

	} else
	if( XSAME( cAttrName, 125 ))	// 능력치테이블
	{
		_tstring strParam = U82SZ( cParam ); 
		_tstring strAttrName = U82SZ( cAttrName );
		ReadTableAry( strAttrName.c_str(), 
					pSkillDat->GetstrIdentifier().c_str(), 
					&pEffect->invokeAbilityMin, strParam.c_str(), pEffect->valtypeInvokeAbility );
		if( pEffect->invokeParameter == 0 )
		{
			XALERT( "skill %s(%s):발동파라메터가 없거나(해석하지 못했거나) 능력치테이블의 앞에 있지 않습니다.",
							pSkillDat->GetszIdentifier(),
							U82SZ( cAttrName ) );
		}
	} else
	if( XSAME( cAttrName, 101 ) )	// 상태발동
	{
		pEffect->invokeState = (int)ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 65 ) || XSAME( cAttrName, 28 ) )	// 발동반경/범위
	{	
		_tstring strParam = U82SZ( cParam );
		_tstring strAttrName = U82SZ( cAttrName );
		ReadTableAry( strAttrName.c_str(),
			pSkillDat->GetstrIdentifier().c_str(),
			&pEffect->aryInvokeSize, strParam.c_str(), xVAL );
	} else
	if( XSAME( cAttrName, 159 ) )	// 발동길이
	{
		pEffect->_invokeSize.w = (float)pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 160 ) )	// 발동폭
	{
		pEffect->_invokeSize.h = (float)pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 29 ) )	// 발동주기
	{	
		pEffect->secInvokeDOT = (float)pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 32 ) )	// 적용대상수
	{	
		pEffect->invokeNumApply = pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 30 ) /*|| XSAME( cAttrName, 162 )*/  )	// 발동대상이펙트
	{	
		if( pEffect->m_invokeTargetEff.m_strSpr.empty() == false )
			XLOGXN("%s: 중복입력. 기존값:%s", U82SZ(cAttrName), pEffect->m_invokeTargetEff.m_strSpr.c_str() );
		pEffect->m_invokeTargetEff.m_strSpr = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 31 ) /*|| XSAME( cAttrName, 163 )*/ )	// 발동대상이펙트id
	{	
		pEffect->m_invokeTargetEff.m_idAct = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 157 ) )	// 발동대상이펙트생성지점
	{
		pEffect->m_invokeTargetEff.m_Point = (xtPoint) ParsingParam( cParam );
	} else
// 	if( XSAME( cAttrName, 98 ) )	// 발동이펙트반복
// 	{
// 		pEffect->invokeEffectPlayMode = (xtAniLoop)ParsingParam( cParam );
// 	} else
	if( XSAME( cAttrName, 162 )  )	// 발동자이펙트
	{	
		if( pEffect->m_invokerEff.m_strSpr.empty() == false )
			XLOGXN("%s: 중복입력. 기존값:%s", U82SZ(cAttrName), pEffect->m_invokerEff.m_strSpr.c_str() );
		pEffect->m_invokerEff.m_strSpr = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 163 ) )	// 발동자대상이펙트id
	{	
		pEffect->m_invokerEff.m_idAct = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 258 ) )	// 발동자이펙트생성지점
	{
		pEffect->m_invokerEff.m_Point = (xtPoint) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 33 ) )	// 발동사운드
	{	
		pEffect->idInvokeSound = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 34 ) )	// 중복가능
	{
		pEffect->bDuplicate = (BOOL)ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 100 ) )	// 버프중첩
	{	
		pEffect->numOverlap = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 175 ) )	// 대상생존
	{
		pEffect->liveTarget = (xtTargetLive) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 129 ) )	// 소환
	{
		pEffect->strCreateObj = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 134 ) )	// 소환id
	{
		pEffect->idCreateObj = (ID)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 130 ) )	// 소환변수1
	{
		pEffect->createObjParam[0] = (float) pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 130 ) )	// 소환변수1
	{
		pEffect->createObjParam[0] = (float) pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 131 ) )	// 소환변수2
	{
		pEffect->createObjParam[1] = (float) pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 132 ) )	// 소환변수3
	{
		pEffect->createObjParam[2] = (float) pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 133 ) )	// 소환변수4
	{
		pEffect->createObjParam[1] = (float) pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 44 ) )	// 사용
	{	
		pEffect->scriptUse = cParam;
	} else
	if( XSAME( cAttrName, 43 ) )	// 대상시전
	{	
		pEffect->scriptCast = cParam;
	} else
	if( XSAME( cAttrName, 39 ) )	// 발동시작
	{	
		pEffect->scriptInit = cParam;
	} else
	if( XSAME( cAttrName, 40 ) )	// 발동중
	{	
		pEffect->scriptProcess = cParam;
	} else
	if( XSAME( cAttrName, 41 ) )	// 발동끝
	{	
		pEffect->scriptUninit = cParam;
	} else
	if( XSAME( cAttrName, 42 ) )	// 도트
	{	
		pEffect->scriptDOT = cParam;
	} else
	if( XSAME( cAttrName, 171 ) )	// 면역여부
	{ 
		pEffect->bImmunity = (BOOL) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 189 ) )	// 지속이펙트
	{	
		pEffect->m_PersistEff.m_strSpr = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 191 ) )	// 지속이펙트생성지점
	{
		pEffect->m_PersistEff.m_Point = (xtPoint) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 190 ) )	// 지속이펙트id
	{	
		pEffect->m_PersistEff.m_idAct = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 207 ) )	// 파라메터1
	{	
		pEffect->dwParam[ 0 ] = ParsingConstantAndNumber( cParam );
	} else
	if( XSAME( cAttrName, 208 ) )	// 파라메터2
	{	
		pEffect->dwParam[1] = ParsingConstantAndNumber( cParam );
	} else
	if( XSAME( cAttrName, 209 ) )	// 파라메터3
	{	
		pEffect->dwParam[2] = ParsingConstantAndNumber( cParam );
	} else
	if( XSAME( cAttrName, 210 ) )	// 파라메터4
	{	
		pEffect->dwParam[3] = ParsingConstantAndNumber( cParam );
	} else
	if( XE::IsSame(cAttrName, "debug") ) {
		int a = pAttr->IntValue();
		pEffect->m_Debug = a;
	} else
	if( CustomParsingEffect( pAttr, cAttrName, cParam, pSkillDat, pEffect ) == FALSE )
	{
		XLOGXN( "unknown variable name: %s", U82SZ( cAttrName ) );
		return FALSE;
	}
	return TRUE;
}

// 스킬파라메터 상수 등록
void XESkillMng::AddConstant( void )
{
	XBREAK( CONSTANT == NULL );
	CONSTANT->Add( XSKTEXT( 48 ), xPASSIVE );
	CONSTANT->Add( XSKTEXT( 49 ), xACTIVE );
	CONSTANT->Add( XSKTEXT( 197 ), xABILITY );
	CONSTANT->Add( XSKTEXT( 19 ), xBST_SELF );			// 자신
	CONSTANT->Add( XSKTEXT( 14 ), xBST_CURR_TARGET );	// 현재타겟
	CONSTANT->Add( XSKTEXT( 66 ), xBST_POSITION );	// 좌표
	CONSTANT->Add( XSKTEXT( 202 ), xBST_CONDITION );	// 조건
	CONSTANT->Add( XSKTEXT( 203 ), xBTC_LEAST_HP_PARTY );	// 가장체력이적은부대
	CONSTANT->Add( XSKTEXT( 204 ), xBTC_HIGHEST_HP_PARTY );	// 가장체력이많은부대
	CONSTANT->Add( XTEXT( 1075 ), xBTC_CURR_TARGET_LEADER );	// 기준타겟리더
	CONSTANT->Add( XSKTEXT( 229 ), xBTC_RANDOM );				// 랜덤파티

	CONSTANT->Add( XSKTEXT( 57 ), xCST_BASE_TARGET );		// 기준타겟
	CONSTANT->Add( XSKTEXT( 147 ), xCST_BASE_TARGET_RADIUS );	// 기준타겟반경
	CONSTANT->Add( XSKTEXT( 58 ), xCST_BASE_TARGET_SURROUND );	// 기준타겟주변
	CONSTANT->Add( XSKTEXT( 59 ), xCST_BASE_TARGET_POS );	// 기준타겟좌표
	CONSTANT->Add( XSKTEXT( 198 ), xCST_BASE_TARGET_PARTY );	// 기준타겟파티
	CONSTANT->Add( XSKTEXT(45 ),  0xffffffff );		// 모두
	CONSTANT->Add( XSKTEXT( 146 ), xIVT_CAST_TARGET );	// 시전대상
	CONSTANT->Add( XSKTEXT( 148 ), xIVT_CAST_TARGET_RADIUS );	// 시전대상반경
	CONSTANT->Add( XSKTEXT( 149 ), xIVT_CAST_TARGET_SURROUND );	// 시전대상주변
	CONSTANT->Add( XSKTEXT( 150 ), xIVT_CAST_TARGET_PARTY );	// 시전대상파티
	CONSTANT->Add( XSKTEXT( 240 ), xIVT_CAST_TARGET_POS );	// 시전대상좌표
	CONSTANT->Add( XSKTEXT( 241 ), xIVT_CAST_POS_RADIUS );	// 시전좌표반경
	CONSTANT->Add( XSKTEXT( 108 ), xIVT_ATTACKER );	// 타격자
	CONSTANT->Add( XSKTEXT( 247 ), xIVT_ATTACKER );	// 공격자
	CONSTANT->Add( XSKTEXT( 109 ), xIVT_ATTACKED_TARGET );	// 피격자
	CONSTANT->Add( XSKTEXT( 222 ), xIVT_ATTACKED_TARGET_RADIUS );
	CONSTANT->Add( XSKTEXT( 223 ), xIVT_ATTACKED_TARGET_SURROUND );
	CONSTANT->Add( XSKTEXT( 224 ), xIVT_ATTACKED_TARGET_PARTY );
	CONSTANT->Add( XSKTEXT( 225 ), xIVT_CURR_TARGET );
//	CONSTANT->Add( XSKTEXT( 236 ), xIVT_ALL );	// 모두(위에 50번("모두")의 0xffffffff과 중복되어서 안넣음.
//	CONSTANT->Add( XSKTEXT( 230 ), xIVT_CONDITION );		// 조건발동
	
	CONSTANT->Add( XSKTEXT(50 ), xfALLY );	// 아군
	CONSTANT->Add( XSKTEXT(144 ), xfALLY );	// 우군
	CONSTANT->Add( XSKTEXT(51 ), xfHOSTILE );
	CONSTANT->Add( XSKTEXT(52 ), xfHOSTILE );
	CONSTANT->Add( XSKTEXT(53 ), xfNEUTRALITY );
	CONSTANT->Add( XSKTEXT(54 ), xfNEUTRALITY );
	CONSTANT->Add( XSKTEXT(55 ), xfHUMAN );
	CONSTANT->Add( XSKTEXT(56 ), xfAI );
	CONSTANT->Add( XSKTEXT(185), xTR_ONE );
	CONSTANT->Add( XSKTEXT(186), xTR_CIRCLE );
	CONSTANT->Add( XSKTEXT(62), xTR_LINE );
	CONSTANT->Add( XSKTEXT(63), xTR_LINE );
	CONSTANT->Add( XSKTEXT(145), xTR_FAN );
	CONSTANT->Add( XSKTEXT(187), xTR_RECT );
	CONSTANT->Add( XSKTEXT(188), xTR_GROUP );
	CONSTANT->Add( XSKTEXT(183), xTC_NEAREST );	// 가장가까운
//	CONSTANT->Add( XSKTEXT(201), xPT_TARGET_POS_CENTER );	// 타겟중심좌표
//	CONSTANT->Add( XSKTEXT(66 ), xATTACK );
	CONSTANT->Add( XSKTEXT(94 ), xAL_LOOP );
	CONSTANT->Add( XSKTEXT(95 ), xAL_ONCE );
	CONSTANT->Add( XSKTEXT(110), xJC_FIRST );
	CONSTANT->Add( XSKTEXT(18), xJC_HIT_POINT );	// 슈팅타겟이펙트타점
	CONSTANT->Add( XSKTEXT(111), xJC_ATTACK );
	CONSTANT->Add( XSKTEXT(113), xJC_DEFENSE );
	CONSTANT->Add( XSKTEXT(114), xJC_EVADE );
	CONSTANT->Add( XSKTEXT(115), xJC_DEAD );
	CONSTANT->Add( XSKTEXT(117), xJC_KILL_ENEMY );
	CONSTANT->Add( XSKTEXT(221), xJC_ALL_HIT );		// 모든피격시
	CONSTANT->Add( XSKTEXT(112), xJC_ALL_HIT );		// 피격시
	CONSTANT->Add( XSKTEXT(123), xJC_CLOSE_HIT );
	CONSTANT->Add( XSKTEXT(124), xJC_RANGE_HIT );
	CONSTANT->Add( XSKTEXT(168), xJC_CLOSE_ATTACK );
	CONSTANT->Add( XSKTEXT(234), xJC_RANGE_ATTACK_START );	// 원거리공격시
	CONSTANT->Add( XSKTEXT(169), xJC_RANGE_ATTACK_ARRIVE );		// 원거리타격시
	CONSTANT->Add( XSKTEXT(170), xJC_LAST );
	CONSTANT->Add( XSKTEXT(231), xJC_ARRIVE_TARGET );
	CONSTANT->Add( XSKTEXT( 238 ), xJC_HP_UNDER );		// 체력이하
	
	CONSTANT->Add( XSKTEXT(138), xPT_TARGET_BOTTOM );
	CONSTANT->Add( XSKTEXT(256), xPT_TARGET_TRACE_BOTTOM );
	CONSTANT->Add( XSKTEXT(139), xPT_TARGET_TOP );
	CONSTANT->Add( XSKTEXT(140), xPT_TARGET_CENTER );
	CONSTANT->Add( XSKTEXT(174), xPT_TARGET_POS );
	CONSTANT->Add( XSKTEXT(253 ), xPT_TARGET_TRACE_CENTER );
	CONSTANT->Add( XSKTEXT( 255 ), xPT_TARGET_TRACE_POS );
	CONSTANT->Add( XSKTEXT(141), xPT_ACTION_EVENT );
	CONSTANT->Add( XSKTEXT(142), xPT_WINDOW_CENTER );
	CONSTANT->Add( XSKTEXT(143), xPT_WINDOW_CENTER_BOTTOM );
	CONSTANT->Add( XSKTEXT(180), xPT_WINDOW_CENTER_BOTTOM );
	CONSTANT->Add( XSKTEXT(172), 1 );	// 가능
	CONSTANT->Add( XSKTEXT(173), 0);	// 불가능
	CONSTANT->Add( XSKTEXT(176), xTL_LIVE );	// 생존
	CONSTANT->Add( XSKTEXT(178), xTL_LIVE );	// 생존자
	CONSTANT->Add( XSKTEXT(177), xTL_DEAD );	// 사망
	CONSTANT->Add( XSKTEXT(179), xTL_DEAD );	// 사망자
	// 발동대상조건
	CONSTANT->Add( XSKTEXT( 215 ), xCOND_HARD_CODE );	// 하드코딩
	CONSTANT->Add( XSKTEXT( 212 ), xATTACK_TARGET_JOB );	// 공격대상직업
	CONSTANT->Add( XSKTEXT( 213 ), xATTACK_TARGET_TRIBE );	// 공격대상종족
	CONSTANT->Add( XSKTEXT( 214 ), xATTACK_TARGET_SIZE );	// 공격대상크기
	CONSTANT->Add( XSKTEXT( 8 ), xEA_ABILITY );		// 능력치
	CONSTANT->Add( XSKTEXT( 60 ), xEA_CAST_RADIUS );	// 시전범위
	//
	CONSTANT->Add( XSKTEXT( 250 ), xWC_IMMEDIATELY ); // 즉시
	CONSTANT->Add( XSKTEXT( 251 ), xWC_BASE_TARGET_NEAR ); // 기준타겟근접
	CONSTANT->Add( XSKTEXT( 252 ), xWC_ATTACK_TARGET_NEAR ); // 공격타겟근접


	// 사용자 정의로 추가할 상수 파라메터값
	OnAddConstant( CONSTANT );	// virtual
}


#ifdef WIN32
// 파라메터 스트링 cParam을 분석해서 DWORD값으로 돌려준다.
DWORD XESkillMng::ParsingParam( LPCTSTR szParam )
{
	DWORD dwParam = CONSTANT->GetValue( szParam );
	if( CONSTANT->IsFound() )
		return dwParam;
	XLOGXN( "unknown parameter: %s", szParam );
	return 0;
}
#endif // WIN32

// 파라메터 스트링 cParam을 분석해서 DWORD값으로 돌려준다.
DWORD XESkillMng::ParsingParam( const char *cParam )
{
	DWORD dwParam = CONSTANT->GetValue( U82SZ( cParam ) );
	if( CONSTANT->IsFound() )
		return dwParam;
	XLOGXN( "unknown parameter: %s", U82SZ( cParam ) );
	return 0;
}

#ifdef WIN32
// 파라메터 스트링 cParam을 분석해서 DWORD값으로 돌려준다.
/**
 @brief 파라메터 스트링을 분석해서 상수로 돌려주고 상수가 아니면 일반 숫자로 돌려준다.
*/
DWORD XESkillMng::ParsingConstantAndNumber( LPCTSTR szParam )
{
	DWORD dwParam = CONSTANT->GetValue( szParam );
	if( CONSTANT->IsFound() )
		return dwParam;
	_tstring strNum = szParam;
	return std::stoul( strNum );
}
#endif // WIN32

/**
 @brief 파라메터 스트링을 분석해서 상수로 돌려주고 상수가 아니면 일반 숫자로 돌려준다.
*/
DWORD XESkillMng::ParsingConstantAndNumber( const char *cParam )
{
	auto szParam = U82SZ(cParam);
	DWORD dwParam = CONSTANT->GetValue( szParam );
	if( CONSTANT->IsFound() )
		return dwParam;
	_tstring strNum = szParam;
	return _tstoi(strNum.c_str());
}

XSkillDat* XESkillMng::Add( XSkillDat *pSkillDat )
{
	for( auto pDat : m_listSkillDat )
	{
		if( pDat->GetstrIdentifier() == pSkillDat->GetstrIdentifier() )
		{
			XALERT("중복된 스킬식별자: %s", pSkillDat->GetstrIdentifier().c_str() );
		}
	}
	m_listSkillDat.push_back( pSkillDat );
	return pSkillDat;
}

XSkillDat* XESkillMng::FindByID( ID idDat )
{
	for( auto pDat : m_listSkillDat )
	{
		if( pDat->GetidSkill() == idDat )
			return pDat;
	}
	return NULL;
}

XSkillDat* XESkillMng::FindByName( ID idName )
{
	for( auto pDat : m_listSkillDat )
	{
		if( pDat->GetidName(), idName )
			return pDat;
	}
	return NULL;
} 

XSkillDat* XESkillMng::FindByRandomActive( void ) 
{
	int size = m_listSkillDat.size();
	int select = random(size);
	int i = 0;
	XArrayLinearN<XSkillDat*, 1024> ary;
	for( auto pSkillDat : m_listSkillDat )
	{
		if( pSkillDat->GetCastMethod() == XSKILL::xACTIVE )
			ary.Add( pSkillDat );
	}

	return ary.GetFromRandom();
}

XSkillDat* XESkillMng::FindByRandomPassive( void ) 
{
	int size = m_listSkillDat.size();
	int select = random(size);
	int i = 0;
	XArrayLinearN<XSkillDat*, 1024> ary;
	for( auto pSkillDat : m_listSkillDat )
	{
		if( pSkillDat->GetCastMethod() == XSKILL::xPASSIVE )
			ary.Add( pSkillDat );
	}

	return ary.GetFromRandom();
}

XSkillDat* XESkillMng::FindByIdentifier( LPCTSTR szIdentifier )
{
	if( XE::IsEmpty(szIdentifier) )
		return nullptr;
	for( auto pDat : m_listSkillDat ) {
		if( XE::IsSame( pDat->GetstrIdentifier().c_str(), szIdentifier ) )
			return pDat;
	}
	return nullptr;
} 

void XESkillMng::LoadCond( TiXmlElement *pElemCond, EFFECT *pEffect )
{
	// "발동조건"안의 attribute들을 읽는다.
	xCOND cond;
	TiXmlAttribute *pCondAttr = pElemCond->FirstAttribute();
	while( pCondAttr )
	{
		// "발동조건"블럭안의 attribute들을 모두 읽는다.
		const char *cAttrName = pCondAttr->Name();
		const char *cVal = pCondAttr->Value();
		XU8LOG( cAttrName );
		XU8LOG( cVal );
		// 조건파라메터를 하나씩 분석해서 어레이에 쌓는다.
		xCOND_PARAM param;
		param.cond = (XSKILL::xtCondition)ParsingParam( cAttrName );
		param.val = ParsingParam( cVal );
		if( param.cond )
			cond.aryParam.Add( param );
		pCondAttr = pCondAttr->Next();
	}
	if( cond.aryParam.size() > 0 )
	{
		if( XASSERT(pEffect) )
			pEffect->aryInvokeCondition.Add( cond );
	}
}

int XESkillMng::LoadEffect( TiXmlElement *pElemEff, 
							XSkillDat *pSkillDat, 
							EFFECT *pEffect )
{
	// "발동조건"블럭이 있는지 본다.
	TiXmlElement *pElemCond = pElemEff->FirstChildElement();
	while( pElemCond )
	{
		if( XSAME( pElemCond->Value(), 235 ) )		// 발동조건
			LoadCond( pElemCond, pEffect );
		pElemCond = pElemCond->NextSiblingElement();
	}
	//
	int numAttr = 0;
	// Attribute들을 루프돈다.
	TiXmlAttribute *pAttr = pElemEff->FirstAttribute();
	if( pAttr == NULL )
		return 0;
	do 
	{
		++numAttr;
		const char *cAttrName = pAttr->Name();
		const char *cParam = pAttr->Value();
		XU8LOG( cAttrName );
		XU8LOG( cParam );
		// 변수명과 값을 파싱해서 pEffect에 넣는다.
		if( cAttrName[0] != '_')
			ParsingEffect( pAttr, cAttrName, cParam, pSkillDat, pEffect );
	} while (( pAttr = pAttr->Next() ));

	return numAttr;
}


xtValType XESkillMng::GetValType( TCHAR tcChar )		
{
	if( tcChar == '%' )	
		return xPERCENT;
	else if( tcChar == '#' )		
		return xVAL;
	else if( tcChar == '!' )		
		return xFIXED_VAL;
	return xNONE_VALTYPE;
}

void XESkillMng::Serialize( XArchive& ar ) const
{
	ar << VER_PROP_SKILL;
	ar << m_strProp;
	ar << (int)m_listSkillDat.size();
	for( auto pDat : m_listSkillDat ) {
		pDat->Serialize( ar );
	}
}
void XESkillMng::DeSerialize( XArchive& ar, int )
{
	Destroy();
	int ver, num;
	ar >> ver;
	ar >> m_strProp;
	ar >> num;
	for( int i = 0; i < num; ++i ) {
		auto pDat = new XSkillDat();
		pDat->DeSerialize( ar, ver );
		m_listSkillDat.Add( pDat );
	}
}

	/**
	 @brief 테이블형태로 된 스트링을 읽어 어레이에 담는다.
	*/
int XESkillMng::ReadTableAry2( LPCTSTR szAttrName,
															LPCTSTR idsSkill,
															XVector<float>* pOutAry,
															LPCTSTR szStr,
															xtValType valType ) {
	CToken token;
	token.LoadString( szStr );
	int idx = 0;
	// pOutAry.size() != 0 경우도 있다. 상위블럭에서 값을 입력한 경우 하위에 상속되기때문에. 그러므로 그런경우는 클리어 시키고 다시 읽는다.
	pOutAry->Clear();
	pOutAry->Add( 0.f );	// index0은 쓰지 않음.
	int level = 0;
	float numFirst = 0;
	while( 1 ) {
		if( token.IsEof() )
			break;
		numFirst = token.GetNumberF();
		if( numFirst == TOKEN_ERROR )
			break;
		if( token.IsEof() )
			break;
		level = (int)numFirst;
		if( level != idx + 1 ) {
			XALERT( "skill %s(%s):레벨번호가 순차적이지 않습니다.level:%d",
				idsSkill,
				szAttrName,
				level );
			return 0;
		}
		token.GetToken();	// :
		float ability = token.GetNumberF();
		if( token.IsError() ) {
			XALERT( "skill %s(%s):잘못된 숫자입니다..level:%d", idsSkill,
																												szAttrName,
																												level );
			return 0;
		}
		if( valType == xPERCENT ) {
			pOutAry->Add( ability / 100.f );
		} else {
			pOutAry->Add( ability );
		}
		++idx;
		// 레벨 10을 넘어가도 더이상 읽지 않는다.
		if( idx >= XGAME::MAX_SKILL_LEVEL )
			break;
	}
	// 만약 스트링이 테이블형태가 아니고 숫자하나일경우 0번인덱스에 값을 넣는다.
	if( pOutAry->size() == 1 ) {
		if( valType == xPERCENT )
			( *pOutAry )[0] = numFirst / 100.f;
		else
			( *pOutAry )[0] = numFirst;
	}
	return pOutAry->size();
}


XE_NAMESPACE_END;

