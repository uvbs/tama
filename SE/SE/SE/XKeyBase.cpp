#include "stdafx.h"
#include "XKeyBase.h"
#include "XLayerBase.h"
#include "XKeyAll.h"
#include "XAniAction.h"
#include "XLayerBase.h"
#include "XSprObjTool.h"
#include "XLuaSprObj.h"
#include "SprDat.h"
#include "FrameView.h"
#include "XActObj.h"
#include "xLayerInfo.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

void XKeyMng::Add( XBaseKey *pKey ) 
{
	if( XBREAK( pKey == nullptr ) )
		return;
	XMapItor_Key itor;
	for( itor = m_mapKey.begin(); itor != m_mapKey.end(); itor++ )
	{
		XBREAK( (*itor).second == pKey );
	}
	m_mapKey[ pKey->GetidKey() ] = pKey;
}
void XKeyMng::Clear( XBaseKey *pKey ) 
{
	if( pKey->GetidKey() == 0 )	
		return;
	if( XBREAK( pKey == nullptr ) )
		return;
	XBREAK( GetKey( pKey->GetidKey() ) == nullptr );		// 맵에 없는키를 찾으려 할때 경고
	m_mapKey.erase( pKey->GetidKey() );	// 맵에서 삭제
}
//////////////////////////////////////////////////////////////////////////
xSpr::xtKey XBaseKey::GetTypeFromLayer( xSpr::xtLayer typeLayer ) {	// 레이어타입에 맞는 메인키 타입을 돌려준다
	switch( typeLayer )
	{
	case xSpr::xLT_IMAGE:		return xSpr::xKT_IMAGE;
	case xSpr::xLT_OBJECT:			return xSpr::xKT_CREATEOBJ;
	case xSpr::xLT_SOUND:	return xSpr::xKT_SOUND;
	case xSpr::xLT_EVENT:		return xSpr::xKT_EVENT;
	case xSpr::xLT_DUMMY:	return xSpr::xKT_DUMMY;
	default:
		XBREAKF( 1, "잘못된 레이어 타입 %d", (int)typeLayer );
		return xSpr::xKT_NONE;
	}
}
// 레이어의 서브타입을 키서브타입으로 바꿔준다.
xSpr::xtKeySub XBaseKey::sGetSubTypeFromLayer( xSpr::xtLayerSub subTypeLayer ) {
	switch( subTypeLayer ) {
	case xSpr::xLTS_NONE:	return xSpr::xKTS_NONE;
	case xSpr::xLTS_MAIN: return xSpr::xKTS_MAIN;
	case xSpr::xLTS_POS:	return xSpr::xKTS_POS;
	case xSpr::xLTS_ROT:	return xSpr::xKTS_ROT;
	case xSpr::xLTS_SCALE: return xSpr::xKTS_SCALE;
	case xSpr::xLTS_EFFECT: return xSpr::xKTS_EFFECT;
	default:
		XBREAKF( 1, "알수없는 레이어 서브타입,subTypeLayer=%d", subTypeLayer );
		break;
	}
	return xSpr::xKTS_NONE;
}
LPCTSTR XBaseKey::GetTypeString( xSpr::xtKey type ) {
	if( type == xSpr::xKT_NONE )
		type = GetType();
	switch( type )
	{
	case xSpr::xKT_IMAGE:		return _T( "xKEY_IMAGE" );
	case xSpr::xKT_CREATEOBJ:	return _T( "xKEY_CREATEOBJ" );
	case xSpr::xKT_SOUND:		return _T( "xKEY_SOUND" );
	case xSpr::xKT_EVENT:		return _T( "xKEY_EVENT" );
	case xSpr::xKT_DUMMY:	return _T( "xKEY_DUMMY" );
	default:
		XBREAKF( 1, "잘못된 키 타입 %d", (int)type );
		return _T( "xKEY_ERROR" );
	}
}
LPCTSTR XBaseKey::GetSubTypeString() {
	xSpr::xtKeySub type = m_SubType;
	switch( type )
	{
	case xSpr::xKTS_MAIN:		return _T( "xSUB_MAIN" );
	case xSpr::xKTS_POS:	return _T( "xSUB_POS" );
	case xSpr::xKTS_EFFECT:		return _T( "xSUB_EFFECT" );
	case xSpr::xKTS_ROT:		return _T( "xSUB_ROT" );
	case xSpr::xKTS_SCALE:		return _T( "xSUB_SCALE" );
	default:
		XBREAKF( 1, "잘못된 서브키 타입 %d", (int)type );
		return _T( "xSUB_ERROR" );
	}
}

