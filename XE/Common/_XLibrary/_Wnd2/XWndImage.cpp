#include "stdafx.h"
#include "XWndImage.h"
#include "XFramework/XEProfile.h"
#include "XFramework/Game/XEComponents.h"
#include "XImageMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XE;

//////////////////////////////////////////////////////////////////////////
XWndImage* XWndImage::sUpdateCtrl( XWnd *pRoot, const char *cIdentifier )
{
	auto pCtrl = SafeCast2<XWndImage*>( pRoot->Find( cIdentifier ) );
	if( pCtrl == nullptr ) {
		pCtrl = new XWndImage();
		pCtrl->SetstrIdentifier( cIdentifier );
		XBREAK( pCtrl == nullptr );
		pRoot->Add( pCtrl );
	}
	return pCtrl;
}
XWndImage* XWndImage::sUpdateCtrl( XWnd *pRoot
																, const XE::VEC2& vPos
																, LPCTSTR resImg
																, XE::xtPixelFormat formatSurface
																, const char *cIdentifier )
{
	auto pCtrl = sUpdateCtrl( pRoot, cIdentifier );
// 	auto pCtrl = SafeCast2<XWndImage*>( pRoot->Find( cIdentifier ) );
// 	if( pCtrl == nullptr ) {
// 		pCtrl = new XWndImage( resImg, formatSurface, vPos );
// 		pCtrl->SetstrIdentifier( cIdentifier );
// 		XBREAK( pCtrl == nullptr );
// 		pRoot->Add( pCtrl );
// 	}
	pCtrl->SetPosLocal( vPos );
	pCtrl->SetSurfaceRes( resImg );
	return pCtrl;
}


//===========================================================
XWndImage::XWndImage( LPCTSTR szRes, XE::xtPixelFormat formatSurface, float x, float y, bool bMakeMask )
	: XWnd( x, y )
{
	Init();
	if( XE::IsHave( szRes ) ) {
		Create( szRes, formatSurface, bMakeMask );
		if( m_pSurface )
			SetSizeLocal( m_pSurface->GetSize() );
		// 마스킹을 만들면 디폴트로 픽셀픽킹모드로 된다.
		if( bMakeMask )
			SetbPixelPicking( TRUE );
	}
}

void XWndImage::Destroy() 
{
//		if( m_bCreate )
	SAFE_RELEASE2( IMAGE_MNG, m_pSurface );	// 레퍼런스 카운트로 관리되고 있으니 무조건 릴리즈해야 하는거 아닌가?
//	AUTO_DESTROY( m_aryAlphaComponent );
}

/**
 @brief szRes리소스 패스의 이미지파일을 읽는다.
 @param sizeTexture 기본적으로는 원래크기의 절반크기로 텍스쳐가 생성되지만 이 크기를 임의로 지정할 수 있다.(구현보류)
*/
void XWndImage::Create( BOOL bHighReso, LPCTSTR szRes, bool bMakeMask, const XE::VEC2& sizeTexture )
{
	XBREAK( bHighReso == FALSE );	// 이제 무조건 TRUE로 바뀜
	SAFE_RELEASE2(IMAGE_MNG, m_pSurface);
	m_pSurface = IMAGE_MNG->Load( szRes );
	// 	m_pSurface = IMAGE_MNG->Load( szRes, FALSE, bMakeMask/*, true*/ );
	XBREAKF( m_pSurface == NULL, "create fail. %s", szRes );
//	m_bCreate = TRUE;
}
/**
 @brief pixelFormat지정버전
*/
void XWndImage::Create( LPCTSTR szRes
											, XE::xtPixelFormat format
											, bool bMakeMask )
{
	SAFE_RELEASE2(IMAGE_MNG, m_pSurface);
 	m_pSurface = IMAGE_MNG->Load( szRes, format, true, false, bMakeMask, false );
	XBREAKF( m_pSurface == nullptr, "create fail. %s", szRes );
//	m_bCreate = TRUE;
}

