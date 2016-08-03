﻿#include "stdafx.h"
#include "XEObjMng.h"
#include "XEWorld.h"
#include "XEWndWorld.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// struct compY {
// 	bool operator()( const XList3<XSPWorldObj>::xNode &lhs,
// 		const XList3<XSPWorldObj>::xNode &rhs ){
// 		float y1 = lhs.value->GetvwPos().y;
// 		float y2 = rhs.value->GetvwPos().y;
// 		if( y1 == y2 )
// 		{
// 			DWORD snObj1 = lhs.value->GetsnObj();
// 			DWORD snObj2 = rhs.value->GetsnObj();
// 			return snObj1 < snObj2;
// 		} else
// 			return y1 < y2;
// 	}
// };

// static int compY( const void *p1, const void *p2 )
// {
// 	XEBaseWorldObj **ppObj1 = (XEBaseWorldObj **)p1; 
// 	XEBaseWorldObj **ppObj2 = (XEBaseWorldObj **)p2;
// 	int y1 = (*ppObj1)? (int)(*ppObj1)->GetvwPos().y : 9999;
// 	int y2 = (*ppObj2)? (int)(*ppObj2)->GetvwPos().y : 9999;
// 	if( y1 == y2 )
// 	{
// 		// y값이 같다면 snObj값으로 소트한다
// 		DWORD snObj1 = (*ppObj1)? (int)(*ppObj1)->GetsnObj() : 0x7fffffff;
// 		DWORD snObj2 = (*ppObj2)? (int)(*ppObj2)->GetsnObj() : 0x7fffffff;
// 		return snObj1 - snObj2;
// 	} else
// 		return y1 - y2;
// 	// 그래도 소트껌뻑이는게 거슬린다면 x좌표소트, snObj소트도 할것.
// }

static bool compY( const XEBaseWorldObj *pObj1, const XEBaseWorldObj *pObj2 )
{
// 	XEBaseWorldObj **ppObj1 = (XEBaseWorldObj **)p1;
// 	XEBaseWorldObj **ppObj2 = (XEBaseWorldObj **)p2;
	int y1 = ( pObj1 ) ? (int)( pObj1 )->GetvwPos().y : 9999;
	int y2 = ( pObj2 ) ? (int)( pObj2 )->GetvwPos().y : 9999;
	if( y1 == y2 ) {
		// y값이 같다면 snObj값으로 소트한다
		DWORD snObj1 = ( pObj1 ) ? (int)( pObj1 )->GetsnObj() : 0x7fffffff;
		DWORD snObj2 = ( pObj2 ) ? (int)( pObj2 )->GetsnObj() : 0x7fffffff;
		return snObj1 < snObj2;
	} else
		return y1 < y2;
	// 그래도 소트껌뻑이는게 거슬린다면 x좌표소트, snObj소트도 할것.
}

//////////////////////////////////////////////////////////////////////////
XEObjMng::XEObjMng( int maxObj ) 
{ 
	Init(); 
//	m_aryVisible.Create( maxObj );
}

void XEObjMng::Destroy()
{
	// 모든 객체의 레퍼런스 카운트가 1인상태여야지 실제 이곳에서 객체가 파괴된다.
	for( auto spObj : m_listObj ) {
		const auto cnt = spObj.use_count();
		XBREAK( cnt != 1 );
	}
}

void XEObjMng::Release()
{
	for( auto spObj : m_listObj ) {
		spObj->Release();
	}
	m_aryVisible.clear();
//	m_listObj.clear();
}

void XEObjMng::DestroyAllObj( void )
{
	// 최종 반환되며 실제 삭제된다.
	for( auto spObj : m_listObj ) {
		spObj->SetDestroy( 1 );
	}
// 	for( auto spObj : m_listObj ) {
// 		const auto cnt = spObj.use_count();
// 		XBREAK( cnt != 1 );
// 	}
}

ID XEObjMng::Add( const XSPWorldObj& spObj )
{
	m_listObj.push_back( spObj );
	m_mapObj[ spObj->GetsnObj() ] = spObj;
	m_bAdded = TRUE;
	return spObj->GetsnObj();
}

void XEObjMng::Add( ID snObj, const XSPWorldObj& spObj )
{
	spObj->SetsnObj( snObj );
	m_listObj.push_back( spObj );
	m_mapObj[ snObj ] = spObj;
	m_bAdded = TRUE;
}

