#include "stdafx.h"
#include "SprObj.h"
#include "XAniAction.h"
//#include "XObjAct.h"
#include "XGraphicsD3DTool.h"
#include "FrameView.h"
#include "AnimationView.h"
#include "SprMng.h"
#include "XLuaSprObj.h"
#include "Sprdef.h"
#include "XLayerAll.h"
#include "XKeyBase.h"
#include "XKeyCreateObj.h"
#include "XActObj.h"
//#include "Graphic2d.h"
using namespace XE;
using namespace xSpr;

XSprObj::XSprObj( DWORD dwID ) 
{
	Init();
	m_dwID = dwID;
}
XSprObj::XSprObj( const char *cSpr ) 
{
	Init();
	m_dwID = XE::GenerateID();
	Load( Convert_char_To_TCHAR( cSpr ) );
}

void XSprObj::NewSprDat()
{
	m_pSprDat = SPRMNG->NewSprDat();
}
void XSprObj::Destroy()
{
	m_spActObjCurr.reset();
#ifdef _DEBUG
	for( auto& spActObj : m_listActObj ) {
		const auto cntRef = spActObj.use_count();
		XBREAK( cntRef > 1 );
	}
#endif // _DEBUG
	m_listActObj.clear();
	// m_pSprDat를 삭제한다. 그러나 매니저를 통해 삭제해야한다. 그리고 RefCnt개념을 써야 한다
	SPRMNG->Release( m_pSprDat );
	m_pSprDat = nullptr;

	// 사전로드 sprobj 삭제. 
	// 나중엔 SprDat Manager를 통해 삭제시켜야 하며 여러 SprObj에서 참조하는 SprDat객체는 레퍼런스 카운터를 이용하여 삭제해야 한다.
	{
		for( auto itor = m_listSprObj.begin(); itor != m_listSprObj.end(); ) {
			USE_SPROBJ *p = (*itor);
			SAFE_DELETE( p->pSprObj );
			SAFE_DELETE( p );
			m_listSprObj.erase( itor++ );
		}
	}

}
////////////// F Action ////////////////////////////
// pNewObjAct가 이미 생성되어있을때 추가하는 함수
// void XSprObj::AddObjAct( XObjAct *pNewObjAct )
// {
// 	m_pSprDat->AddAction( pNewObjAct->m_spActionCurr );
// 	m_listObjAct.push_back( pNewObjAct );
// }
// XObjAct *XSprObj::AddObjAct( SPAction spAction )
// {
// 	XObjAct *pObjAct = CreateObjAct( spAction );
// 	m_listObjAct.push_back( pObjAct );
// 	return pObjAct;
// }

/**
 @brief 빈 액션을 하나 생성시킨다. 
*/
XSPActObj XSprObj::CreateAction( LPCTSTR szActName )
{
	// SprDat에 새 액션을 하나 만든다.
 	auto spAction = m_pSprDat->CreateAction( szActName );
	//// 액션객체를 생성한다.
	auto spActObj = XSPActObj( new XActObj( this, spAction ) );
	m_listActObj.Add( spActObj );
	return spActObj;
}

/**
 @brief spSrcAction의 사본을 떠서 새로운 액션 인스턴스로 만들어 추가시킨다.
*/
//XSPActObj XSprObj::CopyAddAction( XSPActionConst spSrcAction, LPCTSTR szActName )
XSPActObj XSprObj::CreateAddCopyAction( XSPActionConst spSrcAction/*, LPCTSTR szActName*/ )
{
//	auto spActionCopy = spSrcAction->CopyDeepNewActID();
	XSPActObjConst spActObj = GetspActObj( spSrcAction->GetidAct() );
	if( XASSERT(spActObj) ) {
		auto spNewActObj = spActObj->CreateNewInstance();
		XBREAK( spNewActObj->GetspAction() == nullptr );
		m_pSprDat->AddAction( spNewActObj->GetspAction() );
		m_listActObj.Add( spNewActObj );
		return spNewActObj;
	}
//	auto spActionCopy = spSrcAction->CreateNewInstance();
// 	if( XE::IsHave( szActName ) )
// 		spActionCopy->SetszActName( szActName );
// 	spActionCopy->ReassignspActionByKey( spActionCopy );
// //	m_pSprDat->AddAction( spActionCopy );
// 	auto spActObjNew = AddAction( spActionCopy );
// 	SetAction( spActionCopy->GetidAct() );
//	return spActObjNew;
// 	SPAction spDstAction = m_pSprDat->CreateAction( (XE::IsEmpty(szActName))? pSrcAction->GetszActName() : szActName );
// 	pDstAction->CopyActionDeep( pSrcAction );
// 	CreateAction( pDstAction );		// 액션의 내용을 카피한다.
// 	SetAction( pDstAction->GetActID() );		// 새
// 	return pDstAction;
	return nullptr;
}
// spAction을 멤버로 가지는 ObjAct를 만든다
// void XSprObj::CreateAction( SPAction spAction )
// {
// 	XObjAct *pObjAct = AddObjAct( spAction );		// 추가 액션정보를 만든다.
// 	LAYER_INFO *pLayerInfo;
// 	spAction->GetNextLayerInfoClear();
// 	while( pLayerInfo = spAction->GetNextLayerInfo() ) {		// SprDat에 있는 LayerInfo를 읽어서
// 	{
// 		pObjAct->CreateLayer( pLayerInfo );		// 실제 레이어를 만들어준다
// 	}
// 	pObjAct->SetMaxLayer();
// 	spAction->SetAction( pObjAct );
// }
XSPActObj XSprObj::AddAction( XSPAction spAction )
{
	m_pSprDat->AddAction( spAction );
	auto spActObj = XSPActObj( new XActObj( this, spAction ) );
	m_listActObj.Add( spActObj );
	return spActObj;
}