/**
@brief keyType이 layerType에 들어갈수 있는지 검사.
*/
BOOL XBaseKey::IsCorrectLayer( xSpr::xtLayer bitLayer, xSpr::xtKey keyType ) {
	XBREAK( !keyType );
//	if( keyType == xSpr::xKT_NONE )
//		keyType = GetType();
	switch( keyType )
	{
	case xSpr::xKT_IMAGE:		return (bitLayer & xSpr::xLT_IMAGE ) ? TRUE : FALSE;
	case xSpr::xKT_CREATEOBJ:	return (bitLayer & xSpr::xLT_OBJECT ) ? TRUE : FALSE;
	case xSpr::xKT_SOUND:		return (bitLayer & xSpr::xLT_SOUND ) ? TRUE : FALSE;;
	case xSpr::xKT_EVENT:		return (bitLayer & xSpr::xLT_EVENT ) ? TRUE : FALSE;
	case xSpr::xKT_DUMMY:		return (bitLayer & xSpr::xLT_DUMMY ) ? TRUE : FALSE;
	default:
		XBREAKF( 1, "잘못된 키 타입 %d", (int)keyType );
		return FALSE;
	}
	return FALSE;
}

// 거짓조건쪽에 TRUE를 한이유는 spLayer가 nullptr로도 들어오는 경우가 있기때문에 맞다고 리턴하는거다
BOOL XBaseKey::IsCorrectLayer( XSPBaseLayer spLayer, xSpr::xtKey keyType /*= xSpr::xKT_NONE*/ )
{
	return ( spLayer ) ? IsCorrectLayer( spLayer->GetbitType(), keyType ) : TRUE;
}

/**
 @brief 
 @param idLocalInLayer 29미만 파일의 경우 idLocalInLayer가 0일수 있다.
 이 함수는 XBaseKey::sCreateRead()에서만 불려지는걸 가정한다.
*/
XBaseKey* XBaseKey::sCreate( int verSprDat				// XBaseKey::sCreateRead()에서만 부르게 하기위한 컴파일 에러용
													,	XSPAction spAction 					
													, xSpr::xtKey type
													, xSpr::xtKeySub subType
													, ID idLayer
													, ID idLocalInLayer )
{
	XBREAK( verSprDat == 0 );
	if( verSprDat >= 29 ) {
		XBREAK( idLocalInLayer == 0 ); 
	} else {
		// 29아래 버전은 이 값이 0이다.
		XASSERT( idLocalInLayer == 0 );
	}
	switch( type )
	{
	case xSpr::xKT_IMAGE:
	case xSpr::xKT_CREATEOBJ:
	case xSpr::xKT_DUMMY:
		switch( subType ) {
		case xSpr::xKTS_MAIN:
			if( type == xSpr::xKT_IMAGE )
				return new XKeyImage( spAction, idLayer, idLocalInLayer );
			else if( type == xSpr::xKT_CREATEOBJ )
				return new XKeyCreateObj( spAction, idLayer, idLocalInLayer );
			else if( type == xSpr::xKT_DUMMY )
				return new XKeyDummy( spAction, idLayer, idLocalInLayer );
			else {
				XBREAK(1);
			}
			break;
		case xSpr::xKTS_POS:
			return new XKeyPos( spAction, idLayer, idLocalInLayer );
		case xSpr::xKTS_EFFECT:
			return new XKeyEffect( spAction, idLayer, idLocalInLayer );
		case xSpr::xKTS_ROT:
			return new XKeyRot( spAction, idLayer, idLocalInLayer );
		case xSpr::xKTS_SCALE:
			return new XKeyScale( spAction, idLayer, idLocalInLayer );
		default:
			XBREAKF( 1, "알수없는 sub키 타입:typeSub=%d", subType );
			break;
		}
	case xSpr::xKT_SOUND:
		return new XKeySound( spAction, idLayer, idLocalInLayer );
	case xSpr::xKT_EVENT:
		return new XKeyEvent( spAction, idLayer, idLocalInLayer );
	default:
		XBREAKF( 1, "알수없는 키 타입:type=%d", type );
		break;
	}
	return nullptr;
}
/**
 파일스트림에서 데이터를 읽어 그에맞는 레이어객체를 생성시키고 값을 입력한다.
*/
XBaseKey* XBaseKey::sCreateRead( XResFile *pRes
															, XSPAction spAction
															, int verSprDat
															, const std::vector<xLayerInfo>& aryLayerInfo )
{
	BYTE b0;
	WORD w0;
	ID snKey;
	xSpr::xtKeySub typeSub;
	float secFrame;
	if( verSprDat >= 18 ) {
		( *pRes ) >> snKey;
	}
	(*pRes) >> b0;	auto type = (xSpr::xtKey)b0;
	if( verSprDat >= 26 ) {
		( *pRes ) >> b0;	typeSub = (xSpr::xtKeySub)b0;
	}
	(*pRes) >> secFrame;
	ID idLocalInLayer = 0;
	if( verSprDat >= 29 ) {
		( *pRes ) >> idLocalInLayer;
	}
	ID idLayer = 0;
//	XSPBaseLayer spLayer;
	if( verSprDat >= 26 ) {
		if( verSprDat >= 29 ) {
			( *pRes ) >> w0;	// 과거 layer type과 nLayer로 쓰임
		} else {
			( *pRes ) >> b0;	// 과거 layer type과 nLayer로 쓰임
		}
		( *pRes ) >> b0;
		(*pRes) >> idLayer;
// 		spLayer = spAction->GetspLayer( snSerial );
	} else {
// 		(*pRes) >> b0;	auto typeLayer = (xSpr::xtLayer)b0; // 과거 layer type과 nLayer로 쓰임
		// 과거 layer type과 nLayer로 쓰임
		(*pRes) >> b0;	auto typeLayer = sConvertOldTypeToNewType( b0 );		// 새 타입번호로 교체
		(*pRes) >> b0;	int nLayer = b0;
		// 구 코드의 잔재. 타입과 레이어번호로 아이디를 찾는다.
		auto pLayerInfo = spAction->FindpLayerInfoByType( aryLayerInfo, typeLayer, nLayer );
		if( XASSERT(pLayerInfo) )
			idLayer = pLayerInfo->m_idLayer;
//		idLayer = spAction->GetidLayer( typeLayer, nLayer );
// 		spLayer = spAction->GetspLayer( typeLayer, nLayer );

	}
	XBREAK( idLayer == 0 );
	(*pRes) >> b0;	
	if( verSprDat < 26 )
		typeSub = (xSpr::xtKeySub)b0;
	// 
	auto pKey = XBaseKey::sCreate( verSprDat, spAction, type, typeSub
															, idLayer, idLocalInLayer );
	if( XBREAK( pKey == nullptr ) )
		return nullptr;
	XBREAK( pKey->GetidLayer() == 0 );
	if( verSprDat >= 29 ) {
		XBREAK( pKey->GetidLocalInLayer() == 0 );
	} else {
		// 이하버전은 XAniAction::Load에서 넣어준다.
	}
	pKey->SetidKey( snKey );
	pKey->m_Type = type;
	pKey->m_SubType = typeSub;
	pKey->m_fFrame = secFrame;
// 	pKey->//m_spLayer = spLayer;
//	pKey->m_idLayer = idLayer;
	if( verSprDat >= 17 ) {
		int len;
		pRes->Read( &len, 4 ); 		// 루아코드 길이(널포함된것임)
		if( len > 0 ) {
			pKey->m_pcLua = new char[ len + 1 ];
			pRes->Read( pKey->m_pcLua, len + 1 );		// 루아코드 읽음
		}
	}
	// virtual
	pKey->Load( pRes, spAction, verSprDat );
	return pKey;
}
//////////////////////////////////////////////////////////////////////////
/**
 @brief 키를 생성한다.
 @param spAction 소속될 액션
 @param spLayer 소속될 레이어
*/
// XBaseKeyDynamic::XBaseKeyDynamic( const XBaseKeyDynamic& src, XSPActionConst spAct, ID idLocalInLayer )
// //	: (*this = src), XBaseKeyDynamic( std::const_pointer_cast<XAniAction>( spAct ), idLocalInLayer )
// {
// 	*this = src;
// //	m_spAction = spAct;
// 	m_spAction = std::const_pointer_cast<XAniAction>( spAct );
// 	m_idLocalInLayer = idLocalInLayer;
// 
// }

