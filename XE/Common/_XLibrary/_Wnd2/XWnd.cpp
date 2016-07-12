#include "stdafx.h"
//#include "XWindow.h"
#include "etc/xUtil.h"
//#include "XWndMng.h"
//#include "sprite/SprObj.h"
#include "XFontSpr.h"
#include "XFramework/XEToolTip.h"		
#ifdef WIN32
#include "_DirectX/XGraphicsD3DTool.h"
#endif
//#include "XWindow.h"
#include "XImage.h"
#include "XFramework/client/XClientMain.h"
#include "XFramework/client/XEContent.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"
#include "XWndH.h"
#include "XWndView.h"
#include "XWndPopup.h"
using namespace XE;

#pragma message("SetEvent함수의 param2는 버튼에서 id를 전달하는 용도로 쓰이고 있으므로 param2를 건네지 못하게 막을것")

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

BOOL XWnd::s_bDebugMode = FALSE;
XWnd* XWnd::s_pFocusWindow = NULL;
//XWnd* XWnd::s_pDropWnd = NULL;
XWnd* XWnd::s_pDragWnd = NULL;
ID XWnd::s_idLDownWnd = 0;		// LDown한 윈도우의 아이디. LUp이 이뤄지려면 LDown이 되어야 한다.
XWnd* XWnd::s_pLastLUp = nullptr;		// 가장 마지막으로 LButtonUp이 호출된윈도우
#ifdef WIN32
XWnd* XWnd::s_pMouseOver = nullptr;		// 현재 마우스가 올라가있는 가장 위 윈도우
#if defined(_CHEAT) && defined(WIN32)
XVector<XWnd::xWinDepth> XWnd::s_aryMouseOver;		// 현재 마우스가 올라가있는 모든 윈도우의 포인터를 받는다.
#endif
int XWnd::s_depthMouseMove = 0;	// 마우스오버된 윈도우의 뎁스
bool XWnd::s_bDrawOutline = false;		// 디버깅 모드. 모든 윈도우들의 외곽선을 그린다.
bool XWnd::s_bDrawMouseOverWins = false;		// 디버깅 모드. 현재 마우스위치에 오버된 모든윈도우들의 리스트를 출력한다.
#endif // _DEBUG

XList<std::string>	XWnd::s_listAllowClick;		// 이게 세팅되어 있으면 이 윈도우들외에 다른 윈도우는 입력을 막아야 한다.
// SetCapture한 윈도우 리스트
XList<XWnd*>			XWnd::s_listCapture;
XList<XWnd*>			XWnd::s_listCaptureDel;
//////////////////////////////////////////////////////////////////////////
// static
// pCompare윈도우가 클릭해도 되는 윈도우인지 검사한다.
/// <summary>
/// Saves the person.
/// </summary>
/// <param name="person">Person.</param>
BOOL XWnd::IsAllowClickWnd( XWnd *pCompare )
{
	if( s_listAllowClick.size() == 0 )
		return TRUE;
	BOOL bFind = FALSE;
	XLIST_LOOP2( s_listAllowClick, std::string, strKey )
	{
		XWnd *pAllowClick = XE::GetMain()->GetpGame()->Find( (*strKey) );
		if( pAllowClick )
		{
			bFind = TRUE;
			if( pAllowClick->IsHierarchyWnd( pCompare ) )
				return TRUE;
			if( pAllowClick->Find( pCompare ) )		// 허용된 윈도우의 자식이라도 허용함.
				return TRUE;
		}
	} END_LOOP;
	if( bFind )
		return FALSE;
	// 입력을 허용해야할 윈도우가 없으면 모두다 클릭 허용
	return TRUE;
}
/*
// 다이나믹 해상도 적용으로 pWnd의 y좌표가 화면 윗쪽을 벗어났을때 화면안쪽으로 들어오도록 보정해준다.
void XWnd::sAdjustDynamicPos( XWnd *pWnd )
{
	XE::VEC2 vPos = pWnd->GetPosFinal();
	if( vPos.y < 0 )
	{
		XE::VEC2 vLocal = pWnd->GetPosLocal();
		XE::VEC2 vParent;
		if( pWnd->GetpParent() )
			vParent = pWnd->GetpParent()->GetPosFinal();	// final로 해야하나?
		vPos.y = fabs( vParent.y ) + vLocal.y;
		pWnd->SetPosLocal( vPos );
	}
}
*/
/*BOOL XWnd::IsAllowClickWnd( XWnd *pCompare )
{
	if( s_listAllowClick.size() == 0 )
		return TRUE;
	BOOL bFind = FALSE;
	XLIST_LOOP2( s_listAllowClick, ID, idAllow )
	{
		XWnd *pAllowClick = XE::GetMain()->GetpGame()->Find( idAllow );
		if( pAllowClick )
		{
			bFind = TRUE;
			if( pAllowClick->IsHierarchyWnd( pCompare ) )
				return TRUE;
			if( pAllowClick->Find( pCompare ) )		// 허용된 윈도우의 자식이라도 허용함.
				return TRUE;
		}
	} END_LOOP;
	if( bFind )
		return FALSE;
	// 입력을 허용해야할 윈도우가 없으면 모두다 클릭 허용
	return TRUE;
}*/

/**
 @brief 마우스 오버된 윈도우들을 스트링으로 만들어준다.
*/
#ifdef WIN32
XVector<XWnd::xWinDepth>& XWnd::sGetStringByMouseOverWins()
{
	for( auto& winOver : s_aryMouseOver ) {
// 		char cSpace[256];
		auto pWnd = winOver.m_pWnd;
 		winOver.m_str = XE::Format("-%s(0x%08x)", pWnd->GetstrIdentifier().c_str(), pWnd->getid() );
		// depth깊이만큼 스페이스를 앞에 붙인다.
// 		if( winOver.m_Depth > 0 )
// 			memset( cSpace, ' ', winOver.m_Depth );
// 		cSpace[ winOver.m_Depth + 1 ] = 0;
// 		(*pOut) += cSpace;
// 		(*pOut) += XE::Format("%s-%s(0x%08x)\n", pWnd->GetstrIdentifier().c_str(), pWnd->getid() );
	}
	return s_aryMouseOver;
}

XWnd* XWnd::sFindWinMouseOver( ID idWnd )
{
	for( auto& win : s_aryMouseOver ) {
		if( win.m_pWnd->getid() == idWnd ) 
			return win.m_pWnd;
	}
	return nullptr;
}
#endif // WIN32

//////////////////////////////////////////////////////////////////////////
XWnd::XWnd( XLayout *pLayout, const char *cNodeName )
{
	Init();
	pLayout->CreateLayout( cNodeName, this );
	const auto sizeLocal = GetSizeNoTransLayout();
	SetSizeLocal( sizeLocal );
}

/**
 @brief 부모로부터의 로컬 좌표를 얻는다.
 scale이나 rot등의 트랜스폼이 가해지지 않은 좌표이므로 만약 부모나 그 상위부모가 scale되어있다면 부모의 final scale
 을 곱해야 실제 화면상에서 부모로부터 떨어진 좌표를 얻을 수 있다.
*/
const XE::VEC2 XWnd::GetPosLocal() const
{
	return m_vPos + m_vAdjPos;
}

/**
 @brief this를 화면에 찍을때의 최종적인 화면 좌표를 얻는다.
 this의 스케일은 물론 부모의 scale까지 반영한값.
 @note 원래 여기서 vAdjDraw까지 같이 반영해야 맞지만 GetPosFinal()의 함수자체가 drawing용으로만 사용되지 않고
 픽킹등의 용도로도 사용되므로 일단 여기에 반영해선 안됨. 엔진구조가 수정되기전까진 개별 윈도우에서 draw할때 vAdjDraw를 반영해서 찍어야 한다.
 void XWndSome::Draw() {
	auto vScr = GetPosFinal() + GetvAdjFinal();
	pSurface->Draw( vScr );	// 최종적으로 화면에 그릴 좌표.
 }

*/
XE::VEC2 XWnd::GetPosFinal() const
{
	if( m_pParent ) {
		const auto posFinalParent = m_pParent->GetPosFinal();
		const auto posLocal = GetPosLocal();
		const auto scaleLocalParent = m_pParent->GetScaleLocal();
		return posFinalParent + posLocal * scaleLocalParent; // child일때만 좌표까지 스케일해주고. 부모의 스케일과 내 스케일을 곱해준걸 써야한다 
// 		return m_pParent->GetPosFinal() + GetPosLocal() * m_pParent->GetScaleLocal(); // child일때만 좌표까지 스케일해주고. 부모의 스케일과 내 스케일을 곱해준걸 써야한다 
	} else
		return GetPosLocal();	
}	// 값을 곱하는게 있어서 레퍼런스로 못넘기는데 만약 이게 속도문제가 된다면 WndMng.CalcTransform()정도를 만들어서 Final을 한번에 계산해놓고 쓰기만 해야할듯
// 뷰 좌상귀의 화면좌표를 얻는다. 스크롤뷰의 경우 스크롤이 아래로 내려가있는상태라면(m_vAdjust < 0) 뷰는 창 위쪽을 벗어나서 위쪽에 있을것이므로 그 곳의 화면좌표를 얻는다./
XE::VEC2 XWnd::GetPosScreen() 
{
	if( m_pParent ) {
		return m_pParent->GetPosScreen() + GetPosLocal() * m_pParent->GetScaleLocal(); // child일때만 좌표까지 스케일해주고. 부모의 스케일과 내 스케일을 곱해준걸 써야한다 
	} else
		return GetPosLocal();	
}	
// 창의 좌상귀의 화면좌표를 얻는다. 스크롤뷰의 경우 스크롤 상태(m_vAdjust)와 관계없이 뚫린곳(창)의 좌상귀 좌표를 리턴한다.
XE::VEC2 XWnd::GetPosWindow() 
{
	auto p = m_pParent;
	if( p ) {
		const auto vPosParent = p->GetPosWindow();
		const auto vPos = GetPosLocal() + p->GetPosAdjust();
		return vPosParent + vPos/* * vScale*/;
	}	else
		return GetPosLocal();
}
/**
 @brief 윈도우 자체의 좌표와는 별도로 단지 drawing시에만 보정할 좌표를 얻는다.
 this가 scale되어있다면 그것까지 반영해야한다.
*/
const XE::VEC2 XWnd::GetvAdjDrawLocal() const 
{
	return m_vAdjDraw * GetScaleLocal();
}
/**
 @brief 부모의 스케일까지 반영한 drawing보정 좌표
*/
const XE::VEC2 XWnd::GetvAdjDrawFinal() const
{
	if( m_pParent ) {
		const auto vAdjParentFinal = m_pParent->GetvAdjDrawFinal();
		const auto scaleParent = m_pParent->GetScaleLocal();
		const auto vAdjLocal = GetvAdjDrawLocal();
		const auto vFinal = vAdjParentFinal + vAdjLocal * scaleParent;
		return vFinal;
	} else {
		return GetvAdjDrawLocal();
	}
}

void XWnd::SetScaleLocal( float sx, float sy ) 
{ 
	m_vScale.Set( sx, sy );
}
void XWnd::SetRotateLocal( float dRotZ )
{
	m_dRotateZ = dRotZ;
}

void XWnd::SetSizeLocal( const XE::VEC2& vSize )
{ 
	m_vSize.x = vSize.x; m_vSize.y = vSize.y; 
	// 디버그모드일땐 부모의 사이즈가 0인지 체크한다.
	// 자식의 사이즈가 정해졌는데 부모의 사이즈가 0이면 뭔가 이상한것이라 판단한다.
	if( m_vSize.IsZero() == FALSE )
		SetbTouchable( TRUE );
	else
		SetbTouchable( FALSE );
}

void XWnd::SetWidth( float w )
{
	m_vSize.w = w;
	SetbTouchable( !(w == 0) );		// 가로사이즈가 없으면 어차피 터치 안됨
}

void XWnd::SetHeight( float h )
{
	m_vSize.h = h;
	SetbTouchable( !(h == 0) );		// 세로사이즈가 없으면 어차피 터치 안됨
}

XBaseFontDat *XWnd::s_pFontDat = NULL;

void XWnd::Destroy() 
{ 
// 	std::list<XWnd*>::iterator iter;
	int size = m_listItems.size();
	for( auto iter = m_listItems.begin(); iter != m_listItems.end(); iter++ ) {
		XWnd *pWnd = (*iter);
		if( pWnd->GetNumChild() )
			pWnd->Destroy();
#ifdef WIN32
		if( XWnd::s_pMouseOver && XWnd::s_pMouseOver->getid() == pWnd->getid() )
			XWnd::s_pMouseOver = nullptr;
#endif // WIN32
		pWnd->Release();
		SAFE_DELETE( pWnd );
	}
	m_listItems.clear();
}

void XWnd::DestroyChildAll()
{
//	std::list<XWnd*>::iterator iter;
	int size = m_listItems.size();
	for( auto iter = m_listItems.begin(); iter != m_listItems.end(); iter++ ) {
		XWnd *pWnd = (*iter);
		pWnd->SetbDestroy( TRUE );
	}
}

XWnd *XWnd::Add( int id, XWnd *pChild, bool bFront ) 
{
	AddOnly( id, pChild );
	//
	pChild->OnCreate();
	// add되면 최초한번 업데이트는 무조건 불러준다.
	pChild->SetbUpdate( false );
	pChild->Update();
	// Add를 하면서 Update를 최초 한번 하고 시작하는 방식이므로 일반적으로 이때 대부분의 파일들은 로딩이 끝났을거라고 가정함.
	pChild->CallEventHandler( XWM_FINISH_LOADED );
	return pChild;
}

/**
 @brief 리스트에 추가만 하고 OnCreate/Update등의 이벤트를 호출하지 않는다.
*/
XWnd *XWnd::AddOnly( int id, XWnd *pChild, bool bFront ) 
{
	XBREAK( id == 0 );
	XBREAK( pChild->GetID() && pChild->GetID() != id );
	XBREAK( m_id && pChild->getid() == m_id );
	pChild->SetID( id );
	pChild->m_pParent = this;
	// 중복된게 있는지 검사.
	XBREAK( Find( pChild ) );
#ifdef _DEBUG
	const std::string strIds = pChild->GetstrIdentifier();
	if( strIds.empty() == false && pChild->GetDestroy() == FALSE )	{
		// 중복된 identifier의 윈도우가 이미 있다.
		auto pExist = Find( strIds );
		XBREAK( pExist );
	}
#endif // debug
	if( bFront )
		m_listItems.push_front( pChild );
	else
		m_listItems.push_back( pChild );
#ifdef WIN32
	pChild->SetnDepth( m_nDepth + 1 );
#endif
	return pChild;
}

