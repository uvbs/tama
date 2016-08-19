#include "stdafx.h"
#include "XKeyCreateObj.h"
#include "XLayerObject.h"
#include "XLua.h"
#include "SEFont.h"
#include "SprObj.h"
#include "SprDat.h"
#include "XAniAction.h"
#include "DlgObjProperty.h"
#include "SprMng.h"
#include "XActObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

///////////////// Key CreateObj //////////////////////////////////////////
XKeyCreateObj::XKeyCreateObj( XSPAction spAction, DWORD id, LPCTSTR szSprObj, XSPLayerObject spLayer, int nAction, xRPT_TYPE playType, float fFrame, float x, float y )
	: XBaseKey( spAction, xSpr::xKT_CREATEOBJ, xSpr::xKTS_MAIN, fFrame, spLayer )
{
	Init();
	m_dwID = id;
	_tcscpy_s( m_szSprName, szSprObj );
	m_nAction = nAction;
	m_PlayType = playType;
	//m_spLayer = spLayer;
}
// XKeyCreateObj::XKeyCreateObj( SPAction spAction, DWORD id, LPCTSTR szSprObj, xSpr::xtLayer type, int nLayer, int nAction, xRPT_TYPE playType, float fFrame, float x, float y ) 
// 		: XBaseKey( spAction, xSpr::xKT_CREATEOBJ, xSpr::xKTS_MAIN, fFrame, type, nLayer )
// {
// 	Init();
// 	m_dwID = id;
// 	_tcscpy_s( m_szSprName, szSprObj );
// 	m_nAction = nAction;
// 	m_PlayType = playType;
// }
// void XKeyCreateObj::SetspLayer( XSPBaseLayer& spLayer )
// {
// 	XBaseKey::SetspLayer( spLayer );
// 	//m_spLayer = std::static_pointer_cast<XLayerObject>( spLayer );
// }
// XBaseKey* XKeyCreateObj::CopyDeep()
// {
// 	return new XKeyCreateObj( *this );
// 	// 	XKeyCreateObj *pKey = new XKeyCreateObj( spAction, this );
// 	// 	pKey->SetidKey( GetidKey() );
// 	// 	return pKey;
// }


// XBaseKey* XKeyCreateObj::CreateCopy()
// {
// 	return new XKeyCreateObj( *this, spActNew, idLayer, idLocalInLayer );
// }
XBaseKey* XKeyCreateObj::CreateCopy()
{
//	auto pKeyNew = new XKeyCreateObj( *this, spActNew, idLayer, idLocalInLayer );		// 기본카피
	auto pKeyNew = new XKeyCreateObj( *this );		// 기본카피
//	pKeyNew->SetNewInstance( spActNew, idLayer, idLocalInLayer );
	return pKeyNew;
}

void XKeyCreateObj::Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec )
{
	XBaseKey::Execute( spActObj, spLayer, fOverSec );
	//
	const auto pSprObj = spActObj->GetpSprObj();
	XSprObj *pNewSprObj = nullptr;
	int execute = 1;
	// 루아가 있으면 실행
	if( GetpcLua() && XE::IsHave( GetpcLua() ) ) {
		char cTable[ 64 ];
		sprintf_s( cTable, "key_%08x", GetidKey() );		// 테이블 이름 생성
		if( GetpLua()->IsHaveMemberFunc( cTable, "CreateSprObj" ) )
			pNewSprObj = GetpLua()->MemberCall<XSprObj*>( cTable, "CreateSprObj", Convert_TCHAR_To_char( GetszSprName() ), m_nAction, m_PlayType, GetdwID(), pSprObj );		// SprObj생성요청 핸들러 호출
		else
			CONSOLE( "key id=0x%08x: CreateSprObj호출 실패", GetidKey() );
		if( pNewSprObj ) {
			m_idSprObj = pNewSprObj->GetdwID();	// 아이디 바꿔주고
			// 루아로 인해 플레이 정보가 바껴도 이건 '원본'개념으로 그대로 놔두자. 그래서 루아에 넘겨줄땐 항상 원본값을 넘겨주게
		} else
			m_idSprObj = 0;
		// 루아에서 만들어낸 sprobj가 기존거랑 아이디가 달라졌다면
		execute = 0;	// 이건 별 의미가 없군
		if( pNewSprObj ) {
			pNewSprObj->SetpParentKey( this );
			pNewSprObj->SetpParent( pSprObj );
		}
	}	else {

		pNewSprObj = GetpSprObjChild( spActObj, spLayer );
	}
	// 키 실행
	if( pNewSprObj ) {
		if( m_nAction == 0 ) {	// 키에 액션번호가 없으면 오브젝트의 첫번째 액션의 아이디를 자동으로 넣어준다. 이런경우는 없어야 한다.
			auto spActObj = pNewSprObj->GetspActObjByIndex(0);
			if (spActObj) {
				m_nAction = spActObj->GetidAct();
				pNewSprObj->SetAction(m_nAction, m_PlayType);
			}
		}
	}
	auto spLayerObject = std::static_pointer_cast<XLayerObject>( spLayer );
	if( XASSERT( spLayerObject ) )
		spLayerObject->SetpSprObjCurr( pNewSprObj );								// 오브젝트레이어에 추가한 차일드를 기억시켜둔다. 
}

