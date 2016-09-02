#include "stdafx.h"
#include "XWndProgressBar.h"
#include "XWndImage.h"
#include "sprite/SprObj.h"
#include "../XDrawGraph.h"
#include "XFramework/XEProfile.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef max
#undef  max;
#endif
#ifdef min
#undef  min;
#endif

//////////////////////////////////////////////////////////////////////////
//==========================================================
XWndProgressBar::XWndProgressBar( float x, float y, LPCTSTR szImg, LPCTSTR szImgBg )
	: XWnd( x, y ),
	XProgressBar( XE::VEC2( x, y ), szImg, szImgBg )
{
	Init();
	if( GetpsfcBg() ) {
		XE::VEC2 vSize = GetpsfcBg()->GetSize();
		SetSizeLocal( vSize );
	} else
		if( GetpSurface() ) {
			XE::VEC2 vSize = GetpSurface()->GetSize();
			SetSizeLocal( vSize );
		}
}

XWndProgressBar::XWndProgressBar( float x, float y, float w, float h, XCOLOR col )
	: XWnd( x, y, w, h ),
	XProgressBar( XE::VEC2( x, y ), XE::VEC2( w, h ) )
{
	Init();
}


void XWndProgressBar::Draw( void )
{
	XPROF_OBJ_AUTO();
	XE::VEC2 vPos = GetPosFinal();
	if( m_Lerp >= 0 )
		XProgressBar::Draw( vPos, m_Lerp );
	XWnd::Draw();
}

//////////////////////////////////////////////////////////////////////////
XWndProgressBar2* XWndProgressBar2::sUpdateCtrl( XWnd* pParent, const XE::VEC2& vPos, const _tstring& strRes, const std::string& ids )
{
	XWndProgressBar2* pWndBar = nullptr;
	auto pWnd = pParent->Find( ids );
	if( pWnd ) {
		pWndBar = SafeCast<XWndProgressBar2*>( pWnd );
	}
	if( pWndBar == nullptr ) {
		pWndBar = new XWndProgressBar2( vPos, strRes );
		pWndBar->SetstrIdentifier( ids );
		pParent->Add( pWndBar );
	}
	return pWndBar;
}
/**
 @brief 확장성을 좀더 개선한 버전.
*/
XWndProgressBar2::XWndProgressBar2( float x, float y, LPCTSTR szImg )
	: XWnd( x, y )
{
	Init();
	if( XE::IsHave(szImg) )
		AddLayer( 1, szImg );
// 		AddLayer( XE::GenerateID(), XE::MakePath(DIR_UI,szImg) );
}

// XWndProgressBar2::XWndProgressBar2( float x, float y, float w, float h, XCOLOR col )
// 	: XWnd( x, y, w, h )
// {
// 	Init();
// }

void XWndProgressBar2::Destroy()
{
	for( auto& layer : m_listLayers ) {
		SAFE_RELEASE2( IMAGE_MNG, layer.m_psfcBar );
		SAFE_DELETE( layer.m_psoTail );
	}
}