// this의 최상위윈도우에 pWnd를 추가시킨다.
XWnd* XWnd::AddWndTop( ID idWnd, XWnd *pWnd )
{
	XWnd *pTop = GetWndTop();
	pTop->Add( idWnd, pWnd );
	return pWnd;
}

XWnd* XWnd::Insert( const char *cBaseKey, XWnd *pChild )
{
	pChild->SetID( XE::GenerateID() );
	pChild->m_pParent = this;
	// 중복된게 있는지 검사.
	XBREAK( Find( pChild ) );
	auto itor = m_listItems.begin();
	for( ; itor != m_listItems.end(); ++itor ) {
		XWnd *pWnd = (*itor);
		if( pWnd->GetstrIdentifier() == cBaseKey )
			break;
	}
	if( itor == m_listItems.end() )
		m_listItems.push_back( pChild );
	else
		m_listItems.insert( itor, pChild );
	return pChild;
}
/**
 @brief idBase를 윈도id로 갖는 윈도우 앞에 child를 삽입시킨다.
*/
XWnd* XWnd::Insert( ID idBase, XWnd *pChild )
{
	pChild->SetID( XE::GenerateID() );
	pChild->m_pParent = this;
	// 중복된게 있는지 검사.
	XBREAK( Find( pChild ) );
	auto itor = m_listItems.begin();
	for( ; itor != m_listItems.end(); ++itor ) {
		XWnd *pWnd = ( *itor );
		if( pWnd->getid() == idBase )
			break;
	}
	m_listItems.insert( itor, pChild );
	return pChild;
}

XWnd* XWnd::Find( ID id )
{
//	if( XBREAK( id == 0 ) )		// 씬생성자에서 Find를 쓰면 씬id가 0이어서 이 기능은 뺌
//		return NULL;
	if( IsDestroy() )
		return NULL;
	if( id == GetID() )
		return this;
	for( auto pWnd : m_listItems ) {
		if( pWnd->IsDestroy() )	// 파괴된건 찾지 않음.
			continue;
		XWnd *pFind = pWnd->Find( id );
		if( pFind )	// child에서도 검사
			return pFind;
	}
	return nullptr;
}
//
XWnd* XWnd::Find( const char *cIdentifier ) const
{
	if( this == nullptr )
		return nullptr;
	if( XE::IsEmpty(cIdentifier) )		// 편의상 Find호출전 if를 쓰지 않게 하기 위해 그냥 리턴으로 바꿈.
		return nullptr;
//	XBREAK( XE::IsEmpty( cIdentifier ) == TRUE );
	/*
	 자기자신이 파괴된건 걸르지 않게 바뀜.
	 Update()등에서 SetbDestroy(1)했을때 그밑에 Find()가 있다면 실패해버림.
	*/
// 	if( GetDestroyFinal() )
// 		return NULL;
	for( auto pWnd : m_listItems )	{
//		if( pWnd->GetDestroyFinal() )	// 파괴된건 찾지 않음.
		if( pWnd->IsDestroy() )
			continue;
		// 여기서 pWnd가 깨졌다면 ::Destroy()에서 Find()를 호출했을 가능성 있음. OnDestroy 를 사용할것.
		if( XE::IsSame( pWnd->GetszIdentifier(), cIdentifier ) )	
			return pWnd;
		XWnd *pChild;
		if(( pChild = pWnd->Find( cIdentifier ) ))	// child에서도 검사
			return pChild;
	}
	return NULL;
}

XWnd* XWnd::Findf( const char *cIdentifierFormat, ... ) const
{
	char cIdentifier[ 256 ];
	va_list vl;
	va_start( vl, cIdentifierFormat );
	vsprintf_s( cIdentifier, cIdentifierFormat, vl );
	va_end( vl );
	return Find( cIdentifier );
}

/**
 @brief strIdsPrefix ids로 시작하는 윈도우를 찾음.
*/
XWnd* XWnd::FindWithPrefix( const std::string& strIdsPrefix ) const
{
	if( this == nullptr )
		return nullptr;
	if( XBREAK( strIdsPrefix.empty() ) )
		return nullptr;
	/*
	 자기자신이 파괴된건 걸르지 않게 바뀜.
	 Update()등에서 SetbDestroy(1)했을때 그밑에 Find()가 있다면 실패해버림.
	*/
	for( auto pWnd : m_listItems ) {
		if( pWnd->GetDestroyFinal() )	// 파괴된건 찾지 않음.
			continue;
		// 여기서 pWnd가 깨졌다면 ::Destroy()에서 Find()를 호출했을 가능성 있음. OnDestroy 를 사용할것.
// 		if( XE::IsSame( pWnd->GetszIdentifier(), cIdentifier ) )	
		const std::string strHeader = pWnd->GetstrIdentifier().substr(0, 9);
		if( strHeader == strIdsPrefix )
			return pWnd;
		auto pChild = pWnd->FindWithPrefix( strIdsPrefix );
		if( pChild )	// child에서도 검사
			return pChild;
	}
	return NULL;
}
//
XWnd* XWnd::Find( XWnd *_pWnd ) const
{
	if( _pWnd == NULL )
		return NULL;
	if( GetDestroyFinal() )
		return NULL;
	for( auto pWnd : m_listItems ) {
		if( pWnd->GetDestroyFinal() )	// 파괴된건 찾지 않음.
			continue;
		if( pWnd->Find( _pWnd ) )	// child에서도 검사
			return _pWnd;
		if( _pWnd == pWnd )
			return pWnd;
		if( _pWnd->getid() == pWnd->getid() )
			return pWnd;
	} //END_LOOP;
	return NULL;
}

int XWnd::GetListWnds( XArrayLinearN<XWnd*, 256> *pOut )
{
	for( auto pWnd : m_listItems ) {
		pOut->Add( pWnd );			
	}
	return pOut->size();
}

/**
 @brief 
*/
int XWnd::Process( float dt ) 
{
	if( GetDestroyFinal() )
		return 0;
	if( GetbProcess() == FALSE )
		return 0;
	// 생성/삭제시에 애니메이션이 있으면 처리하고 애니메이션을 수행하는 중이면 아래 process()들은 실행 안함.
	{
		const bool bAnimation = IsAnimation();
		if( ProcessEnterLeaveAnimation( dt ) == TRUE )
			return 0;
		// 보통
//		XBREAK( m_bUpdate && ( m_wtWnd != XE::WT_VIEW && m_wtWnd != XE::WT_POPUP ) );	// 이런경우가 있으면 아래처럼 업데이트 하면 안됨.
		if( bAnimation && !IsAnimation() ) {
			// 방금 애니메이션이 끝난경우. 업데이트가 있었다면 바로 업데이트 하고 시작.
			if( m_bUpdate ) {
				m_bUpdate = false;
				Update();
			}
		}
	}
	XBREAK( m_vScale.IsZero() );
	if( m_timerPush.IsOver(0.15f) ) {	// 누른지 0.2초가 지났으면		
		if( m_prefToolTip == NULL )
			m_prefToolTip = CreateToolTip();		// virtual
		if( m_prefToolTip )	{
			XE::VEC2 vPos = GetPosFinal();
			XE::VEC2 vTT( vPos.x, vPos.y - GetprefToolTip()->GetSize().h );
			m_prefToolTip->SetvPos( vTT );
			m_bToolTip = TRUE;
			TOOLTIP = m_prefToolTip;
			m_prefToolTip->Update(); 
		}
		m_timerPush.Off();
	}
//	LIST_MANUAL_LOOP( m_listItems, XWnd*, itor, pWnd ) {
	
	for( auto itor = m_listItems.begin(); itor != m_listItems.end(); ) {
		XWnd* pWnd = (*itor);	
		if( pWnd->GetDestroyFinal() != TRUE && pWnd->GetbProcess() ) {
			if( pWnd->GetbUpdate() ) {		// 업데이트 요청이있었으면 Update()를 호출한다.
				if( !pWnd->IsDestroy() && !pWnd->IsAnimation() ) {
					pWnd->SetbUpdate( FALSE );
					pWnd->Update();
				}
			}
			if( pWnd->m_timerAutoUpdate.IsOver() ) {
				// Update()에서 컨트롤들을 삭제하고 다시 만드는 경우가 있어 update후에 해야 한다.
				pWnd->OnAutoUpdate();		// 이벤트 핸들러
				pWnd->m_timerAutoUpdate.Reset();
			}
			pWnd->Process(dt);
		}
		if( pWnd->GetDestroyFinal() ) {
			if( pWnd->CallEventHandler( XWM_DESTROY ) )	{	// 파괴 이벤트 핸들러가 있다면 호출
				pWnd->OnDestroy();
				pWnd->Release();
				ID idDestroy = pWnd->GetID();
				XWnd *pDestroyAfterWnd = pWnd->GetpAfterDestroyEvent();
				pWnd->PrivateDestroyChild();
				pWnd->ReleaseCapture();		// 만약 캡쳐가 되어있다면 리스트에서도 빼줌
				const auto idWnd = pWnd->getid();
#ifdef WIN32
				if( XWnd::s_pMouseOver && XWnd::s_pMouseOver->getid() == pWnd->getid() )
					XWnd::s_pMouseOver = nullptr;
				if( XWnd::s_bDrawMouseOverWins && sFindWinMouseOver( idWnd ) )
					XWnd::s_aryMouseOver.clear();
#endif // WIN32
				s_listCapture.DelByID( pWnd->getid() );
				const auto strIds = pWnd->GetstrIdentifier();	// 파괴되기전에 아이디와 식별자를 받아둠.
				SAFE_DELETE( pWnd );
				m_listItems.erase( itor++ );
// 				// 자식 윈도우가 파괴되고 난 직후 부모윈도우에게 이벤트가 발생한다.
// 				OnDestroyAfterByChild( idWnd, strIds );
				// 파괴된 직후에 이벤트 발생시킬 윈도우가 있으면 발생시킴
				if( pDestroyAfterWnd )
					pDestroyAfterWnd->OnDestroyAfterByWnd( idWnd, strIds );
//					pDestroyAfterWnd->CallEventHandler( XWM_DESTROY_AFTER, idDestroy );	// 이거 사용하지 말고 위에걸로 쓸것.
			} else	{
				++itor;
				pWnd->SetbDestroy( FALSE );
			}
		} else
			++itor;
	} /// for m_listItems
	return 0;
}

/**
 윈도우 전체에게 업데이트 명령을 내린다.
*/
void XWnd::Update() 
{
	if( GetDestroyFinal() )
		return;
	if( GetbProcess() == FALSE )
		return;
	// 컨트롤이 애니메이션중이면 자동정렬 하지 않음.
	if( m_Align && IsAbleAlign() && !IsAnimation() ) {
		AutoLayoutByAlign( m_Align );
	}
	for( auto pWnd : m_listItems )	{
		if( pWnd->GetDestroy() == FALSE )
			if( !pWnd->IsAnimation() && pWnd->GetbShow() ) {
				pWnd->SetbUpdate( false );		// 상위부모의 업데이트로 인해 하위윈도우들이 중복으로 업데이트하는걸 방지.
				pWnd->Update();
			}
	}
}

// private전용 자식 삭제
// 모든 윈도우는 자신이 SAFE_DELETE되기 전에 이것을 먼저 불러야 한다.
// virturl 파괴자로 인해 부모의 리소스가 먼저삭제되고 자식들의 리소스가 나중에 파괴되어
// 생성순서와 파괴순서가 바뀌는 문제를 해결하려 했다.
void XWnd::PrivateDestroyChild()
{
    for( auto pChild : m_listItems ) {
        pChild->PrivateDestroyChild();
#ifdef WIN32
		if( XWnd::s_pMouseOver && XWnd::s_pMouseOver->getid() == pChild->getid() )
			XWnd::s_pMouseOver = nullptr;
#endif // WIN32
        SAFE_DELETE( pChild );
    }
    m_listItems.clear();
    int size = m_listItems.size();
    size = 1;
}

// 윈도우의 등장/퇴장 애니메이션
BOOL XWnd::ProcessEnterLeaveAnimation( float dt )
{

	if( m_bAnimationEnterLeave ) {
		if( m_stateAppear == xAP_NONE ) {
			m_stateAppear = xAP_ENTER_INIT;		// 나타나기 시작
		}
	}
	if( m_stateAppear )
	{
		if( m_stateAppear == xAP_ENTER_INIT ) {
			if( OnInitEnterLeaveAnimation( TRUE ) == FALSE ) {
				m_stateAppear = xAP_NONE;
				SetbAnimationEnterLeave( FALSE, 0 );
				return FALSE;
			}
			m_timerAppear.Set( m_secAppear );
			m_stateAppear = xAP_ENTER_ING;
			SetbActive( FALSE );		// 애니메이션 중엔 비활성 시킴
		} else
		if( m_stateAppear == xAP_LEAVE_INIT ) {
			if( OnInitEnterLeaveAnimation( FALSE ) == FALSE ) {
				m_stateAppear = xAP_LEAVE_END;
				OnFinishEnterLeave( FALSE );
				SetDestroyFinal();
				SetbActive( TRUE );
				m_timerAppear.Off();
				return FALSE;
			}
			m_timerAppear.Set( m_secAppear );
			m_stateAppear = xAP_LEAVE_ING;
			SetbActive( FALSE );		// 애니메이션 중엔 비활성 시킴
		}
		if( m_timerAppear.IsOn() ) {
			BOOL bEnter = IsEnterAnimation();
			OnProcessEnterLeaveAnimation( bEnter, dt );
			if( m_timerAppear.IsOver() ) {
				if( m_stateAppear == xAP_ENTER_ING ) {
					OnFinishEnterLeave( TRUE );
					OnFinishAppear();
					m_stateAppear = xAP_ENTER_END;
				} else {
					m_stateAppear = xAP_LEAVE_END;
					OnFinishEnterLeave( FALSE );
					SetDestroyFinal();
				}

				SetbActive( TRUE );
				m_timerAppear.Off();
				return FALSE;
			}
			return TRUE;	// 애니메이션 중이다.
		}
	}
	return FALSE;
}