bool XSprObj::SetAction( ID idAct, xRPT_TYPE playType )
{
	if( idAct == 0 ) {
		XALERT( "SetAction(0). 0번은 액션번호로 쓸수 없습니다." );
		return false;
	}
	auto spActObjCurr = GetspActObj( idAct );
	if( spActObjCurr == nullptr )
		return false;
	m_spActObjCurr = spActObjCurr;
	ResetAction( m_spActObjCurr );
	auto spAction = spActObjCurr->GetspAction();
	if( spAction == nullptr )
		return false;
// 	int idx = m_pSprDat->GetidxActionByidAct( id );
// 	SPAction spAction = m_pSprDat->GetspActionByIndex( idx );		// 나중에 GetAction( DWORD id )버전으로 바꿀것
// 	OBJACT_LOOP( itor ) {
// 		if( (*itor)->m_spActionCurr == spAction ) {
// 			m_pObjActCurr = (*itor);
// 			break;
// 		}
// 	}
// 	m_spActionCurr = m_pSprDat->GetspAction( idAct );
// 	SetKeyCurrStart();					// 키포지션을 첨으로 되돌려주고
	if( playType == xRPT_DEFAULT )		// 13.7.23 메테오 이펙트 때문에 추가
		m_PlayType = GetspAction()->GetPlayMode();		// 액션에 지정되어있던 플레이모드로 바뀜
	else
		m_PlayType = playType;
	// 레이어 내용 초기화
	//m_pObjActCurr->ClearLayer();
//	m_spActionCurr->ClearLayer();
	
	SetScale( spAction->GetvScale() );
	SetRotateZ( spAction->GetvRotate().z );
	return true;
}
// SPAction XSprObj::GetspAction()
// {
// 	return ( m_pObjActCurr ) ? m_pObjActCurr->m_spActionCurr : nullptr;
// }

XSPAction XSprObj::GetspAction( ID idAct ) const
{
	if( idAct == 0 ) {
		if( m_spActObjCurr ) {
			if( XASSERT( m_spActObjCurr ) )
			return m_spActObjCurr->GetspAction();
		}
		return nullptr;
	}
	auto spActObj = GetspActObj(idAct);
	if( spActObj )
		return spActObj->GetspAction();
	return nullptr;
}
/**
 @brief idAct의 액션을 리스트에서 지운다.
 @note 이제 액션은 shared_ptr을 쓰므로 refCnt를 올려놓지 않으면 리스트에서 빠짐과 동시에 삭제가 된다.
*/
void XSprObj::DelAction( ID idAct ) 
{ 
	// idAct가 0이면 현재 선택된 액션을 지운다.
	if( idAct == 0 )
		idAct = m_spActObjCurr->GetidAct();
	for( auto itor = m_listActObj.begin(); itor != m_listActObj.end(); ) {
		auto spActObj = (*itor);
		if( spActObj->GetidAct() == idAct ) {
			m_listActObj.erase( itor++ );
			GetpSprDat()->DelAction( idAct );
		} else {
			++itor;
		}
	}

}
// XObjAct* XSprObj::DelAction( DWORD id ) 
// { 
// 	if( m_pObjActCurr->m_spActionCurr->GetActID() == id )
// 		m_pObjActCurr = nullptr;
// 	XObjAct *pDelObjAct = nullptr;
// 	SPAction spAction = nullptr;
// 	OBJACT_MANUAL_LOOP( itor ) {
// 		if( (*itor)->m_spActionCurr->GetActID() == id ) {
// 			pDelObjAct = (*itor);							// 언두를 위해서 실제 삭제하진 않음
// 			m_listObjAct.erase( itor++ );
// 			break;
// 		} else
// 			itor++;
// 	}
// 	spAction = m_pSprDat->DelAction( id ); 
// 	XBREAK( pDelObjAct->m_spActionCurr != spAction );
// 	return pDelObjAct;
// }
////////////////////////////////////////////////////////////////////
void XSprObj::FrameMove( float dt )
{
	if( m_spActObjCurr == nullptr )
		return;
	auto spAction = m_spActObjCurr->GetspAction();
	if( XBREAK(spAction == nullptr) ) 
		return;
	// 자신이 pause상태이거나 부모가 pause상태면 플레이를 하지 않는다
	BOOL bPlay = IsPlaying();
	if( m_pParent )
		bPlay = m_pParent->IsPlaying();
	// 현재 프레임의 키를 실행
//	ExecuteKeyByFrame( spAction, m_fFrameCurrent );
	m_itorKeyCurr = ExecuteKeyByFrame( m_spActObjCurr, m_itorKeyCurr, m_fFrameCurrent );
	UpdateMatrix();		// m_mWorld를 갱신
	// 레이어 갱신
	m_spActObjCurr->FrameMove( dt, m_fFrameCurrent );
	// 이번프레임에 보여줘야할 키실행과 레이어 업데이트는 모두 끝났으므로 더이상 레이어에 업데이트가 일어나선 안됨.
	if( bPlay ) {		// pause상태면 키실행과 레이어 업데이트를 안함.
		m_fFrameCurrent += spAction->GetSpeed()	 * dt;
		const auto fMaxFrame = spAction->GetfMaxFrame();
		// 마지막 프레임이었으면.
		if( m_fFrameCurrent >= fMaxFrame ) {
			switch( m_PlayType )
			{
			case xRPT_LOOP: {
				const auto fRepeatFrm = spAction->GetRepeatMark();
				const auto frameOver = m_fFrameCurrent - fMaxFrame;	// 프레임스키핑으로 fFrameCurr이 maxFrame을 넘어갈수도 있기때문에
				if( fRepeatFrm == 0 ) {
					m_fFrameCurrent = 0 + frameOver;		
					// 실행해야할 키를 첨으로 돌림.
					m_itorKeyCurr = m_spActObjCurr->GetKeyItorBegin();
				} else {
					// 도돌이표 마크가 있을때
					m_fFrameCurrent = fRepeatFrm + frameOver;
					// 도돌이표 앞쪽의 키는 실행하면 안되므로 바로 건너뜀.
					spAction->JumpKey( this, m_itorKeyCurr, m_fFrameCurrent );
				}
			} break;
			case xRPT_1PLAY:
			case xRPT_1PLAY_CONT: {
				m_bFinish = TRUE;
				m_fFrameCurrent = fMaxFrame;
				//m_spActObjCurr->ClearLayer();		// 툴에서는 1Play모드라도 끝나고 애니메이션 사라지지 않는게 좋아서 뺌.
				// 더이상 애니메이션이 실행되지 않도록.
				SetPause( TRUE );
			} break;
			case xRPT_PINGPONG:
			case xRPT_PINGPONG_LOOP:
			case xRPT_REVERSE:
			case xRPT_DEFAULT: {
				// 아직 지원하지 않음.
			} break;
			default:
				XBREAKF(1, "알수없는 루핑옵션:m_PlayType=%d", m_PlayType );
				break;
			}
		}
	}

//	spAction->ExecuteKey( this, m_itorKeyCurr, m_fFrameCurrent );
// 	UpdateMatrix();		// m_mWorld를 갱신
// 	m_spActObjCurr->FrameMove( dt, m_fFrameCurrent );
//	spAction->FrameMove( dt, m_fFrameCurrent, this );
}

