#include "stdafx.h"
#include "XKeyEvent.h"
#include "XLayerEvent.h"
#include "DlgEventProperty.h"
#include "SEFont.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////////
XKeyEvent::XKeyEvent( XSPAction spAction
										, float fFrame
										, XSPLayerEvent spLayer
										, xSpr::xtEventKey Event
										, float x, float y )
	: XBaseKey( spAction, xSpr::xKT_EVENT, xSpr::xKTS_MAIN, fFrame, spLayer )
	, m_aryStrParam(MAX_STR_PARAM)
{
	_Init();
	m_Event = Event;
	m_lx = x;		m_ly = y;
	//m_spLayer = spLayer;
}
XKeyEvent::XKeyEvent( XSPAction spAction
										, float fFrame
										, XSPLayerEvent spLayer
										, DWORD id
										, float x, float y )
	: XBaseKey( spAction, xSpr::xKT_EVENT, xSpr::xKTS_MAIN, fFrame, spLayer )
	, m_aryStrParam(MAX_STR_PARAM)
{
	_Init();
	m_Event = xSpr::xEKT_CREATE_OBJ;
	//m_spLayer = spLayer;
	m_lx = x;		m_ly = y;
// 	m_Param[ 0 ].w[ 0 ] = id;		// 생성할 오브젝트 아이디
//	m_Param[ 0 ].dwParam = id;		// 생성할 오브젝트 아이디
	SeteventidObj( id );
	SeteventdAng( 0 );
//	m_Param[ 1 ].fParam = 0;	// 생성시 각도
}

XBaseKey* XKeyEvent::CreateCopy()
{
//	return new XKeyEvent( *this, spActNew, idLayer, idLocalInLayer );
	auto pKeyNew = new XKeyEvent( *this );		// 기본카피
// 	pKeyNew->SetNewInstance( spActNew, idLayer, idLocalInLayer );
	return pKeyNew;
}

// void XKeyEvent::SetspLayer( XSPBaseLayer& spLayer )
// {
// 	XBaseKey::SetspLayer( spLayer );
// 	//m_spLayer = std::static_pointer_cast<XLayerEvent>( spLayer );
// }
// XBaseKey* XKeyEvent::CopyDeep()
// {
// 	return new XKeyEvent( *this );
// 	// 	XKeyEvent *pKey = new XKeyEvent( spAction, GetfFrame(), GetLayerType(), GetnLayer(), GetEvent(), Getlx(), Getly() );
// 	// 	pKey->SetidKey( GetidKey() );
// 	// 	pKey->CopyParam( this );
// 	// 	pKey->SetSprFile( m_strSpr.c_str() );
// 	// 	return pKey;
// }
void XKeyEvent::Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes )
{
	XResFile& res = (*pRes);
	XBaseKey::Save( typeLayer, nLayer, pRes );
	BYTE b1 = (BYTE)m_Event;
	pRes->Write( &b1, 1 );
	pRes->Write( &m_lx, 4 );
	pRes->Write( &m_ly, 4 );
	pRes->Write( m_Param, sizeof( xSpr::KEY_PARAM ), sizeof( m_Param ) / sizeof( xSpr::KEY_PARAM ) );
	int len = m_strSpr.length();
	pRes->Write( &len, 4 );
	if( len > 0 )
		pRes->Write( (void*)m_strSpr.c_str(), sizeof( char ), len );
	res << m_aryStrParam;
}
void XKeyEvent::Load( XResFile *pRes, XSPAction spAction, int ver )
{
	//	XBaseKey::Load( pRes, spAction, ver );
	//m_spLayer = std::static_pointer_cast<XLayerEvent>( XBaseKey::GetspLayer() );
	BYTE b1;
	pRes->Read( &b1, 1 );		m_Event = ( xSpr::xtEventKey )b1;
	pRes->Read( &m_lx, 4 );
	pRes->Read( &m_ly, 4 );
	if( ver < 15 ) {
		m_lx *= 0.5f;
		m_ly *= 0.5f;
	}
	pRes->Read( m_Param, sizeof( xSpr::KEY_PARAM ), sizeof( m_Param ) / sizeof( xSpr::KEY_PARAM ) );
	if( ver >= 23 ) {
		char cBuff[ 256 ] = {0, };
		int len = 0;
		pRes->Read( &len, 4 );
		if( len > 0 ) {
			pRes->Read( cBuff, sizeof( char ), len );		// spr filename
			m_strSpr = cBuff;
		}
	}
	if( ver >= 33 ) {
		(*pRes) >> m_aryStrParam;
	}
}
BOOL XKeyEvent::EditDialog()
{
	CDlgEventProperty dlg;
	dlg.m_EventType = m_Event;
	dlg.m_strSpr = Convert_char_To_TCHAR( m_strSpr.c_str() );
	dlg.m_idObj = GeteventidObj();
	dlg.m_idAct = GetidAct();
	dlg.SetLoopType( (xRPT_TYPE)GettypeLoop(), GetsecLifeTime() );
	dlg.m_bTraceParent = GetbTraceParent();
	dlg.m_Scale = GetScale();
	dlg.m_strParam[0] = C2SZ(m_aryStrParam[0]);
	dlg.m_strParam[1] = C2SZ(m_aryStrParam[1]);
	dlg.m_strParam[2] = C2SZ(m_aryStrParam[2]);
	dlg.m_strParam[3] = C2SZ(m_aryStrParam[3]);
	if( GetpcLua() )
		dlg.m_strLua = GetpcLua();
	if( dlg.DoModal() == IDOK )
	{
		m_Event = dlg.m_EventType;
		SeteventidObj( dlg.m_idObj );
		m_strSpr = SZ2C( (LPCTSTR)dlg.m_strSpr );
		SetidAct( dlg.m_idAct );
		float secLifeTime = 0;
		SettypeLoop( dlg.GetLoopType( &secLifeTime ) );
		SetsecLifeTime( secLifeTime );
		SetScale( dlg.m_Scale );
		m_aryStrParam[0] = SZ2C((LPCTSTR)dlg.m_strParam[0]);
		m_aryStrParam[1] = SZ2C((LPCTSTR)dlg.m_strParam[1]);
		m_aryStrParam[2] = SZ2C((LPCTSTR)dlg.m_strParam[2]);
		m_aryStrParam[3] = SZ2C((LPCTSTR)dlg.m_strParam[3]);
		if( !dlg.m_strLua.IsEmpty() )		// 루아코드가 입력이 됐다면
		{
			TCHAR szBuff[ 0xffff ];
			_tcscpy_s( szBuff, dlg.m_strLua );
			SetpcLua( CreateConvert_TCHAR_To_char( szBuff ) );
		}
		return TRUE;
	}
	return FALSE;
}