/**
 @brief 키에 지정된 spr을 로딩하고 지정된 액션으로 플레이를 시킨다.
*/
XSprObj* XKeyCreateObj::GetpSprObjChild( XSPActObj spActObj, XSPBaseLayer spLayer )
{
	const auto pSprObj = spActObj->GetpSprObj();
	const ID idAct = spActObj->GetidAct();
	const ID idLayer = spLayer->GetidLayer();
	auto pNewSprObj = pSprObj->AddSprObj( GetszSprName()
//																			, m_nAction
																			, m_PlayType
																			, GetdwID()
																			, -1
																			, idAct
																			, idLayer );	// 루아내부에서 사용하는 idLocal값과 겹치지 않게 하기위해 -1을 씀
	if( pNewSprObj ) {
		ID idActChild = (ID)m_nAction;
		auto spActionChild = pNewSprObj->GetspAction( idActChild );
		if( !spActionChild ) {
			auto spActObjChild = pNewSprObj->GetspActObjByIndex( 0 );
			if( spActObjChild ) {
				idActChild = spActObjChild->GetidAct();
				XALERT( "%s에 %d액션이 없어 %d번 액션으로 대체함", GetszSprName(), m_nAction, idActChild );
				m_nAction = (int)idActChild;
			} else {
				XALERT( "%s에 %d액션이 없음", GetszSprName(), m_nAction );
			}
		}
		if( idActChild )
			pNewSprObj->SetAction( idActChild, m_PlayType );
		m_idSprObj = pNewSprObj->GetdwID();		// 재로딩을 하지않기 위해 생성한 SprObj의 아이디를 받아둠
		pNewSprObj->SetpParentKey( this );
		pNewSprObj->SetpParent( pSprObj );
	} else {
		m_idSprObj = 0;
	}
	return pNewSprObj;
}