// 등장/퇴장 애니메이션의 처리를 담당한다. 0을 리턴하면 중단시킨다.
// XWnd의 기본처리는 보간이다.
int XWnd::OnProcessEnterLeaveAnimation( BOOL bEnter, float dt )
{
	float timeLerp = m_timerAppear.GetSlerp();
	if( timeLerp > 1.0f )
		timeLerp = 1.0f;
	float lerp = XE::xiHigherPowerDeAccel( timeLerp, 1.0f, 0 );
	if( m_stateAppear == xAP_LEAVE_ING )
		lerp = 1.0f - lerp;
//	BOOL bEnter = (m_stateAppear == xAP_ENTER_ING )? TRUE : FALSE;
	ProcessAnimationLerp( bEnter, timeLerp, lerp );		// virtual
	return 1;
}

// XWnd에서 계산된 보간값을 받아서 적절한 처리만 하면 된다.
// aniLerp: bEnter가 true이면 0->1.0로 변화하고 false이면 1.0->0으로 변화한다.
void XWnd::ProcessAnimationLerp( BOOL bEnter, float timeLerp, float aniLerp )
{
	SetAlphaLocal( aniLerp );
}

/**
 @brief 
*/
void XWnd::Draw() 
{
#ifdef _XWIN_DEBUG
	TCHAR szSpace[256];
	szSpace[0] = 0;
	int i;
	for( i = 0; i < m_nDepth; ++i )
		szSpace[i] = '+';
	szSpace[i] = 0;
	if( s_bDebugMode )
		CONSOLE("%sctrl=%s key=%s child=%d show=%d", szSpace, m_strDebug.c_str(), C2SZ(GetstrIdentifier().c_str()), GetNumChild(), (int)GetbShow() );
	szSpace[i] = '+';	// 자식들을 위해 하나 더 추가한다.
	szSpace[i+1] = 0;
#endif
	if( !GetbShow())	return;	// hide상태에선 본인과 자식들모두 감춤
	int idx = 0;
	for( auto pWnd : m_listItems ) {
//		pWnd->SetnDepth( m_nDepth + 1 );
#ifdef _XWIN_DEBUG
		if( s_bDebugMode )
			CONSOLE("%schild %d: ctrl=%s key=%s child=%d", 
						szSpace,
						idx, 
						pWnd->GetstrDebug().c_str(), 
						C2SZ(pWnd->GetstrIdentifier().c_str()), 
						pWnd->GetNumChild() );
#endif // _XWIN_DEBUG
		if( pWnd->GetbShow() && !pWnd->GetDestroyFinal() ) {
			// 크기가 없는 윈도우는 클리핑 처리 안함.
			if( !pWnd->GetSizeLocal().IsMinus() && !pWnd->GetSizeLocal().IsZero() )	{
				// 윈도우의 바운딩박스를 구함.
				const XE::xRECT rBB = pWnd->GetBoundBoxByVisibleFinal();
				if( rBB.vLT.x > (float)GRAPHICS->GetViewportSize().w )
					continue;
				if( rBB.vLT.y > (float)GRAPHICS->GetViewportSize().h )
					continue;
				if( rBB.vRB.x < 0 )
					continue;
				if( rBB.vRB.y < 0 )
					continue;  
			}
#ifdef _XWIN_DEBUG
			if( s_bDebugMode )
				CONSOLE("%schild draw%d", szSpace, idx);
#endif
			if( pWnd->GetpDelegate() )
				pWnd->GetpDelegate()->DelegateBeforeDraw( pWnd );
			pWnd->Draw();
#ifdef WIN32
			if( XWnd::s_bDrawOutline ) {
				const auto vPos = pWnd->GetPosFinal();
				const auto sizeFinal = pWnd->GetSizeFinal();
				GRAPHICS->DrawRectSize( vPos, sizeFinal, XCOLOR_RED );
			}
#endif // WIN32
			++idx;
		}
	}/* END_LOOP;*/
	if( !m_pParent )	{
		// drag & drop 객체
		if( s_pDragWnd ) {
			s_pDragWnd->DrawDrag( INPUTMNG->GetMousePos() );
		}
	}
}

/**
 @brief 
*/
void XWnd::Draw( const XE::VEC2& vParent ) 
{
	XE::VEC2 vViewportLT = XE::VEC2(0);;
	XE::VEC2 vViewportRB = GRAPHICS->GetViewportSize();

	if( GetbShow() == FALSE )	return;	// hide상태에선 본인과 자식들모두 감춤
	for( auto pWnd : m_listItems ) {
		if( pWnd->GetbShow() && pWnd->GetDestroyFinal() == FALSE ) {
			const XE::VEC2 vPos = vParent + pWnd->GetPosLocal() * this->GetScaleLocal();
			const XE::VEC2 vSize = pWnd->GetSizeFinal();
			if( vPos.x > vViewportRB.x )
				continue;
			if( vPos.y > vViewportRB.y )
				continue;
			if( vPos.x + vSize.w < vViewportLT.x )
				continue;
			if( vPos.y + vSize.h < vViewportLT.y )
				continue;
			// 모달타입의 경우 그리기 전에 배경을 어둡게 해준다.
			if( pWnd->GetbModal() )
				GRAPHICS->FillRectSize( XE::VEC2(0), XGAME_SIZE, XCOLOR_RGBA( 0, 0, 0, 200 ) );
			pWnd->Draw( vPos );
		}
	}
// 	if( m_pModalDlg ) {		// 모달다이얼로그는 최상위에 찍힌다
// 		m_pModalDlg->Draw();
// 	}
}

void XWnd::GenerateLoopEvent()
{
	XWND_MESSAGE_MAP msgMap = FindMsgMap( XWM_LOOP_EVENT );
	if( msgMap.pOwner && msgMap.pHandler )	{
		for( auto pWnd : m_listItems ) {
			(msgMap.pOwner->*msgMap.pHandler)( pWnd, msgMap.param, 0 );		// 롤러가 돌아가다 멈추면 이벤트가 발생하며 멈춘아이템의 인덱스를 건넨다
		}
	}
}

int XWnd::CallEventHandler( ID msg, DWORD dwParam2 )
{
	int ret = 1;
	XWND_MESSAGE_MAP msgMap = FindMsgMap( msg );
	if( msgMap.pOwner && msgMap.pHandler ) {
		// 크기가 0인 윈도우에 클릭 이벤트를 걸수는 없다.
		if( msg == XWM_CLICKED ) {
 			if( XBREAK( m_vSize.IsInvalid() ) )
 				return 0;
		}
		ret = (msgMap.pOwner->*msgMap.pHandler)( this, msgMap.param, dwParam2 );		
	}
	
	for( auto& callback : m_listCallback ) {
		if( callback.m_idEvent == msg ) {
			callback.m_funcCallback( this );		// 콜백 함수 호출
		}
	}
// 	for( auto itor = m_listCallback.begin(); itor != m_listCallback.end(); ) {
// 		xCallback& callback = (*itor);
// 		if( callback.m_idEvent == msg ) {
// 			callback.m_funcCallback( this );		// 콜백 함수 호출
// 			m_listCallback.erase( itor++ );
// 		} else {
// 			++itor;
// 		}
// 	}

	return ret;
}

void XWnd::SetEvent2( ID idEvent, std::function<void(XWnd*)> func )
{
	m_listCallback.Add( xCallback( idEvent, func ) );
}

const xnWnd::xClickEvent& XWnd::FindClickEvent( const std::string& strKey ) const
{
	for( auto& event : m_aryClickEvent ) {
		if( event.m_strKeyTarget == strKey ) {
			return event;
		}
	}
	static xnWnd::xClickEvent empty;
	return empty;
}

/**
 @brief layout에서 지정된 click이벤트이 핸들러.
*/
int XWnd::__OnClickLayoutEvent( XWnd* pWnd, DWORD, DWORD )
{
	if( !pWnd )
		return 1;
	auto& event = FindClickEvent( pWnd->GetstrIdentifier() );
	if( event.m_strKeyTarget.empty() )
		return 1;
	if( !event.m_strNodeClick.empty() ) {
		if( event.m_strType == "popup" ) {
			// 아직은 팝업형태로밖에 할수 없을듯.
			auto pPopup = new XWndPopup();
			pPopup->SetLayout( event.m_xmlClick, event.m_strNodeClick, this );
			pPopup->SetEnableNcEvent( TRUE );
			if( event.m_AlignClick ) {
				pPopup->AutoLayoutByAlign( this, event.m_AlignClick );
			} else {
				pPopup->SetPosLocal( event.m_vPopupByLayout );
			}
			Add( pPopup );
		} else {
			// 일반타입
			XLayoutObj layout( event.m_xmlClick );
			layout.CreateLayout( event.m_strNodeClick, this );
		}
// 		XLayoutObj layoutObj( m_xmlClick );
// 		layoutObj.CreateLayout( m_strNodeClick, this );
	}
	return 1;
}

void XWnd::CallEvent( const char *cFunc )
{
	// 사용안해서 막아둠.
// 	XLua *pLua = XE::GetMain()->GetpGame()->GetpLua();
// 	if( pLua )
// 	{
// 		if( pLua->IsHaveFunc( cFunc ) )
// 		{
// 			CONSOLE( "%s: idWnd=%d", Convert_char_To_TCHAR( cFunc ), GetID() );
// 			pLua->Call<void, XWnd*>( cFunc, this );
// 		}
// 	}
}

XE::VEC2 XWnd::GetvChildLocal( const XE::VEC2& vLocal, XWnd* pParent, XWnd* pGrandParent ) const
{
	XE::VEC2 vChildLocal;
	if( pGrandParent )
		vChildLocal = vLocal - (GetPosLocal() * pParent->GetScaleLocal());
	else
		vChildLocal = vLocal - GetPosLocal();
	return vChildLocal;
}

void XWnd::OnLButtonDown( float lx, float ly ) 
{ 
	static XWnd *s_pWndLastTouch = NULL;
	s_pWndLastTouch = NULL;
	if( GetbShow() == FALSE )		return;	// hide상태에선 본인과 자식들모두 감춤
	if( GetbActiveInternal() == false )
		return;
// 	if( GetbActive() == FALSE )		return;	// deActive상태에선 본인과 자식들의 입력을 모두 막음.
	if( GetbEnable() == FALSE )	return;
	if( GetDestroy() )		return;		// 파괴 예약된 상태에선 입력도 모두 막음
	CallEventHandler( XWM_TOUCH_DOWN );
	XE::VEC2 vMouseLocal( lx, ly );
	int size = m_listItems.size();
#ifdef _DEBUG
	int idx = 0;
#endif // _DEBUG
	for( auto rIter = m_listItems.rbegin(); rIter != m_listItems.rend(); rIter++ ) {
		XWnd *pWnd = (*rIter);
		if( pWnd->GetbShow() ) {
			XWnd *pPushWnd = NULL;
// 			XE::VEC2 vChildLocal;
// 			if( m_pParent )
// 				vChildLocal = vMouseLocal - (pWnd->GetPosLocal() * GetScaleLocal());
// 			else
// 				vChildLocal = vMouseLocal - pWnd->GetPosLocal();
			const XE::VEC2 vChildLocal = pWnd->XWnd::GetvChildLocal( vMouseLocal, this, m_pParent );
			const XE::VEC2 vInLocal = pWnd->GetvChildLocal( vMouseLocal, this, m_pParent );		// 영역체크 전용
			BOOL bAreaIn = pWnd->IsWndAreaIn( vInLocal );
// 			BOOL bAreaIn = pWnd->IsWndAreaIn( vChildLocal );
			if( bAreaIn ||
				pWnd->GetSizeLocal().IsZero() ||	// 윈도사이즈가 0,0일경우 통과(그냥 묶음용 루트 윈도우로 쓰이는 경우)
				pWnd->GetSizeLocal().IsMinus() )	// 라디오그룹같은건 사이즈가 -1이다
			{
				if( pWnd->GettimerPush().IsOff() )	// 타이머가 꺼져있을때만
					pWnd->GettimerPush().Set(0);	// 타이머 돌기 시작
				if( pWnd->GetbActiveInternal() && pWnd->GetbEnable() && pWnd->GetDestroy() == FALSE ) {
					m_pwndLastLDown = pWnd;
					// 입력을 막고자 하는 윈도우의 상위 윈도우들은 입력을 허용한다.
					// 자신혹은 부모가 모달속성인 애들은 모두 입력을 허용한다.
					if( XWnd::IsAllowClickWnd( pWnd ) || pWnd->GetbModalAttr() ) {
//						pWnd->m_vTouchDown = vChildLocal;
						// 터치한곳의 화면좌표
						pWnd->m_vTouchDown = pWnd->GetPosWindow() + vChildLocal;
						s_idLDownWnd = pWnd->getid();
						pWnd->OnLButtonDown( vChildLocal.x, vChildLocal.y );	// 차일드에는 상대좌표로 넘겨줌
						if( pWnd->GetbTouchable() ) {
//							s_pWndLastTouch = pWnd;
							if( pWnd->GetSizeLocal().IsMinus() == FALSE
								&& pWnd->GetSizeLocal().IsZero() == FALSE
								) {	// 사이즈 있는 윈도우의 경우 이벤트 처리하고 다른윈도우는 더이상 처리하지 않고 걍 리턴한다.
								// 사이즈가 없는 윈도우는 touchable이라도 lastTouch로 인정하지 않는다.
								s_pWndLastTouch = pWnd;
								return;
							}
						} else {
							// 터치이벤트를 안받는 윈도우의 경우는 그 자식이 터치이벤트를 처리했으면 그냥 리턴하고
							// 처리하지 않았으면 다음윈도우로 넘어간다.
							if( s_pWndLastTouch )
								return;
						}
					}
				}
			}
			// NC이벤트가 활성화 되어있고 영역밖에서 터치가 일어났으면 호출된다.
			if( pWnd->GetbActiveInternal() &&
				pWnd->GetbEnable() &&
				pWnd->IsEnableNcEvent() &&
				bAreaIn == FALSE ) {
				pWnd->OnNCLButtonDown( vChildLocal.x, vChildLocal.y );
				// 최상위 ncEvent를 가진 윈도우만 이벤트를 보내고 더이상 검색하지않음.
				return;
			}
			if( pWnd->GetbModal() )	{ // 모달타입이면 다른 윈도우에대한 처리를 하지 않음.
				CONSOLE("modal click = id=%d ids=%s", pWnd->getid(), C2SZ(pWnd->GetstrIdentifier()) );
				return;
			}
		}
#ifdef _DEBUG
		++idx;		// 디버깅용.
#endif
	}
	m_pwndLastLDown = NULL;
}