/**
 @brief 
 @param spLayer spLayer로부터 새 지역아이디를 받는다. null이라면 기존 아이디를 유지한다.
 @param idLocalInLayer 레이어내에서 고유한 아이디
*/
XBaseKeyDynamic::XBaseKeyDynamic( XSPAction spAction, ID idLocalInLayer ) 
{
	Init();
	//
	m_spAction = spAction;
	m_idLocalInLayer = idLocalInLayer;
// 	if( m_spAction ) {	
// 		// 이 키는 복사된키를 의미함
// 		// 같은 액션내에서 카피된 키는 아이디를 새로 받지 않음.
// 		bool bSameAct = ( m_spAction->GetidAct() == spAction->GetidAct() );
// 		m_spAction = spAction;
// 		// idLayer내에서 고유한 로컬아이디를 만듬.
// //			m_idLocalInLayer = spLayer->GenerateidKeyLocal();
// 	} else {
// 		m_spAction = spAction;
// // 		if( spLayer )
// // 			m_idLocalInLayer = spLayer->GenerateidKeyLocal();
// 	}
}
//////////////////////////////////////////////////////////////////////////
XBaseKey::XBaseKey( XSPAction spAction, ID idLayer, ID idLocalInLayer )
	: XBaseKeyDynamic( spAction, idLocalInLayer )
