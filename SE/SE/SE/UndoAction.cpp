#include "stdafx.h"
#include "UndoAction.h"
#include "SprDat.h"
#include "XAniAction.h"
#include "ConsoleView.h"
#include "SprObj.h"
//#include "XObjAct.h"
#include "Tool.h"
#include "XKeyBase.h"
#include "XLayerBase.h"
#include "Sprite.h"
#include "XActObj.h"
#include "FrameView.h"

// 카피본키의 id얻기
XUndoKey::XUndoKey( XSprObj *pSprObj, XSPAction spAction, XBaseKey* pUndoKey, LPCTSTR szText /*= _T( "" )*/ )
	: XBaseUndo( pSprObj, szText ) 
{
	Init();
//	m_pKey = pUndoKey->CopyDeep();			// 언두할 키의 복사본을 만든다
	m_spAction = spAction;
// 	m_pKey = pUndoKey->CreateCopy( spAction, pUndoKey->GetidLayer(), pUndoKey->GetidLocalInLayer() );
	m_pKey = pUndoKey->CreateCopy();
//	m_pKey->SetidKey( pUndoKey->GetidKey() );							// 언두할 키랑 같은키기때문에 아이디를 맞춰준다
//	m_idAction = pUndoKey->GetspAction()->GetidAct();		// id는 액션번호가 아니라 고유한 시리얼넘버를 말한다
}
void XUndoKey::Destroy() 
{
	SAFE_DELETE( m_pKey );
}
CString XUndoKey::GetText() 
{
	auto spLayer = GetpSprObj()->GetspLayer( m_spAction, GetpKey()->GetidLayer());
	if (spLayer) {
		CString strReplace = GetpKey()->GetToolTipString(spLayer);
		strReplace.Replace('\n', ' ');
		return XBaseUndo::GetText() + _T("(") + strReplace + _T(')');
	}
	return CString();
}

