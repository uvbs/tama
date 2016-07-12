#include "StdAfx.h"
#include "XGRoll.h"
#include "etc/Timer.h"
#include "XLua.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


// 원근투영
void XGRollObj::Projection( XE::VEC2 *pvXY, float *pScale ) 	
{
	pvXY->x = m_vwPos.x / m_vwPos.z;
	pvXY->y = m_vwPos.y / m_vwPos.z;	
	*pScale = 1.0f / m_vwPos.z;
}

void XGRollObj::Transform( const MATRIX& m )
{
	Vec4 v4d;
	MatrixVec4Multiply( v4d, m_vPos, m );		// 주어진 변환행렬로 트랜스폼함
	m_vwPos.x = v4d.x;
	m_vwPos.y = -v4d.y;
	m_vwPos.z = v4d.z;
}

void XGRollSprObj::Draw( float x, float y, float scale, float alpha )
{
	if( XBREAK( m_pSprObj == NULL ) )
		return;
	m_pSprObj->SetfAlpha( alpha );
	m_pSprObj->SetScale( scale );
	m_pSprObj->Draw( x, y );
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void XGRoll::AddObj( XGRollObj *pObj )
{
	m_listObj.Add( pObj );
	// 애드된 오브젝트 개수를 자동으로 세서 원형으로 배치시킨다
	int size = m_listObj.size();
	int i = 0;
	float angMadi = 360.f / (float)size;
	X3D::VEC3 vPos;
	XLIST_LOOP( m_listObj, XGRollObj*, pItem )
	{
		float ang = (float)i * angMadi;
		vPos.x = 0;
		vPos.z = cosf( D2R(ang) ) * -m_sizeRadius;
		vPos.y = sinf( D2R(ang) ) * m_sizeRadius;
		pItem->SetvPos( vPos );
		pItem->SetdAngle( ang );		// 리스트가 소팅이 되어버리기땜에 각자 자기 본래 각도를 가지고 있어야 한다
		++i;
	} END_LOOP;
}

void XGRoll::AddItem( ID idItem, LPCTSTR szSpr, ID idAct )
{
	AddObj( new XGRollSprObj( idItem, szSpr, idAct ) );
}

int XGRoll::Process( float dt )
{
	XLIST_LOOP( m_listObj, XGRollObj*, pObj )
	{
		pObj->FrameMove( dt );
	} END_LOOP;
	if( m_DragState == 0 )		// 손을 뗐을때만 
	{
		if( m_rAngleAccel == 0 )	// 멈춰있음 목표각도로 돌아간다
		{
			// 목표각도쪽으로 자동으로 돌려줌
			float angDist = m_rAngleEnd - m_rAngleStart;	// 각도차
			if( angDist != 0 )		// 돌아가야 할것이 있고
			{
				if( m_timerUp.IsOn() )	// 롤링 타이머가 세팅되면
				{
					float lerpTime = (float)m_timerUp.GetPassTime() / m_timerUp.GetWaitTime();	// 선형적 시간러프값
					if( lerpTime > 1.0f )
						lerpTime = 1.0f;
//					float slerp = XE::xiHigherPowerDeAccel( lerpTime, 1.0f, 0 );		// 감속보간
					float slerp = XE::xiCatmullrom( lerpTime, -10.f, 0, 1, 1 );	//  바운딩
					m_rAngle = m_rAngleStart + angDist * slerp;
					if( lerpTime >= 1.0f )
					{
						m_bMoving = FALSE;
						m_rAngleStart = m_rAngleEnd = 0;
						m_rAngleAccel = 0;
						m_timerUp.Off();
#pragma message("---------------newciv")
						CallEventHandler( XWM_STOP_ROLLING, 0 );
//						XWND_MESSAGE_MAP msgMap = FindMsgMap( XWM_STOP_ROLLING );
//						if( msgMap.pOwner && msgMap.pHandler )
//							(msgMap.pOwner->*msgMap.pHandler)( this );		// 롤러가 돌아가다 멈추면 이벤트가 발생하며 멈춘아이템의 인덱스를 건넨다
					}
				}
			}
		} else
		// 속도가 점점 줄어든다
		if( m_rAngleAccel > 0 )
		{
			m_rAngle += m_rAngleAccel;
			m_rAngleAccel -= D2R(0.15f);		
			if( m_rAngleAccel < 0 )
				m_rAngleAccel = 0;
		}
		else
		if( m_rAngleAccel < 0 )
		{
			m_rAngle += m_rAngleAccel;
			if( m_rAngle < 0 )
				m_rAngle += (3.141592f * 2.0f);
			m_rAngleAccel += D2R(0.15f);
			if( m_rAngleAccel > 0 )
				m_rAngleAccel = 0;
		}
		// 속도가 일정이하면 현재각도에서 목표각도로 돌아간다.
		if( m_timerUp.IsOff() )
		{
			if( m_rAngleAccel >= 0 && fabs( m_rAngleAccel ) < D2R(0.5f) )
			{
				// 현재각도를 45도 단위로 반올림을 해서 목표각도를 찾음
				float dAngle = R2D( m_rAngle );	
				if( dAngle < 0 )
				{
					dAngle = fmod( dAngle, -360.f );
					dAngle = dAngle + 360.f;
				} else
					dAngle = fmod( dAngle, 360.f );
				m_rAngle = D2R( dAngle );
				m_rAngleStart = m_rAngle;
				float dMadi = 360.0f / m_listObj.size();
				dAngle = ((int)((dAngle + (dMadi/2.f)) / dMadi)) * dMadi;		// 각마디각도 단위로 반올림
				m_rAngleEnd = D2R( dAngle );
				if( fabs(m_rAngleEnd - m_rAngleStart) > D2R(0.1)  )
				{
					m_timerUp.Set(0.5f);		//손을 뗀순간 보간타이머 셋
					m_rAngleAccel = 0;		// 일단 멈추고
				}
			}
		}
	} // m_DragState==0
	return 1;
}

bool compZ( XGRollObj *pObj1, XGRollObj *pObj2 )
{
	return (pObj1->GetvwPos().z > pObj2->GetvwPos().z )? true : false;
}
void XGRoll::Draw( void )
{
	XE::VEC2 vPos;
	float scale;
	MATRIX m, mT, mR;
	MatrixScaling( m, m_vScale.x, m_vScale.y, m_vScale.z );
	MatrixRotationX( mR, m_rAngle );				// 회전매트릭스 만듬
	MatrixTranslation( mT, m_vCenter.x, m_vCenter.y, m_vCenter.z );	
	// W = SRT
	MatrixMultiply( m, m, mR );
	MatrixMultiply( m, m, mT );
	// 트랜스폼
	XLIST_LOOP( m_listObj, XGRollObj*, pObj )
	{
		pObj->Transform( m );
	} END_LOOP;
	// z소트
	m_listObj.Sort( compZ );
	// draw
	XE::VEC2 vCenter = m_vPos + m_vSize / 2.0f; // XE::VEC2( 48, 95 );		// 그려질 중심
	XE::VEC2 vHalfSize = m_Size / 2.0f;		// 스크린(?)의 절반 사이즈
	XLIST_LOOP( m_listObj, XGRollObj*, pObj )
	{
		pObj->Projection( &vPos, &scale );	// 오브젝트를 원근투영해서 2d정보를 얻어낸다.
		float alpha = 1.0f / pObj->GetvwPos().z;
		pObj->Draw( vCenter+vPos*vHalfSize, scale, alpha );				// 계산된 2d정보로 그리라고 명령한다
	} END_LOOP;
#ifdef _DEBUG
//	PUT_STRINGF( m_vPos.x, m_vPos.y, XCOLOR_WHITE, "%.2f", m_rAngle );
#endif
}
void XGRoll::OnLButtonDown( float lx, float ly )
{
	XE::VEC2 vMouse = m_vPos + XE::VEC2( lx, ly );
	int retv = 0;
	if( XE::IsArea( m_vPos, m_Size, vMouse ) )	// 영역안에서 찍은것만 유효
	{
		XLIST_LOOP( m_listObj, XGRollObj*, pObj )
		{
			pObj->OnLButtonDown( vMouse );	// vMouse를 Roll내에서의 로컬좌표로 넘겨줘야 한다
		} END_LOOP;
		//
		m_DragState = 1;
		retv = 1;
	}
	m_vPrevMouse = vMouse;
	m_rPrevAngle = m_rAngle;
//	return (retv)? this : NULL;
}
void XGRoll::OnMouseMove( float lx, float ly )
{
	int retv = 0;
	XE::VEC2 vMouse = m_vPos + XE::VEC2( lx, ly );
	XE::VEC2 vDist = vMouse - m_vPrevMouse;		// 드래그한 양
	// 
	if( XE::IsArea( m_vPos, m_Size, vMouse ) )	// 영역안에서 찍은것만 유효
	{
		// 각 객체에도 메시지를 보낸다
		XLIST_LOOP( m_listObj, XGRollObj*, pObj )
		{
			pObj->OnMouseMove( vMouse );	// vMouse를 Roll내에서의 로컬좌표로 넘겨줘야 한다
		} END_LOOP;
		if( m_bMoving )		// 강제이동중에는 처리하지 않는다
			return;
//			return this;
		// 드래그 처리
		if( m_DragState )
		{
			if( m_DragState == 1 )		// 누른 순간
			{
				m_DragState = 2;
			}
			float f = vDist.y / 70.0f;
			if( m_bLock )		// 잠겨있으면 안돌아가게
				f /= 16.0f;;
			if( m_bMoving )		// 강제 이동중이면 움직이지 않게
				f = 0;
			m_rAngle -= f;	// 드래그 한만큼 돌려준다
			if( m_rAngle < 0 )
				m_rAngle += (3.141592f * 2.0f);
			m_rAngleAccel = m_rAngle - m_rPrevAngle;	// 이전프레임의 각도와의 차이. 이값이 속도가 된다
			// 드래그 중엔 보간 하지 않도록 해야 한다
			m_rAngleStart = m_rAngleEnd = 0;
			m_timerUp.Off();			
			if( m_bLock )
				m_rAngleAccel = 0;
			// 아무리 빨리돌려도 일정속도 이상 올라가지 않게
			if( m_rAngleAccel > 0 && m_rAngleAccel > D2R(10.0f) )	
				m_rAngleAccel = D2R(7.5f);
			else
			if( m_rAngleAccel < 0 && m_rAngleAccel < D2R(-10.0f) )
				m_rAngleAccel = D2R(-7.5f);

		}
		retv = 1;
	}
	m_vPrevMouse = vMouse;
	m_rPrevAngle = m_rAngle;
//	return (retv)? this : NULL;
}
void XGRoll::OnLButtonUp( float lx, float ly )
{
	XE::VEC2 vMouse = m_vPos + XE::VEC2( lx, ly );
	if( XE::IsArea( m_vPos, m_Size, vMouse ) )	// 영역안에서 찍은것만 유효
	{
		XLIST_LOOP( m_listObj, XGRollObj*, pObj )
		{
			pObj->OnLButtonUp( vMouse );	// vMouse를 Roll내에서의 로컬좌표로 넘겨줘야 한다
		} END_LOOP;
	}
	int retv = (m_DragState==2)? 1 : 0;	// 이곳에서 드래그 한적있다. 눌렀다 떼기만 하면 인정안함
	m_DragState = 0;
	m_rPrevAngle = m_rAngle;
//	return 0;	
}
void XGRoll::OnNCLButtonUp( float lx, float ly )
{
	XWnd::OnNCLButtonUp( lx, ly );
	m_DragState = 0;
	m_rPrevAngle = m_rAngle;
}
//--------------------------------------------------
void XGRoll::RegisterScript( XLua *pLua )
{
	pLua->Register_Class<XGRoll>("CreateRoller");
	pLua->Register_ClassCon<XGRoll,int,int,int,int,float>();		// 
	pLua->RegisterCPPFunc<XGRoll>("AddItem", &XGRoll::AddItem );
//	pLua->RegisterCPPFunc<XGRoll>("FrameMove", &XGRoll::FrameMove );
//	pLua->RegisterCPPFunc<XGRoll>("Draw", &XGRoll::Draw );

}
