#include "stdafx.h"
//////////////////////////////////////////////////////////////////////////
/// XNEW_DBUACC
#include "XDBUAccount.h"
//#include "XLCard.h"
//#include "XLItem.h"
//#include "XTutorial.h"
#include "VerPacket.h"
#include "XArchive.h"
#include "XWorld.h"
#include "XHero.h"
#include "XLegion.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

template<> XPool<XDBUAccount>* XMemPool<XDBUAccount>::s_pPool = NULL;

XDBUAccount::XDBUAccount() 
{ 
	Init(); 
	m_BinaryResource	<< 0;						//바이너리 데이터1
	m_BinaryWorld		<< 0;						//바이너리 데이터2
	m_BinaryHero		<< 0;						//바이너리 데이터3
	m_BinaryFLevel		<< 0;
}


XDBUAccount::XDBUAccount( ID idaccount )
{
	Init();
	m_BinaryResource << 0;						//바이너리 데이터1
	m_BinaryWorld << 0;						//바이너리 데이터2
	m_BinaryHero << 0;						//바이너리 데이터3
	m_BinaryFLevel << 0;
	m_idAccount = idaccount;
}

void XDBUAccount::Destroy()
{
	m_BinaryResource.Clear();						//바이너리 데이터1
	m_BinaryWorld.Clear();							//바이너리 데이터2
	m_BinaryHero.Clear();							//바이너리 데이터3
	m_BinaryFLevel.Clear();	
}

BOOL XDBUAccount::LoadDB( void *dbAccount )
{
	//
	return TRUE;
}

BOOL XDBUAccount::SaveDB( void )
{
	return TRUE;
}

BOOL XDBUAccount::MakeResourcePacket ( XArchive &p )
{
	// 0xA = 1100
	DWORD dwVer = VER_RESOURCE_SERIALIZE;
	p << dwVer;
	p << m_aryResource;
/*
	for( int i = 0; i < XGAME::xRES_MAX; ++i )
	{
		if( m_aryResource[i] )
			p << m_aryResource[i];
		else
			p << (DWORD)0;
	}	
*/
	return TRUE;
}
BOOL XDBUAccount::RestoreResourcePacket ( XArchive &p )
{
	XAccount::sDeserializeResource( p, m_aryResource );
//	int ver;
//	p >> ver;
//	p >> m_aryResource;
/*
	DWORD dw1 = 0;	
	p >> dw1;
	if( (dw1 & 0xA0000000) == 0xA0000000 )
	{
		// 기존 버전과의 호환성을 위해. DB초기화하고 나면 지워도 됨.
		int ver;
		ver = dw1 & (~0xA0000000);
		p >> m_aryResource;
	} else
	{
		m_aryResource[0] = (int)dw1;
		for( int i = 1; i < XGAME::xRES_MAX; ++i )
			p >> m_aryResource[i];
	}
*/
	return TRUE;
}

/**
 m_aryLegion군단정보를 군단번호별로 따로따로 아카이빙 한다.
 장군정보는 snHero만 저장하는게 아니라 풀정보를 모두 저장한다.
 아카이브 상태로 바로 보낼것이므로 Restore는 필요음슴.
*/
void XDBUAccount::MakeLegionPacketFull( XArchive &p, int idxLegion )
{
	if( m_aryLegion[idxLegion] == NULL )
	{
		p << 0;		// 읽을땐 ver으로 읽힘
		return;
	}
	m_aryLegion[idxLegion]->SerializeFull( p );
}

/*
	군단정보 로딩 방식
	.full serialize 바이너리를 읽는다.
	.별도의 XLegion객체를 생성해서 풀어놓는다.
	.full버전 XLegion객체 데이타를 link버전 XLegion객체로 컨버트 한다.,
	.컨버트한 군단객체를 계정데이타로 쓴다.
*/
XLegion* XDBUAccount::RestoreLegionPacketFullToLink( XArchive &arFull, int idxLegion )
{
	XLegion *pLegionFull = XLegion::sCreateDeserializeFull( arFull );
	if( pLegionFull == nullptr )
	{
		m_aryLegion[ idxLegion ] = NULL;
		return NULL;
	}

// 	int ver;
// 	arFull >> ver;
// 	if( ver == 0 )
// 	{
// 		m_aryLegion[idxLegion] = NULL;
// 		return NULL;
// 	}
// 	XLegion *pLegionFull = new XLegion;
// 	pLegionFull->DeSerializeFull( arFull, ver );
	XLegion *pLegion = pLegionFull->CreateLegionForLink( GetThis() );
	SAFE_DELETE( pLegionFull );
	m_aryLegion[idxLegion] = XSPLegion(pLegion);
	return pLegion;
}

/**
 군단데이타만 따로 로딩할때 사용
*/
XLegion* XDBUAccount::RestoreLegionPacketFull( XArchive& arFull, int idxLegion )
{
	XLegion *pLegion = XLegion::sCreateDeserializeFull( arFull );
	if( pLegion == nullptr )
	{
		m_aryLegion[ idxLegion ] = NULL;
		return NULL;
	}
// 	int ver;
// 	arFull >> ver;
// 	if( ver == 0 )
// 	{
// 		m_aryLegion[idxLegion] = NULL;
// 		return NULL;
// 	}
// 	XLegion *pLegion = new XLegion;
// 	pLegion->DeSerializeFull( arFull, ver );
	m_aryLegion[idxLegion] = XSPLegion(pLegion);
	return pLegion;
}


void XDBUAccount::MakeArchiveHeros( XArchive& ar )
{
	ar << VER_HERO_SERIALIZE;
	int size = m_listHero.size();
	ar << size;
	for( auto pHero : m_listHero )
	{
		pHero->Serialize( ar );
	}
}

BOOL XDBUAccount::RestoreArchiveHeros( XArchive& ar )
{
	int verHero;
	int size;
	ar >> verHero;
	ar >> size ;
	for( int i = 0; i < size; ++i )
	{
		XHero *pHero = new XHero;
		pHero->DeSerialize( ar, GetThis(), verHero );
		m_listHero.Add( pHero );
	}
	return TRUE;
}

/// XNEW_DBUACC
//////////////////////////////////////////////////////////////////////////