XSPWorldObj XEObjMng::Find( ID snObj )
{
	if( snObj == 0 )
		return nullptr;
	auto itor = m_mapObj.find( snObj );
	if( itor != m_mapObj.end() ) {
		return itor->second;
	}
// 	for( auto spObj : m_listObj ) {
// 		if( spObj->GetsnObj() == snObj )
// 			return spObj;
// 	}
	return nullptr;
}

void XEObjMng::DestroyObjWithSN( ID snObj )
{
	auto spObj = Find( snObj );
	if( spObj ) {
		spObj->SetDestroy( 1 );
	}
}


void XEObjMng::FrameMove( XEWndWorld *pWndWorld, float dt )
{
	XPROF_OBJ_AUTO();
// 	std::list<XSPWorldObj>::iterator itor;
	for( auto itor = m_listObj.begin(); itor != m_listObj.end(); )
	{
		XEBaseWorldObj *pObj = itor->get();
		pObj->FrameMove( dt );
		//
		XE::VEC2 vlPos = pWndWorld->GetPosWorldToWindow( pObj->GetvwPos() );
		// 화면에서 벗어나서 삭제됨을 체크해줄것을 요청한다.
		if( pObj->DelegateOutOfBoundary( vlPos ) )
			pObj->SetDestroy( 1 );
		if( pObj->GetDestroy() )
		{
			pObj->Release();
			// 하위상속 객체에 파괴됨을 알린다.
			OnDestroyObj( pObj );		// virtual
			if( pWndWorld->GetpWorld() )
				pWndWorld->GetpWorld()->OnDestroyObj( pObj );
			XSPWorldObj *p = &( *itor );
//			m_aryDestroy.Add( (*itor) );
			m_listObj.erase( itor++ );
		}
		else
			++itor;
	}
	// 삭제될 오브젝트를 한꺼번에 반환시킨다.
// 	XARRAYLINEARN_LOOP( m_aryDestroy, XSPWorldObj*, pspObj )
// 	{
// 		// 소유권 반환
// 		(*pspObj).reset();
// 	} END_LOOP;

}

// 객체들의 루프를 돌며 각 객체별로 델리게이트를 발생시킨다.
void XEObjMng::FrameMoveDelegate( XDelegateObjMng *pDelegate, 
								ID idEvent, 
								float dt )
{
// 	std::list<XSPWorldObj>::iterator itor;
	// 최종 반환되며 실제 삭제된다.
	for( auto itor = m_listObj.begin(); itor != m_listObj.end(); ++itor )
		pDelegate->OnDelegateFrameMoveEachObj( dt, idEvent, (*itor) );
// 	LISTREF_LOOP( m_listObj, XSPWorldObj, itor )
// 	{
// 		pDelegate->OnDelegateFrameMoveEachObj( dt, idEvent, (*itor) );
// 	} END_LOOP;
}

void XEObjMng::Draw( XEWndWorld *pWndWorld )
{
	XPROF_OBJ_AUTO();
	if( pWndWorld )
	{
		XPROF_OBJ("select");
		// 화면에 보이는 만큼 추려냄
		m_aryVisible.Clear();
		std::list<XSPWorldObj>::iterator itor;
		{
		// 최종 반환되며 실제 삭제된다.
		for( itor = m_listObj.begin(); itor != m_listObj.end(); ++itor )
//		LISTREF_LOOP( m_listObj, XSPWorldObj, itor )
		{
			XPROF_OBJ( "select-sel" );
			XEBaseWorldObj *pObj = itor->get();
			XE::xRECT rectBB = pObj->GetBoundBoxWindow();
			if( pWndWorld->IsOutBoundary( rectBB ) == FALSE )
				m_aryVisible.Add( pObj );

		}// END_LOOP;
		}
		// y좌표 소트함
// 		if( m_timerSort.IsOff() )
// 			m_timerSort.Set( 1.f );
// 		if( m_timerSort.IsOver() || m_timerSort.IsOff() || m_bAdded )
// 		{
// 			m_aryVisible.sort( compY );
// 			m_timerSort.Reset();
// 		} 
		{
			XPROF_OBJ( "select-sort" );
			std::sort( m_aryVisible.begin(), m_aryVisible.end(), compY );
//			m_aryVisible.sort( compY );
		}
	}
	
	// 소트한 오브젝트들 찍음.
	DrawVisible( pWndWorld, m_aryVisible );
	{
	}
	m_bAdded = FALSE;
}

