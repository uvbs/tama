#include "stdafx.h"
#include "XKeyPos.h"
#include "XLayerMove.h"
#include "SEFont.h"
#include "DlgPosProp.h"
#include "XAniAction.h"
#include "SprObj.h"
#include "XActObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////////////
XKeyPos::XKeyPos( XSPAction spAction, xSpr::xtKey type, float fFrame, XSPLayerMove spLayer, float x, float y )
	: XBaseKey( spAction, type, sGetChannelType(), fFrame, spLayer )
{
	Init();
	m_x = x;		m_y = y;
	//m_spLayer = spLayer;
}
// void XKeyPos::SetspLayer( XSPBaseLayer& spLayer )
// {
// 	XBaseKey::SetspLayer( spLayer );
// 	//m_spLayer = std::static_pointer_cast<XLayerMove>( spLayer );
// }
// XBaseKey* XKeyPos::CopyDeep()
// {
// 	return new XKeyPos( *this );
// }
// XKeyPos::XKeyPos( const XKeyPos& src, XSPActionConst spActNew, ID idLayer, ID idLocalInLayer )
// 	: /*XKeyPos( src ), */XBaseKey( src, spActNew, idLayer, idLocalInLayer ) 
// {
// //	*this = src;
// //	this만의 깊은복사 처리
// }

XBaseKey* XKeyPos::CreateCopy()
{
//	return new XKeyPos( *this, spActNew, idLayerNew, idLocalInLayer );
	auto pKeyNew = new XKeyPos( *this );		// 기본카피
//	pKeyNew->SetNewInstance( spActNew, idLayerNew, idLocalInLayer );
	return pKeyNew;
}
void XKeyPos::Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes )
{
	XBaseKey::Save( typeLayer, nLayer, pRes );
	pRes->Write( &m_x, 4 );					// 0,0을 중심으로 한 이동좌표
	pRes->Write( &m_y, 4 );
	short s0;
	// 편의상 소숫점은 지원되지 않게 함.
	s0 = (short)m_vRangeX.v1;		pRes->Write( &s0, 2 );
	s0 = (short)m_vRangeX.v2;		pRes->Write( &s0, 2 );
	s0 = (short)m_vRangeY.v1;		pRes->Write( &s0, 2 );
	s0 = (short)m_vRangeY.v2;		pRes->Write( &s0, 2 );
	s0 = (short)m_Radius;		pRes->Write( &s0, 2 );
	BYTE bData = (BYTE)m_Interpolation;
	pRes->Write( &bData, 1 );				// 다음키까지 보간을 하는가
	DWORD dw1 = 0;
//	pRes->Write( &m_idPath, 4 );
	(*pRes) << dw1;
	XKeyPos *pPosKey = nullptr;
// 	if( m_idNextPathKey ) {
// 		XBaseKey *p = m_spAction->GetpKeyMng()->GetKey( m_idNextPathKey );
// 		if( p ) {
// 			pPosKey = dynamic_cast<XKeyPos*>( p );
// 			XBREAK( pPosKey == nullptr );
// 		}
// 		else
// 			m_idNextPathKey = 0;
// 	}
// 	if( pPosKey )
// 		dw1 = pPosKey->GetnIdx();
// 	pRes->Write( &dw1, 4 );		// 게임에서 로딩될때 배열의 인덱스로 저장
// 	pRes->Write( &m_idNextPathKey, 4 );
	(*pRes) << 0;
	(*pRes) << 0;
	//
// 	dw1 = -1;
	pPosKey = nullptr;
// 	if( m_idPrevPathKey ) {
// 		XBaseKey *p = m_spAction->GetpKeyMng()->GetKey( m_idPrevPathKey );
// 		if( p ) {
// 			pPosKey = dynamic_cast<XKeyPos*>( p );
// 			XBREAK( pPosKey == nullptr );
// 		}
// 		else
// 			m_idPrevPathKey = 0;
// 	}
// 	if( pPosKey )	dw1 = pPosKey->GetnIdx();
// 	pRes->Write( &dw1, 4 );		// m_idPrevPathKey's index
// 	pRes->Write( &m_idPrevPathKey, 4 );
	(*pRes) << 0;
	(*pRes) << 0;
	//
 	dw1 = -1;
	auto pNextKey = GetpInterpolationEndKey();
	if( pNextKey ) {
		( *pRes ) << pNextKey->GetnIdx();
		( *pRes ) << pNextKey->GetidKey();
	} else {
		( *pRes ) << -1;
		( *pRes ) << 0;
	}
// 	if( m_idInterpolationEndKey ) {
// 		XBaseKey *p = m_spAction->GetpKeyMng()->GetKey( m_idInterpolationEndKey );
// 		if( p ) {
// 			pPosKey = dynamic_cast<XKeyPos*>( p );
// 			XBREAK( pPosKey == nullptr );
// 		} else
// 			m_idInterpolationEndKey = 0;
// 	}
// 	if( pPosKey )	
// 		dw1 = pPosKey->GetnIdx();
// 	(*pRes) << dw1;
// 	(*pRes) << m_idInterpolationEndKey;
	//
	(*pRes) << 0;
	(*pRes) << 0;
	dw1 = 0;
	( *pRes ) << 0;
	( *pRes ) << 0;
	( *pRes ) << 0;
	( *pRes ) << 0;

}

