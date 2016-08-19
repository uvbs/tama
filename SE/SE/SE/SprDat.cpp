#include "stdafx.h"
#include "SprDat.h"
#include "XAniAction.h"
#include "MainFrm.h"
#include "FrameView.h"
#include "AnimationView.h"
#include "Tool.h"
#include "ResMng.h"
#include "Sprite.h"
#include "XKeyBase.h"
#include "XKeyImage.h"
#include "XLayerBase.h"
#include "XLayerImage.h"
#include <algorithm>					// find함수를 사용하기 위해

// #define ACTION_LOOP( I )				\
// 								XAniAction_Itor I; \
// 								for( I = m_listAction.begin(); I != m_listAction.end(); I ++ ) \
// 		
// #define ACTION_MANUAL_LOOP( I )				\
// 								XAniAction_Itor I; \
// 								for( I = m_listAction.begin(); I != m_listAction.end(); ) 
#define SPRITE_LOOP( I )				\
								XSprite_Itor I; \
								for( I = m_listSprite.begin(); I != m_listSprite.end(); I ++ ) \

#define SPRITE_LOOP2( I, E )		LIST_LOOP( m_listSprite, XSprite*, I, E )

#define SPRITE_MANUAL_LOOP( I )				\
								XSprite_Itor I; \
								for( I = m_listSprite.begin(); I != m_listSprite.end(); ) 


char XSprDat::s_cGlobalLua[ 0x10000 ] = {0,};			// 테스트용 루아 글루함수를 여기다 구현해서 구현한다

void XSprDat::Destroy()
{
	// 액션리스트 삭제
// 	ACTION_MANUAL_LOOP( itor ) {
// 		SPAction spAct = (*itor);
// 		SAFE_DELETE( pAct );					// 액션알맹이 지움
// 		m_listAction.erase( itor++ );			// 액션노드 지움
// 	}
	SPRITE_MANUAL_LOOP( itor ) {
		XSprite *pSpr = (*itor);
		SAFE_DELETE( pSpr );
		m_listSprite.erase( itor++ );
	}
}

bool CompSpr( XSprite *pSpr1, XSprite *pSpr2 )
{
	return pSpr1->GetfPriority() < pSpr2->GetfPriority();
}
void XSprDat::SortSprite()
{
	m_listSprite.sort( CompSpr );
}

XSprite* XSprDat::GetSpriteIndex( int nSpr )
{
	int idx = 0;
	SPRITE_LOOP( itor ) {
		if( idx++ == nSpr )
			return (*itor);
	}
	return nullptr;
}
void XSprDat::GetNextSprSet( int nSpr )
{
	int idx = 0;
	SPRITE_LOOP( itor ) {
		if( idx++ == nSpr )
			m_itorSprite = itor;
	}
}

// 액션리스트를 검사해서 없는 번호를 리턴한다 최대 99까지다
DWORD XSprDat::GenerateActID() 
{			
	ID idAct = 1;			// 액션번호 0번은 비워두자
	while( idAct < 100 ) {
		// 이미 사용되고 있는지 확인
		auto pExist = m_listAction.FindByID( idAct );
		if( !pExist )
			return idAct;
		++idAct;
	}
	XALERT( "더 이상 액션을 생성할수 없습니다" );
	return 0;
}

// 액션을 하나추가하고 그 액션인덱스를 리턴한다.
void XSprDat::AddAction( XSPAction spAction )
{
	m_listAction.push_back( spAction );
}
// id를 가진 액션을 찾아서 배열의 인덱스를 리턴한다
// int XSprDat::GetidxActionByidAct( ID idAct )
// {
// 	int index = 0;
// 	m_listAction.find
// 	ACTION_LOOP( itor )
// 	{
// 		if( (*itor)->GetActID() == id )
// 			return index;
// 		index ++;
// 	}
// 	return -1;
// }

