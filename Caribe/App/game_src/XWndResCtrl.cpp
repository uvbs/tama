#include "stdafx.h"
#include "XWndResCtrl.h"
#include "_Wnd2/XWndText.h"
#include "_Wnd2/XWndImage.h"
#include "_Wnd2/XWndCtrls.h"
#include "XFramework/client/XLayout.h"
#include "_Wnd2/XWndSprObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

////////////////////////////////////////////////////////////////
XWndResourceCtrl::XWndResourceCtrl( const XE::VEC2& vPos, XGAME::xtResource type, int num, XCOLOR col )
	: XWnd( vPos )
	, m_aryNumResPrev( XGAME::xRES_ALL )
{
	Init();
	AddRes( type, num, col );
	SetbUpdate( true );
}

void XWndResourceCtrl::Destroy()
{
}

void XWndResourceCtrl::SetColText( XCOLOR col )
{
	for( auto& res : m_listResource ) {
		res.col = col;
	}
}

void XWndResourceCtrl::AddRes( XGAME::xtResource type, int num, XCOLOR col ) 
{
	if( XGAME::IsInvalidResourceAll(type) || num < 0 ) {
		return;
	}
	xEach res;
	res.type = type;
	res.num = num;
	res.col = col;
	m_listResource.Add( res );
	m_aryNumResPrev[ type ] = num;		// 최초값
	UpdateWnd();  // 즉시 윈도우를 만들어 child wnd와 크기를 가질수 있게 한다.
}

void XWndResourceCtrl::AddRes( const XGAME::xRES_NUM& res, XCOLOR col )
{
	AddRes( res.type, res.num, col );
}

void XWndResourceCtrl::AddResWithAry( const XVector<xReward>& aryReward )
{
	for( auto& reward : aryReward ) {
		if( XASSERT(reward.IsTypeResource()) ) {
			AddRes( reward );
		}
	}
}