void XKeyPos::Load( XResFile *pRes, XSPAction spAction, int ver )
{
	//	XBaseKey::Load( pRes, spAction, ver );
	//m_spLayer = std::static_pointer_cast<XLayerMove>( XBaseKey::GetspLayer() );
	pRes->Read( &m_x, 4 );
	pRes->Read( &m_y, 4 );
	if( ver < 15 ) {		// ver 0 ~ 14
		m_x *= 0.5f;
		m_y *= 0.5f;
	}
	if( ver >= 30 ) {
		short s0;
		pRes->Read( &s0, 2 );		m_vRangeX.v1 = (float)s0;
		pRes->Read( &s0, 2 );		m_vRangeX.v2 = (float)s0;
		pRes->Read( &s0, 2 );		m_vRangeY.v1 = (float)s0;
		pRes->Read( &s0, 2 );		m_vRangeY.v2 = (float)s0;
		pRes->Read( &s0, 2 );		m_Radius = (float)s0;
	}
	BYTE bData;
	pRes->Read( &bData, 1 );
	m_Interpolation = ( xSpr::xtInterpolation )bData;
	DWORD dw1 = 0;
	if( ver >= 20 ) {
//		pRes->Read( &m_idPath, 4 );
		(*pRes) >> dw1;
		//
		if( ver >= 21 ) {
// 			pRes->Read( &dw1, 4 );	// index. 툴에선 스킵
// 			pRes->Read( &m_idNextPathKey, 4 );
// 			pRes->Read( &dw1, 4 );	// index
// 			pRes->Read( &m_idPrevPathKey, 4 );
// 			pRes->Read( &dw1, 4 );	// index
			( *pRes ) >> dw1;
			( *pRes ) >> dw1;
			( *pRes ) >> dw1;
			( *pRes ) >> dw1;
			( *pRes ) >> dw1;
			( *pRes ) >> dw1;		// pRes->Read( &m_idInterpolationEndKey, 4 );
 			
// 			pRes->Read( &dw1, 4 );
// 			m_PathType = ( SPR::xtLINE )dw1;	// path type
// 			pRes->Read( &m_colPath, 4 );
			( *pRes ) >> dw1;
			( *pRes ) >> dw1;
		} else {
// 			pRes->Read( &m_idNextPathKey, 4 );
// 			pRes->Read( &m_idPrevPathKey, 4 );
// 			pRes->Read( &dw1, 4 );
// 			m_PathType = ( SPR::xtLINE )dw1;	// path type
			( *pRes ) >> dw1;
			( *pRes ) >> dw1;
			( *pRes ) >> dw1;
		}
		pRes->Read( &dw1, 4 );			// reserved
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );
	} else {
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );
		pRes->Read( &dw1, 4 );
	}
}
// this와 같은 패스에 속하는 키인지 검사
// BOOL XKeyPos::IsSamePath( XKeyPos *pKeyTarget )
// {
// 	if( GetpNextPathKey() == pKeyTarget )	// 내다음패스로 연결되어 있는 키는 패스아이디가 달라도 무조건 같은 패스로 침
// 		return TRUE;
// 	if( GetidPath() == 0 )	// this다음키도 아니고 아이디는 0이면 무조건 다른 패스
// 		return FALSE;
// 	if( GetidPath() == pKeyTarget->GetidPath() )	// 같은 패스아이디를 갖고 있으면 같은 패스
// 		return TRUE;
// 	return FALSE;
// }

/**
 @brief 키의 위치값을 얻는다. 아직 랜덤적용이 안된상태면 랜덤적용 시킨값을 생성해 리턴한다.
*/
XE::VEC2 XKeyPos::GetPos() 
{
	if( m_vPos.IsInit() ) {
		m_vPos = AssignRandom();
	}
	return m_vPos;
}

