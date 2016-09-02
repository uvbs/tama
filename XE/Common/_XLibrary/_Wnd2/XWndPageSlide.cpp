#include "stdafx.h"
#include "XWndPageSlide.h"
#include "XFramework/client/XEContent.h"
#include "XFramework/XEProfile.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//////////////////////////////////////////////////////////////////////////
XWndPageSlideWithXML::XWndPageSlideWithXML( const XE::VEC2& vPos, 
													const XE::VEC2& vSize, 
													XLayout* pLayout, 
													TiXmlElement *elemSlide,
													LPCTSTR szImgPoint )
	: XWnd( vPos.x, vPos.y, vSize.w, vSize.h )
{ 
	Init(); 
	m_pLayout = pLayout;
	m_elemSlide = elemSlide;
	//
	if( XE::IsHave( szImgPoint ) )
	{
		_tstring strFile = XE::GetFileTitle( szImgPoint );
		_tstring strExt = XE::GetFileExt( szImgPoint );
		strFile += _T("_on.");
		strFile += strExt;
		m_sfcPointOn = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, strFile.c_str() ) );
		//
		strFile = XE::GetFileTitle( szImgPoint );
		strExt = XE::GetFileExt( szImgPoint );
		strFile += _T("_off.");
		strFile += strExt;
		m_sfcPointOff = IMAGE_MNG->Load( TRUE, XE::MakePath( DIR_UI, strFile.c_str() ) );
	}
	//
	m_posPagePoint.y = vSize.h - 13.f;	// 디폴트 페이지포인트 위치
}

// idxBase페이지를 기준으로 다음페이지의 인덱스를 얻는다. 없으면 -1을 리턴한다.
int XWndPageSlideWithXML::GetNextPage( int idxBase )
{
//	if( idxBase == m_listPageKey.size() - 1 )
	if( idxBase == m_numPages - 1 )
		return -1;
	return idxBase + 1;
}
// idxBase페이지를 기준으로 이전페이지의 인덱스를 얻는다. 없으면 -1을 리턴한다.
int XWndPageSlideWithXML::GetPrevPage( int idxBase )
{
	if( idxBase == 0 )
		return -1;
	return idxBase - 1;
}

void XWndPageSlideWithXML::SetnumPages( int numPages )
{
	if( m_pRight && m_pRight->GetidxPage() >= numPages )
		m_pRight = NULL;
	m_numPages = numPages;
	// 포인트시작 위치 갱신
	float sizePagePoint = numPages * m_distPagePoint;
	XE::VEC2 vSize = GetSizeLocal();
	// 슬라이드내 로컬좌표
	m_posPagePoint.x = (vSize.w / 2.f) - (sizePagePoint / 2.f);
}

XWndPageForSlide* XWndPageSlideWithXML::GetRightPage( int idx )
{
	if( idx == m_pCurr->GetidxPage() )
		return m_pRight;
	int NextIdx = GetNextPage(idx);
	for( int i = 0; i < m_listRight.size(); i++ )
	{
		if( NextIdx == m_listRight.GetFromIndex(i)->GetidxPage() )
			return m_listRight.GetFromIndex(i);
	}
	return NULL;
}
XWndPageForSlide* XWndPageSlideWithXML::GetLeftPage( int idx )
{
	if( idx == m_pCurr->GetidxPage() )
		return m_pLeft;
	int PervIdx = GetPrevPage(idx);
	for( int i = 0; i < m_listLeft.size(); i++ )
	{	
		if( PervIdx == m_listLeft.GetFromIndex(i)->GetidxPage() )
			return m_listLeft.GetFromIndex(i);
	}
	return NULL;
}

