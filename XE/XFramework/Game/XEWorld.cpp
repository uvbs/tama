#include "stdafx.h"
#include "XEWorld.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/**
 vSize: 월드 전체 크기
*/
XEWorld::XEWorld( const XE::VEC2& vwSize ) 
{ 
	Init(); 
	// 
	m_vwSize = vwSize;
	// virtual. 오브젝트 매니저를 만든다.
//	m_pObjMng = CreateObjMng( maxObj );	
	// 필요에따라 오브젝트 매니저를 여러개 만들수 있도록. UIMng같은
}


void XEWorld::Destroy() 
{
	SAFE_DELETE( m_pObjMng );
}

/**
 @brief 월드가 갖고있던 모든 오브젝트들을 삭제시킴
*/
void XEWorld::DestroyAllObj( void )
{
	m_pObjMng->DestroyAllObj();
}

void XEWorld::Release( void )
{
	m_pObjMng->Release();
}

/**
 @brief 사용자정의 ObjMng를 붙여준다.
*/
void XEWorld::SetObjMng( XEObjMng *pObjMng )
{
	m_pObjMng = pObjMng;
}

ID XEWorld::AddObj( const XSPWorldObj& spObj )
{
	XBREAK( m_pObjMng == NULL );
	m_pObjMng->Add( spObj );
	return spObj->GetsnObj();
}

// ID XEWorld::AddObj( int type, const XSPWorldObj& spObj )
// {
// 	XBREAK( m_pObjMng == NULL );
// 	m_pObjMng->Add( type, spObj );
// 	return spObj->GetsnObj();
// }

int XEWorld::Process( XEWndWorld *pWndWorld, float dt )
{
	if( m_pObjMng )
		m_pObjMng->FrameMove( pWndWorld, dt );
	return 1;
}

void XEWorld::Draw( XEWndWorld *pWndWorld )
{
	if( m_pObjMng )
		m_pObjMng->Draw( pWndWorld );
}

/**
 @brief 오브젝트를 넘겨주면 배틀필드내 좌표로 보정해서 다시 돌려준다.
 3d 월드좌표계임을 주의
*/
XE::VEC3 XEWorld::ClippingObj( const XE::VEC3& vwPos, const XE::VEC3& vSize )
{
	XE::VEC3 vClipped = vwPos;
	// x-y평면의 좌상귀와 우하귀를 구한다.
	XE::VEC3 vLT = XE::VEC3(0);
	XE::VEC3 vRB = GetvwSize();
	float wObj = vSize.w;
	vRB.x -= wObj / 2.f;
	vLT.x += wObj / 2.f;
	if( vClipped.x > vRB.x )
		vClipped.x = vRB.x;
	if( vClipped.x < vLT.x )
		vClipped.x = vLT.x;
	if( vClipped.y > vRB.y )
		vClipped.y = vRB.y;
	if( vClipped.y < vLT.y )
		vClipped.y = vLT.y;
	return vClipped;
}

XEObjMng* XEWorld::CreateObjMng( int maxObj ) 
{
	return new XEObjMng( maxObj );
}

XSPWorldObj XEWorld::GetObjByidObj( ID idObj ) 
{
	if( idObj == 0 )
		return nullptr;
	return m_pObjMng->Find( idObj );
}