XE::VEC2 XKeyPos::AssignRandom() const
{
	auto vPos = XE::VEC2( m_x, m_y );
	if( m_Radius ) {
		const auto vLocal = XE::GetAngleVector( xRandomF(360.f), m_Radius );
		vPos += vLocal;
	} else {
		if( !m_vRangeX.IsZero() ) {
			vPos.x += xRandomF( m_vRangeX.v1, m_vRangeX.v2 );
		}
		if( !m_vRangeY.IsZero() ) {
			vPos.y += xRandomF( m_vRangeY.v1, m_vRangeY.v2 );
		}
	}
	return vPos;
}

// 패스구간: 스플라인이 적용되어야 하는 전체 키구간. 키끼리 앞뒤로 연결되어 있다.(하지만 각 키패스타입에 따라 스플라인으로 그려질수도 있고 직선으로 그려질수도 있다)
// 보간구간: 보간계산이 이뤄져야 하는 구간
// 각키는 패스구간설정과 별도로 곡선/직선 패스타입을 가지고 있어야 한다
// 패스구간과 보간구간은 다를수 있다.
// 보간구간내에서도 직선과 곡선이 섞여있을수 있다
void XKeyPos::Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec )
{
//	const auto bPlaying = spActObj->GetpSprObj()->IsPlaying();
	auto pSprObj = spActObj->GetpSprObj();
	auto bPlaying = pSprObj->IsPlaying();
	if( pSprObj->GetpParent() )
		bPlaying = pSprObj->GetpParent()->IsPlaying();
	auto spLayerMove = std::static_pointer_cast<XLayerMove>( spLayer );
	if( XASSERT( spLayerMove ) ) {
		// 키가 실행될때마다 랜덤이 새로 적용된다.
		auto& cn = spLayerMove->GetcnPosMutable();
		if( m_Interpolation ) {
			// 보간적용
			auto pInterEndKey = GetpInterpolationEndKey();		// 보간끝키
			if( pInterEndKey ) {
	//		cn.vPos = m_vPos;		// 레이어에서 계산해서 지정하므로 여기선 입력하지 않음. 이전키에서 this키의 랜덤값으로 움직였을수 있으므로 여기서 값을 바꾸진 않음.
				cn.m_interpolation = m_Interpolation;
				cn.m_fStartKeyFrame = GetfFrame();
				if( bPlaying )
					cn.m_vStart = GetPos();
				else
					cn.m_vStart = GetvPosOrig();
				cn.m_fEndKeyFrame = pInterEndKey->GetfFrame();
				cn.m_vEnd = pInterEndKey->GetPos();
			} else {
				// 다음키가 없으면 이전보간값을 레이어에 유지
// 				cn.m_interpolation = m_Interpolation;
				cn.m_interpolation = xSpr::xNONE;
 				cn.m_fStartKeyFrame = GetfFrame();
				cn.m_fEndKeyFrame = GetfFrame();
				if( bPlaying )
					cn.m_vStart = GetPos();
				else
					cn.m_vStart = GetvPosOrig();
			}
		} else {
			// 다음키와 보간안함.(다음키가 있건없건)
// 			cn.m_interpolation = m_Interpolation;
// 			cn.m_fStartKeyFrame = GetfFrame();
			cn.m_interpolation = xSpr::xNONE;
			cn.m_fStartKeyFrame = GetfFrame();
			cn.m_fEndKeyFrame = GetfFrame();
			if( bPlaying )
				cn.m_vStart = GetPos();
			else
				cn.m_vStart = GetvPosOrig();
		}
		if( bPlaying )
			m_vPos = AssignRandom();		// 다음 보간을 위해 랜덤값 만듬.
		else
			m_vPos = GetvPosOrig();		// 에디팅 모드에선 오리지날 값만 씀.
	} // spLayerMove
}