XSPAction XSprDat::CreateAction( LPCTSTR szActName )
{
	ID idAct = GenerateActID();				// 아직 만들어진적이 없는 액션아이디를 만든다
	XSPAction spAction = XSPAction( new XAniAction( this, idAct, szActName ) );
	AddAction( spAction );
	return spAction;
}
void XSprDat::AddSprite( XSprite *pSpr, BOOL bKeepPriority )
{
	if( bKeepPriority == FALSE ) {
		if( m_listSprite.empty() )
			pSpr->SetfPriority( 1 );
		else {
			XSprite *pLastSpr = (*--m_listSprite.end());
			pSpr->SetfPriority( pLastSpr->GetfPriority() + 1.0f );	// 맨마지막 스프라이트의 인덱스에 +1시켜서 새 인덱스를 만듬
		}
	}
	m_listSprite.push_back( pSpr );
}
// XSprite객체를 하나 생성하고 그것을 리스트에 추가시킨다.
XSprite* XSprDat::AddSprite( float surfacew, float surfaceh,
									float adjustX, float adjustY,
									int nMemWidth, int nMemHeight,
									DWORD *pImg )
{
	// 현재는 안쓰이지만 서피스크기 따로 텍스쳐 크기 따로 이렇게 쓸것이다. 그래서 각각 따로 크기를 받아와야한다. 지금은 그냥 * 0.5로 통일
	XBREAK( (int)(surfacew * 2) != nMemWidth );
	XBREAK( (int)(surfaceh * 2) != nMemHeight );
	XSprite *pSprite = new XSprite( m_bHighReso, surfacew, surfaceh, adjustX, adjustY, pImg );
//	XSprite *pSprite = new XSprite( m_bHighReso, nWidth, nHeight, nAdjustX, nAdjustY, pImg );
	AddSprite( pSprite );
	return pSprite;
}

/**
 @brief 새로운 애니메이션 프레임하나를 스프라이트에 추가하고 image키로도 추가한다.
*/
// XSprite* XSprDat::AddAniFrame( XSprObj *pSprObj, 
// 								float surfacew, float surfaceh, 
// 								float adjustX, float adjustY, 
// 								int memw, int memh, 
// 								DWORD *pImg, 
// 								BOOL bCreateKey )
// {
// 	// fuck 구조가 안좋다 pSprObj를 넘겨줄께 아니라 XSprObj에서 이것을 처리해야할듯하다.
// 	XSprite *pSprite = AddSprite( surfacew, surfaceh, adjustX, adjustY, memw, memh, pImg );	// 지정된 액션번호로 스프라이트를 하나 추가한다. 내부에서 ID2D1Bitmap객체도 생성한다.
// 	if( bCreateKey && pSprObj->GetspActObjCurr() ) {
// 		auto spBaseLayer = pSprObj->GetLayer( xSpr::xLT_IMAGE, 0 );
// 		auto spLayer = std::static_pointer_cast<XLayerImage>( spBaseLayer );
// 		AddKeySprAtLast( pSprObj->GetspActObjCurr(), spLayer, pSprite );
// 	}
// 	return pSprite;
// }

XSPAction XSprDat::GetspAction( ID idAct )
{
	return m_listAction.FindValueByID( idAct );
}
XSPAction XSprDat::GetspActDatByIndex( int index ) 
{
	return m_listAction.GetValueByIndex( index );
}