void XWnd::OnMouseMove( float lx, float ly ) 
{
#ifdef WIN32
	BOOL bCheckedMouseOver = FALSE;
#endif // WIN32
	static XWnd *s_pWndLastTouch = NULL;
	s_pWndLastTouch = NULL;
	if( GetbShow() == FALSE )		return;	// hide상태에선 본인과 자식들모두 감춤
	if( GetbActiveInternal() == false )
		return;
	if( GetbEnable() == FALSE )	return;
	XE::VEC2 vMouseLocal( lx, ly );
	// 캡쳐윈도우가 있고 this는 Top일때
	if( s_listCapture.size() > 0 && m_pParent == NULL ) {
		s_listCaptureDel = s_listCapture;
		// 캡쳐윈도우들에게 이벤트 전달.
		XLIST_LOOP( s_listCapture, XWnd*, pWnd )
		{
			auto pParent = pWnd->GetpParent();
			XBREAK( pParent == NULL );
			auto vParent = pParent->XWnd::GetPosFinal();
			// 터치좌표lx,ly를 전체화면좌표로 전환한다.
			XE::VEC2 vTouchWorld = GetPosWindow() + vMouseLocal;
			// 터치좌표lx,ly의 pWnd기준좌표가 나온다
			XE::VEC2 vLocalInWnd = vTouchWorld - pWnd->GetPosWindow();
			// 현재 터치위치의 pParent뷰 기준좌표를 얻는다.
			BOOL bDrag = pWnd->IsDragWnd();
			if( bDrag && pWnd->m_vTouchDown.IsMinus() == FALSE )
			{
				// 윈도우의 화면좌표에서 터치한곳의 화면좌표를 빼서 이동벡터를 얻는다.
				auto vDist = vTouchWorld - pWnd->m_vTouchDown;
				pWnd->ProcessDrag( vDist );
			}
			pWnd->OnMouseMove( vLocalInWnd.x, vLocalInWnd.y );
		} END_LOOP;

	}
	int idx = m_listItems.size() - 1;
	for( auto rIter = m_listItems.rbegin(); rIter != m_listItems.rend(); rIter++ ) {
		XWnd *pWnd = (*rIter);
		--idx;
		if( pWnd->GetbShow() )  {
			XWnd *pWndMove = NULL;
// 			XE::VEC2 vChildLocal;
// 			if( m_pParent )
// 				vChildLocal = vMouseLocal - (pWnd->GetPosLocal() * GetScaleLocal());
// 			else
// 				vChildLocal = vMouseLocal - pWnd->GetPosLocal();
			const XE::VEC2 vChildLocal = pWnd->XWnd::GetvChildLocal( vMouseLocal, this, m_pParent );
			const XE::VEC2 vInLocal = pWnd->GetvChildLocal( vMouseLocal, this, m_pParent );		// 영역체크 전용
			BOOL bAreaIn = pWnd->IsWndAreaIn( vInLocal );
// 			if( (pWnd->IsWndAreaIn( vChildLocal ) ||		// 터치가 윈도영역 위에 있거나
			if( ( pWnd->IsWndAreaIn( vInLocal ) ||		// 터치가 윈도영역 위에 있거나
				pWnd->GetSizeLocal().IsZero() ||	// 윈도사이즈가 0,0일경우 통과
				pWnd->GetSizeLocal().IsMinus() ) && 		// 윈도영역이 -1이거나
				s_listCaptureDel.Find( pWnd ) == FALSE ) {	// 캡쳐윈도우가 아니어야 한다.(위에서 이미 처리했으므로)
				// 이벤트를 전달한다.
				if( pWnd->GetbActiveInternal() && pWnd->GetbEnable() ) {
					BOOL bDrag = pWnd->IsDragWnd();
					if( bDrag && pWnd->m_vTouchDown.IsMinus() == FALSE ) {
// 						XE::VEC2 vTouchWorld = GetPosWindow() - vMouseLocal;	// 왜 이렇게 했지?
						XE::VEC2 vTouchWorld = vMouseLocal - GetPosWindow();
						auto vDist = vTouchWorld - pWnd->m_vTouchDown;
						pWnd->ProcessDrag( vDist );
					}
#ifdef WIN32
					if( pWnd->GetSizeLocal().IsZero() )
						bCheckedMouseOver = TRUE;
					else
					if( bCheckedMouseOver == FALSE ) {
						if( pWnd->GetbTouchable() ) {
							s_pMouseOver = pWnd;
							s_depthMouseMove = pWnd->GetnDepth();
							bCheckedMouseOver = TRUE;
						}
					}
#endif // _DEBUG
					pWnd->OnMouseMove( vChildLocal.x, vChildLocal.y );
#ifdef WIN32
					if( bCheckedMouseOver && s_pMouseOver == nullptr )
						bCheckedMouseOver = FALSE;
#endif // WIN32
					if( pWnd->GetbTouchable() ) {
						s_pWndLastTouch = pWnd;
						if( pWnd->GetSizeLocal().IsMinus() == FALSE
							&& pWnd->GetSizeLocal().IsZero() == FALSE
							)	// 사이즈 있는 윈도우의 경우 이벤트 처리하고 다른윈도우는 더이상 처리하지 않고 걍 리턴한다.
							break;
					}
					else
					{
						// 터치이벤트를 안받는 윈도우의 경우는 그 자식이 터치이벤트를 처리했으면 그냥 리턴하고
						// 처리하지 않았으면 다음윈도우로 넘어간다.
						if( s_pWndLastTouch )
							break;
					}
				}
			} else
				m_bToolTip = FALSE;
			// NC이벤트는 영역체크에 상관없이 호출된다
			if( pWnd->GetbActiveInternal() && pWnd->GetbEnable() && pWnd->IsEnableNcEvent() )	{
				pWnd->OnNCMouseMove( vChildLocal.x, vChildLocal.y );
				return;
			}
			if( pWnd->GetbModal() )	// 모달타입이면 다른 윈도우에대한 처리를 하지 않음.
				break;
		}
	}
	//
}

/**
 @brief 현재 마우스위치에 오버된 모든 윈도우를 찾는다.
*/
#if defined(_CHEAT) && defined(WIN32)
void XWnd::UpdateMouseOverWins( const XE::VEC2& vMouse, int depth, XVector<xWinDepth>* pOutAry )
{
	for( auto pWnd : m_listItems ) {
		if( !pWnd->IsDestroy() && pWnd->GetbShow() ) {
			const auto vPos = pWnd->GetPosFinal();
			const auto vSize = pWnd->GetSizeFinal();
			// 마우스위치에 들어온 윈도우는 모두 리스트에 넣는다.
			if( XE::IsArea( vPos, vSize, vMouse ) || vSize.IsInvalid() ) {
				pOutAry->Add( xWinDepth(pWnd, depth) );
				// 영역에 포함되면 리스트에 넣은 후 리커시브로 그 자식들을 검사한다.
				pWnd->UpdateMouseOverWins( vMouse, depth + 1, pOutAry );
			}
		}
	}
}
#endif // defined(_CHEAT) && defined(WIN32)
/**
 @brief drag&drop의 drag처리를 한다.
*/
void XWnd::ProcessDrag( const XE::VEC2& vDistMove )
{
	if( vDistMove.Lengthsq() > 3.f * 3.f )	// 터치무브가 3픽셀이상 발생하면 드래그이벤트 발생
	{
		CallEventHandler( XWM_DRAG );	// 드래그 이벤트 발생
		// 드래그한 윈도우를 받아둔다.
		XWnd::sSetDragWnd( this );
	}
}
void XWnd::OnLButtonUp( float lx, float ly ) 
{ 
	static XWnd *s_pWndLastTouch = NULL;
	s_pWndLastTouch = NULL;
	m_pwndLastLDown = NULL;
	if( GetbShow() == FALSE )		return;	// hide상태에선 본인과 자식들모두 감춤
	if( GetbActiveInternal() == false ) {
		return;
	}
// 	if( GetbActive() == FALSE )		return;	// deActive상태에선 본인과 자식들의 입력을 모두 막음.
	if( GetbEnable() == FALSE )	return;
	XE::VEC2 vMouseLocal( lx, ly );
	BOOL m_bProceed = FALSE;
	/*
		return을 빼면 캡쳐한 윈도우가 Up이벤트를 두번 받는다.
		return을 넣으면 캡쳐윈도우가 어떤부모의 자식일경우 부모가 Up이벤트를 못받는다.
	해결책:캡쳐윈도우는 일단 단일이 아닌 복수로 지정할 수 있도록 리스트로 만든다.
		터치이벤트 할때 영역을 벗어나 실패한경우 캡쳐리스트에 있으면 이벤트를 실행하게 해준다.
	*/
	// 캡쳐윈도우가 있고 this는 Top일때
	if( s_listCapture.size() > 0 && m_pParent == NULL ) {
		// OnLButtonUp내부에서 Release되어 list에서 사라질수도 있으므로 미리 백업
		s_listCaptureDel = s_listCapture;
		// 캡쳐윈도우들에게 이벤트 전달.
		XLIST_LOOP( s_listCapture, XWnd*, pWnd ) {
			XBREAK( pWnd->GetpParent() == NULL );
			XE::VEC2 vParentFinal = pWnd->GetpParent()->XWnd::GetPosFinal();
			XE::VEC2 vMouseLocalFromParent =  vMouseLocal - vParentFinal;
			XE::VEC2 vChildLocal =  vMouseLocalFromParent - pWnd->XWnd::GetPosLocal();
//			if( !pWnd->IsWndAreaIn( vChildLocal ) ) //현재 구조가 잘못설계되서 이걸넣으면 캡쳐윈도우(버튼등)는 영원히 up이벤트를 못받는다.
				pWnd->OnLButtonUp( vChildLocal.x, vChildLocal.y );
		} END_LOOP;

	}	// tooltip clear
	// drag & drop객체가 있을때
	bool bDrop = false;
	// 드래그중인 윈도우가 있을때
	if( s_pDragWnd ) {
		// 현재 터치위치의 최상위 윈도우가 this일경우
		if( IsDropWnd() ) {
			if( IsWndAreaIn( vMouseLocal ) ) {
				// this윈도우에 drop이벤트 발생시킴
				CallEventHandler( XWM_DROP, s_pDragWnd->GetID() );
				bDrop = true;
				// 드롭이벤트 발생시켰으면 드래그 이벤트는 해제시킴
				s_pDragWnd->m_vTouchDown.Set( -1.f );
				s_pDragWnd = NULL;
			}
		}
	}
	// 이제 버튼뿐 아니라 일반 윈도우도 클릭 이벤트를 줄수가 있게 됨.
	if( bDrop == false && !m_vSize.IsZero() ) {	// 드래그&드랍으로 터치한거면 클릭이벤트 발생시키지 않음.
		if( m_bTouchable ) {	// 드래그&드랍으로 터치한거면 클릭이벤트 발생시키지 않음.
			CallEventHandler( XWM_CLICKED );
		}
		CallEventHandler( XWM_TOOLTIP );
	}
	m_timerPush.Off();
	m_bToolTip = FALSE;
	TOOLTIP = NULL;
	// child
	for( auto rIter = m_listItems.rbegin(); rIter != m_listItems.rend(); rIter++ ) {
		XWnd *pWnd = (*rIter);
		pWnd->m_vTouchDown.Set(-1.f);
		if( pWnd->GetbShow() ) {
// 			XE::VEC2 vChildLocal;
// 			if( m_pParent )
// 				vChildLocal = vMouseLocal - (pWnd->GetPosLocal() * GetScaleLocal());
// 			else
// 				vChildLocal = vMouseLocal - pWnd->GetPosLocal();
			pWnd->GettimerPush().Off();
			pWnd->SetbToolTip( FALSE );
			const XE::VEC2 vChildLocal = pWnd->XWnd::GetvChildLocal( vMouseLocal, this, m_pParent );
			const XE::VEC2 vInLocal = pWnd->GetvChildLocal( vMouseLocal, this, m_pParent );		// 영역체크 전용
			BOOL bAreaIn = pWnd->IsWndAreaIn( vInLocal );
// 			BOOL bAreaIn = pWnd->IsWndAreaIn( vChildLocal );
			// 윈도사이즈가 0,0일경우 통과
			// 터치가 윈도영역 위에 있거나 윈도영역이 없거나
			bool bFlag = (bAreaIn || pWnd->GetSizeLocal().IsInvalid()) && !s_listCaptureDel.Find( pWnd );
			if( bFlag )	{	// 캡쳐윈도우의 경우 두번실행되지 않게 하기 위해
				bool bFlag = (pWnd->GetbActiveInternal() && pWnd->GetbEnable());
				if( bFlag ) {
					m_pwndLastLUp = pWnd;
					// 입력을 막고자 하는 윈도우의 상위 윈도우들은 입력을 허용한다.
					// 자신혹은 부모가 모달속성인 애들은 모두 입력을 허용한다.
					bool bFlag = XWnd::IsAllowClickWnd( pWnd ) || pWnd->GetbModalAttr();
					if( bFlag ) {
						// 글로벌 클릭 핸들러.
						XE::GetGame()->OnClickWnd( pWnd );
// 						XWnd::s_pLastLUp = pWnd;
						pWnd->OnLButtonUp( vChildLocal.x, vChildLocal.y ); 
// 						if( XWnd::s_pLastLUp ) {
						// 이방식의 문제점은 예를들어 박스가 있고 거기 텍스트가 박스크기로 있을때, 텍스트는 클랙핸들러를 호출하지만 박스는 클릭됐음에도 호출이 안된다.
// 							XE::GetGame()->OnClickWnd( XWnd::s_pLastLUp );
// 							XWnd::s_pLastLUp = nullptr;
// 						}
						if( pWnd->GetbTouchable() ) {
							// 사이즈 있는 윈도우의 경우 이벤트 처리하고 다른윈도우는 더이상 처리하지 않고 걍 리턴한다.
							if( pWnd->GetSizeLocal().IsValid() ) {	
								s_pWndLastTouch = pWnd;
								return;
							}
						} else {
							if( s_pWndLastTouch )
								return;
						}
					}
				}
			}
			// NC up이벤트는 영역체크에 상관없이 호출된다
			if( pWnd->GetbActiveInternal() && pWnd->GetbEnable() && pWnd->IsEnableNcEvent() ) {
				pWnd->OnNCLButtonUp( vChildLocal.x, vChildLocal.y ); 
				return;		// 이젠 최상위 ncEvent윈도우만 처리하고 나옴.
			}
			if( pWnd->GetbModal() )	{ // 모달타입이면 다른 윈도우에대한 처리를 하지 않음.
				if( !bAreaIn )
					pWnd->OnNCModal();
				return;
			}
		}
	}
	m_pwndLastLUp = NULL;
	return;
}

void XWnd::OnLButtonDownCancel()
{
}