// this키의 다음패스키를 꺼내는데 이 키는 pSrcKey와 같은패스라야 한다
// XKeyPos* XKeyPos::GetpNextPathKey( ID idPathSrc/* = 0*/ )
// {
// 	if( idPathSrc == 0 )
// 		idPathSrc = m_idPath;
// 	if( m_idNextPathKey == 0 )	return nullptr;
// 	XBREAK( m_spAction == nullptr );
// 	XBaseKey *pBaseKey = m_spAction->GetpKeyMng()->GetKey( m_idNextPathKey );
// 	if( pBaseKey == nullptr )
// 		return nullptr;
// 	XKeyPos *pNextKey = SafeCast<XKeyPos*, XBaseKey*>( pBaseKey );
// 	if( pNextKey == nullptr )
// 		m_idNextPathKey = 0;	// 깨진아이디므로 클리어
// 	// 다음패스키가 같은 패스인지 다른패스인지 검사
// 	if( pNextKey->GetidPath() == idPathSrc ) {		// 비교대상 키와 다음키가 같은 패스인가
// 		return pNextKey;		// 같은 패스.
// 	}
// 	else {
// 		// 비교대상키와 다음키가 패스번호가 다르다
// 		if( m_idPath == idPathSrc )
// 			return pNextKey;	// 그렇다면 내키와 비교대상키가 같은상태라면 내 다음키는 무조건 같은패스다
// 		else
// 			return nullptr;
// 	}
// }
// XKeyPos* XKeyPos::GetpPrevPathKey()
// {
// 	if( m_idPrevPathKey == 0 )	return nullptr;
// 	XBREAK( m_spAction == nullptr );
// 	XBaseKey *pBaseKey = m_spAction->GetpKeyMng()->GetKey( m_idPrevPathKey );
// 	if( pBaseKey == nullptr )
// 		return nullptr;
// 	XKeyPos *pKey = SafeCast<XKeyPos*, XBaseKey*>( pBaseKey );
// 	if( pKey == nullptr )
// 		m_idPrevPathKey = 0;
// 	return pKey;
// }
XKeyPos* XKeyPos::GetpInterpolationEndKey()
{
	if( !m_Interpolation )
		return nullptr;
	auto pBaseKey = GetNextKey();
	if( pBaseKey ) {
		return SafeCast<XKeyPos*>( pBaseKey );
	}
	return nullptr;
// 	if( m_idInterpolationEndKey == 0 )	return nullptr;
// 	XBREAK( m_spAction == nullptr );
// 	XBaseKey *pBaseKey = m_spAction->GetpKeyMng()->GetKey( m_idInterpolationEndKey );
// 	if( pBaseKey == nullptr )
// 		return nullptr;
// 	XKeyPos *pKey = SafeCast<XKeyPos*, XBaseKey*>( pBaseKey );
// 	if( pKey == nullptr )
// 		m_idInterpolationEndKey = 0;
// 	return pKey;
}

// 키가 액션에 애드될때 호출된다. 
// KeyPos는 애드될때 패스키 사이에 끼면 자동으로 앞뒤 키에 연결되어야 한다.
// void XKeyPos::OnAdded()
// {
// 	XBaseKey::OnAdded();
// 	// pos키의 경우 보간키라면 애드될때 자동으로 앞뒤 키와 연결된다
// 	XBaseKey *pNextKey = GetNextKey();
// 	XBaseKey *pPrevKey = GetPrevKey();
// 	XKeyPos *pNextPosKey = nullptr;
// 	XKeyPos *pPrevPosKey = nullptr;
// 	if( pNextKey )
// 		pNextPosKey = XKeyPos::Cast( pNextKey );
// 	if( pPrevKey )
// 		pPrevPosKey = XKeyPos::Cast( pPrevKey );
// 	// 앞에 키가 있으면 this와 연결시킨다,
// 	if( pPrevPosKey ) {
// 		// 앞키의 뒤에 다른키가 연결되어 있었다면
// 		if( /*pPrevPosKey->GetpNextPathKey() ||*/ pPrevPosKey->GetInterpolation() )	{
// 			//			XBREAK( pPrevPosKey->GetidPath() == 0 );
// // 			SetidPath( pPrevPosKey->GetidPath() );		// 앞키의 뒤에 뭐가 연결되어 있었다면 패스다
// // 			pPrevPosKey->SetpNextPathKey( this );	// 앞키의 뒤에 새키를 연결시킴
// // 			SetpPrevPathKey( pPrevPosKey );		// 새키의 앞에 앞키를 연결시킴
// 			// 앞키에 기존 다음키가보간으로 연결되어 있었으면 this키로 바꿔준다
// 			if( pPrevPosKey->GetpInterpolationEndKey() == pNextPosKey ) {
// 				pPrevPosKey->SetpInterpolationEndKey( this );		// 앞키의 보간키를 this로 지정
// 				SetpInterpolationEndKey( pNextPosKey );			// this의 보간키를 다음키로 지정
// 				SetInterpolation( xSpr::xLINEAR );				// 새키의 보간방식은 선형이 디폴트
// 			}
// 		}
// 	}
// 	// 뒤에 키가 있다면 this에 뒷키를 연결시킨다
// 	if( pNextPosKey ) {
// 		// 뒤에 키가 있다면 this에 뒷키를 자동 연결시킨다
// // 		SetpNextPathKey( pNextPosKey );		// this -> next key
// // 		pNextPosKey->SetpPrevPathKey( this );		// this <- next key
// 		//		if( GetInterpolation() )
// // 		{
// // 			// 앞에 키가 없을경우나 앞에 키가 '뒤'에 연결되어 있지 않은경우
// // 			if( pPrevPosKey == nullptr || ( pPrevPosKey && pPrevPosKey->GetpNextPathKey() == nullptr ) ) {
// // 				SetpInterpolationEndKey( pNextPosKey );		// 뒷키를 보간끝키로 지정
// // 			}
// // 		}
// 		SetpInterpolationEndKey( pNextPosKey );		// 새키의 뒤키를 보간으로 연결시킨다
// 	} // pNextPosKey
// 
// }