void XEObjMng::DrawVisible( XEWndWorld *pWndWorld, const XVector<XEBaseWorldObj*>& aryVisible )
//void XEObjMng::DrawVisible( XEWndWorld *pWndWorld, const XList4<XEBaseWorldObj*>& aryVisible )
{
	XPROF_OBJ_AUTO();
	for( auto pObj : aryVisible ) {
		if( pWndWorld ) {
			// 각 오브젝트들의 월드좌표를 스크린좌표로 변환하여 draw를 시킴
			float scale = 1.f;
			// 투영함수에서 카메라 스케일값을 받아온다.
			XE::VEC2 vsPos;
			{
				XPROF_OBJ( "projection" );
				vsPos = pWndWorld->GetPosWorldToScreen( pObj->GetvwPos(), &scale );
			}
			{
				XPROF_OBJ( "draw each" );
				pObj->Draw( vsPos, scale );
			}
		} else {
			XE::VEC2 vs = pObj->GetvwPos().ToVec2();
			pObj->Draw( vs );
		}
	};
}

// void XEObjMng::DrawVisible( XEWndWorld *pWndWorld, XArrayLinear<XEBaseWorldObj*>& aryVisible )
// {
// 	XPROF_OBJ_AUTO();
// 	XARRAYLINEAR_LOOP( aryVisible, XEBaseWorldObj*, pObj )
// 	{
// 		if( pWndWorld )
// 		{
// 			// 각 오브젝트들의 월드좌표를 스크린좌표로 변환하여 draw를 시킴
// 			float scale = 1.f;
// 			// 투영함수에서 카메라 스케일값을 받아온다.
// 			XE::VEC2 vsPos;
// 			{
// 				XPROF_OBJ( "projection" );
// 				vsPos = pWndWorld->GetPosWorldToScreen( pObj->GetvwPos(), &scale );
// 			}
// 				{
// 					XPROF_OBJ( "draw each" );
// 					pObj->Draw( vsPos, scale );
// 				}
// 		}
// 		else
// 		{
// 			XE::VEC2 vs = pObj->GetvwPos().ToVec2();
// 			pObj->Draw( vs );
// 		}
// 	} END_LOOP;
// }

void XEObjMng::OnLButtonUp( float lx, float ly )
{
#ifdef _XOBJMNG_OPTIMIZE
	XList3<XSPWorldObj>::iterator itor;
#else
	std::list<XSPWorldObj>::iterator itor;
#endif
	// 최종 반환되며 실제 삭제된다.
	for( itor = m_listObj.begin(); itor != m_listObj.end(); ++itor )
//	LISTREF_LOOP( m_listObj, XSPWorldObj, itor )
	{
		XEBaseWorldObj *pObj = itor->get();
		if( pObj->GetTouchable() && pObj->GetDestroy() == 0 )
			pObj->OnLButtonUp( lx, ly );
	}// END_LOOP;
}

/**
 @brief 필터에 맞는 오브젝트 중에 가장 가까운 오브젝트를 찾는다.
 만약 범위내에서 못찾으면 전체 검색으로 다시한번 찾는다.
 @param vwPos 검색할 중심좌표
 @param radius 중심좌표로부터 반경
 @param bitSide 피아식별 필터
*/
/*
XSPWorldObj XEObjMng::FindNearObjByFilter( const XE::VEC3& vwPos,
												float radius,
												BIT bitSide )
{
	float minDist = 9999999;
	XSPWorldObj spMinObj(nullptr);
	float minDistAll = 9999999;
	XSPWorldObj spMinObjAll(nullptr);
	std::list<XSPWorldObj>::iterator itor;
	for( itor = m_listObj.begin(); itor != m_listObj.end(); ++itor )
	{
		if( bitSide & (*itor)->GetbitSide() )
		{
			XE::VEC3 vDist = (*itor)->GetvwPos() - vwPos;
			float distsq = vDist.Lengthsq();
			// 주어진 거리내에 있는가
			if( distsq <= radius * radius )
			{
				minDist = distsq;
				spMinObj = (*itor);
			} else
			{
				// 못찾을경우를 대비해서 범위바깥의 오브젝트중에 가장 가까운것도 찾는다.
				minDistAll = distsq;
				spMinObjAll = (*itor);
			}
		}
	}
	// 만약 범위내에서 못찾았으면 범위바깥에서 찾은거라도 돌려준다.
	if( spMinObj == nullptr )
		return spMinObjAll;
	return spMinObj;

}
*/