// 	: XBaseKeyDynamic( spAction, spAction->GenerateidKeyLocal( idLayer, idLocalInLayer ) )
// 										, (idLocalInLayer)? 
// 														idLocalInLayer : spAction->GenerateidKeyLocal( idLayer, idLocalInLayer ) ) 
{
	Init();
	m_idLayer = idLayer;
}
/**
 @brief 키 카피용 생성자
 @param spLayer null이 아니라면 spLayer에 새로생성되는 키이므로 소속 레이어를 바꿔줌.
*/
// XBaseKey::XBaseKey( const XBaseKey& src, XSPActionConst spActNew, ID idLayer, ID idLocalInLayer )
// //	: /*XBaseKey( src ), */XBaseKeyDynamic( src, spActNew, idLocalInLayer ) 
// {
// 	*this = src;
// 	SetspAction( std::const_pointer_cast<XAniAction>( spActNew ) );
// 	SetidLocalInLayer( idLocalInLayer );
// 	m_idLayer = idLayer;
// 	m_pLua = nullptr;
// 	m_pcLua = nullptr;
// }
XBaseKey::XBaseKey( XSPAction spAction, float fFrame, ID idLayer, ID idLocalInLayer )
	: XBaseKeyDynamic( spAction, idLocalInLayer ) {
// 	: XBaseKeyDynamic( spAction, spAction->GenerateidKeyLocal( idLayer, 0 ) ) {
	Init();
	m_fFrame = fFrame;
	m_idLayer = idLayer;
}
XBaseKey::XBaseKey( XSPAction spAction
									, xSpr::xtKey type
									, xSpr::xtKeySub subType
									, float fFrame
									, XSPBaseLayer spLayer )
	: XBaseKeyDynamic( spAction, spLayer->GenerateidKeyLocal() ) 
// 	: XBaseKeyDynamic( spAction, spAction->GenerateidKeyLocal(spLayer->GetidLayer(), 0) ) 
{
	Init();
	m_SubType = subType;
	XBREAK( spLayer == nullptr );
	m_idLayer = spLayer->GetidLayer();
	XBREAK( m_idLayer == 0 );
	if( IsCorrectLayer( spLayer, type ) == FALSE )
		XLOG( "type:%s:%s키를 생성하는데 맞지않는 레이어(%s)가 왔다"
				, GetTypeString( type )
				, GetTypeString()
				, ( spLayer ) ? spLayer->GetTypeString() : _T( "nullptr" ) );
	m_Type = type;
	m_fFrame = fFrame;
}

// XBaseKey::XBaseKey( const XBaseKey& src )
// 	: XBaseKeyDynamic( src )
// {
// 	*this = src;
// // 	m_pcLua = nullptr;
// // 	m_pLua = nullptr;
// // 	m_nIdx = 0;
// 	// m_spLayer나 m_spAction은 일단 카피하면 원본값을 유지한다.
// 	// 만약 다른 레이어나 다른 액션으로 카피될때는 반드시 그 값으로바꿔줘야 한다.
// }

void XBaseKey::Destroy() 
{
	SAFE_DELETE( m_pLua );
	SAFE_DELETE( m_pcLua );
}

// 같은액션 같은레이어에 복사본 키를 만든다.
XBaseKey* XBaseKey::NewInstanceBySameLayer()
{
//	auto pKeyNew = CreateCopy( m_spAction, m_idLayer, GetidLocalInLayer() );
	auto pKeyNew = CreateCopy();
	SetNewInstance( m_spAction, m_idLayer, GetidLocalInLayer() );
	return pKeyNew;
}

/**
 @brief 같은레이어안에서 this키를 복사한 새로운 인스턴스를 만든다.
*/
void XBaseKey::Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec ) 
{ 
//	auto spLayer = pSprObj->GetspLayer( m_spAction, m_idLayer );
	XBREAK( spLayer == nullptr );
//	spLayer->SetpLastKey( this );		// 안쓰는거 같아서 삭제
//	spLayer->SetpLastKey( this );	
	// 루아코드가 있으면 루아객체 생성
	if( GetpLua() == nullptr && GetpcLua() && XE::IsHave( GetpcLua() ) )
		SetpLua( spActObj->GetpSprObj()->CreateScript() );		// virtual create 할당

	// 루아 Execute핸들러 호출
	if( GetpcLua() ) {
		// 테스트용 루아글루함수코드와 키할당 루아코드랑 합쳐서 코드가 실행될수 있도록 한다
		char cBuff[ 0xffff ] = {0,};
		strcpy_s( cBuff, XSprDat::s_cGlobalLua );
		strcat_s( cBuff, "\n" );
		strcat_s( cBuff, spActObj->GetpSprObj()->GetpSprDat()->GetpcLuaAll() );		// 합쳐진 코드로 쓴다. 그래야 키들간에 전역변수도 공유할수 있다
		GetpLua()->DoString( cBuff );	// 각키에 할당된 루아코드를 실행
		//
		char cTable[ 64 ];
		sprintf_s( cTable, "key_%08x", GetidKey() );		// 테이블 이름 생성
		if( GetpLua()->IsHaveMemberFunc( cTable, "Execute" ) ) { // 루아에 Execute함수가 재정의 되어 있으면..
			int execute = GetpLua()->MemberCall<int>( cTable, "Execute", nullptr );	// 실행
		}
	}
}