// 치트용으로만 쓴다.
void XWnd::OnRButtonDown( float lx, float ly )	
{
	static XWnd *s_pWndLastTouch = NULL;
	s_pWndLastTouch = NULL;
	if( GetbShow() == FALSE )		return;	// hide상태에선 본인과 자식들모두 감춤
// 	if( GetbActive() == FALSE )		return;	// deActive상태에선 본인과 자식들의 입력을 모두 막음.
	if( GetbActiveInternal() == false )
		return;
	if( GetbEnable() == FALSE )	return;
	if( GetDestroy() )		return;		// 파괴 예약된 상태에선 입력도 모두 막음
	XE::VEC2 vMouseLocal( lx, ly );
	int size = m_listItems.size();
	for( auto rIter = m_listItems.rbegin(); rIter != m_listItems.rend(); rIter++ )	{
		XWnd *pWnd = (*rIter);
		if( pWnd->GetbShow() ) 	{
			XWnd *pPushWnd = NULL;
// 			XE::VEC2 vChildLocal;
// 			if( m_pParent )
// 				vChildLocal = vMouseLocal - (pWnd->GetPosLocal() * GetScaleLocal());
// 			else
// 				vChildLocal = vMouseLocal - pWnd->GetPosLocal();
// 			if( pWnd->IsWndAreaIn( vChildLocal ) || 
			const XE::VEC2 vChildLocal = pWnd->XWnd::GetvChildLocal( vMouseLocal, this, m_pParent );
			const XE::VEC2 vInLocal = pWnd->GetvChildLocal( vMouseLocal, this, m_pParent );		// 영역체크 전용
			BOOL bAreaIn = pWnd->IsWndAreaIn( vInLocal );
			if( bAreaIn ||
				pWnd->GetSizeLocal().IsZero() ||	// 윈도사이즈가 0,0일경우 통과
				pWnd->GetSizeLocal().IsMinus() ) {	// 라디오그룹같은건 사이즈가 -1이다
				if( pWnd->GettimerPush().IsOff() )	// 타이머가 꺼져있을때만
					pWnd->GettimerPush().Set(0);	// 타이머 돌기 시작
				if( pWnd->GetbActiveInternal() && pWnd->GetbEnable() && pWnd->GetDestroy() == FALSE )	{
					// 입력을 막고자 하는 윈도우의 상위 윈도우들은 입력을 허용한다.
					if( XWnd::IsAllowClickWnd( pWnd ) )	{
						pWnd->OnRButtonDown( vChildLocal.x, vChildLocal.y );	// 차일드에는 상대좌표로 넘겨줌
						if( pWnd->GetbTouchable() ) {
							s_pWndLastTouch = pWnd;
							if( pWnd->GetSizeLocal().IsMinus() == FALSE
								&& pWnd->GetSizeLocal().IsZero() == FALSE
								)	// 사이즈 있는 윈도우의 경우 이벤트 처리하고 다른윈도우는 더이상 처리하지 않고 걍 리턴한다.
								return;
						}
						else
						{
							// 터치이벤트를 안받는 윈도우의 경우는 그 자식이 터치이벤트를 처리했으면 그냥 리턴하고
							// 처리하지 않았으면 다음윈도우로 넘어간다.
							if( s_pWndLastTouch )
								return;
						}
					}
				}
			}
			if( pWnd->GetbModal() )	// 모달타입이면 다른 윈도우에대한 처리를 하지 않음.
				return;
		}
	}
}

void XWnd::OnRButtonUp( float lx, float ly ) 
{ 
	static XWnd *s_pWndLastTouch = NULL;
	s_pWndLastTouch = NULL;
	if( GetbShow() == FALSE )		return;	// hide상태에선 본인과 자식들모두 감춤
// 	if( GetbActive() == FALSE )		return;	// deActive상태에선 본인과 자식들의 입력을 모두 막음.
	if( GetbActiveInternal() == false )
		return;
	if( GetbEnable() == FALSE )	return;
	XE::VEC2 vMouseLocal( lx, ly );
	// child
	for( auto rIter = m_listItems.rbegin(); rIter != m_listItems.rend(); rIter++ )	{
		XWnd *pWnd = (*rIter);
		if( pWnd->GetbShow() )		{
// 			XE::VEC2 vChildLocal;
// 			if( m_pParent )
// 				vChildLocal = vMouseLocal - (pWnd->GetPosLocal() * GetScaleLocal());
// 			else
// 				vChildLocal = vMouseLocal - pWnd->GetPosLocal();
			const XE::VEC2 vChildLocal = pWnd->XWnd::GetvChildLocal( vMouseLocal, this, m_pParent );
			const XE::VEC2 vInLocal = pWnd->GetvChildLocal( vMouseLocal, this, m_pParent );		// 영역체크 전용
			BOOL bAreaIn = pWnd->IsWndAreaIn( vInLocal );
// 			if( (pWnd->IsWndAreaIn( vChildLocal ) ||		// 터치가 윈도영역 위에 있거나
			if( bAreaIn ||
				pWnd->GetSizeLocal().IsZero() ||	// 윈도사이즈가 0,0일경우 통과
				pWnd->GetSizeLocal().IsMinus() )	{
				if( pWnd->GetbActiveInternal() && pWnd->GetbEnable() ) {
					// 입력을 막고자 하는 윈도우의 상위 윈도우들은 입력을 허용한다.
					// 자신혹은 부모가 모달속성인 애들은 모두 입력을 허용한다.
					if( XWnd::IsAllowClickWnd( pWnd ) || pWnd->GetbModalAttr() )
						pWnd->OnRButtonUp( vChildLocal.x, vChildLocal.y ); 
					if( pWnd->GetbTouchable() ) {
						s_pWndLastTouch = pWnd;
						if( pWnd->GetSizeLocal().IsMinus() == FALSE
							&& pWnd->GetSizeLocal().IsZero() == FALSE
							)	// 사이즈 있는 윈도우의 경우 이벤트 처리하고 다른윈도우는 더이상 처리하지 않고 걍 리턴한다.
							return;
					}
					else
					{
						// 터치이벤트를 안받는 윈도우의 경우는 그 자식이 터치이벤트를 처리했으면 그냥 리턴하고
						// 처리하지 않았으면 다음윈도우로 넘어간다.
						if( s_pWndLastTouch )
							return;
					}
				}
			}
			// NC이벤트는 영역체크에 상관없이 호출된다
//			if( pWnd->GetbActive() && pWnd->GetbEnable() )
//				pWnd->OnNCLButtonUp( vChildLocal.x, vChildLocal.y ); 
		if( pWnd->GetbModal() )	// 모달타입이면 다른 윈도우에대한 처리를 하지 않음.
				return;
		}
	}
	return;
}

/**
 핀치투줌 동작을 할때 호출된다. WIN32 버전에서는 휠을 돌릴때 발생한다.
	@param scale 현재 스케일 값. 확대는 양수, 축소는 음수가 넘어온다.
	@param lx 줌동작의 중간 좌표?(손가락 사이의 좌표?)
	@param ly 줌동작의 중간 좌표?(손가락 사이의 좌표?) 
*/
void XWnd::OnZoom( float scale, float lx, float ly )
{
	static XWnd *s_pWndLastTouch = NULL;
	s_pWndLastTouch = NULL;
	if( GetbShow() == FALSE )		return;	// hide상태에선 본인과 자식들모두 감춤
// 	if( GetbActive() == FALSE )		return;	// deActive상태에선 본인과 자식들의 입력을 모두 막음.
	if( GetbActiveInternal() == false )
		return;
	if( GetbEnable() == FALSE )	return;
	XE::VEC2 vMouseLocal( lx, ly );
	// child
	std::list<XWnd*>::reverse_iterator iter;
	for( iter = m_listItems.rbegin(); iter != m_listItems.rend(); iter++ ) {
		XWnd *pWnd = ( *iter );
		if( pWnd->GetbShow() ) {
// 			XE::VEC2 vChildLocal;
// 			if( m_pParent )
// 				vChildLocal = vMouseLocal - ( pWnd->GetPosLocal() * GetScaleLocal() );
// 			else
// 				vChildLocal = vMouseLocal - pWnd->GetPosLocal();
			const XE::VEC2 vChildLocal = pWnd->XWnd::GetvChildLocal( vMouseLocal, this, m_pParent );
			const XE::VEC2 vInLocal = pWnd->GetvChildLocal( vMouseLocal, this, m_pParent );		// 영역체크 전용
			BOOL bAreaIn = pWnd->IsWndAreaIn( vInLocal );
//		if( ( pWnd->IsWndAreaIn( vChildLocal ) ||		// 터치가 윈도영역 위에 있거나
			if( bAreaIn ||
				pWnd->GetSizeLocal().IsZero() ||	// 윈도사이즈가 0,0일경우 통과
				pWnd->GetSizeLocal().IsMinus() ) {
				if( pWnd->GetbActiveInternal() && pWnd->GetbEnable() ) {
					// 입력을 막고자 하는 윈도우의 상위 윈도우들은 입력을 허용한다.
					// 자신혹은 부모가 모달속성인 애들은 모두 입력을 허용한다.
					if( XWnd::IsAllowClickWnd( pWnd ) || pWnd->GetbModalAttr() )
						pWnd->OnZoom( scale, vChildLocal.x, vChildLocal.y );
					if( pWnd->GetbTouchable() ) {
						s_pWndLastTouch = pWnd;
						if( pWnd->GetSizeLocal().IsMinus() == FALSE
							&& pWnd->GetSizeLocal().IsZero() == FALSE
							)	// 사이즈 있는 윈도우의 경우 이벤트 처리하고 다른윈도우는 더이상 처리하지 않고 걍 리턴한다.
							return;
					} else {
						// 터치이벤트를 안받는 윈도우의 경우는 그 자식이 터치이벤트를 처리했으면 그냥 리턴하고
						// 처리하지 않았으면 다음윈도우로 넘어간다.
						if( s_pWndLastTouch )
							return;
					}
				}
			}
			if( pWnd->GetbModal() )	// 모달타입이면 다른 윈도우에대한 처리를 하지 않음.
				return;
		}
	}
	return;
}


void XWnd::SendParentsLButtonDownCancel()
{
	if( m_pParent )	{
		m_pParent->OnLButtonDownCancel();
		m_pParent->SendParentsLButtonDownCancel();
	}
}

void XWnd::SendChildLButtonDownCancel()
{
	for( auto rIter = m_listItems.rbegin(); rIter != m_listItems.rend(); rIter++ ) {
		XWnd *pWnd = (*rIter);
		pWnd->OnLButtonDownCancel(); 
		pWnd->SendChildLButtonDownCancel();
	} 
}

void XWnd::PreCalcLayoutSize()
{
	for( auto pWnd : m_listItems ) {
		pWnd->PreCalcLayoutSize();
	}
}

void XWnd::SetbUpdateChilds()
{
	for( auto pWnd : m_listItems ) {
		pWnd->SetbUpdate( true );
	}
}

BOOL XWnd::RestoreDevice()
{
	for( auto pWnd : m_listItems ) {
		if( pWnd->RestoreDevice() == FALSE )
			return FALSE;
	}
	return TRUE;
}

/**
 @brief 각 객체가 보유한 shared_ptr을 반납해야 한다.
*/
void XWnd::Release()
{
	for( auto pWnd : m_listItems ) {
		pWnd->Release();
	}
}

BOOL XWnd::RestoreGameResource()
{
	for( auto pWnd : m_listItems ) {
		if( pWnd->RestoreGameResource() == FALSE )
			return FALSE;
	}
	return TRUE;
}

/**
 @brief 리로드에서 앞서 자신이 가진 리소스를 모두 파괴하도록 한다.
 이것은 각종 매니저에서 캐싱을 하기때문에 한꺼번에 다 삭제한후 다시 로드해야한다.
*/
void XWnd::DestroyForReload()
{
	for( auto pWnd : m_listItems ) {
		if( !pWnd->GetDestroy() )
			pWnd->DestroyForReload();
	}
}

void XWnd::Reload()
{
	for( auto pWnd : m_listItems ) {
		if( !pWnd->GetDestroy() )
			pWnd->Reload();
	}
}


// 다이나믹 해상도 적용으로 pWnd의 y좌표가 화면 윗쪽을 벗어났을때 화면안쪽으로 들어오도록 보정해준다.
void XWnd::AdjustDynamicPos()
{
	XE::VEC2 vPos = GetPosFinal();
	if( vPos.y < 0 )	{
		XE::VEC2 vLocal = GetPosLocal();
		XE::VEC2 vParent;
		if( GetpParent() )
			vParent = GetpParent()->GetPosFinal();	// final로 해야하나?
		vPos.y = fabs( vParent.y ) + vLocal.y;
		SetPosLocal( vPos );
	}
	for( auto pWnd : m_listItems ) {
		pWnd->AdjustDynamicPos();
	}
}


BOOL XWnd::OnKeyDown( int keyCode ) 
{ 
	if( GetbShow() == FALSE )		return FALSE;	// hide상태에선 입력받지 않음.
	if( GetbActiveInternal() == false )
		return FALSE;
	if( GetbEnable() == FALSE )	return FALSE;
	if( GetDestroy() )		return FALSE;		// 파괴 예약된 상태에선 입력도 모두 막음
	for( auto rIter = m_listItems.rbegin(); rIter != m_listItems.rend(); rIter++ ) {
		XWnd *pWnd = (*rIter);
		if( pWnd->GetbShow() ) {
			if( pWnd->GetbActiveInternal() && pWnd->GetbEnable() && pWnd->GetDestroy() == FALSE ) {
				// 입력을 막고자 하는 윈도우의 상위 윈도우들은 입력을 허용한다.
				if( XWnd::IsAllowClickWnd( pWnd ) ) {
					BOOL bCatched = pWnd->OnKeyDown( keyCode );	
					// pWnd가 키다운 이벤트를 캐치해서 썼다.
					if( bCatched )
						return TRUE;
				}
			}
//			if( pWnd->GetbModal() )	// 모달타입이면 다른 윈도우에대한 처리를 하지 않음.
//				return FALSE;
		}
	}
	return FALSE;
}
BOOL XWnd::OnKeyUp( int keyCode ) 
{ 
	if( GetbShow() == FALSE )		return FALSE;	// hide상태에선 입력받지 않음.
// 	if( GetbActive() == FALSE )		return FALSE;	// deActive상태에선 입력받지 않음.
	if( GetbActiveInternal() == false )
		return FALSE;
	if( GetbEnable() == FALSE )	return FALSE;
	if( GetDestroy() )		return FALSE;		// 파괴 예약된 상태에선 입력도 모두 막음
	for( auto rIter = m_listItems.rbegin(); rIter != m_listItems.rend(); rIter++ ) {
		XWnd *pWnd = (*rIter);
		if( pWnd->GetbShow() ) {
			if( pWnd->GetbActiveInternal() && pWnd->GetbEnable() && pWnd->GetDestroy() == FALSE ) {
				// 입력을 막고자 하는 윈도우의 상위 윈도우들은 입력을 허용한다.
				if( XWnd::IsAllowClickWnd( pWnd ) ) {
					BOOL bCatched = pWnd->OnKeyUp( keyCode );	
					// pWnd가 키다운 이벤트를 캐치해서 썼다.
					if( bCatched )
						return TRUE;
				}
			}
//			if( pWnd->GetbModal() )	// 모달타입이면 다른 윈도우에대한 처리를 하지 않음.
//				return FALSE;
		}
	}
	return FALSE;
}