BOOL XKeyPos::EditDialog()
{
	CDlgPosProp dlg;
	dlg.m_x = m_x;
	dlg.m_y = m_y;
	dlg.m_bInterpolation = ( GetInterpolation() ) ? TRUE : FALSE;
	dlg.m_Interpolation = GetInterpolation();
	dlg.m_minX = m_vRangeX.v1;
	dlg.m_maxX = m_vRangeX.v2;
	dlg.m_minY = m_vRangeY.v1;
	dlg.m_maxY = m_vRangeY.v2;
	dlg.m_radius = m_Radius;
	if( dlg.DoModal() == IDOK )	{
		m_x = dlg.m_x;
		m_y = dlg.m_y;
		m_vRangeX = XE::VEC2( dlg.m_minX, dlg.m_maxX );
		m_vRangeY = XE::VEC2( dlg.m_minY, dlg.m_maxY );
		m_Radius = dlg.m_radius;
		m_Interpolation = dlg.m_Interpolation;
		if( dlg.m_bInterpolation == FALSE )
			m_Interpolation = xSpr::xNONE;
		else
			if( !m_Interpolation )
				m_Interpolation = xSpr::xLINEAR;

		return TRUE;
	}
	return FALSE;
}

void XKeyPos::GetToolTipSize( float *w, float *h )
{
	XBaseKey::GetToolTipSize( w, h );
	*w += SE::g_pFont->GetFontWidth() * 8;
	*h += SE::g_pFont->GetFontHeight() * 7;
}
CString XKeyPos::GetToolTipString( XSPBaseLayer spLayer )
{
	CString str;
	CString strFunc;
	switch( GetInterpolation() )
	{
	case xSpr::xNONE:		strFunc = _T( "보간하지 않음" );		break;
	case xSpr::xLINEAR:		strFunc = _T( "선형" );				break;
	case xSpr::xACCEL:		strFunc = _T( "가속" );		break;
	case xSpr::xDEACCEL:		strFunc = _T( "감속" );		break;
	case xSpr::xSMOOTH:	strFunc = _T( "부드럽게" );		break;
	case xSpr::xSPLINE:		strFunc = _T( "스플라인" );		break;
	}
// 	CString strPath;
// 	switch( m_PathType )
// 	{
// 	case SPR::xLINE_NONE:		strPath = _T( "없음" );	break;
// 	case SPR::xLINE_LINE:		strPath = _T( "직선" );	break;
// 	case SPR::xLINE_SPLINE_CATMULLROM:	strPath = _T( "CatmullRom스플라인" );	break;
// 	default:
// 		XBREAK( 1 );
// 		break;
// 	}
// #ifdef _DEBUG
	auto pNextKey = GetpInterpolationEndKey();
	str.Format( _T( "pos:%d,%d\n\
									 보간방법:%s\n\
									 보간끝키:%.2f(0x%08x)" ),
									 (int)Getx(), (int)Gety()
									 , strFunc,
 									 ( pNextKey ) ? pNextKey->GetfFrame() : -1.f, 
									 ( pNextKey ) ? pNextKey->GetidKey() : 0 );
	str = XBaseKey::GetToolTipString( spLayer ) + str;
// #else
// 	str.Format( _T( "pos:%d,%d\n보간방법:%s\n패스ID:0x%08x\n패스종류:%s\n다음키:%.2f\n이전키:%.2f" ),
// 		(int)Getx(), (int)Gety(), strFunc, m_idPath, strPath,
// // 		( GetpNextPathKey() ) ? GetpNextPathKey()->GetfFrame() : -1.f,
// // 		( GetpPrevPathKey() ) ? GetpPrevPathKey()->GetfFrame() : -1.f );
// #endif
	str += GetToolTipStringTail();
	return str;
}