void XWndImage::SetSurface( XSurface *pSurface ) 
{
	// pSurface가 널일수도 있다.
// 		if( m_bCreate ) {
// 			SAFE_RELEASE2( IMAGE_MNG, m_pSurface );
// 			m_bCreate = FALSE;
// 		}
	if( pSurface ) {
		SAFE_RELEASE2( IMAGE_MNG, m_pSurface );
		m_pSurface = pSurface;
		m_pSurface->IncRefCnt();
		SetSizeLocal( pSurface->GetSize() );
		SetbUpdate( true );
	} else {
		SAFE_RELEASE2( IMAGE_MNG, m_pSurface );
	}
}

// bAutoSizeParent: this의 크기를 포함해서 부모의 전체 바운드박스영역을 재 조정한다.
void XWndImage::SetSurface( LPCTSTR szRes, bool bAutoSizeParent )
{
	SetSurface( szRes, XE::xPF_ARGB4444, bAutoSizeParent );
}
void XWndImage::SetSurface( LPCTSTR szRes, XE::xtPixelFormat formatSurface, bool bAutoSizeParent ) 
{
	if( szRes == nullptr )
		return;
	//  같은 이미지를 세팅하려 했으면 다시 할필요 없음.
	if( m_pSurface && m_pSurface->GetstrRes() == szRes )
		return;
//	if( m_bCreate )
//		SAFE_RELEASE2(IMAGE_MNG, m_pSurface );
	if( XE::IsHave(szRes) )	{
		Create( szRes, formatSurface, false ); 
		if( bAutoSizeParent ) {
			if( m_pParent ) {
				XE::VEC2 sizeParent = m_pParent->GetSizeLocal();
				XE::VEC2 vLTp, vRBp;
				vRBp = sizeParent;
				XE::VEC2 sizeImage( m_pSurface->GetWidth(), m_pSurface->GetHeight() );
				XE::VEC2 vLT = GetPosLocal();
				XE::VEC2 vRB = vLT + sizeImage;
				if( vRB.x > vRBp.x )
					vRBp.x = vRB.x;
				if( vRB.y > vRBp.y )
					vRBp.y = vRB.y;
				m_pParent->SetSizeLocal( vRBp );
			}
		}
	} else {
		SAFE_RELEASE2( IMAGE_MNG, m_pSurface );
//		m_pSurface = nullptr;
//		m_bCreate = FALSE;
	}

	if( m_pSurface ) {
		SetSizeLocal( m_pSurface->GetWidth(), m_pSurface->GetHeight() );
		SetbUpdate( true );
	}
}

/**
 @brief RAW이미지데이타로 XWndImage를 만든다.
 @param bAutoSizeParent: this의 크기를 포함해서 부모의 전체 바운드박스영역을 재 조정한다.
*/
void XWndImage::SetSurface( const char* cKey, DWORD *pImg, int w, int h, bool bAutoSizeParent )
{
	XBREAK( pImg == nullptr );
	XBREAK( w <= 0 );
	XBREAK( h <= 0 );
	XBREAK( XE::IsEmpty(cKey) );
	XUINT64 idRes64 = XE::GetCheckSum( cKey );
	// 같은 키의 이미지를 생성하려 했다면 취소
	if( m_pSurface && m_pSurface->Getid64Res() == idRes64 )
		return;
	// 이전의 서피스가 자체생성한거면 직접 해제시킴.
// 	if( m_bCreate )
		SAFE_RELEASE2( IMAGE_MNG, m_pSurface );

	const XE::POINT ptSize( w, h );
	auto pSurface = IMAGE_MNG->CreateSurface( cKey
																					, ptSize / 2
																					, XE::VEC2()
																					, XE::xPF_ARGB8888
																					, pImg
																					, ptSize
																					, false, false );
	m_pSurface = pSurface;
	if( pSurface ) {
//		m_bCreate = TRUE;
		SetSizeLocal( pSurface->GetSize() );
		SetbUpdate( true );
		if( bAutoSizeParent ) {
			if( m_pParent ) {
				const XE::VEC2 sizeParent = m_pParent->GetSizeLocal();
				XE::VEC2 vLTp, vRBp;
				vRBp = sizeParent;
				const XE::VEC2 sizeImage( m_pSurface->GetWidth(), m_pSurface->GetHeight() );
				const XE::VEC2 vLT = GetPosLocal();
				XE::VEC2 vRB = vLT + sizeImage;
				if( vRB.x > vRBp.x )
					vRBp.x = vRB.x;
				if( vRB.y > vRBp.y )
					vRBp.y = vRB.y;
				m_pParent->SetSizeLocal( vRBp );
			}
		}
	} else {
//		m_bCreate = FALSE;
	}
}