BOOL XSprDat::Save( LPCTSTR szFilename, const std::vector<xLayerInfoByAction>& aryLayerInfoByAction )
{
	// 키가 없는 레이어는 삭제시킴
// 	for( auto& spAction : m_listAction )
// 		spAction->DoDelEmptyLayer();
	// 스프라이트 인덱스번호를 재 정렬
	ReindexingSprite();
	//
	CString strFullPath = TOOL->GetstrPath()+ szFilename;
	XResFile file;
	XResFile *pRes = &file;
	if( pRes->Open( strFullPath, XBaseRes::xWRITE ) == 0 ) {
		m_Error = XBaseRes::GetLastError();
		return FALSE;
	}
	WORD wData=0;
	DWORD dw1=0;
	wData = XTool::SPR_VER;
	pRes->Write( &wData, 2 );			// version
	dw1 = (DWORD)m_bHighReso;
	pRes->Write( &dw1, 4 );		// 고해상도 플래그
	dw1 = 0;
	pRes->Write( &dw1, 4 );	// reserved
	pRes->Write( &dw1, 4 );
	pRes->Write( &dw1, 4 );
//	pRes->Write( &m_idLayerGlobal, 4 );	// 레이어 글로벌 아이디
	pRes->Write( &dw1, 4 );	// idLayerGlobal은 액션내로 옮겨짐.
	//lua
	if( m_pcLuaAll ) {
		dw1 = strlen( m_pcLuaAll ) + 1;
		pRes->Write( &dw1, 4 );		// 루아코드 크기
		pRes->Write( m_pcLuaAll, dw1 );		// 널포함 저장
	} else {
		dw1 = 0;
		pRes->Write( &dw1, 4 );
	}
	// sprites
	int nNumSpr = GetNumSprite();
	pRes->Write( &nNumSpr, 4 );		// spr 개수
	// sprite list save
	for( auto pSpr : m_listSprite ) {
		pSpr->Save( pRes );
	}
	// action list save
	int nNumActs = GetNumActs();
	pRes->Write( &nNumActs, 4 );
	int idx = 0;
	XASSERT( aryLayerInfoByAction.size() == m_listAction.size() );
	for( auto spAction : m_listAction ) {
		file << spAction->GetidAct();
		auto& aryLayerInfo = aryLayerInfoByAction[ idx ];
		XASSERT( aryLayerInfo.m_idAct == spAction->GetidAct() );
		spAction->Save( pRes, aryLayerInfo.m_aryLayerInfo );
		m_indexFromID[ spAction->GetidAct() ] = idx++;
	}
// 	ACTION_LOOP( itor ) {
// 		SPAction spAction = (*itor);
// 		ID idAct = spAction->GetidAct();
// 		pRes->Write( &id, 4 );
// 		spAction->Save( pRes );
// 		m_indexFromID[ spAction->GetidAct() ] = idx++;
// 	}
	
	pRes->Write( m_indexFromID, MAX_ID * sizeof(int) );			// 툴에서는 이것을 로드할필요는 없다
	return TRUE;
}
/**
 @brief 
*/
BOOL XSprDat::Load( LPCTSTR szFilename
									, std::vector<xLayerInfoByAction> *pOutAryLayerInfo, BOOL bSrcKeep )
{
	XResFile file;
	XResFile *pRes = &file;
	CString strFullPath = TOOL->GetstrPath() + szFilename;
	if( TOOL->GetstrPath().IsEmpty() )
		strFullPath = TOOL->GetstrWorkPath() + _T("\\") + DIR_SPR + szFilename;
	if( pRes->Open( strFullPath, XBaseRes::xREAD ) == 0 ) {
		strFullPath = TOOL->GetstrPath() + _T("ko\\") + szFilename;
		if( pRes->Open( strFullPath, XBaseRes::xREAD ) == 0 ) {
			m_Error = XBaseRes::GetLastError();
			return FALSE;
		}
	}
	_tcscpy_s( m_szFilename, szFilename );
	WORD wData;
	DWORD dw1;
	pRes->Read( &wData, 2 );					// version
	// 파일의 버전이 툴꺼보다 더 크면 못읽음.
	if( wData > XTool::SPR_VER ) {
		XALERT_OKCANCEL( "%s 파일의 버전(%d)이 최신버전인 %d보다 높아서 열수가 없다. 최신버전의 SE를 사용하기 바람.", szFilename, (int)wData, XTool::SPR_VER );
		return FALSE;
	}
	if( wData != XTool::SPR_VER ) {				// 버전이 다르면
//		XALERT( "%s 파일의 버전(%d)이 최신버전인 %d와 다르다", szFilename, (int)wData, XTool::SPR_VER );
		CONSOLE( "%s 파일의 버전(%d)이 최신버전인 %d와 다르다", szFilename, (int)wData, XTool::SPR_VER );
	}

	m_nVersion = wData;
	dw1 = 0;
	if( IsUpperVersioin( 14 ) ) {
		DWORD b[4];
		pRes->Read( &dw1, 4 );	// 고해상도 플래그
		m_bHighReso = (BOOL)dw1;
		pRes->Read( b, 4, 3 );		// reserved
	}
//	pRes->Read( &m_idLayerGlobal, 4 );			// layer global id
	pRes->Read( &dw1, 4 );
// 	if( m_nVersion < 11 )		// 11버전 이전에는 여기에 쓰레기값이 담겨있었다
// 		m_idLayerGlobal = 1;
	// 합쳐진 lua코드
	if( IsUpperVersioin(19) ) {
		int len;
		pRes->Read( &len, 4 );		// lua length 널포함 길이
		XBREAK( len < 0 );
		if( len > 0 ) {		// 루아코드가 있을때만
			m_pcLuaAll = new char[ len ];
			pRes->Read( m_pcLuaAll, len );		// lua code read
		}
	}
	// sprites
	int nNumSpr;
	pRes->Read( &nNumSpr, 4 );		// 스프라이트 개수
	int i;
	// sprite list load
	for( i = 0; i < nNumSpr; i ++ ) {
		XSprite *pSpr = new XSprite( m_bHighReso );
		pSpr->Load( pRes, m_nVersion );
		pSpr->SetnIdx( i );
		AddSprite( pSpr );
	}
	// 각 스프라이트들에게 인덱싱을 한다.
//	ReindexingSprite();
	// action list load
	int nNumAct;
	pRes->Read( &nNumAct, 4 );
	for( i = 0; i < nNumAct; i ++ ) {
		DWORD idAct;
		file >> idAct;
//		pRes->Read( &id, 4 );
		if( idAct == 0 )
			idAct = GenerateActID();
		// XSprDat::CreateAction()과 같다
		XSPAction spAction = XSPAction( new XAniAction( this, idAct ) );
//		std::vector<xLayerInfo> aryLayerInfo;
		xLayerInfoByAction layerInfoByAction;
		layerInfoByAction.m_idAct = idAct;		// 검증용
		spAction->Load( this, pRes, &layerInfoByAction.m_aryLayerInfo );
		pOutAryLayerInfo->push_back( layerInfoByAction );
		AddAction( spAction );
	}
	m_bKeepSrc = bSrcKeep;
	return TRUE;
}
// 스프라이트 리스트의 인덱스를 새로 갱신한다
void XSprDat::ReindexingSprite()
{
	int idx = 0;
	SPRITE_LOOP( itor )
		(*itor)->SetnIdx( idx++ );
}
void XSprDat::DelAction( ID idAct )
{
	m_listAction.DelByID( idAct );
}
// XAniAction* XSprDat::DelAction( ID idAct )
// {
// 	ACTION_MANUAL_LOOP( itor )
// 	{
// 		SPAction spAction = ( *itor );
// 		if( spAction->GetidAct() == id )
// 		{
// 			//			SAFE_DELETE( spAction );			// 액션 알맹이 삭제
// 			m_listAction.erase( itor++ );
// 			return spAction;			// 언두를 위해서 실제삭제하진 않고 대신 그 포인터를 돌려준다
// 		}
// 		else
// 			itor++;
// 	}
// 	return nullptr;
// }
// pSpr오브젝트를 리스트에서 찾아 삭제한다
XSprite* XSprDat::DelSprite( XSprite *pSpr )
{
	SPRITE_MANUAL_LOOP( itor ) {
		XSprite *p = (*itor);
		if( p == pSpr ) {
//			SAFE_DELETE( p );		// 언두때문에 실제로 삭제하지 않는다
			m_listSprite.erase( itor++ );
			return p;
		} else
			itor++;
	}
	return nullptr;
}
// pSpr을 사용하는 키가 있는지 모든액션을 상대로 검사하여 그 키를 돌려준다
XBaseKey *XSprDat::FindUseSprite( XSprite *pSpr )
{
	for( auto spAction : m_listAction ) {
		auto pKey = spAction->FindUseSprite( pSpr );
		if( pKey )
			return pKey;
	}
// 	ACTION_LOOP( itor ) {
// 		XBaseKey *pKey = (*itor)->FindUseSprite( pSpr );
// 		if( pKey )
// 			return pKey;
// 	}
	return nullptr;
}