void XWndProgressBar2::SetLerp( ID idLayer, float lerp )
{
	for( auto& layer : m_listLayers ) {
		if( layer.m_idLayer == idLayer ) {
			if( XASSERT(!layer.m_bFixed) )
				layer.m_Lerp = lerp;
		}
	}
}
// fixed속성의 레이어만 제외하고 모든 레이어 전부의 lerp를 동일하게 맞춤.
void XWndProgressBar2::SetLerp( float lerp )
{
	for( auto& layer : m_listLayers ) {
		if( !layer.m_bFixed ) {
			layer.m_Lerp = lerp;
		}
	}
}
/**
 @brief 그래프바 애니메이션을 시작한다.
*/
void XWndProgressBar2::DoLerpAni( ID idLayer
																, int lvOrig
																, int lvStart, float lerpStart
																, int lvDst, float lerpDst
																, float secTotal )
{
	XBREAK( lvOrig == 0 );
	XBREAK( lvStart == 0 );
	XBREAK( lvDst == 0 );
//	XBREAK( lerpStart == 0 );
//	XBREAK( lerpDst == 0 );
	m_bAnimation = true;
	m_lvOrig = lvOrig;
	m_lvStartByAni = lvStart;
	m_lvCurrByAni = lvStart;
	m_lvDstByAni = lvDst;
	for( auto& layer : m_listLayers ) {
		if( layer.m_idLayer == idLayer ) {
			if( XASSERT(!layer.m_bFixed) ) {
				layer.m_LerpStart = lerpStart;
				layer.m_LerpDst = lerpDst;
				layer.m_Lerp = lerpStart;
			}
		}
	}
	m_timerAni.Set( secTotal );
}
void XWndProgressBar2::SetLerp( ID idLayer
																, int lvOrig
																, int lvDst, float lerpDst )
{
	XBREAK( lvOrig == 0 );
	XBREAK( lvDst == 0 );
	m_bAnimation = true;
	m_lvOrig = lvOrig;
	m_lvCurrByAni = lvDst;
	for( auto& layer : m_listLayers ) {
		if( layer.m_idLayer == idLayer ) {
			if( XASSERT(!layer.m_bFixed) ) {
				layer.m_Lerp = lerpDst;
			}
		}
	}
}
/**
 @brief 프로그레스바로 쓸 이미지(레이어)를 추가한다.
 @param idLayer 레이어 식별번호
 @param szImg png파일명. 패스는 붙이지 않는다. 자동으로 "ui"폴더에서 찾는다.
 @param bFixed lerp값에 영향을 받지 않는 레이어. 항상 풀사이즈로 그려진다.
*/
XWndProgressBar2::xLayer* 
XWndProgressBar2::AddLayer( ID idLayer
													, LPCTSTR _szImg
													, bool bFixed
													, const XE::VEC2& vAdj )
{
// 	auto pExist = GetpLayer( idLayer );
// 	if( pExist )
// 		return pExist;
	auto pExist = m_listLayers.FindByIDNonPtr( idLayer );
	if( pExist )
		return pExist;
	const _tstring strImg = _szImg;
	xLayer layer;
	layer.m_idLayer = idLayer;
	layer.m_Lerp = 0;
	layer.m_vAdj = vAdj;
//	const _tstring strPath = XE::MakePath2(DIR_UI,strImg);
//	layer.m_psfcBar = IMAGE_MNG->Load( strPath, FALSE, FALSE );
	const _tstring strExt = XE::GetFileExt( strImg );
	if( strExt == _T("png") ) {
		layer.m_psfcBar = IMAGE_MNG->Load( XE::MakePath2( DIR_UI, strImg ), FALSE, FALSE );
		XBREAK( layer.m_psfcBar == nullptr );
	} else {
		const auto strSpr = strImg;
		layer.m_psoTail = new XSprObj( strSpr );
		if( layer.m_idAct == 0 )
			layer.m_idAct = 1;
//		layer.m_psoTail->SetAction( layer.m_idAct );
	}
	layer.m_bFixed = bFixed;
	if( bFixed )
		layer.m_Lerp = 1.f;
	if( layer.m_psfcBar ) {
		auto sizeCurr = GetSizeLocal();
		auto sizeImg = layer.m_psfcBar->GetSize();
		// 이미지중 가장 큰 이미지를 기준으로 한다.
		sizeCurr.w = std::max( sizeImg.w, sizeCurr.w );
		sizeCurr.h = std::max( sizeImg.h, sizeCurr.h );
		SetSizeLocal( sizeCurr );
	}
	m_listLayers.Add( layer );
	auto itor = m_listLayers.end();
	--itor;
	return &(*itor);
}

const XWndProgressBar2::xLayer* XWndProgressBar2::GetpLayer( ID idLayer )
{
	return m_listLayers.FindByIDNonPtr( idLayer );
}

void XWndProgressBar2::SetShowLayer( ID idLayer, bool bShow )
{
	for( auto& layer : m_listLayers ) {
		if( layer.m_idLayer == idLayer ) {
				layer.m_bShow = bShow;
				return;
		}
	}
}

// XWndProgressBar2::xLayer* XWndProgressBar2::GetLayer( ID idLayer )
// {
// 	for( auto& layer : m_listLayers ) {
// 		if( layer.m_idLayer == idLayer ) {
// 				return &layer;
// 		}
// 	}
// }

