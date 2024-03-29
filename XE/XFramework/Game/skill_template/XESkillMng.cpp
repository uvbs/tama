﻿#include "stdafx.h"
#include "XESkillMng.h"
#include "XSkillDat.h"
#include "SkillDef.h"
#include "XTextTableUTF8.h"
#include "etc/ConvertString.h"

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

NAMESPACE_XSKILL_START

// XSkillMng *XSkillMng::s_pInstance = NULL;
// XAutoPtr XSkillMng::s_pDestroyer;		///< 프로그램 종료될때 자동 파괴될수 있게 사용

////////////////////////////////////////////////////////////////
XESkillMng::XESkillMng()
{
	Init();
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
		// Attribute가 있으면 일단 스킬로 인식하고 Dat객체를 생성한다.
		pSkillDat = CreateSkillDat();
		if( pParentDat )
		{
			ID idSkill = pSkillDat->GetidSkill();
			*pSkillDat = *pParentDat;		// 부모의 데이타를 상속받는다.
			pSkillDat->SetidSkill( idSkill );
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
			XU8LOG( cAttrName );
			XU8LOG( cParam );
			// 변수명과 값을 파싱해서 pSkillDat에 넣는다.
			ParsingAttr( pAttr, cAttrName, cParam, pSkillDat, pEffect );
		} while (( pAttr = pAttr->Next() ));
	}
	
	// pRoot에 폴더가 있는지 찾는다.
	TiXmlElement *pElemChild = pRoot->FirstChildElement();
	if( pElemChild  )
	{
		do 
		{
			// pRoot하의 모든 폴더를 하나씩 꺼낸다.
			const char *cFolderName = pElemChild->Value();
			XU8LOG( cFolderName );
			//////////////////////////////////////////////////////////////////////////
			// "효과"블럭은 따로 처리
			if( XSAME( pElemChild->Value(), 96 ) )	// 효과
			{
				if( pSkillDat == NULL )
				{
					pSkillDat = CreateSkillDat();
					if( pParentDat )
					{
						ID idSkill = pSkillDat->GetidSkill();
						*pSkillDat = *pParentDat;		// 부모의 데이타를 상속받는다.
						pSkillDat->SetidSkill( idSkill );
					}
				}
				EFFECT *pEffBlock = new EFFECT;
				if( pEffect )
					*pEffBlock = *pEffect;	// 하위상속을 위해 내용 복사.

				int numAttr = LoadEffect( pElemChild, pSkillDat, pEffBlock );
				// 효과블럭안에 아무것도 없었으면 지운다.
				if( numAttr == 0 )
					SAFE_DELETE( pEffBlock );
				if( pEffBlock )
					pSkillDat->AddEffect( pEffBlock );
			} else 
			if( cFolderName[0] != '_' )		// 스킬이름이 _로 시작되면 읽지 않는다.
			{
				// 그외 폴더는 일단 스킬로 인식한다.
				XSkillDat* pNewSkillDat = NULL;
				pNewSkillDat = LoadSkill( pElemChild, 
										(pSkillDat)? pSkillDat : pParentDat, 
										(pEffect)? pEffect : pParentEffect );
				if( pNewSkillDat )
				{
					_tstring strIdentifier = U82SZ( pElemChild->Value() );
					pNewSkillDat->SetstrIdentifier( strIdentifier );
					Add( pNewSkillDat );
					//
					if( pNewSkillDat->GetCastMethod() == xACTIVE && 
						pNewSkillDat->GetstrIcon().empty() )
					{
						XLOGXN("%s: not found skill icon filename", pNewSkillDat->GetszIdentifier() );
					}
				}
			}
		} while (( pElemChild = pElemChild->NextSiblingElement() ));
	}
	if( pSkillDat )
	{
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
				pEffect->idInvokeSkill == 0 )
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
			// 파라메터 보정
			LIST_LOOP( pSkillDat->GetlistEffects(), EFFECT*, itor, pEffect )
			{
				// 디폴트 파라메터일때.
				if( pEffect->invokeJuncture == xJC_FIRST )
				{
					if( pSkillDat->GetCastMethod() == XSKILL::xPASSIVE )
						pEffect->invokeJuncture = xJC_PERSIST;
					else
					if( pEffect->secDuration > 0 )
						// 지속시간 타입이 발동시점 "최초"로 되어있으면 지속형으로 바꾼다.
						pEffect->invokeJuncture = xJC_PERSIST;		
				}
				// 발동대상우호가 지정되지 않았으면 시전대상우호를 가져다 쓴다.
				if( pEffect->invokefiltFriendship == xfNONESHIP )
					pEffect->invokefiltFriendship = pEffect->castfiltFriendship;
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
	if( XSAME( cAttrName, 4 ) )		// 이름
	{
//		pSkillDat->SetszName( U82SZ( cParam ) );
		int idName = pAttr->IntValue();
		pSkillDat->SetidName( idName );
	} else
	if( XSAME( cAttrName, 5 ) )		// 설명
	{
		int idDesc = pAttr->IntValue();
		pSkillDat->SetidDesc( idDesc );
//		pSkillDat->SetszDesc( U82SZ( cParam ) );
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
	if( XSAME( cAttrName, 11 ) )	// 아이콘
	{
		pSkillDat->SetszIcon( U82SZ(cParam) );
	} else
	if( XSAME( cAttrName, 12 ) || XSAME( cAttrName, 13 ) )	// 쿨타임/재사용대기시간
	{
		float secCool = (float)pAttr->DoubleValue();
		pSkillDat->SetfCoolTime( secCool );
	} else
	if( XSAME( cAttrName, 14 ) )	// 소모마나
	{
		int useMp = pAttr->IntValue();
		pSkillDat->SetUseMP( (float)useMp );
	} else
	if( XSAME( cAttrName, 12 ) || XSAME( cAttrName, 13 ) )	// 쿨타임/재사용대기시간
	{
		float secCool = (float) pAttr->DoubleValue();
		pSkillDat->SetfCoolTime( secCool ); 
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
	if( XSAME( cAttrName, 2 ) )	// 시전대상
	{
		pEffect->castTarget = (xtCastTarget) ParsingParam( cParam );
		//
		if( pEffect->castTarget == xCST_OTHER || 
			pEffect->castTarget == xCST_TARGET_GROUND )
				pSkillDat->SetUseType( xUST_TARGETTING );
		else
		if( pEffect->castTarget == xCST_GROUND ||
			pEffect->castTarget == xCST_GROUND_RADIUS_OBJ )
				pSkillDat->SetUseType( xUST_GROUND );
		else
			pSkillDat->SetUseType( xUST_IMMEDIATELY );	// 즉시사용
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
	if( XSAME( cAttrName, 18 ) )	// 시전거리
	{	
		pEffect->castRange = (float)pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 128 ) )	// 시전폭
	{
		pEffect->castWidth = (float)pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 126 ) )	// 시전범위
	{
		pEffect->castTargetRange = (xtTargetRange) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 19 ) )	// 시전시간
	{	
		pEffect->castTime = (float)pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 164 ) )	// 타겟이펙트
	{	
		pEffect->strTargetEffect = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 167 ) )	// 타겟이펙트생성지점
	{
		pEffect->targetEffectPoint = (xtPoint) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 166 ) )	// 타겟이펙트반복
	{
		pEffect->targetEffectPlayMode = (xtAniLoop) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 165 ) )	// 타겟이펙트id
	{	
		pEffect->idTargetEffect = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 135 ) )	// 시전자이펙트
	{	
		pEffect->strCasterEffect = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 137 ) )	// 시전자이펙트생성지점
	{
		pEffect->casterEffectPoint = (xtPoint) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 155 ) )	// 시전자이펙트반복
	{
		pEffect->casterEffectPlayMode = (xtAniLoop) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 136 ) )	// 시전자이펙트id
	{	
		pEffect->idCasterEffect = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 20 ) )	// 시전대상이펙트
	{	
		pEffect->strCastTargetEffect = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 21 ) )	// 시전대상이펙트id
	{	
		pEffect->idCastTargetEffect = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 97 ) )	// 시전대상이펙트반복
	{
		pEffect->castTargetEffectPlayMode = (xtAniLoop)ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 156 ) )	// 시전대상이펙트생성지점
	{
		pEffect->castTargetEffectPoint = (xtPoint) ParsingParam( cParam );
	} else