// frame리소스로 this의 크기에 맞는 프레임을 그려준다
void XWnd::DrawFrame( const XWND_RES_FRAME& frame )
{
	// 블럭으로 쪼개진 Frame리소스를 타일화 해서 찍음
	XE::VEC2 vPos = GetPosFinal();		// 좌상귀 좌표(기준좌표)는 스케일에 관계없이 똑같다
	XE::VEC2 vScale = GetScaleFinal();		// 이 윈도우 최종스케일값
	XE::VEC2 vSize = GetSizeFinal();		// 이 윈도우 최종사이즈(스케일이 적용된)
	XE::VEC2 vOrigSize = m_vSize;

	XE::VEC2 vBlockSize;
	vBlockSize.Set( frame.nBlockSize );	// 계산을 단순화하기 위해 블럭사이즈도 스케일1.0을 기준으로 좌표를 지정하고 최종적으로 스케일을 적용하자
	if( (vOrigSize.w - (float)((int)(vOrigSize.w / vBlockSize.w) * vBlockSize.w)) > 0 )
		vOrigSize.w = ((int)(vOrigSize.w / vBlockSize.w) + 1) * vBlockSize.w;
	if( (vOrigSize.h - (float)((int)(vOrigSize.h / vBlockSize.h) * vBlockSize.h)) > 0 )
		vOrigSize.h = ((int)(vOrigSize.h / vBlockSize.h) + 1) * vBlockSize.h;
	XE::VEC2 vSec = (vOrigSize / vBlockSize);		// 스케일등을 고려하지 않은 순수한 오리지날 사이즈
	vSec.x = (float)((int)vSec.x);
	vSec.y = (float)((int)vSec.y);
	XE::VEC2 vSizeHalf = vOrigSize / 2;
	XSurface *psfc = NULL;
	float lx, ly;
	// 프레임은 빼고 중간 블럭으로 size를 채운다
	for( int i = 0; i < (int)vSec.y; i ++ )	// 블럭사이즈로 나누고 남을수 있으니 +1을 더한다
	{
		for( int j = 0; j < (int)vSec.x; j ++ )
		{
			psfc = frame.psfcFrame[ 4 ];	// 중앙 블럭
			psfc->SetScale( vScale );
			lx = j * vBlockSize.x;
			ly = i * vBlockSize.y;
			psfc->SetAdjustAxis( vSizeHalf );		// 창의 중간으로 변환축을 옮김
			psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
		}
	}
	// 좌상귀
	psfc = frame.psfcFrame[ 0 ];	
	psfc->SetScale( vScale );
	lx = 0 * vBlockSize.x - vBlockSize.x;
	ly = 0 * vBlockSize.y - vBlockSize.y;
	psfc->SetAdjustAxis( vSizeHalf );				// 창의 중간으로 변환축을 옮김
	psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
	// 우상귀
	psfc = frame.psfcFrame[ 2 ];	
	psfc->SetScale( vScale );
	lx = vSec.x * vBlockSize.x;
	ly = 0 * vBlockSize.y - vBlockSize.y;
	psfc->SetAdjustAxis( vSizeHalf );				// 창의 중간으로 변환축을 옮김
	psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
	// 좌하귀
	psfc = frame.psfcFrame[ 6 ];	
	psfc->SetScale( vScale );
	lx = 0 * vBlockSize.x - vBlockSize.x;
	ly = vSec.y * vBlockSize.y;
	psfc->SetAdjustAxis( vSizeHalf );				// 창의 중간으로 변환축을 옮김
	psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
	// 우하귀
	psfc = frame.psfcFrame[ 8 ];	
	psfc->SetScale( vScale );
	lx = vSec.x * vBlockSize.x;
	ly = vSec.y * vBlockSize.y;
	psfc->SetAdjustAxis( vSizeHalf );				// 창의 중간으로 변환축을 옮김
	psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
	
	// 위/아래
	for( int i = 0; i < (int)vSec.x; i ++ )
	{
		psfc = frame.psfcFrame[ 1 ];	// 위
		psfc->SetScale( vScale );
		lx = i * vBlockSize.x;
		ly = 0 * vBlockSize.y - vBlockSize.y;
		psfc->SetAdjustAxis( vSizeHalf );		// 창의 중간으로 변환축을 옮김
		psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
		//
		psfc = frame.psfcFrame[ 7 ];	// 아래
		psfc->SetScale( vScale );
		ly = vSec.y * vBlockSize.y;
		psfc->SetAdjustAxis( vSizeHalf );		// 창의 중간으로 변환축을 옮김
		psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
	}
	// 왼쪽/오른쪽
	for( int j = 0; j < (int)vSec.y; j ++ )
	{
		psfc = frame.psfcFrame[ 3 ];	// 왼쪽
		psfc->SetScale( vScale );
		lx = 0 * vBlockSize.x - vBlockSize.x;
		ly = j * vBlockSize.y;
		psfc->SetAdjustAxis( vSizeHalf );		// 창의 중간으로 변환축을 옮김
		psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
		//
		psfc = frame.psfcFrame[ 5 ];	// 아래
		psfc->SetScale( vScale );
		lx = vSec.x * vBlockSize.x;
		psfc->SetAdjustAxis( vSizeHalf );		// 창의 중간으로 변환축을 옮김
		psfc->DrawLocal( vPos.x, vPos.y, lx, ly );		// 로컬좌표로 지정하여 부모를 곱해 그림
	}
} // drawframe

