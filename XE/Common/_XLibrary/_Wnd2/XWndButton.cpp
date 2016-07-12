#include "stdafx.h"
#include "XWndButton.h"
//#include "XWindow.h"
#include "etc/xUtil.h"
#include "sprite/SprObj.h"
#ifdef WIN32
#include "_DirectX/XGraphicsD3DTool.h"
#else
#endif
#include "XFramework/client/XClientMain.h"
#include "XFramework/client/XEContent.h"
#include "XSoundMng.h"
using namespace XE;

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////
// static

//int (XWnd::*XWndButton::s_phSoundDown)( XWnd*, DWORD dwParam1, DWORD dwParam2 ) = NULL;
XWND_MESSAGE_MAP XWndButton::s_mmSoundDown;		// 디폴트 메시지. 사운드.
XWND_MESSAGE_MAP XWndButton::s_mmCreate;		
XWND_MESSAGE_MAP XWndButton::s_mmQuestClicked;		// 디폴트 메시지. 버튼이 눌리면 퀘스트 객체로 이벤트 전달.
XE::xtButtAni XWndButton::s_modeAnimationDefault = XE::xBA_NORMAL;	///< 애니메이션 모드 디폴트값

/*
XWndButton* XWndButton::Find( XWndMng *pWndMng, ID idWnd ) 
{
	XWnd *pWnd = pWndMng->FindWnd( idWnd );
	if( pWnd == NULL )	return NULL;
	return dynamic_cast<XWndButton*>( pWnd );
}
*/
// 생성자
XWndButton::XWndButton( float x, float y, LPCTSTR szSpr,
					   DWORD idActUp, DWORD idActDown, DWORD idActDisable,
					   XEToolTip *pToolTip, BOOL bSrcKeep )
	: XWnd( x, y )
{
	Init();
	SetprefToolTip( pToolTip );
	//
	if( XE::IsHave( szSpr ) )
	{
		// 이 버튼이 독자적으로 쓸 sprobj 생성
		XSprObj *pSprObj = m_pSprObj = new XSprObj( bSrcKeep, szSpr );
		Create( pSprObj, idActUp, idActDown, idActDisable );
	}
}

void XWndButton::Destroy() 
{
	SAFE_RELEASE2( IMAGE_MNG, m_pSurface[0] );
	SAFE_RELEASE2( IMAGE_MNG, m_pSurface[1] );
	SAFE_RELEASE2( IMAGE_MNG, m_pSurface[2] );
	SAFE_DELETE( m_pSprObj );
}

void XWndButton::Create( XSprObj *pSprObj, DWORD idActUp, DWORD idActDown, DWORD idActDisable ) 
{
	XBREAK( idActUp == 0 );
	m_idActUp = idActUp;
	if( idActDown == 0 )
		m_idActDown = idActUp;	// +1방식이었는데 바뀜
	else 
		m_idActDown = idActDown;
	if( idActDisable == 0 )
		m_idActDisable = idActUp;
	else
		m_idActDisable = idActDisable;

	// 이 버튼이 독자적으로 쓸 sprobj 생성
	m_pSprObj = pSprObj;
	m_pSprObj->SetAction( GetidActUp() );
	//	m_pSprObj->FrameMove(0);
	SetSizeLocal( m_pSprObj->GetWidth(), m_pSprObj->GetHeight() );
}

// 생성자
XWndButton::XWndButton( float x, float y, XSprObj *pSprObj,
									DWORD idActUp, DWORD idActDown, DWORD idActDisable ) 
	: XWnd( x, y ) 
{
	Init();
//	Create( x, y, pSprObj, idActUp, idActDown, idActDisable );
	Create( pSprObj, idActUp, idActDown, idActDisable );
}
XWndButton::XWndButton( float x, float y, LPCTSTR szImgUp, 
													LPCTSTR szImgDown, 
													LPCTSTR szImgDisable, 
													XEToolTip *pToolTip, DWORD xfFlag, BOOL _bSrcKeep )
	: XWnd( x, y )
{
	Init();
	bool bSrcKeep = _bSrcKeep != FALSE;
//	XBREAK( szImgUp == NULL );
	SetprefToolTip( pToolTip );
	if( XE::IsHave( szImgUp ) )	{
		m_pSurface[0] = IMAGE_MNG->Load( XE::MakePath( DIR_UI, szImgUp ), XE::xPF_ARGB8888, bSrcKeep );
		if( m_modeAnimaion == XE::xBA_NORMAL ) {
			if( XE::IsHave( szImgDown ) )
				m_pSurface[ 1 ] = IMAGE_MNG->Load( XE::MakePath( DIR_UI, szImgDown ), XE::xPF_ARGB8888, bSrcKeep );
		}
		if( XE::IsHave( szImgDisable ) )
			m_pSurface[2] = IMAGE_MNG->Load( XE::MakePath( DIR_UI, szImgDisable ), XE::xPF_ARGB8888, bSrcKeep );
		if( m_pSurface[0] )
			SetSizeLocal( m_pSurface[0]->GetSize() );

	}
}