/**
 @brief 현재 선택된 키를 기준으로 현재 프레임(fFrameCurr)에서 실행이 되어야 하면 키를 실행시키고 다음 키로 넘어간다.
 더이상 현재 프레임에 실행할수 있는키가 없을때까지 반복된다.
*/
//XBaseKey_Itor XSprObj::ExecuteKeyByFrame( XSPAction spAction
XBaseKey_Itor XSprObj::ExecuteKeyByFrame( XSPActObj spActObj
																				, const XBaseKey_Itor& itorStart
																				, float fFrameCurr )
{
	const auto spAction = spActObj->GetspAction();
	XBaseKey_Itor itorCurr = itorStart;
	const auto& listKey = spAction->GetlistKey();
	if( listKey.empty() )
		return itorCurr;
	if( itorCurr == listKey.end() )
		return itorCurr;
	// 무한루프방지용으로 while을 사용하지 않음.
	for( int i = 0; i < (int)listKey.size(); ++i ) {
		auto pKeyCurr = (*itorCurr);		// 현재 키
		if( fFrameCurr >= pKeyCurr->GetfFrame() ) {
			auto spLayer = spActObj->GetspLayer( pKeyCurr );
			if( XASSERT(spLayer) ) {
				pKeyCurr->Execute( spActObj, spLayer );
			}
			++itorCurr;
			if( itorCurr == listKey.end() )
				break;
		} else
			break;
	}
	return itorCurr;
}

/**
 @brief 프레임을 fFrame으로 바로 이동시키고 그 앞쪽에 있던 키들을 모두 실행시킨다.
*/
void XSprObj::MoveFrame( float fFrame )
{
	// -1이면 현재 프레임
	if( fFrame == -1.0f )
		fFrame = m_fFrameCurrent;
	// 
	auto spAction = m_spActObjCurr->GetspAction();
	if( fFrame > m_spActObjCurr->GetMaxFrame() )
		return;
	m_fFrameCurrent = fFrame;
	m_itorKeyCurr = m_spActObjCurr->GetKeyItorBegin();
	m_spActObjCurr->ClearLayer();
	spAction->InitKeysRandom();		// 랜덤적용되는 모든키의 랜덤값을 초기화 한다.
	m_itorKeyCurr = ExecuteKeyByFrame( m_spActObjCurr, m_itorKeyCurr, fFrame );
	m_spActObjCurr->MoveFrame( fFrame );
	GetFrameView()->Update();
	GetAnimationView()->Update();
}

void XSprObj::MoveFrameBySec( float sec )
{
	float frmPerSecParent = XFPS * m_spActObjCurr->GetSpeed();
	float frmCurr = frmPerSecParent * sec;
	MoveFrame( frmCurr );
// 	auto spAction = m_spActObjCurr->GetspAction();
// 	if( fFrame > m_spActObjCurr->GetMaxFrame() )
// 		return;
// 	m_fFrameCurrent = fFrame;
// 	m_itorKeyCurr = m_spActObjCurr->GetKeyItorBegin();
// 	m_spActObjCurr->ClearLayer();
// 	m_itorKeyCurr = ExecuteKeyByFrame( m_spActObjCurr, m_itorKeyCurr, fFrame );
// 	m_spActObjCurr->MoveFrame( fFrame );
// 	GetFrameView()->Update();
// 	GetAnimationView()->Update();
}