BOOL XWnd::LoadResFrame( LPCTSTR szFile, XWND_RES_FRAME *pFrame, int surfaceBlockSize )
{
	_tcscpy_s( pFrame->szFrame, szFile );
	_tstring strRes;
	XE::MakePath( &strRes, DIR_UI, szFile );
	XASSERT( surfaceBlockSize == 0 || surfaceBlockSize == 16 || surfaceBlockSize == 32 || surfaceBlockSize == 64 );
	pFrame->nBlockSize = surfaceBlockSize;
	// png를 읽음
    XImage image( TRUE );
	if( image.Load( strRes.c_str() ) == FALSE )
		return FALSE;
	// 블럭사이즈 자동계산
	if( surfaceBlockSize == 0 )
	{
		XBREAKF( (image.GetWidth() % 3) != 0, "%s의 이미지가 3으로 나누어 떨어져야 합니다", strRes.c_str() );
		pFrame->nBlockSize = surfaceBlockSize = image.GetWidth() / 3;
	}
	int iw = image.GetMemWidth();		// 이미지 메모리 크기
	int ih = image.GetMemHeight();
	const int memBlockSize = surfaceBlockSize * 2;						// 실제 메모리 블럭 사이즈
//	int memBlockSize = image.ConvertToMemSize( (float)surfaceBlockSize );	// 실제 메모리 블럭 사이즈
	// 9개의 조각으로 서피스를 나눔
	DWORD *pImg = image.GetTextureData();
	pFrame->Destroy();
	pFrame->psfcFrame[0] = GRAPHICS->CreateSurface( TRUE, 0, 0, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );		// 첫째줄
	pFrame->psfcFrame[1] = GRAPHICS->CreateSurface( TRUE, memBlockSize, 0, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[2] = GRAPHICS->CreateSurface( TRUE, memBlockSize*2, 0, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[3] = GRAPHICS->CreateSurface( TRUE, 0, memBlockSize, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );		// 둘째줄
	pFrame->psfcFrame[4] = GRAPHICS->CreateSurface( TRUE, memBlockSize, memBlockSize, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[5] = GRAPHICS->CreateSurface( TRUE, memBlockSize*2, memBlockSize, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[6] = GRAPHICS->CreateSurface( TRUE, 0, memBlockSize*2, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );		// 셋째줄
	pFrame->psfcFrame[7] = GRAPHICS->CreateSurface( TRUE, memBlockSize, memBlockSize*2, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	pFrame->psfcFrame[8] = GRAPHICS->CreateSurface( TRUE, memBlockSize*2, memBlockSize*2, iw, ih, (float)surfaceBlockSize, (float)surfaceBlockSize, 0, 0, pImg, FALSE );
	return TRUE;
}

static bool CompPriority( XWnd *p1, XWnd *p2 )
{
	if( p1->GetPriority() > p2->GetPriority() )
		return true;
	return false;
}
// draw priority순으로 차일드들을 소트한다.
void XWnd::SortPriority()
{
	m_listItems.sort( CompPriority );
}

// void XWnd::GetNextClear(std::list<XWnd*>::iterator *pItor)
// {
// 	*pItor = m_listItems.begin();
// }
// 
// XWnd* XWnd::GetNext(std::list<XWnd*>::iterator& itor)
// {
// 	if( itor == m_listItems.end() )
// 		return NULL;
// 	XWnd *pWnd = *itor++;
// 	return pWnd;
// }
// 
// void XWnd::GetNextClear2( XList<XWnd*>::Itor *pOutItor )
// {
// 	pOutItor->m_Itor = m_listItems.begin();
// }
// 
// XWnd* XWnd::GetNext2( XList<XWnd*>::Itor& itor )
// {
// 	if( itor.m_Itor == m_listItems.end() )
// 		return NULL;
// 	XWnd *pWnd = *itor.m_Itor++;
// 	return pWnd;
// }

// 터치가 윈도우의 영역을 벗어나더라도 이벤트를 받을 수 있게 한다.
void XWnd::SetCapture()
{
	if( s_listCapture.Find(this) == FALSE )
		s_listCapture.Add( this );
}
// 캡쳐 속성을 해제한다.
void XWnd::ReleaseCapture()
{
	s_listCapture.Del( this );
	s_listCaptureDel.Del( this );
}

XWnd* XWnd::sFindCaptureWnd( ID snWnd )
{
	XLIST_LOOP( s_listCapture, XWnd*, pWnd )
	{
		if( pWnd->GetID() == snWnd )
			return pWnd;
	} END_LOOP;
	return NULL;
}
/**
	자식들의 최대크기로 this의 로컬크기를 자동으로 맞춘다.
*/
void XWnd::SetAutoSize() 
{
	auto vSizeLocal = GetSizeNoTransLayout();
	SetSizeLocal( vSizeLocal );
}

/**
 @brief this의 트랜스폼된 최대 레이아웃 크기를 얻는다.
 this의 scale까지 적용한 최대 레이아웃 크기를 리턴한다.
*/
XE::VEC2 XWnd::GetSizeFinalLayout()
{
	const auto sizeLocal = GetSizeNoTransLayout();
	const auto sizeFinal = sizeLocal * m_vScale;		// GetScaleFinal()로 해야하는거 아님?
	return sizeFinal;
}
/**
 @brief this의 총 레이아웃 크기를 얻는다.
 this의 scale은 적용하지 않은 값을 기준으로 하고 자식들은 트랜스폼된것을 기준으로 해서 최대크기를 리턴한다.
*/
XE::VEC2 XWnd::GetSizeNoTransLayout()
{
	XVector<XWnd*> aryWnd;
	for( auto pWnd : m_listItems ) {
		aryWnd.Add( pWnd );
	}
	auto sizeChilds = GetSizeNoTransLayoutWithAry( aryWnd );
	auto sizeTotalLayoutSize = sizeChilds;
	// this의 사이즈와 자식들의 레이아웃사이즈중에서 큰걸 선택한다.
	if( m_vSize.w > sizeTotalLayoutSize.w )
		sizeTotalLayoutSize.w = m_vSize.w;
	if( m_vSize.h > sizeTotalLayoutSize.h )
		sizeTotalLayoutSize.h = m_vSize.h;
	return sizeTotalLayoutSize;

// 	XE::VEC2 vMaxSize;
	// 자식들의 사이즈 구함.
// 	XE::VEC2 sizeLayoutChilds;
// 	for( auto pChild : m_listItems ) {
// 		if (pChild->GetDestroy() == FALSE) {
// 			auto sizeLocalChild = pChild->GetSizeNoTransLayout();
// 			auto sizeFinalChild = sizeLocalChild * pChild->GetScaleLocal();
// 			auto vTotalSize = pChild->GetPosLocal() + sizeFinalChild;
// 			if (vTotalSize.w > sizeLayoutChilds.w)
// 				sizeLayoutChilds.w = vTotalSize.w;
// 			if (vTotalSize.h > sizeLayoutChilds.h)
// 				sizeLayoutChilds.h = vTotalSize.h;
// 		}
// 	}
// 	// this의 사이즈도 비교
// 	auto vSizeThis = m_vSize;
// 	auto sizeTotalLayoutSize = sizeLayoutChilds;
// 	// this의 사이즈와 자식들의 레이아웃사이즈중에서 큰걸 선택한다.
// 	if( vSizeThis.w > sizeTotalLayoutSize.w )
// 		sizeTotalLayoutSize.w = vSizeThis.w;
// 	if( vSizeThis.h > sizeTotalLayoutSize.h )
// 		sizeTotalLayoutSize.h = vSizeThis.h;
// 
// 	return sizeTotalLayoutSize;
}

/**
 @brief aryWnd들의 최대 레이아웃 크기를 구한다.
*/
XE::VEC2 XWnd::GetSizeNoTransLayoutWithAry( const XVector<XWnd*>& aryWnd ) const
{
	// 자식들의 사이즈 구함.
	XE::VEC2 sizeLayoutChilds;
	for( auto pChild : aryWnd ) {
		if( !pChild->GetDestroy() ) {
			const auto sizeLocalChild = pChild->GetSizeNoTransLayout();
			const auto sizeFinalChild = sizeLocalChild * pChild->GetScaleLocal();
			const auto vTotalSize = pChild->GetPosLocal() + sizeFinalChild;
			if( vTotalSize.w > sizeLayoutChilds.w )
				sizeLayoutChilds.w = vTotalSize.w;
			if( vTotalSize.h > sizeLayoutChilds.h )
				sizeLayoutChilds.h = vTotalSize.h;
		}
	}
	return sizeLayoutChilds;
}


/**
 @brief 모든 차일드의 위치와 크기를 계산해서 가장 좌상귀의 좌표와 최대 크기를 얻어낸다
*/
// 이 함수는 무조건 자기 자식들의 크기만 돌려준다. 자식이 없으면 null값을 돌려준다.
void XWnd::GetMaxLTPosByChild( XE::VEC2 *pOutLT, XE::VEC2 *pOutRB )
{
	XE::VEC2 vLTMin(99999.f, 99999.f), vRBMax(-99999.f, -99999.f);
	XE::VEC2 vLT = GetPosLocal();
	XE::VEC2 vRB = vLT + GetSizeLocal();
	// 모든자식들의 최소/최대값을 구한다.
	for( auto pChild : m_listItems ) {
		XE::VEC2 vChildLocalLT, vChildLocalRB;
		// pChild의 자식들의 최소/최대값
		pChild->GetMaxLTPosByChild( &vChildLocalLT, &vChildLocalRB );
		if( pChild->GetNumChild() == 0 )	{
			// pChild의 자식이 없으면 pChild의 크기만으로 최소/최대값을 구한다.
			XE::VEC2 v = pChild->GetPosScreen();
			XE::VEC2 v2 = v + pChild->GetSizeFinal();
			vLTMin.x = std::min( vLTMin.x, v.x );
			vLTMin.y = std::min( vLTMin.y, v.y );
			vRBMax.x = std::max( vRBMax.x, v2.x );
			vRBMax.y = std::max( vRBMax.y, v2.y );
		} else		{
			// pChild의 크기와 pChild의 차일드크기를 더한다.

			XE::VEC2 v = pChild->GetPosScreen();
			XE::VEC2 v2 = v + pChild->GetSizeFinal();
			XE::VEC2 vlt, vrb;
			vlt.x = std::min( v.x, vChildLocalLT.x );
			vlt.y = std::min( v.y, vChildLocalLT.y );
			vrb.x = std::max( v2.x, vChildLocalRB.x );
			vrb.y = std::max( v2.y, vChildLocalRB.y );
			vLTMin.x = std::min( vLTMin.x, vlt.x );
			vLTMin.y = std::min( vLTMin.y, vlt.y );
			vRBMax.x = std::max( vRBMax.x, vrb.x );
			vRBMax.y = std::max( vRBMax.y, vrb.y );
		}
	}
	// 자식들의 최소/최대값만 리턴한다.
	*pOutLT = vLTMin;
	*pOutRB = vRBMax;
}
/**
 @brief this의 GetSizeLocal()을 얻는다. 그러나 사이즈가 invalid하면 부모의 크기로받는다.
 @param pParent 부모를 직접 지정할수있다.
*/
XE::VEC2 XWnd::GetSizeValidNoTrans()
{
	XE::VEC2 vSize = GetSizeLocalNoTrans();
	// this의 사이즈가 invalid하면 부모의 사이즈를 얻는다.
	if( vSize.IsInvalid() ) {
		if( GetpParent() )
			return GetpParent()->GetSizeValidNoTrans();
		// 부모가 없다면 전체 게임화면사이즈를 돌려준다.
		return XE::GetGameSize();
	} 
	return vSize;
}

float XWnd::GetSizeValidNoTransWidth()
{
	float width = GetSizeLocalNoTrans().w;
	// this의 사이즈가 invalid하면 부모의 사이즈를 얻는다.
	if( width <= 0 ) {
		if( GetpParent() )
			return GetpParent()->GetSizeValidNoTransWidth();
		return XE::GetGameSize().w;
	}
	return width;
}

float XWnd::GetSizeValidNoTransHeight()
{
	float height = GetSizeLocalNoTrans().h;
	// this의 사이즈가 invalid하면 부모의 사이즈를 얻는다.
	if( height <= 0 ) {
		if( GetpParent() )
			return GetpParent()->GetSizeValidNoTransHeight();
		// 부모가 없다면 전체 게임화면사이즈를 돌려준다.
		return XE::GetGameSize().h;
	}
	return height;
}

// this의 부모중에 가로 align이 있는 부모를 찾는다.
XWnd* XWnd::GetpParentHaveAlignH() 
{
	auto pParent = GetpParent();
	if( pParent && (pParent->GetAlign() & (xALIGN_HCENTER | xALIGN_RIGHT)))
		return pParent;
	return nullptr;
}

// this의 부모중에 세로 align이 있는 부모를 찾는다.
XWnd* XWnd::GetpParentHaveAlignV() 
{
	auto pParent = GetpParent();
	if( pParent && ( pParent->GetAlign() & ( xALIGN_VCENTER | xALIGN_BOTTOM ) ) )
		return pParent;
	return nullptr;
}

/**
 @brief 자동으로 부모의 크기에 맞춰 가로의 중앙으로 맞춰준다.
 @param pParent null일경우 최상위라고 판단하고 게임화면 전체를 기준으로 한다.
 @note 이것은 부모가 0,0 ~ w,h 일때를 가정한것이다. 그러나 부모가 스프라이트 
       객체라면 vLT가 반드시 0,0이라는 보장이 없으므로 boundbox를 구해서 계산하는게 맞다. XWndImage::Update()
*/
void XWnd::AutoLayoutHCenter( XWnd *pParent )
{
	// 폭이 있는 부모를 찾음.
	if( pParent )
		pParent = ( pParent->GetSizeLocalNoTrans().w > 0 ) ? pParent : pParent->GetpParentValidWidth();
	if( !pParent )
		return;
	const float wParent = ( pParent ) ? pParent->GetSizeValidNoTransWidth() : XE::GetGameSize().w;
	const float wLocalLayout = GetSizeNoTransLayout().w * m_vScale.x;
	const float left = wParent * 0.5f - wLocalLayout * 0.5f;
	SetX( left );
// 	if( pParent )
// 		pParent = (pParent->IsValidSize())? pParent : pParent->GetpParentValid();
// 	const XE::VEC2 vSizeParent = (pParent)? pParent->GetSizeValidNoTrans() : XE::GetGameSize();
// // 	const auto rectBB = pParent->GetBoundBoxByVisibleNoTrans();
// 	const XE::VEC2 sizeLocalLayout = GetSizeNoTransLayout() * m_vScale;
// 	const XE::VEC2 vPosLT = vSizeParent * 0.5f - sizeLocalLayout * 0.5f;
// 	const XE::VEC2 vPosLocal = GetPosLocal();
// 	SetX( vPosLT.x );
}

/**
 자동으로 부모의 크기에 맞춰 세로의 중앙으로 맞춰준다.
*/
void XWnd::AutoLayoutVCenter( XWnd *pParent )
{
	if( pParent )
		pParent = ( pParent->GetSizeLocalNoTrans().h > 0 ) ? pParent : pParent->GetpParentValidHeight();
	const float hParent = ( pParent ) ? pParent->GetSizeValidNoTransHeight() : XE::GetGameSize().h;
	const float hLocalLayout = GetSizeNoTransLayout().h * m_vScale.y;
	const float top = hParent * 0.5f - hLocalLayout * 0.5f;
	SetY( top );
// 	if( pParent )
// 		pParent = ( pParent->IsValidSize() ) ? pParent : pParent->GetpParentValid();
// 	const XE::VEC2 vSizeParent = ( pParent ) ? pParent->GetSizeValidNoTrans() : XE::GetGameSize();
// 	const XE::VEC2 sizeLocalLayout = GetSizeNoTransLayout();
// 	const XE::VEC2 vPosLT = vSizeParent / 2.f - sizeLocalLayout / 2.f;
// 	const XE::VEC2 vPosLocal = GetPosLocal();
// 	SetY( vPosLT.y );
}

/**
 자동으로 부모의 크기에 맞춰 가로세로 정중앙에 맞춰준다.
*/
void XWnd::AutoLayoutCenter( XWnd *pParent )
{
	// 가로,세로값중 하나만 유효한 윈도우도 있어서 따로 함.
	AutoLayoutHCenter( pParent );
	AutoLayoutVCenter( pParent );
// 	if( pParent )
// 		pParent = ( pParent->IsValidSize() ) ? pParent : pParent->GetpParentValid();
// 	const XE::VEC2 vSizeParent = (pParent)? pParent->GetSizeValidNoTrans() : XE::GetGameSize();
// 	const XE::VEC2 sizeLocalLayout = GetSizeNoTransLayout();
// 	const XE::VEC2 vPosLT = vSizeParent / 2.f - sizeLocalLayout / 2.f;
// 	SetPosLocal( vPosLT );
}

void XWnd::AutoLayoutRight( XWnd *pParent )
{
	if( pParent )
		pParent = ( pParent->GetSizeLocalNoTrans().w > 0 ) ? pParent : pParent->GetpParentValidWidth();
	const float wParent = ( pParent ) ? pParent->GetSizeValidNoTransWidth() : XE::GetGameSize().w;
	const float wLocalLayout = GetSizeNoTransLayout().w * m_vScale.x;
	const float left = wParent - wLocalLayout;
	SetX( left );
// 	if( pParent )
// 		pParent = ( pParent->IsValidSize() ) ? pParent : pParent->GetpParentValid();
// 	XE::VEC2 vSizeParent = ( pParent ) ? pParent->GetSizeValidNoTrans() : XE::GetGameSize();
// // 	const auto rectBB = pParent->GetBoundBoxByVisibleNoTrans();
// 	XE::VEC2 sizeLocalLayout = GetSizeNoTransLayout();
// 	XE::VEC2 vPosLT = vSizeParent - sizeLocalLayout;
// 	XE::VEC2 vPosLocal = GetPosLocal();
// 	vPosLocal.x = vPosLT.x;
// 	SetPosLocal( vPosLocal );
}

void XWnd::AutoLayoutBottom( XWnd *pParent )
{
	if( pParent )
		pParent = ( pParent->GetSizeLocalNoTrans().h > 0 ) ? pParent : pParent->GetpParentValidHeight();
	const float hParent = ( pParent ) ? pParent->GetSizeValidNoTransHeight() : XE::GetGameSize().h;
	const float hLocalLayout = GetSizeNoTransLayout().h * m_vScale.y;
	const float top = hParent - hLocalLayout;
	SetY( top );
// 	if( pParent )
// 		pParent = ( pParent->IsValidSize() ) ? pParent : pParent->GetpParentValid();
// 	XE::VEC2 vSizeParent = ( pParent ) ? pParent->GetSizeValidNoTrans() : XE::GetGameSize();
// // 	const auto rectBB = pParent->GetBoundBoxByVisibleNoTrans();
// 	XE::VEC2 sizeLocalLayout = GetSizeNoTransLayout();
// 	XE::VEC2 vPosLT = vSizeParent - sizeLocalLayout;
// 	XE::VEC2 vPosLocal = GetPosLocal();
// 	vPosLocal.y = vPosLT.y;
// 	SetPosLocal( vPosLocal );
}

/**
 @brief align값에 의해 부모를 기준으로 자동으로 m_vPos값을 정렬한다.
*/
void XWnd::AutoLayoutByAlign( XWnd *pParent, XE::xAlign align )
{
	// left align의 경우는 현재값을 그대로 둔다.
	if( align && align != xALIGN_LEFT ) {
		// 수평정렬
		if( align & xALIGN_LEFT ) {
			SetX(0);
		} else
		if( align & xALIGN_HCENTER )
			AutoLayoutHCenter( pParent );
		else
		if( align & xALIGN_RIGHT )
			AutoLayoutRight( pParent );
		// 수직정렬
		if( align & xALIGN_TOP )
			SetY(0);
		else
		if( align & xALIGN_VCENTER )
			AutoLayoutVCenter( pParent );
		else
		if( align & xALIGN_BOTTOM )
			AutoLayoutBottom( pParent );
	}
}

void XWnd::AutoLayoutHCenterWithAry( XArrayLinearN<XWnd*, 256>& aryChilds, float marginLR )
{
	if( aryChilds.size() == 0 )
		return;
	auto sizeLocalThis = GetSizeLocal();
	if( sizeLocalThis.IsInvalid() )
		sizeLocalThis = GetSizeNoTransLayout();
	if( sizeLocalThis.w <= 0 )
		return;		// 사이즈를 못구하면 취소
	// this의 전체 폭에서 양쪽마진 크기를 빼준다.
	sizeLocalThis.w -= marginLR * 2.f;	
	//
	XE::VEC2 v( marginLR, 0 );
	XARRAYLINEARN_LOOP( aryChilds, XWnd*, pWnd ) {
		// 이함수의 콜은 aryChilds들의 바로 상위 부모로부터 콜이되어야 한다.
		XBREAK( pWnd->GetpParent()->getid() != getid() );
		XE::VEC2 vSizeElem;
		// element당 차지할 크기
		vSizeElem.w = sizeLocalThis.w / aryChilds.size();
//		XE::VEC2 vSizeWnd = pWnd->GetSizeFinal();
		const auto scaleWnd = pWnd->GetScaleLocal();
		const auto sizeLocalLayoutByWnd = pWnd->GetSizeNoTransLayout() * scaleWnd;
		XE::VEC2 vl = pWnd->GetPosLocal();
		vl.x = (v.x + vSizeElem.w * 0.5f) - (sizeLocalLayoutByWnd.w * 0.5f);
//		vl.y = pWnd->GetPosLocal().y;
		pWnd->SetPosLocal( vl );
		v.x += vSizeElem.w;
	} END_LOOP;
}

/**
 @brief ary의 윈도우들을 중앙 정렬시킨다.
 @param marginLR 마진이 있다는것은 양쪽 마진을 남겨두고 윈도우들을 균등하게 배분한다는 의미다. 다시말해 윈도우사이에 틈이 생긴다는 것이다.
                 elem들 사이에 틈이 없게 하려면 마진을 0으로 준다.
 위에 XArrayLinearN버전과는 약간 다르다. 위는 무조건 좌측 마진위치부터 시작하며 elem사이에 틈이 생긴다.
*/
void XWnd::AutoLayoutHCenterWithAry( XVector<XWnd*>& aryWnd, float marginLR )
{
	if( aryWnd.size() == 0 )
		return;
	auto sizeLocalThis = GetSizeLocal();
	if( sizeLocalThis.IsInvalid() )
		sizeLocalThis = GetSizeNoTransLayout();
	if( sizeLocalThis.w <= 0 )
		return;		// 사이즈를 못구하면 취소
	// this의 전체 폭에서 양쪽마진 크기를 빼준다.
	sizeLocalThis.w -= marginLR * 2.f;
	float wTotal = 0;
	if( marginLR > 0 ) {
		wTotal = sizeLocalThis.w / aryWnd.Size();
	}
	// ary에 담긴 윈도우들의 최대 레이아웃 크기
	const auto sizeAryLayout = GetSizeNoTransLayoutWithAry( aryWnd );
	XE::VEC2 v( marginLR, 0 );
	if( marginLR == 0 ) {
		// 마진값이 없으면 중앙을 기준으로 elem사이에 틈이없게 중앙정렬한다.
		v.x = (sizeLocalThis.w * 0.5f) - (sizeAryLayout.w * 0.5f);
	}
// 	XE::VEC2 wAreaElem;
	const float wAreaElem = sizeLocalThis.w / aryWnd.size();
	for( auto pWnd : aryWnd ) {
		// 이함수의 콜은 aryChilds들의 바로 상위 부모로부터 콜이되어야 한다.
//		XBREAK( pWnd->GetpParent()->getid() != getid() );
		// element당 차지할 크기
		if( marginLR > 0 ) {
			// 마진이 있으면 마진을 뺀 전체크기에서 elem개수로 나눠 한 elem당 차지하는 폭을 사용한다.
			const auto scaleWnd = pWnd->GetScaleLocal();
			const auto sizeLocalLayoutByWnd = pWnd->GetSizeNoTransLayout() * scaleWnd;
			XE::VEC2 vl = pWnd->GetPosLocal();
			vl.x = ( v.x + wAreaElem * 0.5f ) - ( sizeLocalLayoutByWnd.w * 0.5f );
			pWnd->SetPosLocal( vl );
			v.x += wAreaElem;
		} else {
			pWnd->SetPosLocal( v );
			v.x += pWnd->GetSizeLocal().w;
		}
	}
}

/**
 @brief aryChild윈도우들을 세로로 중앙정렬하여 나열한다.
 @param marginTB 위아래 마진
 @param dist elem간 간격
*/
void XWnd::AutoLayoutVCenterWithAry( XVector<XWnd*>& aryChilds, float marginTB )
{
	const int num = aryChilds.size();
	if( num == 0 )
		return;
	auto sizeLocalThis = GetSizeValidNoTrans();
	if( sizeLocalThis.h <= 0 )
		return;		// 사이즈를 못구하면 취소
	// this의 전체 크기에서 양쪽마진 크기를 빼준다.
	sizeLocalThis.h -= marginTB * 2.f;	
	float hSector = sizeLocalThis.h / num;		// 양쪽마진을 뺀공간을 elem개수만큼 나눠 섹터당 크기를 구한다.
	//
	XE::VEC2 v( 0, marginTB );
	for( auto pWnd : aryChilds ) {
		// 이함수의 콜은 aryChilds들의 바로 상위 부모로부터 콜이되어야 한다.
		XBREAK( pWnd->GetpParent()->getid() != getid() );
		const auto scaleWnd = pWnd->GetScaleLocal();
		const auto sizeLocalLayoutByWnd = pWnd->GetSizeNoTransLayout() * scaleWnd;
		XE::VEC2 vl;// = pWnd->GetPosLocal();
		vl.y = v.y + (hSector * 0.5f) - (sizeLocalLayoutByWnd.h * 0.5f);
//		vl.y = pWnd->GetPosLocal().y;
		pWnd->SetY( vl.y );
		v.y += hSector;
	}
}

void XWnd::AutoLayoutVCenterByChilds( float marginTB )
{
	XVector<XWnd*> ary;
	for( auto pWnd : m_listItems ) {
		ary.Add( pWnd );
	}
	AutoLayoutVCenterWithAry( ary, marginTB );
}

/**
 @brief this의 child윈도우들을 중앙정렬 한다.
 child윈도우들의 크기는 모두 같다고 가정한다.
*/
void XWnd::AutoLayoutHCenterByChilds( float marginLR )
{
	if( m_listItems.size() == 0 )
		return;
	auto sizeLocalThis = GetSizeLocalNoTrans();
	if( sizeLocalThis.IsZero() )
		sizeLocalThis = GetSizeNoTransLayout();
	if( sizeLocalThis.w <= 0 )
		return;		// 사이즈를 못구하면 취소
	// this의 전체 폭에서 양쪽마진 크기를 빼준다.
	sizeLocalThis.w -= marginLR * 2.f;	
	//
	// element당 차지할 크기
	const XE::VEC2 vSizeElem = sizeLocalThis / (float)m_listItems.size();
	XE::VEC2 v( marginLR, 0 );
	for( auto pChild : m_listItems ) {
		static auto sizeLocalLayoutByWnd = pChild->GetSizeNoTransLayout();
		XE::VEC2 vl = pChild->GetPosLocal();
		vl.x = (v.x + vSizeElem.w * 0.5f) - (sizeLocalLayoutByWnd.w * 0.5f);
		pChild->SetPosLocal( vl );
		v.x += vSizeElem.w;
	}
}

/**
 @brief cIdentifier를 키로 갖고 있는 윈도우를 찾아 삭제시킨다.
*/
void XWnd::DestroyWndByIdentifier( const char *cIdentifier )
{
	if( XE::IsEmpty(cIdentifier) )
		return;
	XWnd *pWnd = Find( cIdentifier );
	if( pWnd  )
		pWnd->SetbDestroy( TRUE );
}

void XWnd::DestroyWndByIdentifierf( const char *cIdentifierFormat, ... )
{
	char cIdentifier[ 256 ];
	va_list vl;
	va_start( vl, cIdentifierFormat );
	vsprintf_s( cIdentifier, cIdentifierFormat, vl );
	va_end( vl );
	return DestroyWndByIdentifier( cIdentifier );
}

/**
 @brief pRoot트리아래서 윈도우좌표 lx, ly지점을 포함하는 최상위 드랍가능한 윈도우를 얻는다.
*/
XWnd* XWnd::sGetInsideWnd( XWnd *pBase, float lx, float ly ) 
{ 
	XE::VEC2 vMouseLocal( lx, ly );
	// child
	for( auto rIter = pBase->m_listItems.rbegin(); rIter != pBase->m_listItems.rend(); rIter++ )	{
		XWnd *pWnd = (*rIter);
		if( pWnd->GetbShow() )
		{
// 			XE::VEC2 vChildLocal;
// 			if( pBase->m_pParent )
// 				vChildLocal = vMouseLocal - (pWnd->GetPosLocal() * pBase->GetScaleLocal());
// 			else
// 				vChildLocal = vMouseLocal - pWnd->GetPosLocal();
			const XE::VEC2 vChildLocal = pWnd->XWnd::GetvChildLocal( vMouseLocal, pBase, pBase->m_pParent );
			const XE::VEC2 vInLocal = pWnd->GetvChildLocal( vMouseLocal, pBase, pBase->m_pParent );		// 영역체크 전용
			BOOL bAreaIn = pWnd->IsWndAreaIn( vInLocal );
//			if( (pWnd->IsWndAreaIn( vChildLocal ) ||		// 터치가 윈도영역 위에 있거나
			if( ( bAreaIn ||
				pWnd->GetSizeLocal().IsZero() ||	// 윈도사이즈가 0,0일경우 통과
				pWnd->GetSizeLocal().IsMinus() ) &&		// 윈도영역이 -1이거나
				s_listCaptureDel.Find(pWnd) == FALSE )		// 캡쳐윈도우의 경우 두번실행되지 않게 하기 위해
			{
				XWnd *pInside = NULL;
				pInside = sGetInsideWnd( pWnd, vChildLocal.x, vChildLocal.y ); 
				if( pInside && pInside->IsDropWnd() ) {
					return pInside;
				} else {
				if( pWnd->IsDropWnd() )
					return pWnd;
				else
					return nullptr;
				}
			}
		}
	}
	return NULL;
}

void XWnd::GetMatrix( MATRIX *pOut )
{
	MATRIX mTrans, mScale, mRot, mWorld/*, mAxis, mReverseAxis*/;
	const auto vPos = GetPosLocal();
	MatrixTranslation( mTrans, vPos.x, vPos.y, 0.f );
	MatrixScaling( mScale, m_vScale.x, m_vScale.y, 1.f );
	MatrixIdentity( mRot );
	MatrixMultiply( mWorld, mScale, mRot );
	MatrixMultiply( *pOut, mWorld, mTrans );

}

/**
 @brief 하위자식들의 절대좌표를 base기준 상대좌표로 바꾼다.
*/
void XWnd::ConvertChildPosToLocalPos( const XE::VEC2& vBase )
{
	for( auto pWnd : m_listItems )	{
		XE::VEC2 vLocal = pWnd->GetPosFinal() - vBase;
		pWnd->SetPosLocal( vLocal );
	}
}

/**
 @brief sec초 주기로 자동으로 SetbUpdate(TRUE)를 호출한다.
*/
void XWnd::SetAutoUpdate( float sec )
{
// 	// 다른 주기로 변경하려면 일단 클리어하고 다시 불러야함.
// 	XBREAK( m_timerAutoUpdate.IsOn() && 
// 			m_timerAutoUpdate.GetWaitSec() != sec );		// clearAutoUpdate()사용
	// 기존 타이머와 주기가 같으면 그냥 리턴
	if( m_timerAutoUpdate.IsOn() && 
		m_timerAutoUpdate.GetWaitSec() == sec )
		return;
	if( sec > 0 )
	{
		m_timerAutoUpdate.Set( sec );
		// 시작하자마자 한번 업데이트 할수 있도록.
		m_timerAutoUpdate.SetPassSec( m_timerAutoUpdate.GetWaitSec() );
	}
	else
		m_timerAutoUpdate.Off();
}


void XWnd::DrawDebugInfo( float x, float y, XCOLOR col, XBaseFontDat* pFontDat )
{
	for( auto pWnd : m_listItems ) {
		pWnd->DrawDebugInfo( x, y, col, pFontDat );
	}
}


/**
 @brief SetbTouchable()을 자식들에게까지 호출하는 함수. 
 현재 Touchable시스템 설계가 꼬여서 이렇게 땜빵함수를 만들었음.
*/
void XWnd::SetbTouchableWithChild( bool bFlag )
{
	BOOL b = (bFlag)? TRUE : FALSE;
	SetbTouchable( b );
	for( auto pWnd : m_listItems ) {
		pWnd->SetbTouchableWithChild( bFlag );
	}}

void XWnd::SetstrIdentifierf( const char *cIdentifierFormat, ... )
{
	char cIdentifier[ 256 ];
	va_list vl;
	va_start( vl, cIdentifierFormat );
	vsprintf_s( cIdentifier, cIdentifierFormat, vl );
	va_end( vl );
	SetstrIdentifier( cIdentifier );
}

void XWnd::GetDebugString( _tstring& strOut )
{
	_tstring sid = C2SZ(GetstrIdentifier().c_str());
	strOut +=  XFORMAT("id=0x%08X, ids=%s", m_id, sid.c_str() );;
}

// XWndImage* XWnd::GetImageCtrl( const char *cKey ) const
// {
// 	XWnd *pWnd = Find( cKey );
// 	if( pWnd == nullptr )
// 		return nullptr;
// 	return SafeCast<XWndImage*, XWnd*>( pWnd );
// }

/**
 @brief pBase로부터의 this,의 좌표를 구한다.
*/
XE::VEC2 XWnd::GetPosFromBase( XWnd *pBase )
{
	auto vThis = GetPosFinal();
	auto vBase = pBase->GetPosFinal();
	return vThis - vBase;
}

BOOL XWnd::IsWndAreaIn( float lx, float ly ) 
{
	int w = (int)GetSizeFinal().w;
	int h = (int)GetSizeFinal().h;
	if( XE::IsArea2( 0, 0, w, h, lx, ly ) )
		return TRUE;
	return FALSE;
}
/**
 @brief 스크린전체좌표로 검사하는 버전
*/
bool XWnd::IsWndAreaInByScreen( float x, float y )
{
	auto vPos = GetPosScreen();
	auto vSize = GetSizeFinal();
	if( XE::IsArea( vPos, vSize, XE::VEC2(x,y) ) )
		return true;
	return false;
}
/**
 @brief this의 부모윈도우를 돌려주는데 사이즈가 없는것은 그룹윈도우로 쓰이므로 그 상위 윈도우를 돌려준다.
*/
XWnd* XWnd::GetpParentValid()
{
	if( m_pParent && m_pParent->GetSizeLocal().IsInvalid() )
		return m_pParent->GetpParentValid();
	return m_pParent;
}

/**
 @brief 가로폭만 유효한값을 가진 부모를 찾는다.
*/
XWnd* XWnd::GetpParentValidWidth()
{
	if( m_pParent && m_pParent->GetSizeLocal().w <= 0 )
		return m_pParent->GetpParentValidWidth();
	return m_pParent;
}

/**
 @brief 세로폭만 유효한값을 가진 부모를 찾는다.
*/
XWnd* XWnd::GetpParentValidHeight()
{
	if( m_pParent && m_pParent->GetSizeLocal().h <= 0 )
		return m_pParent->GetpParentValidHeight();
	return m_pParent;
}

float XWnd::GetWidthFinalValid() 
{
	if( m_vSize.w <= 0 ) {
		if( m_pParent )
			return m_pParent->GetWidthFinalValid();
	}
	return m_vSize.w * GetScaleFinal().x;
}

float XWnd::GetHeightFinalValid()
{
	if( m_vSize.h <= 0 ) {
		if( m_pParent )
			return m_pParent->GetHeightFinalValid();
	}
	return m_vSize.h * GetScaleFinal().y;
}

/**
 @brief 최종 화면에  렌더되는 위치와 크기의 바운딩박스 영역을 얻는다.
*/
XE::xRECT XWnd::GetBoundBoxByVisibleFinal()
{
	const auto bbNoTrans = GetBoundBoxByVisibleNoTrans();
	const auto vLT = XWnd::GetPosFinal() + bbNoTrans.GetvLT();
	const auto vSizeFinal = bbNoTrans.GetSize() * GetScaleFinal();
	return XE::xRECT( vLT, vSizeFinal );
}

/**
 @brief 모든 자식윈도우에게 string형태의 메시지를 보낸다.
*/
void XWnd::SendMsgToChilds( const std::string& strMsg )
{
	for( auto pWnd : m_listItems ) {
		if( !pWnd->IsDestroy() ) {
			pWnd->ProcessMsg( strMsg );
		}
	}
}