void XWndButton::OnPlaySound( ID id )
{
	SOUNDMNG->OpenPlaySound( id );
}

// void XWndButton::Update()
// {
// 	auto pParentValid = GetpParentValid();
// 	if( m_xfFlag & xfALIGN_HCENTER )	{
// 		AutoLayoutHCenter( pParentValid );
// 	}
// 	if( m_xfFlag & xfALIGN_VCENTER ) {
// 		AutoLayoutVCenter( pParentValid );
// 	}
// 	if( m_xfFlag & xfALIGN_RIGHT ) {
// 		AutoLayoutRight( pParentValid );
// 	}
// 	if( m_xfFlag & xfALIGN_BOTTOM ) {
// 		AutoLayoutBottom( pParentValid );
// 	}
// 	XWnd::Update();
// }
void XWndButton::SetFlag( DWORD xfFlag, XWnd *pParent )
{
//	m_xfFlag = xfFlag;
	SetbUpdate( true );
// 	if( xfFlag & xfALIGN_HCENTER )	{
// 		AutoLayoutHCenter( pParent );
// 	}
// 	if( xfFlag & xfALIGN_VCENTER ) {
// 		AutoLayoutVCenter( pParent );
// 	}
}

// 버튼이미지를 교체
// 0: up, 1:down, 2:disable
void XWndButton::SetpSurface( int idx, LPCTSTR szRes )
{
	XBREAK( idx < 0 || idx > 2 );
	SAFE_RELEASE2(IMAGE_MNG, m_pSurface[ idx ] );
	if( szRes ) {
		_tstring strImg = szRes;
		m_pSurface[ idx ] = IMAGE_MNG->Load( strImg, XE::xPF_ARGB8888 );
		if( idx == 0 && XASSERT(m_pSurface[ idx ]) )
			SetSizeLocal( m_pSurface[ idx ]->GetSize() );
	}
}
/**
 @brief 글로우 이펙트를 추가한다.
 @param szPostfix 글로우이펙트 파일명은 0번서피스의 파일명+szPostfix가 된다. 디폴트로는 _glow가 된다.
 @parama bOn 이펙트셋과 동시에 이펙을 켤것인지 말것인지.
*/
void XWndButton::SetGlow( LPCTSTR szPostfix, const XE::VEC2& vAdj, bool bOn )
{
	m_vAdjGlow = vAdj;
	if( m_pSurface[0] ) {
		_tstring strImg, strExt;
		strImg = XE::GetFileTitle( m_pSurface[0]->GetstrRes() );
		strExt = XE::GetFileExt( m_pSurface[0]->GetstrRes() );
		_tstring strPostfix = (XE::IsEmpty(szPostfix))? _T("_glow") : szPostfix;
		strImg += strPostfix + _T(".") + strExt;
		bool bLoad = false;
		if( m_pSurface[ 3 ] ) {
			_tstring strExist = XE::GetFileName( m_pSurface[ 3 ]->GetstrRes() );
			if( strExist != strImg ) {
				SAFE_RELEASE2( IMAGE_MNG, m_pSurface[3] );
				bLoad = true;
			}
		} else
			bLoad = true;
		if( bLoad ) {
			m_pSurface[ 3 ] = IMAGE_MNG->Load( XE::MakePath( DIR_UI, strImg ), XE::xPF_ARGB8888 );
			m_timerGlow.Set( 1.f );
		}
	}
}

/**
 @brief spr만 읽는다.
*/
void XWndButton::LoadSprObj( LPCTSTR szSpr, ID idAct )
{
	SAFE_DELETE( m_pSprObj );
	m_pSprObj = new XSprObj( szSpr );
	if( XBREAK( idAct == 0 ) )
		idAct = 1;
	m_pSprObj->SetAction( idAct );
}
// 새로주어지는 szSpr로 m_pSprObj를 다시 생성한다.
void XWndButton::SetSprObj( LPCTSTR szSpr, ID idActUp, ID idActDown )
{
	SAFE_DELETE( m_pSprObj );
	XSprObj *pSprObj = m_pSprObj = new XSprObj( szSpr );
	if( idActUp != 0 )
		m_idActUp = idActUp;
	pSprObj->SetAction( GetidActUp() );
	if( m_modeAnimaion == XE::xBA_NORMAL ) {
		if( idActDown != 0 )
			m_idActDown = idActDown;
	}
	SetSizeLocal( pSprObj->GetWidth(), pSprObj->GetHeight() );
}