/*
int XWndPageSlideWithXML::AddPageKey( const char *cKeyPage ) 
{
	m_listPageKey.Add( cKeyPage );
	int idx = m_listPageKey.size() - 1;
	// 페이지가 늘어날때마다 페이지 포인트시작 위치도 갱신
	float sizePagePoint = m_listPageKey.size() * m_distPagePoint;
	XE::VEC2 vSize = GetSizeLocal();
	// 슬라이드내 로컬좌표
	m_posPagePoint.x = (vSize.w / 2.f) - (sizePagePoint / 2.f);
	return idx;
}
*/
/**
 현재 페이지를 idxPage번호로 세팅하고 xml에 세팅된 컨트롤들을 모두 생성한다.
 그와 동시에 왼쪽 오른쪽 페이지의 컨트롤들도 생성한다.
*/
/*
void XWndPageSlideWithXML::_SetCurrPageForXML( int idxPage )
{
	XBREAK( idxPage < 0 || idxPage >= m_listPageKey.size() );
	if( m_pCurr )
	{
		if( m_pCurr->GetidxPage() == idxPage )		// 이미 같은페이지면 다시 만들필요 없다.
			return;
		XWnd::DestroyWnd( m_pCurr );
	}
	// 현재 페이지 생성
	m_pCurr = _CreatePageForXML( idxPage );
	Add( m_pCurr );
	// 왼쪽페이지 생성
	if( m_pLeft )
		XWnd::DestroyWnd( m_pLeft );
	int idxPrevPage = GetPrevPage( idxPage );
	if( idxPrevPage >= 0 )
	{
		m_pLeft = _CreatePageForXML( idxPrevPage );
		XE::VEC2 vPos( -XE::GetGameWidth(), 0 );
		m_pLeft->SetPosLocal( vPos );
		Add( m_pLeft );
	}
	// 오른족 페이지생성
	if( m_pRight )
		XWnd::DestroyWnd( m_pRight );
	int idxNextPage = GetNextPage( idxPage );
	if( idxNextPage >= 0 )
	{
		m_pRight = _CreatePageForXML( idxNextPage );
		XE::VEC2 vPos( XE::GetGameWidth(), 0 );
		m_pRight->SetPosLocal( vPos );
		Add( m_pRight );
	}
}
*/
// 현재 페이지를 지정한다.
void XWndPageSlideWithXML::SetCurrPage( int idxPage )
{
/*	if( m_listPageKey.size() > 0 )
	{
		_SetCurrPageForXML( idxPage );
		return;
	} */
	XBREAK( idxPage < 0 || idxPage >= m_numPages );
	if( m_pCurr )
	{
		if( m_pCurr->GetidxPage() == idxPage )		// 이미 같은페이지면 다시 만들필요 없다.
			return;
		XWnd::DestroyWnd( m_pCurr );
	}
	// 현재 페이지 생성
	m_pCurr = CreatePage( idxPage );
	Add(m_pCurr);
	// 왼쪽페이지 생성
	if( m_pLeft )
		XWnd::DestroyWnd( m_pLeft );
	int idxPrevPage = GetPrevPage( idxPage );
	if( idxPrevPage >= 0 )
	{
		m_pLeft = CreatePage( idxPrevPage );
		XE::VEC2 vPos( -XE::GetGameWidth(), 0 );
		m_pLeft->SetPosLocal( vPos );
		Add(m_pLeft);
	}
	// 오른족 페이지생성
	if( m_pRight )
		XWnd::DestroyWnd( m_pRight );
	int idxNextPage = GetNextPage( idxPage );
	if( idxNextPage >= 0 )
	{
		m_pRight = CreatePage( idxNextPage );
		XE::VEC2 vPos( XE::GetGameWidth(), 0 );
		m_pRight->SetPosLocal( vPos );
		Add(m_pRight);
	}
	//그리지 않는 페이지 생성
	m_listLeft.Destroy();
	m_listRight.Destroy();
	for( int i = idxPage+2; i < m_numPages; i++ )
	{
		XWndPageForSlide *pRight = CreatePage( i );
		if( m_pRight )
		{
			XE::VEC2 vPosRight = m_pRight->GetPosLocal();
			pRight->SetPosLocal(vPosRight);
		}
		m_listRight.push_back( pRight );
		Add(pRight);
	}
	for( int i = idxPage-2; i >= 0; i-- )
	{
		XWndPageForSlide *pLeft = CreatePage( i );
		if( m_pLeft )
		{
			XE::VEC2 vPosLeft = m_pLeft->GetPosLocal();
			pLeft->SetPosLocal(vPosLeft);
		}
		m_listLeft.push_back( pLeft );
		Add(pLeft);
	}
}
/*
XWndPageForSlide* XWndPageSlideWithXML::_CreatePageForXML( int idxPage )
{
	XBREAK( idxPage < 0 || idxPage >= m_listPageKey.size() );
	// 셋팅하려는 페이지의 키문자열을 구해서 xml node를 구한다.
	string strKeyPage = m_listPageKey.GetFromIndex( idxPage );
	TiXmlElement *elemPage = m_elemSlide->FirstChildElement( strKeyPage.c_str() );
	// 빈 페이지를 만들어 현재 페이지에 붙인다.
//	XE::VEC2 vPos = GetPosFinal();
	XE::VEC2 vSize = GetSizeFinal();
	
	XWndPageForSlide *pPage = new XWndPageForSlide( idxPage,  XE::VEC2(0), vSize );
	// 현재페이지에 xml에 세팅된 컨트롤들을 생성해서 붙인다.
	m_pLayout->CreateLayout( elemPage, pPage );
	//
	return pPage;
} */
XWndPageForSlide* XWndPageSlideWithXML::_CreatePageForXML( int idxPage )
{
	TiXmlElement *elemPage = m_elemSlide->FirstChildElement( "page" );
	int idx = 0;
	do
	{
		const char *cKeyPage = elemPage->Value();
		if( XE::IsSame( cKeyPage, "page" ) )
		{
			if( idx++ == idxPage )		// idxPage에 해당하는 노드를 순차검색한다.
				break;
		}
	} while(( elemPage = elemPage->NextSiblingElement() ));
	// 빈 페이지를 만들어 현재 페이지에 붙인다.
	XE::VEC2 vSize = GetSizeFinal();

	if( elemPage == NULL )
		return NULL;
	XWndPageForSlide *pPage = new XWndPageForSlide( idxPage,  XE::VEC2(0), vSize );
	// 현재페이지에 xml에 세팅된 컨트롤들을 생성해서 붙인다.
	m_pLayout->CreateLayout( elemPage, pPage );
	//
	return pPage;
}