// pImg와 완전히 같은 스프라이트가 있는가
XSprite* XSprDat::IsExistSpr( DWORD *pImg, int adjx, int adjy )
{
	SPRITE_LOOP2( itor, pSpr ) {
		if( pSpr->IsSameImage( pImg ) && pSpr->GetAdjustX() == adjx && pSpr->GetAdjustY() == adjy )
			return pSpr;
	} END_LOOP;
	return nullptr;
}
// 주어진 인수와 같은 소스이미지를 가지는 스프라이트가 있는가. pRect가 nullptr이면 rect정보는 비교하지 않음
XSprite* XSprDat::IsExistSprInfo( LPCTSTR szFilename, RECT *pRect )
{
	if( XBREAK( szFilename == nullptr ) )	return nullptr;
	if( XE::IsEmpty( szFilename ) )
		return nullptr;
	SPRITE_LOOP2( itor, pSpr ) {
		// 파일명부터 비교
		if( _tcsicmp( pSpr->GetszSrcFile(), szFilename ) == 0 ) {
			if( pRect ) {		// pRect가 널이 아닐때만 비교
				RECT r = pSpr->GetrectSrc();
				if( r.left == pRect->left && r.right == pRect->right &&	
					r.top == pRect->top && r.bottom == pRect->bottom )			// 모두 같은지 검사
					return pSpr;
			} else
				return pSpr;			// 이경우는 파일명만 같으면 같은걸로 간주
		}
	} END_LOOP;
	return nullptr;
}