/**
 @brief 버튼하나를 스프라이트로 지정한다.
 @param idxButt 0:up 1:down 2:disable
*/
void XWndButton::SetSprButton( int idxButt, LPCTSTR szSpr, ID idAct )
{
	if( !m_pSprObj || (m_pSprObj && !XE::IsSame(m_pSprObj->GetszFilename(), szSpr) ) ) {
		SAFE_DELETE( m_pSprObj );
		m_pSprObj = new XSprObj( szSpr );
	}
	switch( idxButt ) {
	case 0: m_idActUp = idAct; break;
	case 1: m_idActDown = idAct; break;
	case 2: m_idActDisable = idAct; break;
	default:
		XBREAK(1);
		break;
	}
	m_pSprObj->SetAction( idAct );
	if( idxButt == 0 )
		SetSizeLocal( m_pSprObj->GetSize() );
}


XWndButton::XWndButton( float x, float y, float w, float h, XEToolTip *pToolTip )
	: XWnd( x, y, w, h )
{
	Init();
	SetprefToolTip( pToolTip );
}

/**
 @brief 버튼이 애니메이션 중일땐 자동정렬을 금지시킴
*/
bool XWndButton::IsAbleAlign() const
{
	return( m_stateAni == 0 );
}

int XWndButton::Process( float dt )
{
	XWnd::Process( dt );
	if( m_pSprObj )
		m_pSprObj->FrameMove( dt );
	switch( m_modeAnimaion ) {
	case XE::xBA_BOUNCE: {
		if( m_stateAni != 0 ) {
			float lerp = m_timerAni.GetSlerp();
			if( lerp > 1.f )
				lerp = 1.f;
			float scale = 1.f;
			if( m_stateAni == 1 ) {		// 눌림 애니
				if( m_bPush )
					scale = 1.0f - ( lerp * 0.05f );	// 100%->90%로 변화
				else
					scale = 0.95f + ( lerp * 0.05f );	// 90 -> 100으로
			} else
			if( m_stateAni == 2 ) {		// 튀어나오며 바운스애니
				scale = 0.95f + (XE::xiSin( lerp, 0, 1, 0.5f ) * 0.1f);
				if( lerp == 1.f ) {
					m_stateAni = 0;
					DispatchEvent();
				}
			}
			SetScaleLocal( scale );
		}
	} break;
	case XE::xBA_MOVE: {
		if( m_stateAni != 0 ) {
			float lerp = m_timerAni.GetSlerp();
			if( lerp > 1.f )
				lerp = 1.f;
			float adj = 1.f;
			if( m_stateAni == 1 ) {		// 눌림 애니
				if( m_bPush )
					adj = lerp; // 0~1로 변화
				else
					adj = 1.f - lerp;  // 1~0으로 변화
			} else
			if( m_stateAni == 2 ) {		// 튀어나오며 바운스애니
				adj = 1.f - lerp;
				if( lerp == 1.f ) {
					m_stateAni = 0;
					DispatchEvent();
				}
			}
//			m_vAdjPush = XE::VEC2(2,2) * adj;
			static const XE::VEC2 sv( 2, 2 );
			const auto vAdjDraw = sv * adj;
			SetvAdjDrawLocal( vAdjDraw );
		}
	} break;
	}

	return 0;
}

void XWndButton::SetEnterAnimationByID( ID idActEnter )
{
	XBREAK( idActEnter == 0 );
	if( m_pSprObj && idActEnter )
	{
		m_idActEnter = idActEnter;
		m_pSprObj->SetAction( idActEnter, xRPT_1PLAY );
		float secPlayTime = m_pSprObj->GetPlayTime();
		XBREAK( secPlayTime == 0 );
		SetbAnimationEnterLeave( TRUE, secPlayTime );
	}
}
// spr을 사용하지 않는 버전
void XWndButton::SetEnterAnimationByTime( float sec )
{
	SetbAnimationEnterLeave( TRUE, sec );
}