void XSprObj::Draw( const D3DXMATRIX &mParent )
{
	D3DXMATRIX mTrans, mScale, mRot, mWorld, mAxis, mReverseAxis;
	if( m_DrawMode == xDM_NONE )		// 아무것도 그리지 않는 모드
		return;
	float roty = m_fRotY;
	if( m_dwDrawFlag & EFF_FLIP_HORIZ ) {
		roty = fmodf( m_fRotY + 180.0f, 360.f );
		SetRotateY( roty );
	} else
		SetRotateY( roty );
	if( m_dwDrawFlag & EFF_FLIP_VERT )
		SetRotateX( 180.0f );
	else
		SetRotateX( 0 );
//	x += m_fAdjustAxisX;		// 좌표축 보정
//	y += m_fAdjustAxisY;
	D3DXMatrixTranslation( &mTrans, m_vPos.x, m_vPos.y, 0 );		// 
	D3DXMatrixRotationYawPitchRoll( &mRot, D3DXToRadian(m_fRotY), D3DXToRadian(m_fRotX), D3DXToRadian(m_fRotZ) );
	D3DXMatrixScaling( &mScale, m_fScaleX, m_fScaleY, 1.0f );
	D3DXMatrixTranslation( &mAxis, -m_fAdjustAxisX, -m_fAdjustAxisY, 0 );
	D3DXMatrixTranslation( &mReverseAxis, m_fAdjustAxisX, m_fAdjustAxisY, 0 );
	mWorld = mAxis * mScale * mRot * mReverseAxis * mTrans * mParent;
//	m_spActionCurr->Draw( mWorld );		// x, y를 매트릭스에 반영했기때문에 로컬좌표 0을 넘겨준다
	m_spActObjCurr->Draw( mWorld );
}

// lx, ly는 this가 화면에 출력된 좌표를 0,0으로 하는 로컬좌표
DWORD XSprObj::GetPixel( float cx, float cy, 
												float mx, float my, 
												const D3DXMATRIX &m, 
												BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb ) const
{
	D3DXMATRIX mWorld;
	if( m_DrawMode == xDM_NONE )		// 아무것도 그리지 않는 모드
		return 0;
	GetMatrix( &mWorld, cx, cy );
	mWorld *= m;
	return m_spActObjCurr->GetPixel( cx, cy, mx, my, mWorld, pa, pr, pg, pb );

}
// GetPixel과 GetLayerInPixel은 외부에서 단독으로 불릴수 있기때문에 내부 형식이 같아야 한다
XSPBaseLayer XSprObj::GetLayerInPixel( float cx, float cy, float mx, float my, const D3DXMATRIX &m ) const
{
	D3DXMATRIX mWorld;
	if( m_DrawMode == xDM_NONE )		// 아무것도 그리지 않는 모드
		return 0;
	GetMatrix( &mWorld, cx, cy );
	mWorld *= m;
	return m_spActObjCurr->GetLayerInPixel( cx, cy, mx, my, mWorld );
}

XSPBaseLayer XSprObj::GetLayerInPixel( const D3DXMATRIX& mCamera, const D3DXVECTOR2& mv ) const
{
//	D3DXMATRIX mWorld;
	if( m_DrawMode == xDM_NONE )		// 아무것도 그리지 않는 모드
		return 0;
//	GetMatrix( &mWorld, cx, cy );
//	mWorld *= m;
	return m_spActObjCurr->GetLayerInPixel( mCamera, mv );
}

D3DXMATRIX* XSprObj::GetMatrix( D3DXMATRIX *pOut, float lx, float ly ) const
{
	lx += m_fAdjustAxisX;		// 좌표축 보정
	ly += m_fAdjustAxisY;
	D3DXMATRIX mTrans, mScale, mRot, mAxis;
	D3DXMatrixTranslation( &mTrans, lx, ly, 0 );
	D3DXMatrixRotationYawPitchRoll( &mRot, D3DXToRadian(m_fRotY), D3DXToRadian(m_fRotX), D3DXToRadian(m_fRotZ) );
	D3DXMatrixScaling( &mScale, m_fScaleX, m_fScaleY, 1.0f );
	D3DXMatrixTranslation( &mAxis, -m_fAdjustAxisX, -m_fAdjustAxisY, 0 );
	*pOut = mAxis * mScale * mRot * mTrans;
	return pOut;
}

/**
 @brief 
*/
BOOL XSprObj::Save( LPCTSTR szFilename ) 
{ 
	// 각 액션들의 바운딩박스를 재계산한다.
	CalcBoundBox();

	std::vector<xLayerInfoByAction> aryLayerInfoByAction;
	for( const auto& spActObj : m_listActObj ) {
		// 액션내 키가 하나도 없는 레이어를 자동으로 삭제
		spActObj->DoDelEmptyLayer();
		// 	저장에 앞서 동적체에 있는 레이어정보들을 SprDat로 업데이트 한다.
//		XList4<xLayerInfo> listLayerInfo;
// 		std::vector<xLayerInfo> aryLayerInfo;
		xLayerInfoByAction layerInfoByAction;
		layerInfoByAction.m_idAct = spActObj->GetidAct();
		XBREAK( layerInfoByAction.m_idAct == 0 );
  	spActObj->MakeLayerInfo( &layerInfoByAction.m_aryLayerInfo );
		aryLayerInfoByAction.push_back( layerInfoByAction );
//		spActObj->GetspAction()->SetLayerInfoForSave( aryLayerInfo );
// 		spActObj->SetLayerInfo( listLayerInfo );
//		m_pSprDat->SetLayerInfo( spActObj->GetidAct(), listLayerInfo );
	}

//	OBJACT_LOOP( itor )
// 	{
// 		XObjAct *pObjAct = (*itor);
// 		pObjAct->DoDelEmptyLayer();		// 키가하나도 없는 레이어를 찾아 삭제하라
// 		if( pObjAct->m_spActionCurr->GetNumLayerInfo() != pObjAct->GetNumLayer() )		// 레이어와 레이어인포수가 다르면 경고
// 		{
// 			XALERT( "레이어수(%d)와 레이어인포수(%d)가 다릅니다.", pObjAct->GetNumLayer(), pObjAct->m_spActionCurr->GetNumLayerInfo() );
// 			pObjAct->m_spActionCurr->AddLayerInfo( XBaseLayer::xIMAGE_LAYER, 2, 7 ); 
// 		}
// 	}
	return m_pSprDat->Save( szFilename, aryLayerInfoByAction ); 
}