XWndPageForSlide* XWndPageSlideWithXML::CreatePage( int idxPage )
{
	XBREAK( idxPage < 0 || idxPage >= m_numPages );
	// 디폴트 레이아웃이 없으면 개별지정된 레이아웃리스트로 생성.
	if( m_strPageLayout.empty() == true )
		return _CreatePageForXML( idxPage );
	//
	XE::VEC2 vSize = GetSizeFinal();
	XWndPageForSlide *pPage = NULL;

	// 디폴트 레이아웃을 슬라이드 하위노드에서 먼저 찾아본다.
	TiXmlElement *elemLayout = m_pLayout->GetElement( m_strPageLayout.c_str(), m_elemSlide );
	// 없으면 루트에서 찾아본다.
	if( elemLayout == NULL )
		elemLayout = m_pLayout->GetElement( m_strPageLayout.c_str() );
	if( elemLayout )
	{
		pPage = new XWndPageForSlide( idxPage,  XE::VEC2(0), vSize );
		// 현재페이지에 레이아웃에 세팅된 컨트롤들을 생성해서 붙인다.
		m_pLayout->CreateLayout( elemLayout, pPage );
		if( m_pDelegate )
			m_pDelegate->OnFinishCreatePageLayout( pPage );
	}

	return pPage;
}



void XWndPageSlideWithXML::OnLButtonDown( float lx, float ly )
{
	m_vPrev = XE::VEC2( lx, ly );
	if( m_AutoScrollDir == XE::xDIR_NONE )
	{
		m_vStartScroll = XE::VEC2( lx, ly );
		m_bTouchDown = TRUE;
		// 일단은 하위 윈도우에도 이벤트를 보낸다.
		XWnd::OnLButtonDown( lx, ly );		
	} else
		m_bTouchDown = FALSE;
}

void XWndPageSlideWithXML::OnLButtonDownCancel( void )
{
	m_bTouchDown = FALSE;
}