void XWndButton::SetLeaveAnimationByID( ID idActLeave )
{
	XBREAK( idActLeave == 0 );
	if( m_pSprObj && idActLeave )
	{
		m_idActLeave = idActLeave;
		m_pSprObj->SetAction( idActLeave, xRPT_1PLAY );
		float secPlayTime = m_pSprObj->GetPlayTime();
		XBREAK( secPlayTime == 0 );
		SetbAnimationEnterLeave( TRUE, secPlayTime );
	}
}

void XWndButton::SetLeaveAnimationByTime( float sec )
{
	SetbAnimationEnterLeave( FALSE, sec );
}
// 등장 애니메이션 초기화
BOOL XWndButton::OnInitEnterLeaveAnimation( BOOL bEnter )
{
	if( bEnter ) {
		if( m_pSprObj )
			if( m_idActEnter )
				// 혹시 다른곳에서 액션이 바꼈을수도 있으니 다시한번 시도
				m_pSprObj->SetAction( m_idActEnter, xRPT_1PLAY );
	} else {
		if( m_pSprObj ) {
			if( m_idActLeave )
				m_pSprObj->SetAction( m_idActLeave, xRPT_1PLAY );
			else
				return FALSE; // 초기화 취소시키고 등장/퇴장 애니메이션도 취소
		}
	}
	return TRUE;
}

// 버튼의 등장/퇴장 처리. 버튼은 별도의 등장퇴장 애니메이션이 있을수 있다.
int XWndButton::OnProcessEnterLeaveAnimation( BOOL bEnter, float dt )
{
	if( m_pSprObj && ((m_idActEnter && bEnter) || 
					 (m_idActLeave && bEnter == FALSE )) ) {
		m_pSprObj->FrameMove( dt );
	} else
		// spr을 사용하지 않거나 등장/퇴장 애니메이션이 없으면 디폴트 동작을 하도록 한다.
		return XWnd::OnProcessEnterLeaveAnimation( bEnter, dt );
	return 1;
}

void XWndButton::OnFinishEnterLeave( BOOL bEnter )
{
/*	if( m_pSprObj && (m_idActEnter && bEnter) )
	{
		if( GetbEnable()
		if( m_bPush )
			m_pSprObj->SetAction( m_idActDown );
		else
			m_pSprObj->SetAction( m_idActUp );
	} */
}



void XWndButton::Draw( void )
{
	const auto posFinal = GetPosFinal();
	Draw( posFinal );
}

void XWndButton::Draw( const XE::VEC2& _vPos )
{
	auto vPos = _vPos + GetvAdjDrawFinal()/* + m_vAdjPush*/;		// 이함수만 단독으로 불릴수도 있어서 여기서 보정함.
	const auto blendFunc = GetblendFunc();
	const auto vPosLocal = GetPosLocal();
// 	const auto vOrig = XE::VEC2(0) + GetvAdjDrawFinal();		// 원점
	const auto vOrig = XE::VEC2( 0 ); // + GetvAdjDrawFinal();		// 원점
	const Vec3 v3Orig( vOrig.x, vOrig.y, 0.f );
	MATRIX mWorld;
	GetMatrix( &mWorld );
	Vec4 v4;
	MatrixVec4Multiply( v4, v3Orig, mWorld );
	vPos -= XE::VEC2( v4.x, v4.y ) - vPosLocal;
	if( m_bPush ) {
		// 눌린상태 draw
		if( !DrawButt( vPos, 1, blendFunc ) )
			DrawButt( vPos, 0, blendFunc );				// down이미지가 지정되어있지 않으면 up이미지로 대체한다.
	} else {
		// 뗀상태 draw
		if( m_bEnable ) {
			// 활성상태 draw
			DrawButt( vPos, 0, blendFunc );
		} else {
			// 비활성상태 draw
			if( !DrawButt( vPos, 2, XE::xBF_GRAY ) )
				DrawButt( vPos, 0, XE::xBF_GRAY );			// disable이미지가 지정되어있지 않다면 up이미지로 대체하고 gray로 그린다.
		}
	}
	// glow 덧씌움
	if( m_bShowGlow ) {
		auto pSurface = m_pSurface[ 3 ];		// 인덱스3은 글로우 이미지
		if( pSurface ) {
			const XE::VEC2 vScale = GetScaleFinal();
			const float rotZ = GetRotateFinal();
			const float alpha = GetAlphaFinal();
			const XE::VEC2 vSize = GetSizeLocal();
			//
			auto lerpTime = m_timerGlow.GetSlerp();
			if( lerpTime >= 1.f ) {
				lerpTime = 1.f;
				m_timerGlow.Reset();
			}
			auto a = 0.5f + (XE::xiCos( lerpTime, 0, 1, 0 ) * 0.5f);
			pSurface->SetfAlpha( alpha * a );
			pSurface->SetAdjustAxis( vSize * 0.5f );
			pSurface->SetBlendFunc( XE::xBF_ADD );
			pSurface->Draw( vPos + m_vAdjGlow );
// 		pSurface->Draw( vPos + XE::VEC2(-5,-6) );
		}
	}
	XWnd::Draw();
}