MATRIX* XWndImage::GetMatrix( MATRIX *pOut, float lx, float ly )
{
	lx += m_vAdjustAxis.x;		// 좌표축 보정
	ly += m_vAdjustAxis.y;
	MATRIX mTrans, mScale, mAxis;
	MatrixTranslation( mTrans, lx, ly, 0 );
	MATRIX mRz;
	MatrixRotationY( mRz, D2R( GetRotateLocal() ) );
	MatrixScaling( mScale, GetScaleLocal().x, GetScaleLocal().y, 1.0f );
	MatrixTranslation( mAxis, -m_vAdjustAxis.x, -m_vAdjustAxis.y, 0 );
//	*pOut = mAxis * mScale * mRz * mTrans;
	MatrixMultiply( *pOut, mAxis, mScale );
	MatrixMultiply( *pOut, *pOut, mRz );
	MatrixMultiply( *pOut, *pOut, mTrans );
	return pOut;
}
/**
 영역터치 함수를 재정의 해서 쓴다.
*/
BOOL XWndImage::IsWndAreaIn( float lx, float ly ) 
{
	if( m_bPixelPicking == FALSE )	{
		int w = (int)GetSizeFinal().x;
		int h = (int)GetSizeFinal().y;
		if( XE::IsArea2( 0, 0, w, h, lx, ly ) )
			return TRUE;
	} else	{
		// 픽셀 픽킹 모드일때는 서피스 객체의 마스킹 이미지를 이용해 터치를 감지한다.
		if( XBREAK( m_pSurface == NULL ) )
			return FALSE;
		MATRIX mWorld, mInv;
		GetMatrix( &mWorld, 0, 0 );		// 원점의 트랜스폼 매트릭스를 얻음.
		MatrixInverse( mInv, mWorld );
		X3D::VEC3 v(lx, ly, 0);
		Vec4 vInv;
		MatrixVec4Multiply( vInv, v, mInv );
//		BYTE a = m_pSurface->GetMask( v.x, v.y );
		BYTE a = m_pSurface->GetMask( vInv.x, vInv.y );
		if( a > 0 )
			return TRUE;
	}
	return FALSE;
}

void XWndImage::GetMatrix( MATRIX *pOut )
{
	MATRIX mTrans, mScale, mRot, mWorld, mAxis, mReverseAxis;
	MatrixTranslation( mAxis, -m_vAdjustAxis.x, -m_vAdjustAxis.y, 0.f );
	MatrixScaling( mScale, GetScaleLocal().x, GetScaleLocal().y, 1.f );
	MatrixIdentity( mRot );
	MatrixTranslation( mTrans, GetPosLocal().x, GetPosLocal().y, 0.f );
	MatrixTranslation( mReverseAxis, m_vAdjustAxis.x, m_vAdjustAxis.y, 0.f );
	MatrixMultiply( mWorld, mAxis, mScale );
	MatrixMultiply( mWorld, mWorld, mRot );
	MatrixMultiply( mWorld, mWorld, mTrans );
	MatrixMultiply( *pOut, mWorld, mReverseAxis );
}