void XWndPageSlideWithXML::OnMouseMove( float lx, float ly )
{
	if( m_bLockSlide == FALSE )
	{
		if( m_AutoScrollDir == XE::xDIR_NONE && m_bTouchDown )
		{
			XE::VEC2 vDist = XE::VEC2( lx, ly ) - m_vPrev;	// 마우스 이동량

			// 가로로 더 많이 움직였다면.
			if( fabs(vDist.x) > fabs(vDist.y) && m_bScrollExclusive == FALSE )
			{
				m_bScrollExclusive = TRUE;
				// 자식들에게 터치다운이벤트 했던거 다 취소하라고 알려줌.
				SendChildLButtonDownCancel();
			}
			if( m_bScrollExclusive )
			{
				if( m_pCurr )
				{
					XE::VEC2 vPos = m_pCurr->GetPosLocal();
					vPos.x += vDist.x;
					m_pCurr->SetPosLocal( vPos );
				}
				if( m_pRight )
				{
					XE::VEC2 vPos = m_pRight->GetPosLocal();
					vPos.x += vDist.x;
					m_pRight->SetPosLocal( vPos );
				}
				if( m_pLeft )
				{
					XE::VEC2 vPos = m_pLeft->GetPosLocal();
					vPos.x += vDist.x;
					m_pLeft->SetPosLocal( vPos );
				}
				// 터치움직임을 슬라이드가 독점하고 자식들에게 던져주지 않는다.
			} else
				XWnd::OnMouseMove( lx, ly );
		} else
			XWnd::OnMouseMove( lx, ly );
		m_vPrev = XE::VEC2( lx, ly );
	}
}
void XWndPageSlideWithXML::OnLButtonUp( float lx, float ly )
{
	// 스크롤을 독점하고 있던 상황이면 자식들에게 이벤트를 넘겨주지 않음.
	if( m_bScrollExclusive == FALSE )
		XWnd::OnLButtonUp( lx, ly );
	m_bScrollExclusive = FALSE;

	if( m_AutoScrollDir == XE::xDIR_NONE && m_bTouchDown &&  m_pCurr )
	{
//		float wHalf = XE::GetGameWidth() / 2.f;
		float wHalf = XE::GetGameWidth() * 0.3f;	// 30%
		XE::VEC2 vPos = m_pCurr->GetPosLocal();
		// 현재페이지의 좌상귀x좌표가 왼쪽으로 화면의 절반을 넘어간상태면
		if( vPos.x < -wHalf )
		{
			if( m_pRight == NULL )
				DoAutoScroll( XE::xDIR_CENTER, vPos );		// 오른쪽화면이 더이상 없으므로 제자리로 돌아가기
			else
				DoAutoScroll( XE::xDIR_RIGHT, vPos );
		} else
		if( vPos.x > wHalf )
		{
			if( m_pLeft == NULL )
				DoAutoScroll( XE::xDIR_CENTER, vPos );		// 왼쪽화면이 더이상 없으므로 제자리로 돌아가기
			else
				DoAutoScroll( XE::xDIR_LEFT, vPos );
		} else
		{
			// 아니면 제자리로 돌아가야 한다.
			DoAutoScroll( XE::xDIR_CENTER, vPos );
		}
	}
	m_bTouchDown = FALSE;
}