/**
 @brief .spr파일을 읽어들여 SprObj객체로 만든다.
*/
BOOL XSprObj::Load( LPCTSTR szFilename ) 
{ 
	XBREAK( SPRMNG == nullptr );
	if( m_pSprDat ) {
		XLOG( "m_pSprDat가 해제되지 않았다 %s", szFilename );
		SPRMNG->Release( m_pSprDat );
	}
	std::vector<xLayerInfoByAction> aryLayerInfoByAction;
	// sprDat loading
	m_pSprDat = SPRMNG->Load( szFilename, &aryLayerInfoByAction, TRUE );
	if( !m_pSprDat ) {		// 일단 SprDat를 읽고
		auto bRet = XALERT_OKCANCEL( "XSprObj::Load %s 읽기 실패", szFilename );
		if( bRet == IDCANCEL ) 
			m_bPause = true;		// 플레이 멈춤
		return FALSE;
	}
	// XActObj객체 및 레이어생성
	int idxAct = 0;
	XASSERT( m_pSprDat->GetlistAction().size() == aryLayerInfoByAction.size() );
	for( const auto& spAction : m_pSprDat->GetlistAction() ) {
		auto& aryLayerInfo = aryLayerInfoByAction[ idxAct ];
		XASSERT( aryLayerInfo.m_idAct == spAction->GetidAct() );
		auto spActObj = XSPActObj( new XActObj( this, spAction, aryLayerInfo.m_aryLayerInfo ) );
//		XTrace(_T("cntRef=%d"), spActObj.use_count() );
		if( XASSERT(spActObj) ) {
			if( m_pSprDat->GetnVersion() < 29 ) {
				// 낮은버전 파일을 위해 idLocalKeyInLayer값을 넣어준다.
				spAction->SetidLocalKeyInLayerToKeys( spActObj );
			}
			m_listActObj.Add( spActObj );
		}
		++idxAct;
	}
	TraceActObj();
// 	m_pSprDat->GetNextActionClear();
// 	while( SPAction spAction = m_pSprDat->GetNextAction() ) {	// SprDat에 있는 Action정보를 읽어서
// 
// 		XObjAct *pObjAct = AddObjAct( spAction );		// 추가 액션정보를 만든다.
// 		spAction->GetNextLayerInfoClear();
// 		while( LAYER_INFO *pLayerInfo = spAction->GetNextLayerInfo() ) {		// SprDat에 있는 LayerInfo를 읽어서
// 			pObjAct->CreateLayer( pLayerInfo );		// 실제 레이어를 만들어준다
// 		}
// 		pObjAct->SetMaxLayer();
// 		spAction->SetAction( pObjAct );
// 	}
	auto spActDat = m_pSprDat->GetspActDatByIndex(0);
	if( spActDat )
		SetAction(spActDat->GetidAct() );		// 가장첫번째에 있는 액션을 활성화 시킨다
	TraceActObj();
	return TRUE;
}

void XSprObj::DestroySprObj( DWORD dwID )
{
	std::list<USE_SPROBJ *>::iterator itor;
	USE_SPROBJ *p;
	for( itor = m_listSprObj.begin(); itor != m_listSprObj.end(); ) {
		p = (*itor);
		if( p->dwID == dwID )				// dwID를 가진 노드를 찾았으면
		{
			SAFE_DELETE( p->pSprObj );		// SprObj삭제
			SAFE_DELETE( p );					// 스트럭쳐 삭제
			m_listSprObj.erase( itor );			// 노드삭제
			break;
		}
		itor ++;
	}
}
XSprObj* XSprObj::AddSprObj( LPCTSTR szSprObj
// 													, ID idAct
													, xRPT_TYPE playMode
													, ID idBase
													, ID idLocal
													, ID idActParent
													, ID idLayerParent )
{
		for( auto itor = m_listSprObj.begin(); itor != m_listSprObj.end(); itor ++ ) {
			USE_SPROBJ *p = (*itor);
			// sprObj재사용 규칙
			// 사용되는 오브젝트 레이어별로 별도의 인스턴스가 생성된다.
			if( p->m_idActParent == idActParent ) {
				if( p->m_idLayerParent == idLayerParent ) {
					p->pSprObj->ResetAction( p->pSprObj->GetspActObjCurr() );		// 애니가 한바퀴 돌고 다시 CreateObj키가 실행되 이리로 들어오면 액션이 바뀐게 없으므로 SetAction()이 무시가 된다. 그래서 리셋시켜야 함
					return p->pSprObj;									// 이미 로드한것이므로 다시 생성하지 않는다
				}
			}
		}
	ID idSprObj = XE::GenerateID();
	USE_SPROBJ *p;
	p = new USE_SPROBJ;
	p->dwID = idSprObj;
	p->m_idActParent = idActParent;		// 생성되는 sprObj가 사용되는 부모측 액션
	p->m_idLayerParent = idLayerParent;		// 생성되는 sprObj가 사용되는 부모측 액션의 레이어
	p->pSprObj = new XSprObjTool( idSprObj );
	if( p->pSprObj->Load( szSprObj ) == FALSE ) {
		SAFE_DELETE( p->pSprObj );
		SAFE_DELETE( p );
		return nullptr;
	}
// 	if( idAct ) {
// 		p->pSprObj->SetAction( idAct, playMode );
// 	}
	_tcscpy_s( p->szFilename, szSprObj );
	m_listSprObj.push_back( p );
	return p->pSprObj;
}

