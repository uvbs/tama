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


// ��������
void XGRollObj::Projection( XE::VEC2 *pvXY, float *pScale ) 	
{
	pvXY->x = m_vwPos.x / m_vwPos.z;
	pvXY->y = m_vwPos.y / m_vwPos.z;	
	*pScale = 1.0f / m_vwPos.z;
}

void XGRollObj::Transform( const MATRIX& m )
{
	Vec4 v4d;
	MatrixVec4Multiply( v4d, m_vPos, m );		// �־��� ��ȯ��ķ� Ʈ��������
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
	// �ֵ�� ������Ʈ ������ �ڵ����� ���� �������� ��ġ��Ų��
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
		pItem->SetdAngle( ang );		// ����Ʈ�� ������ �Ǿ�����ⶫ�� ���� �ڱ� ���� ������ ������ �־�� �Ѵ�
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
	if( m_DragState == 0 )		// ���� �������� 
	{
		if( m_rAngleAccel == 0 )	// �������� ��ǥ������ ���ư���
		{
			// ��ǥ���������� �ڵ����� ������
			float angDist = m_rAngleEnd - m_rAngleStart;	// ������
			if( angDist != 0 )		// ���ư��� �Ұ��� �ְ�
			{
				if( m_timerUp.IsOn() )	// �Ѹ� Ÿ�̸Ӱ� ���õǸ�
				{
					float lerpTime = (float)m_timerUp.GetPassTime() / m_timerUp.GetWaitTime();	// ������ �ð�������
					if( lerpTime > 1.0f )
						lerpTime = 1.0f;
//					float slerp = XE::xiHigherPowerDeAccel( lerpTime, 1.0f, 0 );		// ���Ӻ���
					float slerp = XE::xiCatmullrom( lerpTime, -10.f, 0, 1, 1 );	//  �ٿ��
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
//							(msgMap.pOwner->*msgMap.pHandler)( this );		// �ѷ��� ���ư��� ���߸� �̺�Ʈ�� �߻��ϸ� ����������� �ε����� �ǳٴ�
					}
				}
			}
		} else
		// �ӵ��� ���� �پ���
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
		// �ӵ��� �������ϸ� ���簢������ ��ǥ������ ���ư���.
		if( m_timerUp.IsOff() )
		{
			if( m_rAngleAccel >= 0 && fabs( m_rAngleAccel ) < D2R(0.5f) )
			{
				// ���簢���� 45�� ������ �ݿø��� �ؼ� ��ǥ������ ã��
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
				dAngle = ((int)((dAngle + (dMadi/2.f)) / dMadi)) * dMadi;		// �����𰢵� ������ �ݿø�
				m_rAngleEnd = D2R( dAngle );
				if( fabs(m_rAngleEnd - m_rAngleStart) > D2R(0.1)  )
				{
					m_timerUp.Set(0.5f);		//���� ������ ����Ÿ�̸� ��
					m_rAngleAccel = 0;		// �ϴ� ���߰�
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
	MatrixRotationX( mR, m_rAngle );				// ȸ����Ʈ���� ����
	MatrixTranslation( mT, m_vCenter.x, m_vCenter.y, m_vCenter.z );	
	// W = SRT
	MatrixMultiply( m, m, mR );
	MatrixMultiply( m, m, mT );
	// Ʈ������
	XLIST_LOOP( m_listObj, XGRollObj*, pObj )
	{
		pObj->Transform( m );
	} END_LOOP;
	// z��Ʈ
	m_listObj.Sort( compZ );
	// draw
	XE::VEC2 vCenter = m_vPos + m_vSize / 2.0f; // XE::VEC2( 48, 95 );		// �׷��� �߽�
	XE::VEC2 vHalfSize = m_Size / 2.0f;		// ��ũ��(?)�� ���� ������
	XLIST_LOOP( m_listObj, XGRollObj*, pObj )
	{
		pObj->Projection( &vPos, &scale );	// ������Ʈ�� ���������ؼ� 2d������ ����.
		float alpha = 1.0f / pObj->GetvwPos().z;
		pObj->Draw( vCenter+vPos*vHalfSize, scale, alpha );				// ���� 2d������ �׸���� ����Ѵ�
	} END_LOOP;
#ifdef _DEBUG
//	PUT_STRINGF( m_vPos.x, m_vPos.y, XCOLOR_WHITE, "%.2f", m_rAngle );
#endif
}
void XGRoll::OnLButtonDown( float lx, float ly )
{
	XE::VEC2 vMouse = m_vPos + XE::VEC2( lx, ly );
	int retv = 0;
	if( XE::IsArea( m_vPos, m_Size, vMouse ) )	// �����ȿ��� �����͸� ��ȿ
	{
		XLIST_LOOP( m_listObj, XGRollObj*, pObj )
		{
			pObj->OnLButtonDown( vMouse );	// vMouse�� Roll�������� ������ǥ�� �Ѱ���� �Ѵ�
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
	XE::VEC2 vDist = vMouse - m_vPrevMouse;		// �巡���� ��
	// 
	if( XE::IsArea( m_vPos, m_Size, vMouse ) )	// �����ȿ��� �����͸� ��ȿ
	{
		// �� ��ü���� �޽����� ������
		XLIST_LOOP( m_listObj, XGRollObj*, pObj )
		{
			pObj->OnMouseMove( vMouse );	// vMouse�� Roll�������� ������ǥ�� �Ѱ���� �Ѵ�
		} END_LOOP;
		if( m_bMoving )		// �����̵��߿��� ó������ �ʴ´�
			return;
//			return this;
		// �巡�� ó��
		if( m_DragState )
		{
			if( m_DragState == 1 )		// ���� ����
			{
				m_DragState = 2;
			}
			float f = vDist.y / 70.0f;
			if( m_bLock )		// ��������� �ȵ��ư���
				f /= 16.0f;;
			if( m_bMoving )		// ���� �̵����̸� �������� �ʰ�
				f = 0;
			m_rAngle -= f;	// �巡�� �Ѹ�ŭ �����ش�
			if( m_rAngle < 0 )
				m_rAngle += (3.141592f * 2.0f);
			m_rAngleAccel = m_rAngle - m_rPrevAngle;	// ������������ �������� ����. �̰��� �ӵ��� �ȴ�
			// �巡�� �߿� ���� ���� �ʵ��� �ؾ� �Ѵ�
			m_rAngleStart = m_rAngleEnd = 0;
			m_timerUp.Off();			
			if( m_bLock )
				m_rAngleAccel = 0;
			// �ƹ��� ���������� �����ӵ� �̻� �ö��� �ʰ�
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
	if( XE::IsArea( m_vPos, m_Size, vMouse ) )	// �����ȿ��� �����͸� ��ȿ
	{
		XLIST_LOOP( m_listObj, XGRollObj*, pObj )
		{
			pObj->OnLButtonUp( vMouse );	// vMouse�� Roll�������� ������ǥ�� �Ѱ���� �Ѵ�
		} END_LOOP;
	}
	int retv = (m_DragState==2)? 1 : 0;	// �̰����� �巡�� �����ִ�. ������ ���⸸ �ϸ� ��������
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