// this와 XBaseKey부분만 같은 복사본을 만들어 낸다
// XBaseKey* XBaseKey::CopyKey()
// {
// 	// fuck 이코드가 왜 필요한건지 보고 개선할것.
// //	XBaseKey *pKey = new XBaseKey( m_spAction, m_Type, m_SubType, m_fFrame, GetLayerType(), GetnLayer() );
// 	XBaseKey *pKey = new XBaseKey( m_spAction, m_Type, m_SubType, m_fFrame, m_spLayer );
// 	pKey->SetpShadowParent( this );
// 	return pKey;
// }

// this가 pKey와 같은 종류의 레이어에 있는지 검사한다.
// bCheckOpen이 트루라면 +인터페이스가 열려있는지도 검사한다.
//bool XBaseKey::IsSameLayerByCheckOpen( XBaseKey *pKeyOther, bool bCheckOpen )
//{
//	// !bCheckOpen 이면 레이어 오픈여부와는 상관없이 아이디만 다르면 다른게 된다.
//	if( m_idLayer != pKeyOther->GetidLayer() )
//		return false;
//	if( bCheckOpen ) {
//		// +열리기가능한 레이어면서 +가 열려있는 상태면 서브타입까지 같은지 확인해야 한다 
//		// fuck 걍 m_spLayer->getid로 비교하면 안됨?
//		auto spLayer = spActObj->GetspLayer( m_idLayer );
//		if( m_spLayer->GetbAbleOpen() && m_spLayer->GetbOpen() ) {
//			if( GetSubType() == pKey->GetSubType() )		// 서브타입이 같은지 확인(??이거 필요함?)
//				return TRUE;
//			else
//				return FALSE;
//		}
//		// 닫혀있으면 그냥 비교성공
//	}
//	return TRUE;
//}
//BOOL XBaseKey::IsSameLayer( XBaseKey *pKey, BOOL bCheckOpen )
 //{
 //	// !bCheckOpen 이면 레이어 오픈여부와는 상관없이 아이디만 다르면 다른게 된다.
 //	if( m_idLayer != pKey->GetidLayer() )
 //		return FALSE;
 //// 	if( m_spLayer->getid() != pKey->GetspLayer()->getid() )
 //// 		return FALSE;
 //// 	if( (GetLayerType() == pKey->GetLayerType() 
 //// 		&& GetnLayer() == pKey->GetnLayer()) == FALSE )		// 레이어타입과 레이어번호가 같은가
 ////		return FALSE;
 //	// 아이디부터 다르면 실패
 //// 	if( getid() != pKey->getid() )
 //// 		return false;
 //	XBREAK( GetType() != pKey->GetType() );
 //// 	if( GetType() != pKey->GetType() )
 //// 		return FALSE;
 //	// bCheckOpen이면 레이어가 열려있어야 같다고 인식한다.
 //	if( bCheckOpen ) {
 //		// +열리기가능한 레이어면서 +가 열려있는 상태면 서브타입까지 같은지 확인해야 한다 
 //		// fuck 걍 m_spLayer->getid로 비교하면 안됨?
 //		auto spLayer = spActObj->GetspLayer( m_idLayer );
 //		if( m_spLayer->GetbAbleOpen() && m_spLayer->GetbOpen() ) {
 //			if( GetSubType() == pKey->GetSubType() )		// 서브타입이 같은지 확인(??이거 필요함?)
 //				return TRUE;
 //			else
 //				return FALSE;
 //		}
 //		// 닫혀있으면 그냥 비교성공
 //	}
 //	return TRUE;
 //}

// ID XBaseKey::GetidLayer() const 
// {
// 	return m_spLayer->GetidLayer();
// }

XBaseKey *XBaseKey::GetNextKeyConst() const
{
	xSpr::xtKey type = GetType();
	xSpr::xtKeySub subType = GetSubType();
	auto& spAction = m_spAction;
//	XBaseKey *pNextKey = nullptr;

	auto itor = spAction->GetNextKeyFirst();
	while( XBaseKey *pKey = spAction->GetNextKey( &itor ) ) {
		if( pKey == this )		// this키위치부터 탐색하고
			break;
	}
	while( auto pKey = spAction->GetNextKey( &itor ) ) {
		// 같은레이어/채널내에서 pKey가 this키의 뒤에 있음
		if( IsSameLayerAndChannel( pKey ) )
			return pKey;
	}
	return nullptr;
}