XSprObj* XSprObj::GetpSprObjChild( ID idSprObj ) const
{
	for( auto pUseSpr : m_listSprObj ) {
		if( pUseSpr->dwID == idSprObj )
			return pUseSpr->pSprObj;
	}
	return nullptr;
}

// fFrameCurrent위치로 바로 점프한다. 그사이의 키는 실행하지 않는다
void XSprObj::JumpKeyPos( XSPAction spAction, float fJumpFrame )
{
//	SetKeyCurrStart();		// 키를 첨으로 돌린다
	m_itorKeyCurr = spAction->GetKeyItorBegin();
	// 키위치를 점프프레임위치의 키로 바로 이동
	spAction->JumpKey( this, m_itorKeyCurr, fJumpFrame );
}

/**
 @brief this의 월드트랜스폼 매트릭스를 업데이트 한다.
*/
void XSprObj::UpdateMatrix()
{
	auto mLocal = GetLocalMatrix();
// 	D3DXMATRIX mTrans, mScale, mRot, mAxis, mReverseAxis;
// 	D3DXMatrixTranslation( &mAxis, -m_fAdjustAxisX, -m_fAdjustAxisY, 0 );
// 	D3DXMatrixTranslation( &mTrans, m_vPos.x, m_vPos.y, 0 );
// 	D3DXMatrixRotationYawPitchRoll( &mRot, D3DXToRadian(m_fRotY), D3DXToRadian(m_fRotX), D3DXToRadian(m_fRotZ) );
// 	D3DXMatrixScaling( &mScale, m_fScaleX, m_fScaleY, 1.0f );
// 	D3DXMatrixTranslation( &mReverseAxis, m_fAdjustAxisX, m_fAdjustAxisY, 0 );
// 	m_mWorld = mAxis * mScale * mRot * mReverseAxis * mTrans;	// 좌표축 옮겨놓고 * 축소하고 * 회전하고 * 다시 좌표축 돌려놓고 * 원하는 스크린위치에다 옮김 * 마지막으로 부모곱함

	if( m_pParent ) {
		m_pParent->UpdateMatrix();
		m_mWorld = mLocal * m_pParent->GetMatrix();
	} else {
		m_mWorld = mLocal;
	}
}

/**
 @brief 현재 sprObj의 트랜스폼 매트릭스를 얻는다.
*/
D3DXMATRIX XSprObj::GetLocalMatrix() const
{
	D3DXMATRIX mTrans, mScale, mRot, mAxis, mReverseAxis;
	D3DXMatrixTranslation( &mAxis, -m_fAdjustAxisX, -m_fAdjustAxisY, 0 );
	D3DXMatrixTranslation( &mTrans, m_vPos.x, m_vPos.y, 0 );
	D3DXMatrixRotationYawPitchRoll( &mRot, D3DXToRadian( m_fRotY ), D3DXToRadian( m_fRotX ), D3DXToRadian( m_fRotZ ) );
	D3DXMatrixScaling( &mScale, m_fScaleX, m_fScaleY, 1.0f );
	D3DXMatrixTranslation( &mReverseAxis, m_fAdjustAxisX, m_fAdjustAxisY, 0 );
	// 좌표축 옮겨놓고 * 축소하고 * 회전하고 * 다시 좌표축 돌려놓고 * 원하는 스크린위치에다 옮김
	const auto mLocal = mAxis * mScale * mRot * mReverseAxis * mTrans;	
	return mLocal;
}

/**
 @brief 각 액션의 바운딩 박스를 계산한다.
*/
void XSprObj::CalcBoundBox()
{
	if( m_spActObjCurr == nullptr )
		return;
	// 현재 액션아이디를 받아둠.
//	1D idActOld = m_spActObjCurr->GetidAct();
	//
	UpdateMatrix();
	// 각 액션의 바운딩박스를 업데이트 한다.
	for( auto& spActObj : m_listActObj ) {
		spActObj->PreCalcBoundBox();
	}

// 	for( auto& spAction : m_pSprDat->GetlistAction() ) {
// 		float frame = 0;
// 		auto itorKeyCurr = spAction->GetKeyItorBegin();
// 		SetAction( spAction->GetidAct() );
// 		while( 1 ) {
// 			spAction->ExecuteKey( this, itorKeyCurr, frame );
// 			spAction->InitBoundBox();
// 			spAction->CalcBoundBox( 1.0f, frame, this );
// 			frame += 1.0f;
// 			if( frame >= spAction->GetfMaxFrame() ) {
// 				frame = spAction->GetfMaxFrame();
// 				break;
// 			}
// 		}
// 		spAction->CalcBoundBox( 1.0f, frame, this );		// 마지막 프레임것까지 계산.
// 	}
// 	// 다 끝나면 원래 액션으로 돌림
// 	if( m_pSprDat->GetNumActs() > 0 )
// 		SetAction( idActOld );
}

XLuaSprObj* XSprObj::CreateScript() 
{
	return new XLuaSprObj;
}