bool XWndButton::DrawButt( const XE::VEC2& vPos, int idxButt, XE::xtBlendFunc blendFunc )
{
	if( XBREAK(idxButt < 0 || idxButt > 2) )
		return false;
	const XE::VEC2 vScale = GetScaleFinal();
	const float rotZ = GetRotateFinal();
	const float alpha = GetAlphaFinal();
	const XE::VEC2 vSize = GetSizeLocal();
	bool bDraw = false;
	//
	if( m_pSurface[ idxButt ] ) {
		auto pSurface = m_pSurface[ idxButt ];
		// 해당인덱스버튼이 Surface로 지정되어있을때
		pSurface->SetScale( vScale );
		pSurface->SetRotateZ( rotZ );
		pSurface->SetfAlpha( alpha );
		pSurface->SetAdjustAxis( GetSizeLocalNoTrans() * 0.5f );
		pSurface->SetBlendFunc( blendFunc );
		pSurface->Draw( vPos );
		bDraw = true;
	}
	if( m_pSprObj ) {
		int idAct = 0;
		switch( idxButt ) {
		case 0:	idAct = m_idActUp;	break;
		case 1: idAct = m_idActDown; break;
		case 2: idAct = m_idActDisable; break;
		}
		if( idAct ) {
			// idxButt버튼이 spr로지정되어있음.
			if( m_pSprObj ) {
				m_pSprObj->SetScale( vScale );
				m_pSprObj->SetRotateZ( rotZ );
				m_pSprObj->SetfAlpha( alpha );
				m_pSprObj->Draw( vPos );
				bDraw = true;
			}
		}
	}
	
	return bDraw;
}


void XWndButton::OnNCMouseMove( float lx, float ly ) 
{
	// 영역을 벗어나든 아니든 무조건 들어오게 바껴서 삭제
//	m_bPush = FALSE;	// 영역을 벗어나면 뗀걸로 표시
}
/**
 @brief 
 @paramn lx/ly this컨트롤 기준 마우스 좌표
*/
void XWndButton::OnMouseMove( float lx, float ly ) 
{ 
	const XE::VEC2 vTouchByThis(lx,ly);
	const auto vLocalFromParent = GetPosLocal() + vTouchByThis;
//	const auto vInLocal = GetvChildLocal( vInThisLocal, m_pParent, m_pParent->GetpParent() );
// 	if( this->IsWndAreaIn( lx, ly ) )	{
	if( this->IsWndAreaIn( vLocalFromParent.x, vLocalFromParent.y ) ) {
		if( m_bFirstPush ) {	// ButtonDown이 this버튼에서 일어났었을때만 처리 this 버튼영역에 x, y가 있는지 검사
			if( m_bPush == FALSE )
				m_timerAni.Set( 0.05f );
			m_bPush = TRUE;
		} else 
			m_bPush = FALSE;
	} else {
		if( m_bPush )
			m_timerAni.Set( 0.05f );
		m_bPush = FALSE;	// 영역을 벗어나면 뗀걸로 표시
	}
}

void XWndButton::OnLButtonDown( float lx, float ly ) 
{ 
	SetCapture();
	m_bPush = TRUE;
	m_bFirstPush = TRUE;
	GettimerPush().Set(0);
	m_timerAni.Set(0.05f);
	m_stateAni = 1;
	CallEventHandler( XWM_LBUTTONDOWN );
	CallEventHandler( XWM_SOUND_DOWN );
}
void XWndButton::OnNCLButtonUp( float lx, float ly ) 
{
	m_bPush = FALSE;			// 버튼영역 밖에서 떼도 클리어 해줘야한다
	m_bFirstPush = FALSE;
}
void XWndButton::OnLButtonUp( float lx, float ly ) 
{ 
	ReleaseCapture();
//	ID idWnd = 0;
	if( m_bFirstPush && m_bPush && m_bEnable ) {	// ButtonDown이 this버튼에서 일어났었을때만 처리 this 버튼영역에 x, y가 있는지 검사
		m_timerAni.Set( 0.10f );
		m_stateAni = 2;
//		OnDelegateLButtonUp( lx, ly );
		int ret = 0;
		if( m_modeAnimaion == 0 ) {
			// 애니메이션 없을때만 up에서 바로 이벤트를 보내고 애니메이션의 경우는 애니메이션이 끝난 후 디스패치한다.
			ret = DispatchEvent();
		}
		CallEventHandler( XWM_SOUND_UP );
//		idWnd = GetID();
	}
	m_bFirstPush = FALSE;
	m_bPush = FALSE;
}

