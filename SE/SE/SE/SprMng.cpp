#include "stdafx.h"
#include "SprMng.h"
#include "Tool.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// #define SPRDAT_LOOP( I )				\
// 								XSprDat_Itor I; \
// 								for( I = m_listSprDat.begin(); I != m_listSprDat.end(); I ++ ) \
// 		
// #define SPRDAT_MANUAL_LOOP( I )				\
// 								XSprDat_Itor I; \
// 								for( I = m_listSprDat.begin(); I != m_listSprDat.end(); ) 


XSprMng *SPRMNG = NULL;
/*XSprMng* GetSprMng()
{
	static XSprMng s_SprMng;
	return &s_SprMng;
} */

void XSprMng::Destroy()
{
	// 매니저가 최종 삭제될때까지 release되지 않은 객체가 있으면 경고알려줌
	CheckRelease();
	// 실제 삭제
//	SPRDAT_MANUAL_LOOP( itor )
	
	for( auto itor = m_listSprDat.begin(); itor != m_listSprDat.end(); ) {
//		XSprDat *pSprDat = ( *itor );
		auto& spr = (*itor);
		SAFE_DELETE( spr.m_pSprDat );
		m_listSprDat.erase( itor++ );
	}
	
	{
// 		XSprDat *pSprDat = (*itor);
// 		SAFE_DELETE( pSprDat );
// 		m_listSprDat.erase( itor++ );
	} 
}

// 해제되지 않은 객체가 있는지 검사한다
void XSprMng::CheckRelease()
{
//	SPRDAT_LOOP( itor )
	for( auto& spr : m_listSprDat ) {
//		XSprDat *pSprDat = (*itor);
		XALERT( "SprMng: 해제되지 않은 스프라이트 발견. %s", spr.m_pSprDat->GetszFilename() );
	} 
}

void XSprMng::Release( XSprDat *pSprDat )
{
	//SPRDAT_MANUAL_LOOP( itor )
	
	for( auto itor = m_listSprDat.begin(); itor != m_listSprDat.end(); ) {
		xDat& dat = (*itor);
		if( dat.m_pSprDat == pSprDat ) {
			// 이곳에 레퍼런스카운트 비교루틴을 넣어야 한다
			dat.m_pSprDat->DecRefCnt();
			XBREAK( dat.m_pSprDat->GetnRefCnt() < 0 );
			if( dat.m_pSprDat->GetnRefCnt() == 0 )	// 더이상 참조하는곳이 없으면 삭제시킨다 
			{
				SAFE_DELETE( dat.m_pSprDat );
				m_listSprDat.erase( itor++ );
			} else 
				itor++;
			return;
		} else
			itor++;
	}
}

XSprMng::xDat XSprMng::Find( LPCTSTR szFilename, BOOL bSrcKeep )
{
//	SPRDAT_LOOP( itor )
	for( auto& spr: m_listSprDat ) {
//		XSprDat *pSprDat = (*itor);
		auto pSprDat = spr.m_pSprDat;
#ifdef WIN32
		if( _tcsicmp( pSprDat->GetszFilename(), szFilename ) == 0 )
#else
		if( stricmp( pSprDat->GetszFilename(), szFilename ) == 0 )
#endif
		{
			if( pSprDat->GetbKeepSrc() == TRUE )
				return spr;
			if( pSprDat->GetbKeepSrc() == bSrcKeep )		// 만약 이미 로딩한 파일이 있으나 메모리 보존상황이 다르면 새로 생성하도록 한다
				return spr;		// 가급적 이쪽으로 오지 않게끔 로딩을 배치하자
//			else
//				XBREAK(1);
		}
		
	}
	return xDat();
}

// bAddRefCnt: 레퍼런스 카운트를 증가시킬지 말지
XSprDat *XSprMng::Load( LPCTSTR szFilename, std::vector<xLayerInfoByAction> *pOutAryLayerInfo, BOOL bAddRefCnt, BOOL bSrcKeep )
{
	XBREAK( bAddRefCnt == FALSE );		// 이제 이 옵션은 필요없게 됬다. 사전로딩을 위한거였는데 사전로딩도 TRUE로 읽어야 하게 바뀜
#pragma message( "레퍼런스 카운트가 0이되어도 아직 삭제하지 말고 오랫동안 안쓰면 삭제하도록 바꾸자" )
	// 리스트에 szFilename으로 생성된게 있는지 찾는다.
//	XSprDat *pSprDat = Find( szFilename, bSrcKeep );
	xDat spr = Find( szFilename, bSrcKeep );
	//
	auto pSprDat = spr.m_pSprDat;
	if( pSprDat ) {
		if( bAddRefCnt )
			pSprDat->AddRefCnt();
		*pOutAryLayerInfo = spr.m_aryLayerInfoAction;
		// 한번 읽었던거기땜에 버전 최신걸로 바꿔준다.
		pSprDat->SetnVersion( XTool::SPR_VER );
		return pSprDat;
	}

  XLOG("%s...loaded", szFilename );
//	pSprDat = new XSprDat;
	spr.m_pSprDat = new XSprDat;
	if( spr.m_pSprDat->Load( szFilename, pOutAryLayerInfo, bSrcKeep ) ) {
		spr.m_aryLayerInfoAction = *pOutAryLayerInfo;
		Add( spr );
		if( bAddRefCnt )
			spr.m_pSprDat->AddRefCnt();
		return spr.m_pSprDat;
	}
	// failed
	SAFE_DELETE( spr.m_pSprDat );
	return NULL;
}

XSprDat *XSprMng::NewSprDat()
{
	XSprDat *pSprDat = new XSprDat;
	pSprDat->AddRefCnt();
	xDat spr;
	spr.m_pSprDat = pSprDat;
	Add( spr );
	return pSprDat;
}
