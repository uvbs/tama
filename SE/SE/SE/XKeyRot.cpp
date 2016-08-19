#include "stdafx.h"
#include "XKeyRot.h"
#include "XLayerMove.h"
#include "DlgRotProp.h"
#include "SEFont.h"
#include "SprObj.h"
#include "XActObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

///////////////// Key Rot /////////////////////////////////////////
XKeyRot::XKeyRot( XSPAction spAction, xSpr::xtKey type, float fFrame, XSPLayerMove spLayer, float az )
	: XBaseKey( spAction, type, sGetChannelType(), fFrame, spLayer )
{
	Init();
	m_fAngleZ = az;
	//m_spLayer = spLayer;
}

XBaseKey* XKeyRot::CreateCopy()
{
//	return new XKeyRot( *this, spActNew, idLayer, idLocalInLayer );
	auto pKeyNew = new XKeyRot( *this );		// 기본카피
// 	pKeyNew->SetNewInstance( spActNew, idLayer, idLocalInLayer );
	return pKeyNew;
}

// void XKeyRot::SetspLayer( XSPBaseLayer& spLayer )
// {
// 	XBaseKey::SetspLayer( spLayer );
// 	//m_spLayer = std::static_pointer_cast<XLayerMove>( spLayer );
// }

/*
// 키 카피의 경우.
	auto pNew = new XKeyRot( *pKeySrc, spAction ) {
		*this = src;
		m_Key = spAction->GetKey( src.GetKey()->GetidLocal() );)
	}
	pNew->ResetAfterCopy( spAction );
// 액션 카피의 경우
	auto spActNew = new XAction( *spAct.get() );
	XAction( const XAction& src ) {
		for( pKey : listKey ) {
			auto pKeyNew = new XKey????;
			auto KeyNew = *pKey
			auto pKeyNew = pKey->CreateNewVirtual(); { return new XKeyRot( *this ); }
			pKeyNew->ResetAfterCopy( spActNew );
			//
			auto pKeyNew = pKey->CreateNewVirtual( spActNew ) { 
				auto pNew = new XKeyRot( *this );
				pNew->ResetAfterCopy( spActNew );
				//
				auto pNew = new XKeyRot( *this, spActNew, idLayer, idLocalInLayer ) {
					*this = src;
					m_spAction = spActNew;
				}
				return pNew;
			}
			pActNew->AddKey( pKeyNew );


		}
	}
*/
// XBaseKey* XKeyRot::CopyDeep( XSPActionConst spActNew )
// {
// 	return new XKeyRot( *this );
// }
void XKeyRot::Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes )
{
	XBaseKey::Save( typeLayer, nLayer, pRes );
	BYTE b1;
	DWORD dw1 = 0;
	b1 = (BYTE)m_Interpolation;
	pRes->Write( &b1, 1 );
	short s0;
	s0 = (short)(m_vRange.v1 * 100.f);		pRes->Write( &s0, 2 );
	s0 = (short)(m_vRange.v2 * 100.f);		pRes->Write( &s0, 2 );
	// 	pRes->Write( &dw1, 4 );		// reserved
 	pRes->Write( &dw1, 4 );		// reserved
	pRes->Write( &m_fAngleZ, 4 );
}
void XKeyRot::Load( XResFile *pRes, XSPAction spAction, int ver )
{
	//	XBaseKey::Load( pRes, spAction, ver );
	//m_spLayer = std::static_pointer_cast<XLayerMove>( XBaseKey::GetspLayer() );
	BYTE b1;
	DWORD dw1;
	pRes->Read( &b1, 1 );		m_Interpolation = ( xSpr::xtInterpolation )b1;
	if( ver >= 31 ) {
		short s0;
		pRes->Read( &s0, 2 );		m_vRange.v1 = (float)s0 / 100.f;
		pRes->Read( &s0, 2 );		m_vRange.v2 = (float)s0 / 100.f;
	} else {
		pRes->Read( &dw1, 4 );
	}
	pRes->Read( &dw1, 4 );
	pRes->Read( &m_fAngleZ, 4 );
}

float XKeyRot::AssignRandom() const
{
	return m_fAngleZ + xRandomF( m_vRange.v1, m_vRange.v2 );
}