// 액션에서 this키 뒤에 같은 타입의 키가 있으면 그것을 반환한다
XBaseKey *XBaseKey::GetNextKey()
{
	xSpr::xtKey type = GetType();
	xSpr::xtKeySub subType = GetSubType();
	XSPAction& spAction = m_spAction;
	auto& listKey = spAction->GetlistKey();
	auto itor = listKey.begin();
	for( ; itor != listKey.end(); ++itor ) {
		auto pKey = (*itor);
		if( pKey == this ) {
			++itor;
			break;
		}
	}
	for( ; itor != listKey.end(); ++itor ) {
		auto pKey = (*itor);
		if( IsSameLayerAndChannel( pKey ) )
			return pKey;
	}
	// 	spAction->GetNextKeyClear();
// 	while( XBaseKey *pKey = spAction->GetNextKey() ) {
// 		if( pKey == this )		// this키위치부터 탐색하고
// 			break;
// 	}
// 	while( auto pKey = spAction->GetNextKey() ) {
// 		// 같은레이어/채널내에서 pKey가 this키의 뒤에 있음
// 		if( IsSameLayerAndChannel( pKey ) )
// 			return pKey;
// 	}
	return nullptr;
}
XBaseKey *XBaseKey::GetPrevKey()
{
	xSpr::xtKey type = GetType();
	xSpr::xtKeySub subType = GetSubType();
	XSPAction& spAction = m_spAction;
	spAction->GetNextKeyClear();
	// 키 리스트에서 this키를 찾는다. 찾으면서 같은타입의 바로 이전키는 받아둔다.
	XKeyPos* pKeyPrev = nullptr;
	auto& listKey = spAction->GetlistKey();
	for( auto pKey : listKey ) {
		if( IsSameLayerAndChannel( pKey ) ) {
			if( pKey->getid() == getid() ) {
				// this키를 찾았으면 바로 이전에 찾았던 같은타입의 키를 돌려준다.
				return pKeyPrev;
			}
			// 같은타입의 키는 일단 받아둔다.
			pKeyPrev = SafeCast<XKeyPos*>( pKey );
		}
	}
	// 	spAction->GetNextKeyClear();
// 	// fuck 검색 희한하게 하는군
// 	while( XBaseKey *pKey = spAction->GetNextKey() ) {
// 		if( pKey->getid() == getid() ) {		// this키위치부터 탐색하고
// 			spAction->GetPrevKey();		// this키의 앞에 있는 키부터 탐색하도록 포인터를 돌려준다
// 			break;
// 		}
// 	}
// 	while( auto pKey = spAction->GetPrevKey() ) {
// 		// 같은레이어/채널내에서 pKey가 this키의 앞에 있음
// 		if( IsSameLayerAndChannel( pKey ) )
// 			return pKey;
// 	}
	return nullptr;
}

// key_012abc 형태의 테이블 이름을 생성한다
CString XBaseKey::MakeTableName()
{
	CString strTable;
	strTable.Format( _T("key_%08x"), GetidKey() );		
	return strTable;
}
// 
void XBaseKey::ReplaceFuncName( CString *pstrLua, TCHAR *cOld )
{
	CString strNewName;
	strNewName.Format( _T("%s:%s"), MakeTableName(), cOld );		// key012abc:CreateSprObj"
	pstrLua->Replace( cOld, strNewName );				// 함수명을 테이블함수 형태로 바꿈
}

void XBaseKey::CompositLuaCodes( CString *pstrLua )
{
	ReplaceFuncName( pstrLua, _T("Execute") );		// 함수명을 테이블함수 형태로 바꾼다
}

