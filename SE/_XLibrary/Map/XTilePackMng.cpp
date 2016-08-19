#include "stdafx.h"
#include "XTilePackMng.h"
#include "XFactory.h"

XTilePackMng *TILEPACK_MNG = NULL;

void XTilePackMng::Destroy() 
{
	XLIST_LOOP( m_listTilePack, XTilePack*, pPack )
	{
		SAFE_DELETE( pPack );
	} END_LOOP;
}

void XTilePackMng::Add( XTilePack* pTilePack )
{
	m_listTilePack.Add( pTilePack );
}

XTilePack* XTilePackMng::Find( LPCTSTR szTPK )
{
	// ����Ʈ�� �̹� �ִ��� �˻�.
	XLIST_LOOP( m_listTilePack, XTilePack*, pPack )
	{
#ifdef WIN32
		if( _tcsicmp( pPack->GetszTPK(), szTPK ) == 0 )
#else
		if( stricmp( pPack->GetszTPK(), szTPK ) == 0 )
#endif
		{
			return pPack;
		}
	} END_LOOP;
	return NULL;
}

XTilePack* XTilePackMng::CreateTilePack( LPCTSTR szTPK, BOOL bMakePage, BOOL bSrcKeep )
{
	XTilePack *pPack = new XTilePack;
	if( pPack->Load( szTPK, bMakePage, bSrcKeep ) )
		return pPack;
	SAFE_DELETE( pPack );
	return NULL;
}

XTilePack* XTilePackMng::Load( LPCTSTR szTPK, BOOL bSrcKeep )
{
	TCHAR szFilename[32];
	_tcscpy_s( szFilename, XE::GetFileName( szTPK ) );		// ���ϸ� ����
	XTilePack *pPack = Find( szFilename );		// ���ϸ����� �˻�.
	if( pPack )
	{
		pPack->IncRefCnt();
		return pPack;
	}
	//
	pPack = CreateTilePack( szTPK, TRUE, bSrcKeep );
	if( pPack )
	{
		pPack->IncRefCnt();
		Add( pPack );
		return pPack;
	}
	return NULL;
}