float XSprObj::GetSpeedCurrentAction() const
{
// 	if( m_spActionCurr )
// 		return m_spActionCurr->GetSpeed();
	if( m_spActObjCurr )
		return m_spActObjCurr->GetSpeed();
	return 0.f;
}
void XSprObj::SetSpeedCurrentAction( float speed ) 
{
	if( XASSERT(m_spActObjCurr) )
		m_spActObjCurr->SetSpeed( speed );
}

BOOL XSprObj::IsEndFrame() const
{ 
	return ( GetFrmCurr() >= m_spActObjCurr->GetMaxFrame() ); 
}		// 현재 마지막 프레임위치에 있는가

ID XSprObj::GetActionID() const
{ 
	return ( m_spActObjCurr ) ? m_spActObjCurr->GetidAct() : 0; 
}		// 현재액션의 아이디를 얻음. GetAction()->GetActID()하는거보다 안전하다
// void XSprObj::SetKeyCurrStart() 
// { 
// 	m_itorKeyCurr = m_spActionCurr->GetKeyItorBegin(); 
// }			// 현재 가리키는 키를 맨 처음으로 돌린다.
// XBaseKey* XSprObj::AddKey( SPAction spAction, SPBaseLayer spLayer, XSprite *pSprite ) 
// { 
// 	return m_pSprDat->AddKey( m_spActionCurr, spLayer, pSprite ); 
// }
// int XSprObj::AddKey( SPAction spAction, SPBaseLayer spLayer, float fFrame, XSprite *pSprite ) 
// { 
// 	return m_pSprDat->AddKey( m_spActionCurr, spLayer, fFrame, pSprite ); 
// }
// int XSprObj::AddKey( SPAction spAction, SPBaseLayer spLayer, float fFrame, float x, float y ) 
// { 
// 	return m_pSprDat->AddKey( m_spActionCurr, spLayer, fFrame, x, y ); 
// }
// XKeyCreateObj *XSprObj::AddKeyCreateObj( LPCTSTR szSprObj, ID idAct, xRPT_TYPE playType, SPBaseLayer spLayer, float fFrame, float x, float y ) 
// {
// 	XKeyCreateObj *pKey = m_spActionCurr->AddKeyCreateObj( GenerateID(), szSprObj, idAct, playType, spLayer, fFrame, x, y );
// 	return pKey;
// }
/**
 @brief 키 삭제.
 @param spAction 어느액션의 키를 삭제할것인지 명시적으로 넘겨주게 바뀜.
*/
void XSprObj::DelKey( XSPActionConst _spAction, XBaseKey *pDelKey ) 
{
	if( pDelKey->GetType() == xSpr::xKT_CREATEOBJ 
		&& pDelKey->GetSubType() == xSpr::xKTS_MAIN ) {		// 서브타입까지 검사해야 XKeyCreateObj가 된다
		XKeyCreateObj *pKeyObj = SafeCast<XKeyCreateObj*, XBaseKey*>( pDelKey );
		DWORD id = pKeyObj->GetdwID();
		DestroySprObj( id );										// CreateObj키의 경우는 키가 삭제될때 자기로 인해 생성됬던 SprObj를 삭제해준다
	}
	auto spAction = GetspAction(_spAction->GetidAct());
	spAction->DelKey( this, pDelKey );
}


// XSPLayerMove XSprObj::GetspLayerMove( xSpr::xtLayer type, int nLayer ) 
// {
// // 	if( XBREAK( m_spActionCurr == nullptr ) )
// // 		return XSPLayerMove();
// //	return m_spActionCurr->GetspLayerMove( type, nLayer );
// //	return dynamic_cast<>( GetLayer( type, nLayer ) );
// //	XBREAK( spLayer->IsTypeLayerMove() == FALSE );
// //	return (XLayerMove *)spLayer;
// }

void XSprObj::DelLayer( XSPBaseLayer spLayer, BOOL bDelLayerOnly ) 
{
	if( XASSERT(m_spActObjCurr) ) {
		m_spActObjCurr->DelLayer( spLayer, bDelLayerOnly );
		// 노드삭제로인해 m_KeyCurr가 깨질수도 있기땜에 이걸 해줘야 한다
		m_itorKeyCurr = m_spActObjCurr->GetKeyItorBegin();		
		if( bDelLayerOnly == FALSE )
			MoveFrame();
	}
}
void XSprObj::UpdateLayersYPos( const XE::VEC2& sizeKey ) 
{
	if( m_spActObjCurr )
		m_spActObjCurr->UpdateLayersPos( sizeKey );
}

/**
 @brief 현재액션의 레이어두개의 위치를 바꾼다.
*/
bool XSprObj::SwapLayer( XSPBaseLayer spLayer1, XSPBaseLayer spLayer2, const XE::VEC2& sizeKey ) 
{
	if( m_spActObjCurr )
		return m_spActObjCurr->SwapLayer( spLayer1, spLayer2, sizeKey );
	return false;
}
XSPBaseLayer XSprObj::GetLayer( xSpr::xtLayer type, int nLayer ) 
{ 
	if( m_spActObjCurr )
		return m_spActObjCurr->GetspLayer( type, nLayer ); 
	return nullptr;
}
XSPBaseLayer XSprObj::GetspLayerCurrAct( ID idLayer ) const
{
	if( m_spActObjCurr )
		return m_spActObjCurr->GetspLayerByidLayer( idLayer );
	return nullptr;
}
XSPBaseLayer XSprObj::GetspLayer( XSPActionConst spAction, ID idLayer ) const
{
	if( !spAction )
		return nullptr;
	return GetspLayer( spAction->GetidAct(), idLayer );
}
XSPBaseLayer XSprObj::GetspLayer( ID idAct, ID idLayer ) const
{
	auto spActObj = GetspActObj( idAct );
	if( XASSERT( spActObj ) ) {
		return spActObj->GetspLayerByidLayer( idLayer );
	}
	return nullptr;
}
XSPBaseLayer XSprObj::GetspLayer( XBaseKey* pKey ) const
{
	return GetspLayer( pKey->GetspAction(), pKey->GetidLayer() );
}