/**
 @brief 구코드의 잔재때문에 typeLayer와 nLayer를 넘기수밖에 없었음.
 구조변경때 제거해야함.
*/
void XBaseKey::Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes )
{
	DWORD dw;
	WORD w0;
	if( GetidKey() == 0 )
		SetidKey( XE::GenerateID() );		// 버그로 아이디가 0으로 들어갔던 키가 몇개 있었다. 0이면 저장할때 아이디 다시 만들어줌
	dw = GetidKey();
	pRes->Write( &dw, 4 );		// id
	BYTE bData = m_Type;
	pRes->Write( &bData, 1 );			// key type
	bData = m_SubType;				// 26버전부터 저장위치가 옮겨짐
	pRes->Write( &bData, 1 );		// key sub type
	pRes->Write( &m_fFrame, 4 );		// frame
	(*pRes) << GetidLocalInLayer();
	// 이제는 필요없어졌지만 게임쪽 코드와 호환성을 맞춰주기위해 일단은 저장한다.
	XBREAK( typeLayer > 0xffff );
	w0 = (WORD)typeLayer;
	pRes->Write( &w0, 2 );
	XBREAK( nLayer > 0xff );
	bData = (BYTE)nLayer;
	pRes->Write( &bData, 1 );
	( *pRes ) << m_idLayer;	// XBaseLayer::m_idSerial
// 	XBREAK( spLayer->GetType() > 0xff );
// 	bData = (BYTE)spLayer->GetType();
// 	pRes->Write( &bData, 1 );
// 	XBREAK( spLayer->GetnLayer() > 0xff );
// 	bData = (BYTE)spLayer->GetnLayer();
// 	pRes->Write( &bData, 1 );
// 	(*pRes) << spLayer->GetidLayer();	// XBaseLayer::m_idSerial
	// key sub type
	bData = m_SubType;	
	pRes->Write( &bData, 1 );		// 하위호환을 위해 남겨둠.
	// 루아 코드 저장
	if( m_pcLua ) {
		int len = strlen( m_pcLua );
		pRes->Write( &len, 4 );
		pRes->Write( m_pcLua, len+1 );		// 널포함해서 저장
	} else {
		DWORD d = 0;			// 루아코드가 없으면 0을 저장
		pRes->Write( &d, 4 );
	}
}
/**
 @brief 
 @param ver SPR_VER
*/
// void XBaseKey::Load( XResFile *pRes, SPAction spAction, int ver )
// {
// 	BYTE bData;
// 	if( ver >= 18 ) {
// 		DWORD dw=0;
// 		pRes->Read( &dw, 4 );
// 		SetidKey( dw );
// 	}
// 	// key type;
// 	pRes->Read( &bData, 1 );	m_Type = (xSpr::xtKey)bData;
// 	if( ver >= 26 ) {
// 		pRes->Read( &bData, 1 );	// reserved for subtype
// 	}
// 	pRes->Read( &m_fFrame, 4 );				// key frame
// 	// 호환성유지를 위에 그냥 더미로 읽음.
// // 	pRes->Read( &bData, 1 );	// m_LayerType = (xSpr::xtLayer)bData; // layer type
// // 	pRes->Read( &bData, 1 );	// m_nLayer = bData;	// layer num
// 	( *pRes ) >> bData;	// reserved
// 	( *pRes ) >> bData;	// reserved
// 	if( ver >= 26 ) {
// 		ID snSerial;
// 		(*pRes) >> snSerial;
// 		m_spLayer = spAction->GetspLayer( snSerial );
// 		XBREAK( m_spLayer == nullptr );
// 	}
// 	pRes->Read( &bData, 1 );	// reserved
// 	m_SubType = (xSpr::xtKeySub)bData;
// 	if( ver >= 17 ) {
// 		int len;
// 		pRes->Read( &len, 4 ); 		// 루아코드 길이(널포함된것임)
// 		if( len > 0 ) {
// 			m_pcLua = new char[ len+1 ];
// 			pRes->Read( m_pcLua, len+1 );		// 루아코드 읽음
// 		}
// 	}
// }

// left, top : 스크린 좌상귀 좌표
// wx = start world x
/**
 @param speed spAction->GetSpeed() 없으면 0.2디폴트
*/
XE::VEC2 XBaseKey::UpdateDrawPos( XSprObj *pSprObj, float speed, float left, float top, float wx, float fScale )
{
	auto spLayer = pSprObj->GetspLayer( m_spAction, m_idLayer );
	if( spLayer->GetbOpen() ) {
		int idx = 0;
		auto subType = GetSubType();
		if( subType ==xSpr::xKTS_POS )			idx ++;		// 포지션채널은 항상열려있기때문에 포지션 키는 조건검사없이 바로 ++
		if( GetSubType() ==xSpr::xKTS_ROT && spLayer->GetBitDrawChannel( CHA_ROT ) ) {
			idx ++;
			idx ++;
		} else
		if( subType ==xSpr::xKTS_SCALE ) {
			idx ++;
			if( spLayer->GetBitDrawChannel( CHA_ROT ) )			
				idx ++;
			if( spLayer->GetBitDrawChannel( CHA_SCALE ) )		
				idx ++;
		} else
		if( subType ==xSpr::xKTS_EFFECT ) {
			idx ++;
			if( spLayer->GetBitDrawChannel( CHA_ROT ) )			
				idx ++;
			if( spLayer->GetBitDrawChannel( CHA_SCALE ) )		
				idx ++;
			if( spLayer->GetBitDrawChannel( CHA_EFFECT ) )		
				idx ++;
		}

		top += KEY_HEIGHT * (float)idx;
		
	}
// 	SPAction spAction = SPROBJ->m_spAction;
// 	if( !spAction ) return XE::VEC2();
// 	float fSpeed = (spAction) ? spAction->GetSpeed() : 0.2f;
	float fGridPerFrame = 1.0f / speed;			// 1프레임당 작은눈금수 = 1.0 / ani speed;
	float fWidthPerFrame = (KEY_WIDTH * fGridPerFrame);		// 1.0프레임당 가로길이
	float right, bott;
//	ID2D1HwndRenderTarget *pRT = GRAPHICS_D3DTOOL->GetRenderTarget();	
	Setscrwx( GetfFrame() * fWidthPerFrame );		// 이 키의 월드좌표
	Setscrwy( spLayer->Getscry() );
	left += Getscrwx() - wx;
	right = left + KEY_WIDTH;
	top += Getscrwy();
	bott = top + KEY_HEIGHT;
	SetvDrawPos( XE::VEC2( left, top ) );
//	Setsx( left );		Setsy( top );			// 스크린좌표에 저장
	return XE::VEC2( left, right );
}
void XBaseKey::DrawKey( D3DCOLOR color, float fScale, bool bSelected )
{
	XE::VEC2 vScr = GetvDrawPos();
	float left = vScr.x;
	float top = vScr.y;
	float right = left + KEY_WIDTH;
	float bott = top + KEY_HEIGHT;
	left++;		top++;		// 한도트 안쪽으로 해서 작게 찍는다
//	right--;	bott--;
	if( IsShadow() )
		XE::DrawRectangle( left, top, right, bott, color );			// 쉐도우키는 외곽선으로만 그린다
	else {
		XE::FillRectangle( left, top, right, bott, color );
		if( bSelected )
			XE::DrawRectangle( left, top, right, bott, XCOLOR_RED );
	}
}