void XWndPageSlideWithXML::DoAutoScroll( XE::xtDir dir, const XE::VEC2& vStart )
{
	m_AutoScrollDir = dir;
	m_vStartScroll = vStart;
}
#define xSEC_SLIDE		0.15f
int XWndPageSlideWithXML::Process( float dt )
{
	if( m_AutoScrollDir == XE::xDIR_RIGHT )
	{
		// ------->>>>
		// 오른쪽 화면이 나타나고 현재화면이 왼쪽으로 사라진다.
		float lerp = m_itpScroll.GetInterpolationValueWithSet( xSEC_SLIDE );
		XE::VEC2 vPos = m_pCurr->GetPosLocal();
		float wScreen = XE::GetGameWidth();
		vPos.x = m_vStartScroll.x + ( (-wScreen) - m_vStartScroll.x ) * lerp;
		m_pCurr->SetPosLocal( vPos );
		if( m_pRight )
		{
			XE::VEC2 vPosRight = m_pRight->GetPosLocal();
			vPosRight.x = vPos.x + wScreen;
			m_pRight->SetPosLocal( vPosRight );
		}
		if( m_itpScroll.IsFinish() )
		{
			m_itpScroll.Off();
			m_AutoScrollDir = XE::xDIR_NONE;
			// 현재 화면이 왼쪽으로 완전히 사라졌다.
			if( m_pLeft )
			{
				m_listLeft.push_back( m_pLeft );
				//XWnd::DestroyWnd( m_pLeft );		// 원래있던 왼쪽페이지를 삭제
			}
			m_pLeft = m_pCurr;
			m_pCurr = m_pRight;
			m_pRight = NULL;
			// 오른쪽 페이지 생성
			int idxRightPage = GetNextPage( m_pCurr->GetidxPage() );
			if( m_numPages > idxRightPage && idxRightPage >= 0 )
			{
				if( m_listRight.size() > 0 )
				{
					m_pRight = m_listRight.GetFirst();
					m_listRight.pop_front();
				}
				//m_pRight = CreatePage( idxRightPage );
				//if( m_pRight && !Find( m_pRight ) )
				//	Add( m_pRight );
				XE::VEC2 v = m_pCurr->GetPosLocal();
				v.x += wScreen;
				m_pRight->SetPosLocal( v );	// 화면 오른쪽에 놓는다.
			}
			CallEventHandler( XWM_FINISH_SLIDE, m_pCurr->GetidxPage() );
		}
	} else
	if( m_AutoScrollDir == XE::xDIR_LEFT )
	{
		// <<<<-----
		// 왼쪽 화면이 나타나고 현재화면이 오른쪽으로 사라진다.
		float lerp = m_itpScroll.GetInterpolationValueWithSet( xSEC_SLIDE );
		XE::VEC2 vPos = m_pCurr->GetPosLocal();
		float wScreen = XE::GetGameWidth();
		vPos.x = m_vStartScroll.x + ( wScreen - m_vStartScroll.x ) * lerp;
		m_pCurr->SetPosLocal( vPos );
		if( m_pLeft )
		{
			XE::VEC2 vPosLeft = m_pLeft->GetPosLocal();
			vPosLeft.x = vPos.x - wScreen;
			m_pLeft->SetPosLocal( vPosLeft );
		}
		if( m_itpScroll.IsFinish() )
		{
			m_itpScroll.Off();
			m_AutoScrollDir = XE::xDIR_NONE;
			// 현재 화면이 오른쪽으로 완전히 사라졌다.
			if( m_pRight )
			{
				m_listRight.push_front( m_pRight );
				//XWnd::DestroyWnd( m_pRight );		// 원래있던 오른쪽페이지를 삭제
			}
			m_pRight = m_pCurr;
			m_pCurr = m_pLeft;
			m_pLeft = NULL;
			// 왼쪽 페이지 생성
			int idxLeftPage = GetPrevPage( m_pCurr->GetidxPage() );
			if( m_numPages > idxLeftPage && idxLeftPage >= 0 )
			{
				if( m_listLeft.size() > 0 )
				{
					m_pLeft = m_listLeft.GetLast();
					m_listLeft.pop_back();
				}
				//m_pLeft = CreatePage( idxLeftPage );
				//if( m_pLeft && !Find( m_pLeft ) )
				//	Add( m_pLeft );
				XE::VEC2 v = m_pCurr->GetPosLocal();
				v.x -= wScreen;
				m_pLeft->SetPosLocal( v );		// 화면 왼쪽에 놓는다.
			}
			CallEventHandler( XWM_FINISH_SLIDE, m_pCurr->GetidxPage() );
		}
	} else
	if( m_AutoScrollDir == XE::xDIR_CENTER )
	{
		// 제자리로 돌아가기
		float lerp = m_itpScroll.GetInterpolationValueWithSet( xSEC_SLIDE );
		float wScreen = XE::GetGameWidth();
		XE::VEC2 vPos = m_pCurr->GetPosLocal();
		vPos.x = m_vStartScroll.x + ( 0 - m_vStartScroll.x ) * lerp;
		m_pCurr->SetPosLocal( vPos );
		if( m_pRight )
		{
			XE::VEC2 vPosRight = m_pRight->GetPosLocal();
			vPosRight.x = vPos.x + wScreen;
			m_pRight->SetPosLocal( vPosRight );
		}
		if( m_pLeft )
		{
			XE::VEC2 vPosLeft = m_pLeft->GetPosLocal();
			vPosLeft.x = vPos.x - wScreen;
			m_pLeft->SetPosLocal( vPosLeft );
		}
		if( m_itpScroll.IsFinish() )
		{
			m_itpScroll.Off();
			m_AutoScrollDir = XE::xDIR_NONE;
		}
	} 
//	else
	return XWnd::Process( dt );
//	return 1;
}

void XWndPageSlideWithXML::Draw( void )
{
	XPROF_OBJ_AUTO();
#ifdef _DEBUG
	PUT_STRINGF( 273, 186, XCOLOR_WHITE, "page:%d", m_pCurr->GetidxPage() );
#endif
	XWnd::Draw();
//	if( m_pRight )
//		m_pRight->Draw();
	// 페이지 포인트 그림
	XE::VEC2 vPos = GetPosFinal();
//	int numPages = m_listPageKey.size();
	XE::VEC2 posPoint = vPos + m_posPagePoint;
	for( int i = 0; i < m_numPages; ++i )
	{
		if( i == m_pCurr->GetidxPage() )
		{
			if( m_sfcPointOn )	
				m_sfcPointOn->Draw( posPoint );
		}
		else
		{
			if( m_sfcPointOff )
				m_sfcPointOff->Draw( posPoint );
		}
		posPoint.x += m_distPagePoint;
	}
}