/**
 @brief 등록된 자원 리스트를 UI로 만든다.
 정렬규칙
   .자원라인 하나는 표준 컨트롤높이가 정해졌있다.
	 .자원아이콘의 크기가 각자 달라도 표준 컨트롤 높이의 중앙을 기준으로 정렬한다.
	 .텍스트도 그 중앙을 기준으로 세로 정렬한다.
	 .아이콘과 텍스트는 부모자식관계로 하지 않는다.
	 .아이콘spr은 모두 adj를 0을 기준으로 한다.
*/
void XWndResourceCtrl::UpdateWnd()
{
	auto vScale = GetScaleLocal();
	const XE::VEC2 sizeIcon( 14, 14 );		// 리소스를 스프라이트를 쓰기땜에 금화의 경우 반짝이는 애니메이션땜에 사이즈가 더 크게 나온다. 그래서 아이콘 크기를 고정시킴.
	const XE::VEC2 sizeLine(0, sizeIcon.h + (m_spaceY * vScale.y));		// 각 라인의 표준높이(가로는 사용안함)
	const float yCenter = sizeLine.h * 0.5f;		// 세로 중앙
	XE::VEC2 vResLineLT(sizeIcon.w * 0.5f, 0);		// 각 리소스라인의 기준 좌표
	//
	int idx = 0;
	for( auto& res : m_listResource ) {
		// 아이콘
		XE::VEC2 sizeLayout;		// 아이콘 텍스트 합친 레이아웃 크기
		{
			if( XBREAK( XGAME::IsInvalidResourceAll(res.type) ) ) {
				res.type = XGAME::xRES_WOOD;
				res.num = 1;
			}
			_tstring resIcon = XGAME::GetResourceSpr( res.type );
			if( resIcon.empty() )
				continue;
			auto pWndIcon = Findf( "wnd.%d", res.type );
			if( pWndIcon == nullptr ) {
				// 자원아이콘(spr)
// 				pWndIcon = new XWndSprObj( resIcon, 1, XE::VEC2( 0 ) );
// 				pWndIcon->SetstrIdentifierf( "spr.%d", res.type );
// 				Add( pWndIcon );
				pWndIcon = new XWndResSpr( res.type );
				pWndIcon->SetstrIdentifierf( "wnd.%d", res.type );
				Add( pWndIcon );
			}
			XBREAK( pWndIcon == nullptr );
			// 자원아이콘 위치를 보정함. 표준높이의 중앙을 기준으로 배치한다.
			XE::VEC2 vAdjIcon;		// 보정치
//			const auto sizeIcon = pWndIcon->GetSizeLocal();
//			vAdjIcon.y = yCenter - ( sizeIcon.h * 0.5f );
			const auto v = (vResLineLT + XE::VEC2(0,yCenter)) - (sizeIcon.h * 0.5f);
			pWndIcon->SetPosLocal( v );
		} // 아이콘
		// 자원양 텍스트
		{
			auto pWndText = xGET_TEXT_CTRLF( this, "text.%s", XGAME::GetIdsRes( res.type ) );
			if( pWndText == nullptr ) {
				// 컨트롤 생성
				const float sizeFont = 20.f;
				if( m_bCounter ) {
					auto pTextCnt = new XWndTextNumberCounter( XE::VEC2( 0 ),
																											res.num,
																											FONT_RESNUM, sizeFont, res.col );
					pTextCnt->SetbComma( TRUE );
					pTextCnt->SetbSymbol( true );
					pWndText = pTextCnt;
				} else {
					pWndText = new XWndTextString( XE::VEC2( 0 ),
																				_T( "" ),
																				FONT_RESNUM, sizeFont, res.col );
				}
				pWndText->SetStyleStroke();
				pWndText->SetstrIdentifierf( "text.%s", XGAME::GetIdsRes( res.type ) );
				Add( pWndText );
			}
			XBREAK( pWndText == nullptr );
			//
			const float xTextStart = 14.f;
			sizeLayout.w += xTextStart;
			if( pWndText ) {
				if( m_bCounter ) {
					UpdateTextCounter( SafeCast<XWndTextNumberCounter*>( pWndText ), res.type, res.num );
				} else {
					const _tstring strNum = ( res.num < 0 ) ?
																		_T( "???" )
																		: ( ( m_bSymbolNumber && res.num >= 1000000 ) ?
																			XE::NtSK( res.num )
																			: XE::NtS( res.num ) );
					pWndText->SetText( strNum );
				}
				pWndText->SetColorText( res.col );
				// 텍스트 좌표 보정
				const auto sizeText = pWndText->GetSizeNoTransLayout();
				const auto v = ( vResLineLT + XE::VEC2( xTextStart, yCenter ) ) - ( sizeText.h * 0.5f );
				pWndText->SetPosLocal( v );
// 				vAdj.y = yCenter - ( sizeText.h * 0.5f );
// 				pWndText->SetPosLocal( vResLineLT + vAdj );
				sizeLayout.w += sizeText.w * pWndText->GetScaleLocal().x;
			} // pWndText
		} // 텍스트
		// 다음 라인으로
		if( m_bVert ) {
			vResLineLT += sizeLine;
		} else {
			vResLineLT.x += (sizeLayout.w + 20.f);
		}
		++idx;
	} // for
	auto vSizeLocal = GetSizeNoTransLayout();
// 	vSizeLocal.h = std::min(vSizeLocal.h, 14.f);
	SetSizeLocal( vSizeLocal );
}
/**
 @brief typeRes리소스의 값을 카운팅 애니메이션이 되도록 업데이트한다. 값은 미리 입력
*/
void XWndResourceCtrl::UpdateTextCounter( XGAME::xtResource typeRes )
{
	if( !m_bCounter )
		return;
	for( auto res : m_listResource ) {
		if( res.type == typeRes || typeRes == xRES_NONE ) {
			const auto numPrev = m_aryNumResPrev[ res.type ];
			if( res.num != numPrev ) {		// 최적화를 위해서 먼저 비교하고 들어가도록 함.
				// 업데이트할때 이전값과 달라졌으면 카운터텍스트 갱신
				auto pTextCnt = SafeCast<XWndTextNumberCounter*>( Findf( "text.%s", XGAME::GetIdsRes( res.type ) ) );
				if( XASSERT( pTextCnt ) ) {
					UpdateTextCounter( pTextCnt, res.type, res.num );
				}
			}
		}
	}
}