void XKeyCreateObj::Destroy()
{
	//	SPAction& spAction = m_spAction;
	// 키가 삭제될때 액션의 listSprObj에서 같은 id가 있는지 검사해보고 그걸 삭제
	//	spAction->DestroySprObj( m_dwID );
}
void XKeyCreateObj::Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes )
{
	XBaseKey::Save( typeLayer, nLayer, pRes );
	pRes->Write( &m_dwID, 4 );
	WORD len = (WORD)(_tcslen( m_szSprName ) * sizeof( TCHAR ) + sizeof( TCHAR ));	// strlen + nullptr
	pRes->Write( &len, 2 );
	pRes->Write( m_szSprName, len );
	pRes->Write( &m_nAction, 4 );
	DWORD dw1 = 0;
	// reserved
	dw1 = (DWORD)m_PlayType;
	pRes->Write( &dw1, 4 );		// playType
	dw1 = 0;
	pRes->Write( &dw1, 4 );		// speed로 예약
	pRes->Write( &dw1, 4 );		// rotate로 예약
	pRes->Write( &dw1, 4 );		// 
	pRes->Write( &dw1, 4 );		// 
	pRes->Write( &dw1, 4 );		// scale로 예약
	pRes->Write( &dw1, 4 );		// 
	pRes->Write( &dw1, 4 );		// 
	pRes->Write( &dw1, 4 );		// 
}
void XKeyCreateObj::Load( XResFile *pRes, XSPAction spAction, int ver )
{
	//	XBaseKey::Load( pRes, spAction, ver );
	//m_spLayer = std::static_pointer_cast<XLayerObject>( XBaseKey::GetspLayer() );
	pRes->Read( &m_dwID, 4 );
	WORD w1;
	pRes->Read( &w1, 2 );						// string 길이
	pRes->Read( m_szSprName, w1 );
	pRes->Read( &m_nAction, 4 );
	if( ver >= 9 ) {
		DWORD dw1 = 0;
		// reserved
		pRes->Read( &dw1, 4 );		// playType
		m_PlayType = (xRPT_TYPE)dw1;
		pRes->Read( &dw1, 4 );		// speed로 예약
		pRes->Read( &dw1, 4 );		// rotate로 예약
		pRes->Read( &dw1, 4 );		// 
		pRes->Read( &dw1, 4 );		// 
		pRes->Read( &dw1, 4 );		// scale로 예약
		pRes->Read( &dw1, 4 );		// 
		pRes->Read( &dw1, 4 );		// 
		pRes->Read( &dw1, 4 );		// 
	}
}
BOOL XKeyCreateObj::EditDialog()
{
	CDlgObjProperty dlg;
	dlg.m_PlayType = m_PlayType;
	dlg.m_id = m_nAction;
	if( GetpcLua() )
		dlg.m_strLua = GetpcLua();
	if( dlg.DoModal() == IDOK )
	{
		m_PlayType = dlg.m_PlayType;
		m_nAction = dlg.m_id;
		if( !dlg.m_strLua.IsEmpty() )		// 루아코드가 입력이 됐다면
		{
			TCHAR szBuff[ 0xffff ] = {0, };
			_tcscpy_s( szBuff, dlg.m_strLua );
			SetpcLua( CreateConvert_TCHAR_To_char( szBuff ) );
		}
		return TRUE;
	}
	return FALSE;
}
CString XKeyCreateObj::GetToolTipString( XSPBaseLayer spLayer )
{
	CString str, strAct, strPlay;
	std::vector<xLayerInfoByAction> aryLayerInfo;
	XSprDat *pSprDat = SPRMNG->Load( m_szSprName, &aryLayerInfo, TRUE );
	if( pSprDat )
		if( pSprDat->GetspAction( m_nAction ) )
			strAct = CString( pSprDat->GetspAction( m_nAction )->GetszActName() );
	switch( m_PlayType )
	{
	case xRPT_LOOP:	strPlay = _T( "반복" );		break;
	case xRPT_1PLAY:	strPlay = _T( "1회플레이" );	break;
	}
	str.Format( _T( "ID:0x%x\r\r\n파일:%s\r\n액션:%s(%d)\r\n플레이방식:%s" ), m_dwID, m_szSprName, strAct, m_nAction, strPlay );
	str = XBaseKey::GetToolTipString( spLayer ) + str;
	str += XBaseKey::GetToolTipStringTail();
	SPRMNG->Release( pSprDat );
	return str;
}
// cDst의 스트링에 this가 가지고 있는 루아코드를 덧붙이는데 함수명을 교체해서 넣어준다
void XKeyCreateObj::CompositLuaCodes( CString *pstrLua )
{
	XBaseKey::ReplaceFuncName( pstrLua, _T( "CreateSprObj" ) );		// 함수명을 테이블함수 형태로 바꾼다
	//
	XBaseKey::CompositLuaCodes( pstrLua );
}

void XKeyCreateObj::GetToolTipSize( float *w, float *h ) 
{
	XBaseKey::GetToolTipSize( w, h );
	*w += SE::g_pFont->GetFontWidth() * 6;
	*h += SE::g_pFont->GetFontHeight() * 4;
};