// 	if( XSAME( cAttrName, 27 ) || XSAME( cAttrName, 126 ) )	// 시전반경/시전범위
// 	{	
// 		pEffect->castRadius = (float)pAttr->DoubleValue();
// 	} else
// 	if( XSAME( cAttrName, 127 ) )	// 시전길이
// 	{
// 		pEffect->castSize.w = (float)pAttr->DoubleValue();
// 	} else
// 	if( XSAME( cAttrName, 128 ) )	// 시전폭
// 	{
// 		pEffect->castSize.h = (float)pAttr->DoubleValue();
// 	} else
	if( XSAME( cAttrName, 22 ) )	// 지속시간
	{	
		pEffect->secDuration = (float)pAttr->DoubleValue();
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
		pEffect->invokeJuncture = (xtJuncture) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 25 ) )	// 발동조건
	{
		pEffect->invokeCondition = (xtCondition) ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 99 ) )	// 발동스킬id
	{	
		pEffect->idInvokeSkill = (ID)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 26 ) )	// 발동스킬
	{	
		pEffect->strInvokeSkill = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 116 ) )	// 발동확률
	{
		pEffect->invokeRatio = (float)pAttr->DoubleValue() / 100.f;
	} else
	if( XSAME( cAttrName, 7 ) )	// 발동파라메터
	{	
		pEffect->invokeParameter = (int)ParsingParam( cParam );
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
		pEffect->invokeAbilityMin = token.GetNumberF();
		if( pEffect->valtypeInvokeAbility == xPERCENT )
			pEffect->invokeAbilityMin = pEffect->invokeAbilityMin / 100.f;
		// 일단은 하나만 쓰는걸로
		pEffect->invokeAbilityMax = pEffect->invokeAbilityMin;

	} else
	if( XSAME( cAttrName, 101 ) )	// 상태발동
	{
		pEffect->invokeState = (int)ParsingParam( cParam );
	} else
	// 상태시간이란게 있으면 안된다 시간관리는 버프객체에서 담당해야 한다.