void XWndResourceCtrl::UpdateTextCounter( XWndTextNumberCounter* pTextCnt
																				, XGAME::xtResource typeRes
																				, int numCurr )
{
	if( XBREAK( !m_bCounter ) )
		return;
	if( XBREAK( pTextCnt == nullptr ) )
		return;
	if( XGAME::IsInvalidResourceAll( typeRes ) ) {
		return;
	}
	const auto numPrev = m_aryNumResPrev[ typeRes ];
	if( numCurr != numPrev ) {
		// 이전과 값이 달라졌으면 카운팅 애니메이션을 한다.
		pTextCnt->SetnumMax( numCurr );		// 최대로 올라가야할 숫자
		pTextCnt->SetOn( numPrev );		// 애니메이션을 시작시키고 카운팅 초기값을 입력.
		m_aryNumResPrev[ typeRes ] = numCurr;		// 최신값으로 갱신.
	}
}
void XWndResourceCtrl::Update()
{
  UpdateWnd();
	XWnd::Update();
}

/**
 @brief 기 등록된 리소스의 개수를 바꿈.
 @param num 기존리스트의 자원수를 바꾼다. -1이면 기존값을 건드리지 않는다.
*/
void XWndResourceCtrl::EditRes( XGAME::xtResource type, int num, XCOLOR col )
{
	if( XGAME::IsInvalidResourceAll( type ) || num < 0 ) {
		return;
	}
	for( auto& res : m_listResource ) {
		if( res.type == type ) {
			if( num >= 0 ) {
				m_aryNumResPrev[ res.type ] = res.num;		// 이전값을 받아둠.
				res.num = num;
			}
			if( col )
				res.col = col;
			break;
		}
	}
	SetbUpdate( true );
}

void XWndResourceCtrl::EditResNum( XGAME::xtResource type, int num )
{
	if( XGAME::IsInvalidResourceAll( type ) || num < 0 ) {
		return;
	}
	for( auto& res : m_listResource ) {
		if( res.type == type ) {
			if( num >= 0 ) {
				m_aryNumResPrev[res.type] = res.num;		// 이전값을 받아둠.
				res.num = num;
			}
			break;
		}
	}
	SetbUpdate( true );
}

void XWndResourceCtrl::EditResColor( XGAME::xtResource type, XCOLOR col )
{
	if( XGAME::IsInvalidResourceAll( type ) ) {
		return;
	}
	for( auto& res : m_listResource ) {
		if( res.type == type ) {
			if( col )
				res.col = col;
			break;
		}
	}
	SetbUpdate( true );
}

void XWndResourceCtrl::Draw()
{
	XWnd::Draw();
}

////////////////////////////////////////////////////////////////
/**
 @brief 리소스 아이콘 래퍼
 리소스 아이콘이 spr이 되어서 레이아웃 크기를 구하면 제각각으로 나온다(빛나는 효과 때문)
 그래서 사이즈를 고정시키기 위해 별도의 래퍼를 만듬.
*/
XWndResSpr::XWndResSpr( XGAME::xtResource resType )
{
	Init();
	m_resType = resType;
	SetSizeLocal( 14.f, 14.f );
	_tstring resIcon = XGAME::GetResourceSpr( resType );
	if( !resIcon.empty() ) {
		auto pWndSpr = new XWndSprObj( resIcon, 1, XE::VEC2( 0 ) );
		Add( pWndSpr );
	}
}