/**
 @brief pKeySrc의 복사본을 만든다.
*/
XBaseKey* XUndoKey::CreateCopy( XBaseKey* pKeySrc )
{
// 	return pKeySrc->CreateCopy( m_spAction, pKeySrc->GetidLayer(), pKeySrc->GetidLocalInLayer() );
	return pKeySrc->CreateCopy();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// 언두행동(생성했던 키 삭제)
BOOL XUndoCreateKey::DoUndo() 
{ 
	XBaseKey *pKey = m_spAction->FindKey( m_pKey->GetidKey() );
	if( !pKey ) return FALSE;
	m_pSprObj->DelKey( m_spAction, pKey );		// m_pKey의 id를 가진 키를 액션에서 찾아서 지운다.
	return TRUE;
}		
// 리두행동(언두로인해 삭제했던 키를 다시 생성함)
BOOL XUndoCreateKey::DoRedo() 
{
	XBREAK( m_spAction == nullptr );
//	auto pNewKey = GetpKey()->CopyDeep();
//	auto pNewKey = GetpKey()->CreateCopy( m_spAction, GetpKey()->GetidLayer(), GetpKey()->GetidLocalInLayer() );
	auto pNewKey = CreateCopy( GetpKey() );
	m_spAction->AddKey( pNewKey, -1, FALSE );
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 언두행동(삭제했던 키 되살림)
BOOL XUndoDelKey::DoUndo() 
{ 
	XBREAK( m_spAction == nullptr );
//	XBaseKey *pNewKey = GetpKey()->CopyDeep();
//	auto pNewKey = GetpKey()->CreateCopy( m_spAction, GetpKey()->GetidLayer(), GetpKey()->GetidLocalInLayer() );
	auto pNewKey = CreateCopy( GetpKey() );
	m_spAction->AddKey( pNewKey, -1, FALSE );
	return TRUE;
}		
// 리두행동(언두로 인해 되살려진 키를 다시 삭제함)
BOOL XUndoDelKey::DoRedo() 
{
	XBaseKey *pKey = m_spAction->FindKey( m_pKey->GetidKey() );
	if( !pKey ) return FALSE;
	m_pSprObj->DelKey( m_spAction, pKey );		// m_pKey의 id를 가진 키를 액션에서 찾아서 지운다.
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 언두행동(바뀐키값을 원래대로 회복)
XUndoKeyChange::XUndoKeyChange(XSprObj *pSprObj
														, XSPActObj spActObj
														, XBaseKey *pKey
														, LPCTSTR szText /*= _T("")*/)
	: XUndoKey(pSprObj, spActObj->GetspAction(), pKey, szText) {	// pKey:바뀌기전 오리지날 키
	Init();
	m_spActObj = spActObj;
}
void XUndoKeyChange::Destroy()
{
	SAFE_DELETE( m_pKeyAfter );
}
CString XUndoKeyChange::GetText() 
{
	auto spLayer = GetpSprObj()->GetspLayer( m_spActObj->GetspAction()
																					 , GetpKey()->GetidLayer());
	if (spLayer) {
		CString strReplace = GetpKey()->GetToolTipString(spLayer);
		strReplace.Replace('\n', ' ');
		CString strReplace2 = (m_pKeyAfter) ? m_pKeyAfter->GetToolTipString(spLayer) : _T("");
		strReplace2.Replace('\n', ' ');
		return XBaseUndo::GetText() + _T("(") + strReplace + _T("->") + strReplace2 + _T(')');
	}
	return CString();
}
// 값이 바뀌고난후의 키의 복사본을 만든다(리두에 쓰임)
void XUndoKeyChange::SetKeyAfter( XBaseKey *pKeyAfter ) 
{				
//	m_pKeyAfter = pKeyAfter->CopyDeep();			// 리두할 키의 복사본을 만든다
//	m_pKeyAfter = pKeyAfter->CreateCopy( m_spAction, pKeyAfter->GetidLayer(), pKeyAfter->GetidLocalInLayer() );
	m_pKeyAfter = CreateCopy( pKeyAfter );
	//		m_pKeyAfter->SetidKey( GetpKey()->GetidKey() );				// m_pKey랑 같은 키기때문에 아이디를 맞춰준다
}

BOOL XUndoKeyChange::DoUndo() 
{ 
	XBaseKey *pKey = m_spAction->FindKey( m_pKey->GetidKey() );
	if( !pKey ) return FALSE;
//	XBaseKey *pNewKey = GetpKey()->CopyDeep();
//	auto pNewKey = GetpKey()->CreateCopy( m_spAction, GetpKey()->GetidLayer(), GetpKey()->GetidLocalInLayer() );
	auto pNewKey = CreateCopy( GetpKey() );
	m_pSprObj->DelKey( m_spAction, pKey );		// 일단 언두시킬 키를 삭제하고
	m_spAction->AddKey( pNewKey, -1, FALSE );		// 언두된 키를 다시 애드시킨다
	m_spActObj->SortKey();						// 일단 여기다 소트를 넣어놨는데 다중키를 언두시킬때 느려지면 XUndoMultiKeyChange같은걸 만들어서 거기서 소트하자
	return TRUE;
}		
// 리두행동(언두로 인해 복구된 키값을 다시 바꾼다)
BOOL XUndoKeyChange::DoRedo() 
{
	// 이거 좀 애매하군 사실 m_pKey와 m_pKeyAfter와 id는 같을텐데
	XBaseKey *pKey = m_spAction->FindKey( m_pKey->GetidKey() );
	if( !pKey ) return FALSE;
//	XBaseKey *pNewKey = m_pKeyAfter->CopyDeep();		// 바뀐후의 키의 복사본을 만든다
//	auto pNewKey = m_pKeyAfter->CreateCopy( m_spAction, m_pKeyAfter->GetidLayer(), m_pKeyAfter->GetidLocalInLayer() );
	auto pNewKey = CreateCopy( m_pKeyAfter );
	m_pSprObj->DelKey( m_spAction, pKey );		// 일단 리두시킬 키를 삭제하고
	m_spAction->AddKey( pNewKey, -1, FALSE );		// 리두된 키를 다시 애드시킨다
	m_spActObj->SortKey();						// 일단 여기다 소트를 넣어놨는데 다중키를 언두시킬때 느려지면 XUndoMultiKeyChange같은걸 만들어서 거기서 소트하자
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                      XUndoLayer  
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
// 카피본레이어의 id얻기
XSPBaseLayer XUndoLayer::FindIDLayer( XSPBaseLayer spLayer ) 
{ 
	if( spLayer == nullptr )
		spLayer = m_spLayer;
	XSPAction spAction = m_pSprObj->GetspAction();
	if( !spAction ) {
		GetConsoleView()->Message( _T("실패: 액션이 없습니다") );
		return nullptr;
	}
// 	SPBaseLayer spFindLayer = m_pSprObj->GetspActionCurr()->FindLayer( spLayer->GetidSerial() );
	auto spActObj = m_pSprObj->GetspActObjCurr();
	if (spActObj) {
		auto spFindLayer = spActObj->GetspLayerByidLayer(spLayer->GetidLayer());
		if (!spFindLayer) {
			GetConsoleView()->Message(_T("실패: ID:%d의 레이어가 %s 액션에 없습니다"), spLayer->GetidLayer(), spAction->GetszActName());
			return nullptr;
		}
		return spFindLayer;
	}
	return nullptr;
}		
CString XUndoLayer::GetText() 
{
	return XBaseUndo::GetText() + _T( "(" ) + GetspLayer()->GetTypeString() + _T( ')' );
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// 언두행동(생성했던 레이어 삭제)
BOOL XUndoCreateLayer::DoUndo() 
{ 
//	SPBaseLayer spLayer = FindIDLayer();
//	if( !spLayer ) return FALSE;
	XBREAK( m_spLayer == nullptr );
	m_pSprObj->DelLayer( m_spLayer, TRUE );		// m_spLayer의 id를 가진 레이어를 찾아서 지운다.(키는 지우지 않는다)
	return TRUE;
}		
// 리두행동(언두로인해 삭제했던 레이어를 다시 생성함)
BOOL XUndoCreateLayer::DoRedo() 
{
//	SPBaseLayer spNewLayer = m_spLayer->CopyDeep();
//	auto spAction = m_pSprObj->GetspActionCurr();
	auto spActObj = m_pSprObj->GetspActObjCurr();
//	spNewLayer->SetidSerial( m_spLayer->GetidSerial() );		// 아이디는 복사본이 갖고 있던 아이디로 바꿔줘야 한다.
//	spAction->AddLayer( spNewLayer, FALSE );
	spActObj->AddLayer( m_spLayer, FALSE );
	spActObj->SortLayer( CFrameView::sGet()->GetSizeKey() );
// 	LAYER_INFO *pLayerInfo = new LAYER_INFO;
// 	*pLayerInfo = GetLayerInfo();
// 	m_pSprObj->GetspAction()->AddLayerInfo( pLayerInfo );

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 언두행동(삭제했던 레이어 되살림)
BOOL XUndoDelLayer::DoUndo() 
{ 
//	SPBaseLayer spNewLayer = m_spLayer->CopyDeep();
//	auto spAction = m_pSprObj->GetspActionCurr();
//	spNewLayer->SetidSerial( m_spLayer->GetidSerial() );		// 아이디는 복사본이 갖고 있던 아이디로 바꿔줘야 한다.
	auto spActObj = m_pSprObj->GetspActObjCurr();
	spActObj->AddLayer( m_spLayer, FALSE );
//	spAction->AddLayer( spNewLayer, FALSE );
	spActObj->SortLayer( CFrameView::sGet()->GetSizeKey() );
// 	LAYER_INFO *pLayerInfo = new LAYER_INFO;
// 	*pLayerInfo = GetLayerInfo();
// 	m_pSprObj->GetspAction()->AddLayerInfo( pLayerInfo );
	return TRUE;
}		
// 리두행동(언두로 인해 되살려진 레이어를 다시 삭제함)
BOOL XUndoDelLayer::DoRedo() 
{
	XSPBaseLayer spLayer = FindIDLayer();
	if( !spLayer ) return FALSE;
	m_pSprObj->DelLayer( spLayer, TRUE );		// m_spLayer의 id를 가진 레이어를 찾아서 지운다.(키는 지우지 않는다. 따로 DelKey푸쉬가 들어간다)
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
XUndoAction::XUndoAction( XSprObj *pSprObj, XSPAction spUndoAction, LPCTSTR szText )
	: XBaseUndo( pSprObj, szText ) {
	Init();
	m_spAction = spUndoAction;
	m_idAct = spUndoAction->GetidAct();
}
//////////////////////////////////////////////////////////////////////////
/**
 @brief 액션내 값변경에 대한 언두
 액션의 완전한 deep사본을 떠놓고 언두할때 액션 내부값만 카피시킨다.
*/
XUndoActionChange::XUndoActionChange( XSprObj *pSprObj, XSPAction spUndoAction, LPCTSTR szText )
	: XUndoAction( pSprObj, spUndoAction, szText ) {
	Init();
	// 액션 내부값만 변경해야 하므로 사본을 만듬.
//	m_spAction = spUndoAction->CopyDeep();	// 복사본 만듬,원본의 아이디를 보존함
	m_spAction = spUndoAction->CreateCopy( pSprObj->GetpSprDat() );

}
BOOL XUndoActionChange::DoUndo() 
{ 
	XSPAction spSrcAct = m_pSprObj->GetspAction( GetspAction()->GetidAct() );
	if( !spSrcAct )	return FALSE;
//	여기서ㅑ Assign의 의미는 가지고 있던 키값까지 복구를 의미하는가 XAniAction에만 있는값을 의미하는가.
 	spSrcAct->RestoreValue( m_spAction ); 
	return TRUE;
}		
// 리두행동(언두로 인해 되살려진 액션값들을 After값으로 재실행)
BOOL XUndoActionChange::DoRedo() 
{
	XSPAction spSrcAct = m_pSprObj->GetspAction( GetspAction()->GetidAct() );
	if( !spSrcAct )	return FALSE;
 	spSrcAct->RestoreValue( m_spActionAfter );
	return TRUE;
}
void XUndoActionChange::SetActionAfter( XSPAction spActionAfter ) 
{
//	m_spActionAfter = spActionAfter->CopyDeep();
	m_spActionAfter = spActionAfter->CreateCopy( m_pSprObj->GetpSprDat() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 언두행동(삭제했던 액션 되살림)
BOOL XUndoDelAction::DoUndo()
{
	TOOL->AddAction( m_spAction );
	m_spAction = nullptr;
	return TRUE;
}
// 리두행동(언두로 인해 되살려진 액션를 다시 삭제함)
BOOL XUndoDelAction::DoRedo()
{
	// 액션을 삭제하고 this에 보관한다.
	m_spAction = TOOL->DelAct( m_idAct );
	//	m_pAction = m_pObjAct->GetspAction();
	//	m_idAct = m_spAction->GetidAct();
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 언두행동(삭제했던 스프라이트 되살림)
BOOL XUndoDelSprite::DoUndo() 
{ 
	m_pSprObj->GetpSprDat()->AddSprite( GetpSprite(), TRUE );		// 스프라이트 복구시킨다. 인덱스는 새로만들지 않고 원래 값을 그대로 쓴다
	m_pSprObj->GetpSprDat()->SortSprite();
	// 다시 스프라이트을 살려서 돌려줬기때문에 this가 파괴될때 스프라이트을 파괴하면 안되기때문에 널로 만듬
	m_pSprite = nullptr;
	return TRUE;
}		
// 리두행동(언두로 인해 되살려진 스프라이트를 다시 삭제함)
BOOL XUndoDelSprite::DoRedo() 
{
	m_pSprite = m_pSprObj->GetpSprDat()->DelSprite( m_pSpriteBackup );		// 다시 삭제했으면 삭제한 스프라이트포인터를 다시 받아온다
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 언두행동(바뀐스프라이트값을 원래대로 회복)
void XUndoSprite::Destroy() 
{ 
	SAFE_DELETE( m_pSprite ); 
}
//////////////////////////////////////////////////////////////////////////
// pSprite:바뀌기전 오리지날 스프라이트
XUndoSpriteChange::XUndoSpriteChange( XSprObj *pSprObj, 
									XSprite *pUndoSprite, 
									LPCTSTR szText /*= _T( "" )*/ ) 
	: XUndoSprite( pSprObj, pUndoSprite, szText ) 
{	
	Init();
	m_pSprite = m_pSpriteBackup = pUndoSprite->Copy( TRUE );		// image데이타를 제외한 속성값만 카피
	m_pSprite->SetidSpr( pUndoSprite->GetidSpr() );
}
void XUndoSpriteChange::Destroy() 
{
	SAFE_DELETE( m_pSprite );
	SAFE_DELETE( m_pSpriteAfter );
}
BOOL XUndoSpriteChange::DoUndo() 
{ 
	XSprite *pSrcSpr = m_pSprObj->GetpSprDat()->FindSprite( m_pSprite->GetidSpr() );
	pSrcSpr->ValueAssign( m_pSprite );		// 카피본의 값을 카피 메모리할당관련 변수들은 카피하지 않음
	pSrcSpr->ChangeAdjust( pSrcSpr->GetAdjustX(), pSrcSpr->GetAdjustY() );		// 바뀐 adjust값들을 반영하려면 이걸 불러줘서 버텍스버퍼 내용까지 고쳐줘야 한다
	return TRUE;
}
// 리두 행동
BOOL XUndoSpriteChange::DoRedo() 
{ 
	XSprite *pSrcSpr = m_pSprObj->GetpSprDat()->FindSprite( m_pSpriteAfter->GetidSpr() );
	pSrcSpr->ValueAssign( m_pSpriteAfter );
	pSrcSpr->ChangeAdjust( pSrcSpr->GetAdjustX(), pSrcSpr->GetAdjustY() );
	return TRUE;
}
// 값이 바뀌고난후의 스프라이트의 복사본을 만든다(리두에 쓰임)
void XUndoSpriteChange::SetSpriteAfter( XSprite *pSpriteAfter ) 
{				
	m_pSpriteAfter = pSpriteAfter->Copy( TRUE );			// 리두할 스프라이트의 복사본을 만든다
	m_pSpriteAfter->SetidSpr( pSpriteAfter->GetidSpr() );
}

//////////////////////////////////////////////////////////////////////////
// XUndoObjAct::XUndoObjAct( XSprObj *pSprObj, XObjAct *pUndoObjAct, LPCTSTR szText )
// 	: XUndoAction( pSprObj, pUndoObjAct->GetspAction(), szText ) 
// {
// 	Init();
// 	m_pObjAct = pUndoObjAct;		// 액션언두는 복사본을 만들지 않고 포인터만 옮겨온다(액션복사본 만들려면 장난아니다)
// 	m_pAction = pUndoObjAct->GetspAction();
// 	m_idAct = m_pAction->GetidAct();
// }
// 
// void XUndoObjAct::Destroy() 
// {
// 	SAFE_DELETE( m_pObjAct );
// 	SAFE_DELETE( m_pAction );		// m_pObjAct가 파괴될때 m_pAction은 파괴하지 않기때문에 따로 파괴해야함
// }