// void XSprObj::DrawLayerLabel( float left, float top ) 
// { 
// 	m_spActionCurr->DrawLayerName( left, top ); 
// }
// XObjAct* XSprObj::CreateObjAct( SPAction spAction ) 
// {
// #ifdef _XTOOL
// 	XBREAK( 1 );	// 툴에선 XObjActTool을 new 해야 한다
// #endif
// 	return new XObjAct( spAction );
// }

// 현재설정된 액션그대로 초기화만 시킨다
void XSprObj::ResetAction( XSPActObj spActObj ) 	
{
	m_fFrameCurrent = 0;
//	SetKeyCurrStart();
	// 키를 처음으로 돌린다.
	m_itorKeyCurr = spActObj->GetKeyItorBegin(); 
	m_bFinish = FALSE;
}

// new로 생성이 안되고 캐시에서 다시 가져오게 됐을경우에도 기본적인 사항은 리셋시켜함
// void XSprObj::Reset() 
// {
// 	ResetAction();
// 	//SetpObjActCurr( NULL );
// //	m_spActionCurr.reset();		//SetAction을 하면 자동으로 교체되므로 굳이 여기서 클리어시키지 않음. 이터레이터 키 같은것도 현재 액션의 키이기때문.
// }

XSPActObj XSprObj::GetspActObj( ID idAct ) const
{
	for( auto spActObj : m_listActObj ) {
		if( spActObj->GetidAct() == idAct )
			return spActObj;
	}
	return XSPActObj();
}

/**
 @brief idAct액션의 spLayer에 키를 추가한다.
*/
XBaseKey* XSprObj::AddKeySprAtLast( ID idAct, ID idLayer, XSprite *pSprite )
{
	auto spActObj = GetspActObj( idAct );
	if( XASSERT(spActObj) ) {
		return spActObj->AddKeySprAtLast( idLayer, pSprite );
	}
	return nullptr;
// 	float fFrame = spLayer->GetfLastFrame();			// nLayer에 마지막으로 추가되었던 프레임이 몇번이었는지 검사해서 거기에 + 1를 한 번호로 키를 추가한다.
// 	XBaseKey *pKey = spAction->AddKeyImage( fFrame, spLayer, pSprite );
// 	spLayer->SetfLastFrame( fFrame + 1.0f );
//	return pKey;
}

int XSprObj::AddKeySprWithFrame( ID idAct, ID idLayer, float fFrame, XSprite *pSprite )
{
	auto spActObj = GetspActObj( idAct );
	if( XASSERT(spActObj) )
		return spActObj->AddKeySprWithFrame( idLayer, fFrame, pSprite );
	return 0;
}
int XSprObj::AddKeyPosWithFrame( ID idAct, ID idLayer, float fFrame, float x, float y )
{
	auto spActObj = GetspActObj( idAct );
	if( XASSERT( spActObj ) )
		return spActObj->AddKeyPosWithFrame( idLayer, fFrame, x, y );
	return 1;
}

/**
 @brief 새로운 애니메이션 프레임하나를 스프라이트에 추가하고 image키로도 추가한다.
*/
XSprite* XSprObj::AddAniFrame( const XE::VEC2& sizeSurface
															, const XE::VEC2& vAdj 
															, const XE::VEC2& sizeMem 
															, DWORD *pImg
															, BOOL bCreateKey )
{
	// fuck 구조가 안좋다 pSprObj를 넘겨줄께 아니라 XSprObj에서 이것을 처리해야할듯하다.
	XSprite *pSprite = m_pSprDat->AddSprite( sizeSurface, vAdj, sizeMem, pImg );	// 지정된 액션번호로 스프라이트를 하나 추가한다. 내부에서 ID2D1Bitmap객체도 생성한다.
	if( bCreateKey && m_spActObjCurr ) {
		// 0번 이미지레이어를 꺼냄
		pSprite = m_spActObjCurr->AddAniFrame( sizeSurface, vAdj, sizeMem, pSprite );
// 		auto spBaseLayer = GetLayer( xSpr::xLT_IMAGE, 0 );
// 		auto spLayer = std::static_pointer_cast<XLayerImage>( spBaseLayer );
// 		if( XASSERT(spLayer) )
// 			AddKeySprAtLast( GetidAct(), spLayer->GetidLayer(), pSprite );
	}
	return pSprite;
}

/**
 @brief 현재 선택된 액션의 아이디를 얻는다.
*/
ID XSprObj::GetidAct() const
{
	if( XASSERT(m_spActObjCurr) )
		return m_spActObjCurr->GetidAct();
	return 0;
}

XSPActObj XSprObj::GetspActObjByIndex(int index)
{
	return m_listActObj.GetValueByIndex(index);
}

XSPActionConst XSprObj::GetspActionCurr() const
{
	if( !m_spActObjCurr )
		return nullptr;
	return m_spActObjCurr->GetspAction();
}


void XSprObj::TraceActObj()
{
	for( auto& spActObj : m_listActObj ) {
		XTrace(_T("idAct=%d cntRef=%d"), spActObj->GetidAct(), spActObj.use_count() );
	}
}