// x, y좌표가 이 키의 영역안에 있는지 검사
BOOL XBaseKey::IsInside( float x, float y, float fScale )
{
	XE::VEC2 vScr = GetvDrawPos();
	if( x >= vScr.x && x <= vScr.x+KEY_WIDTH-2 )
		if( y >= vScr.y && y <= vScr.y+KEY_HEIGHT-2 )
			return TRUE;
	return FALSE;
}
// vStart-vEnd 사각영역안에 이 키가 들어있는지 검사
BOOL XBaseKey::IsInside( const XE::VEC2& vStart, const XE::VEC2& vEnd, float fScale )
{
	float l  = min( vStart.x, vEnd.x );
	float r = max( vStart.x, vEnd.x );
	float t = min( vStart.y, vEnd.y );
	float b = max( vStart.y, vEnd.y );
	XE::VEC2 vScr = GetvDrawPos();
	XE::VEC2 vCenter( vScr.x + (KEY_WIDTH * 0.5f), vScr.y + (KEY_HEIGHT * 0.5f) );	// 키의 중심좌표
	if( vCenter.x >= l && vCenter.x < r )
		if( vCenter.y >= t && vCenter.y < b )
			return TRUE;
	return FALSE;
}
/**
 @brief 이 키가 spLayer에 속한 키인지 찾음.
*/
bool XBaseKey::IsSameLayerOnly( XSPBaseLayer spLayer ) const
{
	return spLayer->GetidLayer() == m_idLayer;
}

// BOOL XBaseKey::IsSameLayer( XSPBaseLayer spLayer ) 
// {
// 	return m_spLayer->IsSameLayer( spLayer );
// //	return ( m_spLayer->GetType() == spLayer->GetType() && GetnLayer() == spLayer->GetnLayer() ); 
// }
// BOOL XBaseKey::IsSameLayer( xSpr::xtLayer type, int nLayer ) { 
// 	return m_spLayer->IsSameType( type ) && m_spLayer->GetnLayer() == nLayer;
// //	return ( GetLayerType() == type && GetnLayer() == nLayer ); 
// }

void XBaseKey::GetToolTipSize( float *w, float *h ) 
{
	*w = SE::g_pFont->GetFontWidth() * 6;
	*h = SE::g_pFont->GetFontHeight() * 2;
	if( GetpcLua() ) {
		*w += SE::g_pFont->GetFontWidth() * 9;
		*h += SE::g_pFont->GetFontHeight() * 9;
	}
};

CString XBaseKey::GetToolTipString( XSPBaseLayer spLayer ) 
{
	CString str;
#ifdef _DEBUG
	str.Format( _T( "snKey: 0x%08x\n" ), GetidKey() );
	str += XE::Format( _T( "idLocalInLayer: %d\n" ), GetidLocalInLayer() );
	str += XE::Format( _T( "layer%d(id=%d)\n" ), spLayer->GetnLayer(), spLayer->GetidLayer() );
#endif
	return str;
}

CString XBaseKey::GetToolTipStringTail() 
{
	CString str;
	if( GetpcLua() ) {
		CString strLua( Convert_char_To_TCHAR( GetpcLua() ) );
		if( strLua.GetLength() > 128 )
		{
			strLua.Left( 128 );		// 코드가 너무 길면 128자에서 자름
			strLua += _T( "..." );
		}
		str += _T( "\n\n<lua code>\n" ) + strLua + _T( "\n" );
	}
	return str;
}