int XWndButton::DispatchEvent( void )
{
	int ret = CallEventHandler( XWM_CLICKED, xboolToByte(m_bLock) );
	CallEventHandler( XWM_TOOLTIP );		// 일단은 이렇게 하는데 다른 기능이 있는 버튼의 툴팁을 보여주려면 꾹 누르고 있을때 이 이벤트가 발생해야 할듯 싶음.
	CallEventHandler( XWM_HELP_CLICKED, GetID() );
	if( ret ) {	// 클릭핸들러에서 실패리턴값을 받으면 이건 호출하지 않는다.
		CallEventHandler( XWM_QUEST_CLICKED, (DWORD)( GetstrIdentifier().c_str() ) );
	}
	return ret;
}

void XWndButton::GetMatrix( MATRIX* pOut )
{
	const auto vAdjAxis = GetSizeLocalNoTrans() * 0.5f;
	MATRIX& mWorld = ( *pOut );
	MATRIX m;
	MatrixIdentity( mWorld );
	if( !vAdjAxis.IsZero() ) {
		MatrixTranslation( m, -vAdjAxis.x, -vAdjAxis.y, 0 );
		MatrixMultiply( mWorld, mWorld, m );
	}
	const auto vScale = GetScaleLocal();
	if( vScale.x != 1.0f || vScale.y != 1.0f  ) {
		MatrixScaling( m, vScale.x, vScale.y, 1.0f );
		MatrixMultiply( mWorld, mWorld, m );
	}
	{
		MatrixIdentity( m );
		MatrixMultiply( mWorld, mWorld, m );
	}

// 	if( GetfRotZ() ) {
// 		MatrixRotationZ( m, D2R( GetfRotZ() ) );
// 		MatrixMultiply( mWorld, mWorld, m );
// 	}
// 	if( GetfRotY() ) {
// 		MatrixRotationY( m, D2R( GetfRotY() ) );
// 		MatrixMultiply( mWorld, mWorld, m );
// 	}
	if( !vAdjAxis.IsZero() ) {
		MatrixTranslation( m, vAdjAxis.x, vAdjAxis.y, 0 );
		MatrixMultiply( mWorld, mWorld, m );
	}
	const auto vPos = GetPosLocal();
	MatrixTranslation( m, vPos.x, vPos.y, 0 );
	MatrixMultiply( mWorld, mWorld, m );
}

XE::VEC2 XWndButton::GetvChildLocal( const XE::VEC2& vLocal, XWnd* pParent, XWnd* pGrandParent ) const
{
	return vLocal;
}

BOOL XWndButton::IsWndAreaIn( float lx, float ly )
{
	const auto vPosLocal = GetPosLocal();
	const auto vOrig = XE::VEC2( 0 ) + GetvAdjDrawFinal();		// 원점
	const Vec3 v3Orig( vOrig.x, vOrig.y, 0.f );
	MATRIX mLocal;
	GetMatrix( &mLocal );
	Vec4 v4;
	MatrixVec4Multiply( v4, v3Orig, mLocal );
	const auto vDist = XE::VEC2( v4.x, v4.y ) - vPosLocal;

	Vec3 v3Local( lx + vDist.x, ly + vDist.y, 0.f );
// 	Vec4 v4;
	MATRIX /*mLocal, */mInv;
//	GetMatrix( &mLocal );
	MatrixInverse( mInv, mLocal );
	MatrixVec4Multiply( v4, v3Local, mInv );
// 	v4.x += vDist.x;
// 	v4.y += vDist.y;
	if( m_pSprObj && !m_pSurface[0] ) {
		XE::VEC2 vAdj = m_pSprObj->GetAdjust();
		XE::VEC2 v = XE::VEC2( lx, ly ) - vAdj;
		return XWnd::IsWndAreaIn( v.x, v.y );		// 버튼이미지가 중앙정렬 되어 있을수도있으니 adj값을 보정해야한다.
	} else {
		const auto sizeLocalNT = GetSizeLocalNoTrans();
		if( XE::IsArea( XE::VEC2(0), sizeLocalNT, XE::VEC2(v4.x, v4.y) ) )
			return TRUE;
//		return XWnd::IsWndAreaIn( v4.x, v4.y );		// 버튼이미지가 중앙정렬 되어 있을수도있으니 adj값을 보정해야한다.
// 		return XWnd::IsWndAreaIn( lx, ly );		// 버튼이미지가 중앙정렬 되어 있을수도있으니 adj값을 보정해야한다.
	}
	return FALSE;
}