//	if( XSAME( cAttrName, 125 ) )	// 상태시간
//	{
//		pEffect->secState = (float) pAttr->DoubleValue();
//	} else
	if( XSAME( cAttrName, 102 ) )	// 상태변수1
	{
		pEffect->invokeStateParam[0] = (float) pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 103 ) )	// 상태변수2
	{
		pEffect->invokeStateParam[1] = (float) pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 104 ) )	// 상태변수3
	{
		pEffect->invokeStateParam[2] = (float) pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 105 ) )	// 상태변수4
	{
		pEffect->invokeStateParam[3] = (float) pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 28 ) || XSAME( cAttrName, 158 ) )	// 발동반경/범위
	{	
		pEffect->invokeRadius = (float)pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 159 ) )	// 발동길이
	{
		pEffect->invokeSize.w = (float)pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 160 ) )	// 발동폭
	{
		pEffect->invokeSize.h = (float)pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 29 ) )	// 발동간격
	{	
		pEffect->secInvokeDOT = (float)pAttr->DoubleValue();
	} else
	if( XSAME( cAttrName, 32 ) )	// 적용대상수
	{	
		pEffect->invokeNumApply = pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 30 ) || XSAME( cAttrName, 162 )  )	// 발동이펙트, 발동대상이펙트
	{	
		if( pEffect->strInvokeEffect.empty() == false )
			XLOGXN("%s: 중복입력. 기존값:%s", U82SZ(cAttrName), pEffect->strInvokeEffect.c_str() );
		pEffect->strInvokeEffect = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 31 ) || XSAME( cAttrName, 163 ) )	// 발동이펙트id, 발동대상이펙트id
	{	
		pEffect->idInvokeEffect = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 98 ) )	// 발동이펙트반복
	{
		pEffect->invokeEffectPlayMode = (xtAniLoop)ParsingParam( cParam );
	} else
	if( XSAME( cAttrName, 157 ) )	// 발동이펙트생성지점
	{
		pEffect->invokeEffectPoint = (xtPoint) ParsingParam( cParam );
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
	if( XSAME( cAttrName, 36 ) )	// 발사체
	{	
		pEffect->strShootObj = U82SZ( cParam );
	} else
	if( XSAME( cAttrName, 37 ) )	// 발사체id
	{	
		pEffect->idShootObj = (int)pAttr->IntValue();
	} else
	if( XSAME( cAttrName, 38 ) )	// 발사체속도
	{	
		pEffect->shootObjSpeed = (float)pAttr->DoubleValue();
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
	CONSTANT->Add( XSKTEXT(48), xPASSIVE );
	CONSTANT->Add( XSKTEXT(49 ),  xACTIVE );	
	CONSTANT->Add( XSKTEXT(57 ),  xCST_SELF );
	CONSTANT->Add( XSKTEXT(58 ),  xCST_OTHER );	// 타겟
	CONSTANT->Add( XSKTEXT(198), xCST_OTHER );		// 타인
	CONSTANT->Add( XSKTEXT(59 ),  xCST_GROUND );
	CONSTANT->Add( XSKTEXT(61 ),  xCST_GROUND_RADIUS_OBJ );
	CONSTANT->Add( XSKTEXT(66 ),  xCST_TARGET_GROUND );
//	CONSTANT->Add( XSKTEXT(145),  xCST_FAN );
//	CONSTANT->Add( XSKTEXT(62 ),  xCST_LINE );
//	CONSTANT->Add( XSKTEXT(63 ),  xCST_LINE );
//	CONSTANT->Add( XSKTEXT(60 ),  xCST_RADIUS );
//	CONSTANT->Add( XSKTEXT(64 ),  xCST_SURROUND );
	CONSTANT->Add( XSKTEXT(65 ),  xCST_PARTY );
	CONSTANT->Add( XSKTEXT(121),  xCST_DEAD );
	CONSTANT->Add( XSKTEXT(46 ),  xCST_ALL_TARGET );	// 전체
	CONSTANT->Add( XSKTEXT(45 ),  0xffffffff );		// 모두
	CONSTANT->Add( XSKTEXT(146),  xIVT_SELF );	// 시전대상자
	CONSTANT->Add( XSKTEXT(147),  xIVT_SELF );	// 대상자
//	CONSTANT->Add( XSKTEXT(147),  xIVT_CASTED_AREA );	// 시전영역
	CONSTANT->Add( XSKTEXT(148),  xIVT_RADIUS );		// 대상자반경
	CONSTANT->Add( XSKTEXT(161),  xIVT_RADIUS );		// 반경
	CONSTANT->Add( XSKTEXT(149),  xIVT_SURROUND );		// 대상자주변
	CONSTANT->Add( XSKTEXT(145),  xIVT_FAN );
	CONSTANT->Add( XSKTEXT(62 ),  xIVT_LINE );
	CONSTANT->Add( XSKTEXT(63 ),  xIVT_LINE );
	CONSTANT->Add( XSKTEXT(150),  xIVT_PARTY );		// 대상자파티
	CONSTANT->Add( XSKTEXT(108), xIVT_ATTACKER );	// 공격자(시전대상자를 공격하는넘)
	CONSTANT->Add( XSKTEXT(109), xIVT_DEFENDER );	// 피격자(시전대상자가 때리는놈_)
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
	CONSTANT->Add( XSKTEXT(201), xPT_TARGET_POS_CENTER );	// 타겟중심좌표


//	CONSTANT->Add( XSKTEXT(66 ), xATTACK );
/*
	// 한글보정상수가 필요하면 하위 상속받는 객체에서 처리할것.
	CONSTANT->Add( XSKTEXT(67 ), xADJ_ATTACK );
	CONSTANT->Add( XSKTEXT(68 ), xADJ_DEFENSE );
	CONSTANT->Add( XSKTEXT(69 ), xADJ_MAX_HP );
	CONSTANT->Add( XSKTEXT(70 ), xADJ_HP );
	CONSTANT->Add( XSKTEXT(71 ), xADJ_HP_REGEN_SEC );
	CONSTANT->Add( XSKTEXT(72 ), xADJ_HP_REGEN );
	CONSTANT->Add( XSKTEXT(73 ), xADJ_MAX_MP );
	CONSTANT->Add( XSKTEXT(74 ), xADJ_MP );
	CONSTANT->Add( XSKTEXT(75 ), xADJ_MP_REGEN_SEC );
	CONSTANT->Add( XSKTEXT(76 ), xADJ_MP_REGEN );
	CONSTANT->Add( XSKTEXT(77 ), xADJ_MP_REDUCE );
	CONSTANT->Add( XSKTEXT(78 ), xADJ_ACQUIRE_MANA );
	CONSTANT->Add( XSKTEXT(79 ), xADJ_MOVE_SPEED );
	CONSTANT->Add( XSKTEXT(80 ), xADJ_ATTACK_SPEED );
	CONSTANT->Add( XSKTEXT(81 ), xADJ_ATTACK_RANGE );
	CONSTANT->Add( XSKTEXT(82 ), xADJ_PHY_DAMAGE );
	CONSTANT->Add( XSKTEXT(83 ), xADJ_MAG_DAMAGE );
	CONSTANT->Add( XSKTEXT(84 ), xADJ_PHY_DAMAGE_RECV );
	CONSTANT->Add( XSKTEXT(85 ), xADJ_MAG_DAMAGE_RECV );
	CONSTANT->Add( XSKTEXT(118), xADJ_DAMAGE_RECV );
	CONSTANT->Add( XSKTEXT(86 ), xADJ_HEAL );
	CONSTANT->Add( XSKTEXT(87 ), xADJ_HEAL_RECV );
	CONSTANT->Add( XSKTEXT(88 ), xADJ_SKILL_POWER );
	CONSTANT->Add( XSKTEXT(89 ), xADJ_SIGHT );
	CONSTANT->Add( XSKTEXT(106), xADJ_PENETRATION_RATE );
	CONSTANT->Add( XSKTEXT(107), xADJ_PENETRATION_PROB );
	CONSTANT->Add( XSKTEXT(119), xADJ_EVADE_RATE );
	CONSTANT->Add( XSKTEXT(120), xADJ_BLOCKING_RATE );
	CONSTANT->Add( XSKTEXT(90 ), xHP );
	CONSTANT->Add( XSKTEXT(91 ), xMP );
	CONSTANT->Add( XSKTEXT(92 ), xVAMPIRIC_HP );
	CONSTANT->Add( XSKTEXT(93 ), xVAMPIRIC_MP );
*/
	CONSTANT->Add( XSKTEXT(94 ), xAL_LOOP );
	CONSTANT->Add( XSKTEXT(95 ), xAL_ONCE );
	CONSTANT->Add( XSKTEXT(110), xJC_FIRST );
	CONSTANT->Add( XSKTEXT(111), xJC_ATTACK );
//	CONSTANT->Add( XSKTEXT(112), xJC_HIT );
	CONSTANT->Add( XSKTEXT(113), xJC_DEFENSE );
	CONSTANT->Add( XSKTEXT(114), xJC_EVADE );
	CONSTANT->Add( XSKTEXT(115), xJC_DEAD );
	CONSTANT->Add( XSKTEXT(117), xJC_KILL_ENEMY );
	CONSTANT->Add( XSKTEXT(123), xJC_CLOSE_HIT );
//	CONSTANT->Add( XSKTEXT(124), xJC_RANGE_HIT );
	CONSTANT->Add( XSKTEXT(168), xJC_CLOSE_ATTACK );
	CONSTANT->Add( XSKTEXT(169), xJC_RANGE_ATTACK );
	CONSTANT->Add( XSKTEXT(170), xJC_LAST );
	CONSTANT->Add( XSKTEXT(138), xPT_TARGET_BOTTOM );
	CONSTANT->Add( XSKTEXT(139), xPT_TARGET_TOP );
	CONSTANT->Add( XSKTEXT(140), xPT_TARGET_CENTER );
	CONSTANT->Add( XSKTEXT(174), xPT_TARGET_POS );
	CONSTANT->Add( XSKTEXT(141), xPT_ACTION_EVENT );
	CONSTANT->Add( XSKTEXT(142), xPT_WINDOW_CENTER );
	CONSTANT->Add( XSKTEXT(143), xPT_WINDOW_CENTER_BOTTOM );
	CONSTANT->Add( XSKTEXT(180), xPT_WINDOW_CENTER_BOTTOM );
	CONSTANT->Add( XSKTEXT(172), 1 );	// 가능
	CONSTANT->Add( XSKTEXT(173), 0);	// 불가능
	CONSTANT->Add( XSKTEXT(176), xTL_LIVE );	// 생존
	CONSTANT->Add( XSKTEXT(178), xTL_LIVE );	// 생존자
	CONSTANT->Add( XSKTEXT(177), xTL_DEAD );	// 사망
	CONSTANT->Add( XSKTEXT(178), xTL_DEAD );	// 사망자
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

XSkillDat* XESkillMng::Add( XSkillDat *pSkillDat )
{
	m_listSkillDat.push_back( pSkillDat );
	return pSkillDat;
}

XSkillDat* XESkillMng::FindByID( ID idDat )
{
	LIST_LOOP( m_listSkillDat, XSkillDat*, itor, pDat )
	{
		if( pDat->GetidSkill() == idDat )
			return pDat;
	} END_LOOP;
	return NULL;
}

XSkillDat* XESkillMng::FindByName( ID idName )
{
	LIST_LOOP( m_listSkillDat, XSkillDat*, itor, pDat )
	{
		if( pDat->GetidName(), idName )
			return pDat;
	} END_LOOP;
	return NULL;
} 

XSkillDat* XESkillMng::FindByRandomActive( void ) 
{
	int size = m_listSkillDat.size();
	int select = random(size);
	int i = 0;
	XArrayLinearN<XSkillDat*, 1024> ary;
	LIST_LOOP( m_listSkillDat, XSkillDat*, itor, pSkillDat )
	{
		if( pSkillDat->GetCastMethod() == XSKILL::xACTIVE )
			ary.Add( pSkillDat );
	} END_LOOP;

	return ary.GetFromRandom();
}

XSkillDat* XESkillMng::FindByRandomPassive( void ) 
{
	int size = m_listSkillDat.size();
	int select = random(size);
	int i = 0;
	XArrayLinearN<XSkillDat*, 1024> ary;
	LIST_LOOP( m_listSkillDat, XSkillDat*, itor, pSkillDat )
	{
		if( pSkillDat->GetCastMethod() == XSKILL::xPASSIVE )
			ary.Add( pSkillDat );
	} END_LOOP;

	return ary.GetFromRandom();
}

XSkillDat* XESkillMng::FindByIdentifier( LPCTSTR szIdentifier )
{
	LIST_LOOP( m_listSkillDat, XSkillDat*, itor, pDat )
	{
		if( XE::IsSame( pDat->GetstrIdentifier().c_str(), szIdentifier ) )
			return pDat;
	} END_LOOP;
	return NULL;
} 

int XESkillMng::LoadEffect( TiXmlElement *pElemEff, 
							XSkillDat *pSkillDat, 
							EFFECT *pEffect )
{
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

NAMESPACE_XSKILL_END;

