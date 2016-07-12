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

ID XEWorld::AddObj( const WorldObjPtr& spObj )
{
	XBREAK( m_pObjMng == NULL );
	m_pObjMng->Add( spObj );
	return spObj->GetsnObj();
}

ID XEWorld::AddObj( int type, const WorldObjPtr& spObj )
{
	XBREAK( m_pObjMng == NULL );
	m_pObjMng->Add( type, spObj );
	return spObj->GetsnObj();
}

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
 @brief 지정된 좌표를 중심으로 반경내에 있는 오브젝트중 가장 가까운 오브젝트를 찾는다.
 @param vwPos 중심좌표
 @param radiusSight 시야거리. 만약 시야거리내에 오브젝트가 없다면 시야를 적용하지 않고 다시 검사한다.
 @param bitSide 어느편 오브젝트를 검색할지 필터. 비트형태로 써야한다.
 @return 찾은 오브젝트의 포인터를 리턴한다.
 @todo 함수포인터 형태로 모두 바꾸기 위해 삭제함.
*/
/*
WorldObjPtr XEWorld::FindNearObjByFilter( const XE::VEC3& vwPos, 
										float radiusSight, 
										BIT bitSide )
{
	if( XBREAK( m_pObjMng == NULL ) )
		return NULL;
	return m_pObjMng->FindNearObjByFilter( vwPos, radiusSight, bitSide );
}

*/
// WorldObjPtr XEWorld::FindNearObjByFunc( XEBaseWorldObj *pSrcObj, 
// 										const XE::VEC3& vwPos,
// 										float radiusSight,
// 										BOOL ( *pFunc )( XEBaseWorldObj*, XEBaseWorldObj* ) )
// {
// 	if( XBREAK( m_pObjMng == NULL ) )
// 		return WorldObjPtr();
// 	return m_pObjMng->FindNearObjByFunc( pSrcObj, vwPos, radiusSight, pFunc );
// }
// 
// WorldObjPtr XEWorld::FindNearObjByMore( XEBaseWorldObj *pSrcObj,
// 										const XE::VEC3& vwPos,
// 										float radiusSight,
// 										BOOL( *pfuncFilter )( XEBaseWorldObj*, XEBaseWorldObj* ),
// 										BOOL( *pfuncCompare )( XEBaseWorldObj*, XEBaseWorldObj*, XEBaseWorldObj* ) )
// {
// 	if( XBREAK( m_pObjMng == NULL ) )
// 		return WorldObjPtr();
// 	return m_pObjMng->FindNearObjByMore( pSrcObj, vwPos, radiusSight, pfuncFilter, pfuncCompare );
// }


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