CString XKeyEvent::GetToolTipString( XSPBaseLayer spLayer )
{
	CString strEvent;
	switch( m_Event )
	{
	case xSpr::xEKT_CREATE_OBJ:
		strEvent.Format( 
			_T( "이벤트타입:오브젝트 생성\nobj ID:%d\n좌표:%d,%d\n각도:%4.1f" )
			, (int)GeteventidObj()
			, (int)m_lx, (int)m_ly
			, GeteventdAng() );
		break;
	case xSpr::xEKT_HIT:
		strEvent.Format( 
			_T( "이벤트타입:타격\nobj ID:%d\n좌표:%d,%d\n각도:%4.1f" )
			, (int)GeteventidObj()
			, (int)m_lx, (int)m_ly
			, GeteventdAng() );
		break;
	case xSpr::xEKT_CREATE_SFX: {
		const _tstring strSpr = C2SZ( m_strSpr );
		strEvent.Format( _T( "이벤트타입:이펙트 생성\n" ) \
			_T( "좌표:%d,%d\n" ) \
			_T( "스케일:%.2f\n" ) \
			_T( "spr:%s\n" ) \
			_T( "idAct:%d\n" ) \
			_T( "반복방식:%s\n" ) \
			_T( "반복시간:%.1f\n" ) \
			_T( "각도:%.1f\n" ) \
			_T( "부모종속:%s" )
			, (int)m_lx, (int)m_ly
			, GetScale()
			,	strSpr.c_str()
			, GetidAct()
			, SPR::GetStringLoopType( (xRPT_TYPE)GettypeLoop()
															, ( GetsecLifeTime() == -1.f ) ? TRUE : FALSE )
			, GetsecLifeTime()
			, GeteventdAng()
			, ( GetbTraceParent() ) ? _T( "예" ) : _T( "아니오" ) );
	} break;
	case xSpr::xEKT_ETC: {
		const _tstring strSpr = C2SZ( m_strSpr );
		strEvent.Format( _T( "이벤트타입:기타\n" ) \
			_T( "idObj:%d\n") \
			_T( "좌표:%d,%d\n" ) \
			_T( "스케일:%.2f\n" ) \
			_T( "spr:%s\n" ) \
			_T( "idAct:%d\n" ) \
			_T( "반복방식:%s\n" ) \
			_T( "반복시간:%.1f\n" ) \
			_T( "각도:%.1f\n" ) \
			_T( "부모종속:%s" )
			, GeteventidObj()
			, (int)m_lx, (int)m_ly
			, GetScale()
			,	strSpr.c_str()
			, GetidAct()
			, SPR::GetStringLoopType( (xRPT_TYPE)GettypeLoop()
															, ( GetsecLifeTime() == -1.f ) ? TRUE : FALSE )
			, GetsecLifeTime()
			, GeteventdAng()
			, ( GetbTraceParent() ) ? _T( "예" ) : _T( "아니오" ) );
	} break;
	}
#ifdef _DEBUG
	strEvent = XBaseKey::GetToolTipString( spLayer ) + strEvent;
#endif
	strEvent += XBaseKey::GetToolTipStringTail();
	return strEvent;
}

void XKeyEvent::GetToolTipSize( float *w, float *h ) 
{
	XBaseKey::GetToolTipSize( w, h );
	*w += SE::g_pFont->GetFontWidth() * 8;
	*h += SE::g_pFont->GetFontHeight() * 8;
};