XSprite* XSprDat::FindSprite( ID idSpr )
{
	SPRITE_LOOP2( itor, pSpr ) {
		if( pSpr->GetidSpr() == idSpr )
			return pSpr;
	} END_LOOP;
	return nullptr;
}

// strDst에 이 파일내에서 쓰는 모든 루아코드를 합친다
void XSprDat::CompositLuaCodes( char *cDst, int bufflen )
{
	for( auto spAction : m_listAction ) {
		spAction->CompositLuaCodes( cDst, bufflen );
	}
}

// 
void XSprDat::DrawPathLayer( XLayerMove *spLayer )
{
}

BOOL XSprDat::ConvertLowReso()
{
	m_bHighReso = FALSE;
	// 스프라이트들의 이미지를 저해상도로 변환함
	LIST_LOOP( m_listSprite, XSprite*, itor, pSpr ) {
		pSpr->ConvertLowReso();
	} END_LOOP;
	return TRUE;
}
/**
 @brief 레이어의 맨뒤에다 추가한다.
*/
// XBaseKey* XSprDat::AddKeySprAtLast( XSPAction spAction, xSPLayerImage spLayer, XSprite *pSprite )
// {
// 	float fFrame = spLayer->GetfLastFrame();			// nLayer에 마지막으로 추가되었던 프레임이 몇번이었는지 검사해서 거기에 + 1를 한 번호로 키를 추가한다.
// 	XBaseKey *pKey = spAction->AddKeyImage( fFrame, spLayer, pSprite );
// 	spLayer->SetfLastFrame( fFrame + 1.0f );
// 	return pKey;
// }
// 
// int XSprDat::AddKeySprWithFrame( XSPAction spAction, xSPLayerImage spLayer, float fFrame, XSprite *pSprite )
// {
// 	spAction->AddKeyImage( fFrame, spLayer, pSprite );
// 	spLayer->SetfLastFrame( fFrame + 1.0f );
// 	return 1;
// }
// int XSprDat::AddKeyPosWithFrame( XSPAction spAction, XSPLayerMove spLayer, float fFrame, float x, float y )
// {
// 	spAction->AddKeyPos( fFrame, spLayer, x, y );
// //	spLayer->SetfLastFrame( fFrame + 1.0f );
// 	return 1;
// }

/**
 @brief idAct액션의 레이어인포를 listLayerInfo로 바꾼다.
*/
// void XSprDat::SetLayerInfo( ID idAct, const XList4<xLayerInfo>& listLayerInfo )
// {
// 	auto spAction = GetspAction( idAct );
// 	if( XASSERT(idAct) ) {
// 		spAction->SetLayerInfo( listLayerInfo );
// 	}
// }