BOOL XWndButton::OnCreate( void )
{
	if( XE::GetMain()->GetpGame() ) {
		XLua *pLua = XE::GetMain()->GetpGame()->GetpLua();
		if( pLua ) {
			if( pLua->IsHaveFunc( "OnEventCreate" ) ) {
				pLua->Call<void, XWnd*>( "OnEventCreate", this );
			}
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
XWndButtonDebug* XWndButtonDebug::sUpdateCtrl( XWnd *pRoot
																							, const char* cIds
																							, const XE::VEC2& v
																							, const XE::VEC2& vSize
																							, LPCTSTR szText
																							, bool bShow )
{
	auto pWnd = pRoot->Find( cIds );
	if( pWnd == nullptr ) {
		if( bShow ) {
			auto pCtrl = new XWndButtonDebug( v, vSize, szText );
			pCtrl->SetstrIdentifier( cIds );
			pRoot->Add( pCtrl );
			return pCtrl;
		}
	} else {
		pWnd->SetbShow( bShow );
		return dynamic_cast<XWndButtonDebug*>( pWnd );
	}
	return nullptr;
}

void XWndButtonDebug::Create( const XE::VEC2& vSize, LPCTSTR szText,
							XBaseFontDat *pFontDat/* = nullptr*/,
							XCOLOR col/*=XCOLOR_WHITE*/ )
{
	if( pFontDat == nullptr )
		pFontDat = BASE_FONT;
	m_pFontObj = pFontDat->CreateFontObj();;
	m_pFontObj->SetLineLength( vSize.w );
	m_pFontObj->SetAlign( XE::xALIGN_CENTER );
	m_pFontObj->SetArea( vSize );
	if( XE::IsHave(szText) )
		_tcscpy_s( m_szText, szText );
}

// BOOL XWndButtonDebug::IsWndAreaIn( float lx, float ly )
// {
// 	return XWnd::IsWndAreaIn( lx, ly );		// 버튼이미지가 중앙정렬 되어 있을수도있으니 adj값을 보정해야한다.
// }
// int XWndButtonDebug::Process( float dt )
// {
// 	XWnd::Process( dt );
// 	return 1;
// }
void XWndButtonDebug::Draw( void )
{
	XPROF_OBJ_AUTO();
	XE::VEC2 vPos = GetPosFinal();
	XE::VEC2 vSize = GetSizeFinal();
	XE::VEC2 vCenter = vPos + vSize / 2.f;	
	if( m_bEnable )	{
		if( m_bPush )	{
			GRAPHICS->FillRectSize( vPos, vSize, XCOLOR_WHITE );
			GRAPHICS->DrawRectSize( vPos, vSize, XCOLOR_BLUE );
		} else{
			if( m_bCheck ) {
				GRAPHICS->FillRectSize( vPos, vSize, XCOLOR_WHITE );
				GRAPHICS->DrawRectSize( vPos, vSize, XCOLOR_BLACK );
			} else {
				GRAPHICS->FillRectSize( vPos, vSize, XCOLOR_GRAY );
				GRAPHICS->DrawRectSize( vPos, vSize, XCOLOR_BLACK );
			}
		}
		m_pFontObj->SetColor( XCOLOR_BLACK );
		m_pFontObj->DrawString( vPos, m_szText );
	} else	{
		GRAPHICS->FillRectSize( vPos, vSize, XCOLOR_WHITE );
		GRAPHICS->DrawRectSize( vPos, vSize, XCOLOR_BLACK );
		m_pFontObj->SetColor( XCOLOR_LIGHTGRAY );
		m_pFontObj->DrawString( vPos, m_szText );
	}
}
//////////////////////////////////////////////////////////////////////////
void XWndButtonCheck::OnMouseMove( float lx, float ly ) 
{ 
}
void XWndButtonCheck::OnLButtonDown( float lx, float ly ) 
{ 
	m_bFirstPush = TRUE;
}
void XWndButtonCheck::OnNCLButtonUp( float lx, float ly ) 
{
	m_bFirstPush = FALSE;
}
void XWndButtonCheck::OnLButtonUp( float lx, float ly ) 
{ 
	ID idWnd = 0;
	if( m_bFirstPush ) {	
		if( CallEventHandler( XWM_CLICKED ) )	{
			m_bPush = !m_bPush;
			idWnd = GetID();
		}
	}	
	m_bFirstPush = FALSE;
}

////////////////////////////////////////
XWndButtonString::XWndButtonString( float x, float y, LPCTSTR szString, XCOLOR col, 
						XBaseFontDat *pFontDat, 
						LPCTSTR szSpr, ID idActUp, ID idActDown, ID idActDisable ) 
	: XWnd( x, y ), 
	XWndButton( x, y, szSpr, idActUp, idActDown, idActDisable ) 
{
	Init();
	m_Color = col;
	auto vSize = GetSizeLocal();
	m_pText = new XWndTextString( XE::VEC2(0), vSize, szString, pFontDat, col );
	m_pText->SetAlign( XE::xALIGN_CENTER );		// 텍스트는 중앙정렬하도록 한다.
	m_pText->SetstrIdentifier("__text.label");
	Add( m_pText );
}

XWndButtonString::XWndButtonString( float x, float y, LPCTSTR szString, XCOLOR col, 
						LPCTSTR szFont, float sizeFont,
						LPCTSTR szSpr, ID idActUp, ID idActDown, ID idActDisable )
	: XWnd( x, y ), 
	XWndButton( x, y, szSpr, idActUp, idActDown, idActDisable ) 
{
		Init();
		m_Color = col;
		auto vSize = GetSizeLocal();
		m_pText = new XWndTextString( XE::VEC2(0), vSize, szString, szFont, sizeFont, col );
		m_pText->SetAlign( XE::xALIGN_CENTER );		// 텍스트는 중앙정렬하도록 한다.
		m_pText->SetstrIdentifier( "__text.label" );
		Add( m_pText );
}

XWndButtonString::XWndButtonString( float x, float y, LPCTSTR szString, XCOLOR col, XBaseFontDat *pFontDat, 
						LPCTSTR szImgUp, LPCTSTR szImgDown, LPCTSTR szImgDisable ) 
	: XWnd( x, y ), 
	XWndButton( x, y, szImgUp, szImgDown, szImgDisable ) 
{
		Init();
		m_Color = col;
		auto vSize = GetSizeLocal();
		m_pText = new XWndTextString( XE::VEC2(0), vSize, szString, pFontDat, col );
		m_pText->SetAlign( XE::xALIGN_CENTER );		// 텍스트는 중앙정렬하도록 한다.
		m_pText->SetstrIdentifier( "__text.label" );
		Add( m_pText );
		m_pText->SetLineLength( vSize.w );
}
XWndButtonString::XWndButtonString( float x, float y, LPCTSTR szString, XCOLOR col, 
						LPCTSTR szFont, float sizeFont,
						LPCTSTR szImgUp, LPCTSTR szImgDown, LPCTSTR szImgDisable ) 
	: XWnd( x, y ), 
	XWndButton( x, y, szImgUp, szImgDown, szImgDisable ) 
{
		Init();
		m_Color = col;
		auto vSize = GetSizeLocal();
		m_pText = new XWndTextString( XE::VEC2( 0 ), vSize, szString, szFont, sizeFont, col );
		m_pText->SetAlign( XE::xALIGN_CENTER );		// 텍스트는 중앙정렬하도록 한다.
		m_pText->SetstrIdentifier( "__text.label" );
		Add( m_pText );
		m_pText->SetLineLength( vSize.w );
}

void XWndButtonString::Draw( void ) 
{ 
	XPROF_OBJ_AUTO();
	if( GetbEnable() == FALSE )
	{
		if( m_pText )
			m_pText->SetColorText( XCOLOR_DARKGRAY );
	} else
		m_pText->SetColorText( m_Color );		// 색이 바꼈을수도 있으므로 실시간으로 교체해줌.
	XWndButton::Draw();
}

//////////////////////////////////////////////////////////////////////////
XWndButton* xGET_BUTT_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return dynamic_cast<XWndButton*>( pWnd );
}

XWndButton* xGET_BUTT_CTRLF( XWnd *pRoot, const char *cKeyFormat, ... ) {
	XBREAK( pRoot == nullptr );
	char cKey[256];
	va_list vl;
	va_start( vl, cKeyFormat );
	vsprintf_s( cKey, cKeyFormat, vl );
	va_end( vl );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return dynamic_cast<XWndButton*>( pWnd );
}
