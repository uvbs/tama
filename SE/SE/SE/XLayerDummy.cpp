#include "stdafx.h"
#include "XLayerDummy.h"
#include "MovableInterface.h"
#include "AnimationView.h"
#include "XAniAction.h"
#include "XKeyBase.h"
#include "XKeyDummy.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////
XLayerDummy::XLayerDummy( const XLayerDummy& rhs )
{
	*this = rhs;
	ClearMICrossDir();	// 이건 카피하지 않음(카피안해도 됨?)
}
XLayerDummy::XLayerDummy()
	: XLayerMove( xSpr::xLT_DUMMY, CString( _T( "Dummy" ) ), TRUE )
{
	Init();
}

bool XLayerDummy::OnCreate()
{
	m_pMICrossDir = new XMILayerCrossDir( GetThis(), 0, 0, 32.0f, 24.0f, 16.0f );
	return true;
}

XSPBaseLayer XLayerDummy::CreateCopy( XSPActObjConst spActObjSrc ) const
{
	auto spNewLayer = XSPLayerDummy( new XLayerDummy( *this ) );
	spNewLayer->OnCreate();
	return spNewLayer;
}

// XSPBaseLayer XLayerDummy::CreateCopyTest( std::shared_ptr<const XLayerDummy> spLayerSrc ) const
// {
// 	auto spNewLayer = spLayerSrc->CreateCopyTest2( spLayerSrc );
// 	return spNewLayer;
// }
// XSPBaseLayer XLayerDummy::CreateCopyTest2( std::shared_ptr<const XLayerDummy> spLayerSrc ) const
// {
// 	auto spNewLayer = XSPLayerDummy( new XLayerDummy( *this ) );
// 	return spNewLayer;
// }

// XSPBaseLayer XLayerDummy::CopyDeep()
// {
// 	auto spLayer = XSPBaseLayer( new XLayerDummy( *this ) );
// 	return spLayer;
// 	// 	XLayerDummy *pNewLayer = new XLayerDummy( FALSE );
// 	// 	*pNewLayer = *( this );
// 	// 	pNewLayer->m_pMICrossDir = nullptr;
// 	// 	return pNewLayer;
// }

void XLayerDummy::Destroy()
{
	GetAnimationView()->DestroyMI( m_pMICrossDir );
	SAFE_DELETE( m_pMICrossDir );
}

void XLayerDummy::ClearMICrossDir()
{
	m_pMICrossDir = nullptr;
}
// AnimationView에 십자선 인터페이스를 등록한다
void XLayerDummy::RegisterMI()
{
	GetAnimationView()->RegisterMI( m_pMICrossDir );
}
void XLayerDummy::UnRegisterMI()
{
	GetAnimationView()->UnRegisterMI( m_pMICrossDir );
}

void XLayerDummy::Draw( float x, float y, const D3DXMATRIX &m, XSprObj *pParentSprObj )
{
	XMICrossDir *pmi = m_pMICrossDir;
	if( pmi && m_bActive )
	{
		// 매트릭스m이랑 곱해야함
		XE::VEC2 vPosT;
		Transform( &vPosT );
		pmi->SetPos( vPosT );
		pmi->SetfAngle( GetcnRot().fAngle );
		GetAnimationView()->DrawMICRossDir( pmi );
	}
}
// subType키에 맞는 키를 현재 위치에 생성한다
XBaseKey* XLayerDummy::DoCreateKeyDialog( XSPAction spAction, float fFrame, xSpr::xtLayerSub subType )
{
	XBaseKey *pNewKey = nullptr;
	if( pNewKey = XLayerMove::DoCreateKeyDialog( spAction, fFrame, subType ) )
		return pNewKey;
	// 현재 프레임에 더미키를 추가한다
	XKeyDummy *pMainKey;
	pMainKey = spAction->AddKeyDummy( fFrame, GetThis(), ON );	// on키를 삽입
	return pMainKey;
}

void XLayerDummy::SetNewInstanceInSameAction( ID idLayer, int nLayer )
{
	XBaseLayer::SetNewInstanceInSameAction( idLayer, nLayer );
	if( m_pMICrossDir ) {
		// 복사
		m_pMICrossDir = new XMILayerCrossDir( *m_pMICrossDir );
		m_pMICrossDir->SetspLayer( GetThis() );
	}
}
void XLayerDummy::SetNewInstanceInOtherAction()
{
	XBaseLayer::SetNewInstanceInOtherAction();
	if( m_pMICrossDir ) {
		// 복사
		m_pMICrossDir = new XMILayerCrossDir( *m_pMICrossDir );
		m_pMICrossDir->SetspLayer( GetThis() );
	}
}