XSPWorldObj XEObjMng::FindNearObjByFunc( XEBaseWorldObj *pSrcObj, 
										const XE::VEC3& vwPos,
										float radius,
										BOOL( *pfuncFilter )( XEBaseWorldObj*, XEBaseWorldObj* ) )
{
	float minDist = 9999999;
	XSPWorldObj spMinObj(nullptr);
	float minDistAll = 9999999;
	XSPWorldObj spMinObjAll;
#ifdef _XOBJMNG_OPTIMIZE
	XList3<XSPWorldObj>::iterator itor;
#else
	std::list<XSPWorldObj>::iterator itor;
#endif
	// 최종 반환되며 실제 삭제된다.
	for( itor = m_listObj.begin(); itor != m_listObj.end(); ++itor )
	{
		if( pfuncFilter( pSrcObj, (*itor).get() ) )
		{
			XE::VEC3 vDist = ( *itor )->GetvwPos() - vwPos;
			float distsq = vDist.Lengthsq();
			// 주어진 거리내에 있는가
			if( distsq <= radius * radius )
			{
				minDist = distsq;
				spMinObj = ( *itor );
			}
			else
			{
				// 못찾을경우를 대비해서 범위바깥의 오브젝트중에 가장 가까운것도 찾는다.
				minDistAll = distsq;
				spMinObjAll = ( *itor );
			}
		}
	}
	// 만약 범위내에서 못찾았으면 범위바깥에서 찾은거라도 돌려준다.
	if( spMinObj == nullptr )
		return spMinObjAll;
	return spMinObj;

}

/**
 @brief 조건함수를 이용해 더 ~한 오브젝트를 찾는다.
*/
XSPWorldObj XEObjMng::FindNearObjByMore( XEBaseWorldObj *pSrcObj,
										const XE::VEC3& vwPos,
										float radius,
										BOOL( *pfuncFilter )( XEBaseWorldObj*, XEBaseWorldObj* ),
										BOOL( *pfuncCompare )( XEBaseWorldObj*, XEBaseWorldObj*, XEBaseWorldObj* ) )
{
	XSPWorldObj spCompObj;
	float minDist = 99999999.f;
	XSPWorldObj spCompObjForAll;
#ifdef _XOBJMNG_OPTIMIZE
	XList3<XSPWorldObj>::iterator itor;
#else
	std::list<XSPWorldObj>::iterator itor;
#endif
	// 최종 반환되며 실제 삭제된다.
	for( itor = m_listObj.begin(); itor != m_listObj.end(); ++itor )
	{
		BOOL bCondition = FALSE;
		// 사용자 정의 필터함수를 호출해서 TRUE인것만 대상으로 한다.
		if( ( *itor )->GetsnObj() != pSrcObj->GetsnObj() &&
			pfuncFilter( pSrcObj, itor->get() ) )
		{
			if( radius > 0 )
			{
				// 일단 주어진 거리내에 있어야 한다.
				XE::VEC3 vDist = ( *itor )->GetvwPos() - vwPos;
				float distsq = vDist.Lengthsq();
				if( distsq <= radius * radius )
				{
					bCondition = TRUE;
				}
				else
				{
					// 거리밖에 있는것들중에서 젤 가까운것도 찾아둔다.
					if( distsq < minDist )	// 자기자신은 검색하지 않는다.
					{
						minDist = distsq;
						spCompObjForAll = ( *itor );
					}
				}
			}
			else
				// 범위가 없으면 거리 검사 안함.
				bCondition = TRUE;
			if( bCondition )
			{
				if( spCompObj == nullptr )
					spCompObj = ( *itor );
				else
					// 사용자 정의 비교함수를 호출해서 TRUE인것만 취한다.
					if( pfuncCompare( pSrcObj, itor->get(), spCompObj.get() ) )
						spCompObj = ( *itor );
			}
		}
	}
	// 거리내에 조건을 만족하는게 없다.
	if( spCompObj == nullptr )
	{
		// 그렇다면 거리밖에있는것들중에 가장가까운것을 리턴한다.
		if( spCompObjForAll != nullptr )
			return spCompObjForAll;
		else
			return spCompObj;	// 거리밖에서 조차 없다면 그냥 널리턴
	}
	return spCompObj;

}