int XWndProgressBar2::Process( float dt )
{
	if( m_bAnimation && m_timerAni.IsOn() ) {
		float lerp = m_timerAni.GetSlerp();
		if( lerp > 1.f )
			lerp = 1.f;
 		// 역으로 가는 애니메이션
//  		if( m_lvDstByAni < m_lvStartByAni )
//  			bForward = false;
		for( auto& layer : m_listLayers ) {
			if( !layer.m_bFixed && layer.m_LerpDst >= 0 ) {
				bool bForward = true;
				// 목표레벨이 시작레벨보다 작으면 역애니
				if( m_lvDstByAni < m_lvStartByAni ) {
					bForward = false;
				} else 
				if( m_lvDstByAni == m_lvStartByAni ) {
					// 같은레벨대에서 움직일때 목표값이 시작값보다 작으면 역애니
					if( layer.m_LerpDst < layer.m_LerpStart )
						bForward = false;
				}
				float lerpStart = layer.m_LerpStart;
				// 현재레벨(애니중)이 시작레벨보다 높아졌으면 그래프는 처음부터 애니메이션 된다.
				if( bForward ) {
					if( m_lvCurrByAni > m_lvStartByAni )
						lerpStart = 0.f;
				} else {
					if( m_lvCurrByAni < m_lvStartByAni )
						lerpStart = 1.f;
				}
				float lerpDst = layer.m_LerpDst;
				if( bForward ) {
					// 현재레벨이 아직 목표레벨이 아니면 그래프 애니메이션은 끝까지 된다.
					if( m_lvCurrByAni < m_lvDstByAni )
						lerpDst = 1.f;
				} else {
					// 역애니중 현재레벨이 아직 목표레벨이 아니면 그래프 애니메이션은 끝(0.f)까지 된다.
					if( m_lvCurrByAni > m_lvDstByAni )
						lerpDst = 0.f;
				}
				layer.m_Lerp = lerpStart + (lerpDst - lerpStart) * lerp;
				if( lerp >= 1.f ) {
					bool bArrive = false;
					if( bForward ) {
						if( m_lvCurrByAni < m_lvDstByAni ) 
							++m_lvCurrByAni;
						else
							bArrive = true;
					} else {
						if( m_lvCurrByAni > m_lvDstByAni )
							--m_lvCurrByAni;
						else
							bArrive = true;
					}
					if( !bArrive ) {
						if( m_pDelegate )
							m_pDelegate->OnDelegateChangeLevel( this, m_lvCurrByAni );
						if( bForward ) {
							SetShowLayer( 2, false );		// 예상경험치가 현재레벨을 넘어서면 메인bar는 감춘다.
							SetShowLayer( 5, true );
							float sec = m_timerAni.GetWaitSec() * 1.1f;		// 레벨이 증가할수록 점차 애니메이션이 느려지는 효과
							m_timerAni.Set( sec );
						} else {
							// 애니메이션이 최초 레벨까지 내려왔으면 메인bar를 다시 보이고 하고 더미바는 감춘다.
							if( m_lvCurrByAni <= m_lvOrig ) {
								SetShowLayer( 2, true );		// 
								SetShowLayer( 5, false );
							} else {
								SetShowLayer( 2, false );		// 예상경험치가 현재레벨을 넘어서면 메인bar는 감춘다.
								SetShowLayer( 5, true );
							}
							m_timerAni.Reset();
						}
					} else {
						m_timerAni.Off();
						m_lvDstByAni = 0;
						layer.m_LerpDst = -1.f;
						layer.m_LerpStart = 0.f;
					}
				}
			}
		}
	}
	for( auto& layer : m_listLayers ) {
		if( layer.m_psoTail )
			layer.m_psoTail->FrameMove( dt );
	}
	return XWnd::Process( dt );
}

void XWndProgressBar2::Draw()
{
	auto vPos = GetPosFinal();
	const auto vScale = GetScaleFinal();
	for( auto& layer : m_listLayers ) {
		if( layer.m_bShow ) {
			if( layer.m_psfcBar )
				layer.m_psfcBar->SetBlendFunc( layer.m_blendFunc );
			const auto v = vPos + layer.m_vAdj * vScale;
			if( m_bHoriz ) {
				if( layer.m_idTraceTail ) {
					// 바 끝부분 좌상귀의 좌표를 얻어서 찍음.
					auto vlTail = GetvlPosTail( layer.m_idTraceTail );
					if( layer.m_psfcBar ) {
						layer.m_psfcBar->SetFlipHoriz( layer.m_bFlipH );
						layer.m_psfcBar->SetFlipVert( layer.m_bFlipV );
						layer.m_psfcBar->SetScale( vScale );
						layer.m_psfcBar->Draw( v + vlTail * vScale);
					} else {
						layer.m_psoTail->SetFlipHoriz( layer.m_bFlipH );
						layer.m_psoTail->SetFlipVert( layer.m_bFlipV );
						layer.m_psoTail->SetScale( vScale );
						layer.m_psoTail->Draw( v + vlTail * vScale);
					}
				} else {
//					XUTIL::DrawBarHoriz( v, layer.m_Lerp, layer.m_psfcBar, m_bReverse );
					XUTIL::DrawBarHoriz2( v, layer.m_Lerp, layer.m_psfcBar, vScale, m_bReverse );
				}
			} else {
				XUTIL::DrawBarVert( v, layer.m_Lerp, layer.m_psfcBar, m_bReverse );
			}
		}
	}
	XWnd::Draw();
}

/**
 @brief idLayer바의 끝부분 로컬좌표를 얻는다.
*/
XE::VEC2 XWndProgressBar2::GetvlPosTail( ID idLayer )
{
	XE::VEC2 vl(0, 0);
	auto pLayer = GetpLayer( idLayer );
	if( pLayer ) {
		if( !m_bReverse )
			vl.x = pLayer->m_psfcBar->GetWidth() * pLayer->m_Lerp;
		else {
			vl.x = pLayer->m_psfcBar->GetWidth() * (1.f - pLayer->m_Lerp);
		}
	}
	return vl;
}