void XKeyRot::Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec )
{
// 	const auto bPlaying = spActObj->GetpSprObj()->IsPlaying();
	auto pSprObj = spActObj->GetpSprObj();
	auto bPlaying = pSprObj->IsPlaying();
	if( pSprObj->GetpParent() )
		bPlaying = pSprObj->GetpParent()->IsPlaying();
	auto spLayerMove = std::static_pointer_cast<XLayerMove>( spLayer );
	if( XASSERT( spLayerMove ) ) {
// 		spLayerMove->SetAngleZ( m_fAngleZ );
		auto& cn = spLayerMove->GetcnRotMutable();
		// 실시간으로 이 키의 뒤에 로테이션키가 있는지 찾는다
		cn.interpolation = m_Interpolation;
		if( bPlaying ) {
			cn.fAngleSrc = GetdAngle();
		} else {
			cn.fAngleSrc = GetdAngleOrig();
		}
		cn.fStartKeyFrame = GetfFrame();
		auto pNextKey = dynamic_cast<XKeyRot *>( GetNextKey() );
		if( pNextKey ) {
//		cn.fAngle = m_dAngle;
			if( bPlaying ) {
				cn.fAngleDest = pNextKey->GetdAngle();
			} else {
				cn.fAngleDest = pNextKey->GetdAngleOrig();
			}
			cn.fNextKeyFrame = pNextKey->GetfFrame();
		} else {
			// 다음키가 없는경우는 현재 레이어를 건드리지 않는다.
			cn.interpolation = xSpr::xNONE;
			cn.fNextKeyFrame = GetfFrame();
		}
		if( bPlaying ) {
			m_dAngle = AssignRandom();
		} else {
			m_dAngle = GetdAngleOrig();
		}
		// 채널정보 씀.
//		spLayerMove->SetcnRot( cn );
// 		CHANNEL_ROT cn;
// 		// 실시간으로 이 키의 뒤에 로테이션키가 있는지 찾는다
// 		auto pNextKey = dynamic_cast<XKeyRot *>( GetNextKey() );
// 		if( m_Interpolation ) {			// 다음키가 있으면 보간으로 움직이도록 한다
// 			if( pNextKey ) {
// 				CHANNEL_ROT cn;
// 				cn.fAngle = m_dAngle;
// 				cn.interpolation = m_Interpolation;
// 	//			cn.fAngleDest = pNextKey->GetfAngleZ();
// 	//			cn.fAngleSrc = m_fAngleZ;
// 				cn.fAngleDest = pNextKey->GetdAngle();
// 				cn.fAngleSrc = m_dAngle;
// 				cn.fNextKeyFrame = pNextKey->GetfFrame();
// 				cn.fStartKeyFrame = GetfFrame();
// 				// 채널정보 씀.
// 				spLayerMove->SetcnRot( cn );
// 			} else {
// 				int a = 0;
// 			}
// 		} else {
// 		}
//		cn.fAngle = m_dAngle;
	}
}
BOOL XKeyRot::EditDialog()
{
	CDlgRotProp dlg;
	dlg.m_fRotZ = m_fAngleZ;
	dlg.m_vRange = m_vRange;
	dlg.m_bInterpolation = ( GetInterpolation() ) ? TRUE : FALSE;
	dlg.m_Interpolation = GetInterpolation();
	if( dlg.DoModal() == IDOK ) {
		m_fAngleZ = dlg.m_fRotZ;
		m_vRange = dlg.m_vRange;
// 		m_Interpolation = dlg.m_Interpolation;
		m_Interpolation = dlg.m_Interpolation;
		if( dlg.m_bInterpolation == FALSE )
			m_Interpolation = xSpr::xNONE;
		else
			if( !m_Interpolation )
				m_Interpolation = xSpr::xLINEAR;
	}
	return FALSE;
}
void XKeyRot::GetToolTipSize( float *w, float *h )
{
	XBaseKey::GetToolTipSize( w, h );
	*w += SE::g_pFont->GetFontWidth() * 6;
	*h += SE::g_pFont->GetFontHeight() * 2;
}
CString XKeyRot::GetToolTipString( XSPBaseLayer spLayer )
{
	CString str;
	str.Format( _T( "보간:%d\n각도:%4.1f" ), (int)m_Interpolation, m_fAngleZ );
#ifdef _DEBUG
	str = XBaseKey::GetToolTipString( spLayer ) + str;
#endif
	str += GetToolTipStringTail();
	return str;
}