void XWndImage::Draw()
{
	XPROF_OBJ_AUTO();
	if( m_pSurface ) {
		const auto vAdjDrawFinal = GetvAdjDrawFinal();
		const auto vPos = GetPosFinal() + vAdjDrawFinal + m_vAdjust;		// <<= m_vAdjust도 this의 스케일을 적용하는게 맞다. 지금은 이변수에 대한 외부의존이 높기때문에 건드리지 않는다.
		const auto vScale = GetScaleFinal();
		m_pSurface->SetRotateY( m_dAngY );
		m_pSurface->SetRotateZ( m_dAngZ );
		m_pSurface->SetScale( vScale );
		m_pSurface->SetAdjustAxis( m_vAdjustAxis );
		m_pSurface->SetfAlpha( GetAlphaFinal() );
//		m_pSurface->SetDrawMode( GetblendFunc() );
		m_pSurface->SetBlendFunc( GetblendFunc() );
		m_pSurface->SetColor( m_Color );
//		m_pSurface->SetColor( XCOLOR_RED );
		m_pSurface->Draw( vPos );
#ifdef _CHEAT
		if( XWnd::s_bDebugMode )
			CONSOLE("wndImg:pos=%d,%d", (int)vPos.x, (int)vPos.y );
#endif // _CHEAT
	}
	XWnd::Draw();
}

void XWndImage::Draw( const XE::VEC2& vPos )
{
	XPROF_OBJ_AUTO();
	if( m_pSurface )	{
		const auto vAdjDrawFinal = GetvAdjDrawFinal();
		const auto v = vPos + vAdjDrawFinal + m_vAdjust;
		const auto vScale = GetScaleFinal();
		m_pSurface->SetAdjustAxis( m_vAdjustAxis );
		m_pSurface->SetScale( vScale );
		m_pSurface->SetfAlpha( GetAlphaFinal() );
		m_pSurface->SetColor( m_Color );
		m_pSurface->Draw( v );
	}
	XWnd::Draw( vPos );
}

void XWndImage::Draw( XE::VEC2& vPos, xDM_TYPE drawMode )
{
	XPROF_OBJ_AUTO();
	if( m_pSurface ) {
		const auto vAdjDrawFinal = GetvAdjDrawFinal();
		const auto v = vPos + vAdjDrawFinal + m_vAdjust;
		m_pSurface->SetAdjustAxis( m_vAdjustAxis );
		m_pSurface->SetfAlpha( GetAlphaFinal() );
		m_pSurface->SetDrawMode( drawMode );
		m_pSurface->SetColor( m_Color );
		m_pSurface->Draw( v );
	}
//	XWnd::Draw();
}

void XWndImage::GetDebugString( _tstring& strOut )
{
	XWnd::GetDebugString( strOut );
	if( m_pSurface )
		strOut += XFORMAT( ", img=%s", m_pSurface->GetstrRes().c_str() );;
}

int XWndImage::Process( float dt )
{
	// 시범적으로 콤포넌트 방식을 써봄.
	if( m_compMngByAlpha.IsbActive() ) {
		m_compMngByAlpha.Process( dt );
		auto pCompAlpha = m_compMngByAlpha.FindComponentByFunc( "alpha" );
		if( pCompAlpha ) {
			const float alpha = pCompAlpha->GetValue();
			SetAlphaLocal( alpha );
		}
	}
	return XWnd::Process( dt );
}

XEComponent* XWndImage::AddComponentWave( const std::string& ids
																				, float secDelay /*= 1.f*/
																				, float secFade /*= 1.f*/
																				, float minValue
																				, float maxValue )
{
	return m_compMngByAlpha.AddComponentWave( ids, nullptr, secDelay, secFade, minValue, maxValue );
}

