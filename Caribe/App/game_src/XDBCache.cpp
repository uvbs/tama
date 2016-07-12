#include "stdafx.h"
#ifdef _XDB_CACHE
#include "XDBCache.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XDBCache::XDBCache( int max )
{
	Init();
	m_listAcc.Create( max );
}

// 캐시에 계정데이타를 올린다.
XCCAccount* XDBCache::Add( ID idAccount, XPacket& p )
{
	// 캐시용 새 객체 생성
	XCCAccount *pAcc = new XCCAccount;
	// 계정 데이타 채워놓음.
	pAcc->RestorePacket( p );
	// 패킷 내용 카피
	pAcc->SetPacket( p );
	//
	m_listAcc.Add( pAcc );
	// 
	m_mapIDAcc[ idAccount ] = pAcc;
	//
	XBREAK( pAcc->m_strUUID.empty() == true );
	m_mapUUID[ pAcc->m_strUUID ] = pAcc;
	//	
	XBREAK( pAcc->m_strID.empty() == true ));
	m_mapID[ pAcc->m_strID ] = pAcc;

}

XCCAccount* XDBCache::FindFromUUID( const char *cUUID )
{
	map<string, XCCAccount*>::iterator itor;
	itor = m_mapUUID.find( string( cUUID ) );
	if( itor == m_mapUUID.end() )
		return NULL;
	XCCAccount *pAcc = (*itor).second;
	XBREAK( pAcc == NULL );
	return pAcc;
}

XCCAccount* XDBCache::FindFromAccountID( ID idAccount )
{
	map<ID, XCCAccount*>::iterator itor;
	itor = m_mapIDAcc.find( idAccount );
	if( itor == m_mapIDAcc.end() )
		return NULL;
	XCCAccount *pAcc = (*itor).second;
	XBREAK( pAcc == NULL );
	return pAcc;
}

XCCAccount* XDBCache::FindFromPassword( const char *cID, const char *cPassword )
{
	map<string, XCCAccount*>::iterator itor;
	itor = m_mapID.find( string( cID ) );
	if( itor == m_mapID.end() )
		return NULL;
	XCCAccount *pAcc = (*itor).second;
	XBREAK( pAcc == NULL );
	return pAcc;
}
#endif // _XDB_CACHE
