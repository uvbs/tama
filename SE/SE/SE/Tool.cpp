#include "stdafx.h"
#include "Tool.h"
#include "MainFrm.h"
#include "ActionListView.h"
#include "FrameView.h"
#include "ThumbView.h"
#include "AdjustView.h"
#include "AnimationView.h"
#include "ConsoleView.h"
#include "Token.h"
#include "UndoMng.h"
#include "XViews.h"
#include "XAniAction.h"
#include "Sprite.h"
#include "XKeyBase.h"
#include "XLayerBase.h"
#include "XLayerAll.h"
#include "XActObj.h"
//#include "WndLeftTool.h"

using namespace xSpr;
//#define SELECTKEY_LOOP( I )				\
//								XBaseKey_Itor I; \
//								for( I = m_listSelectKey.begin(); I != m_listSelectKey.end(); I ++ ) \
		
//#define SELECTKEY_MANUAL_LOOP( I )				\
//								XBaseKey_Itor I; \
//								for( I = m_listSelectKey.begin(); I != m_listSelectKey.end(); ) 
#define SHADOWKEY_LOOP( I )				\
								XBaseKey_Itor I; \
								for( I = m_listShadowKey.begin(); I != m_listShadowKey.end(); I ++ ) \
		
#define SHADOWKEY_MANUAL_LOOP( I )				\
								XBaseKey_Itor I; \
								for( I = m_listShadowKey.begin(); I != m_listShadowKey.end(); ) 

//////////////////////////////////////////////////////////////////////////
_D3DVIEWPORT9 XViewport::s_Viewport = { 0, 0, 0, 0, 0, 0 };	// 현재 뷰포트
std::stack<_D3DVIEWPORT9> XViewport::s_stackViewport;	// 뷰포트 스택
XViewport::XViewport( DWORD x, DWORD y, DWORD r, DWORD b ) 
{
	// 기존뷰포트값은 스택에 푸쉬시킨다.
	s_stackViewport.push( s_Viewport );
	// 현재 뷰포트 값을 세팅한다.
	s_Viewport.X = x;
	s_Viewport.Y = y;
	s_Viewport.Width = r - x;
	s_Viewport.Height = b - y;
	D3DDEVICE->SetViewport( &s_Viewport );
}

XViewport::~XViewport()
{
	// 스택의 뷰포트값을 복구시킨다.
	s_Viewport = s_stackViewport.top();
	// 초기값은 처리하지 않음.
	if( s_Viewport.Width > 0 && s_Viewport.Height > 0 )
		D3DDEVICE->SetViewport( &s_Viewport );
}

//////////////////////////////////////////////////////////////////////////
static XTool s_Tool;
XTool* GetTool()
{
//	static XTool s_Tool;
	return &s_Tool;
}
XTool::XTool() 
{ 
	m_pDefineAct = nullptr;
	UNDOMNG = new XUndoMng;
	Init(); 
	Create(); 
}
void XTool::Destroy()
{
	m_spSelActObj.reset();
	SAFE_DELETE( m_pSprObj );
	m_SelectKey.Clear();
	m_ShadowKey.Clear();
	m_SelToolTipKey.Clear();
	m_SelSpr.Clear();
	UNDOMNG->Clear();		// SprNew나 SprLoad같은게 실행되면 언두스택도 모두 클리어
	SAFE_DELETE( SPRMNG );
//	SPRMNG->Destroy();		// 스프라이트 데이타도 모두 지워줘야 함
}

void XTool::Create( BOOL bCreateDat )
{
	SPRMNG = new XSprMng;
	m_pSprObj = new XSprObjTool;
	if( bCreateDat )
		m_pSprObj->NewSprDat();
}

void XTool::SaveINI( LPCTSTR _szFilename )
{
	TCHAR szWork[ 1024 ];
	XE::GetCwd( szWork, sizeof( szWork ) / sizeof( TCHAR ) );	// SE.exe 실행파일 폴더를 얻어냄
	CString strIniName = szWork;
	strIniName += _T( "\\se.ini" );
	CONSOLE( "GetCwd:%s", szWork );
	GetCurrentDirectory( 1024, szWork );		// spr파일을 직접 더블클릭해서 실행시킬땐 se.ini를 spr이 있는 폴더에서 찾아서 이방법 포기함. 현재디렉토리(작업디렉터리)를 얻어옴
	CONSOLE( "GetCurrentDirectory:%s", szWork );

	if( strIniName.IsEmpty() )
		strIniName = _T("se.ini");
	XResFile file, *pRes = &file;
	if( !m_strWorkPath.IsEmpty() )
	{
		if( pRes->Open( strIniName, XBaseRes::xWRITE ) ) 
		{
			CString str;
			TCHAR szBuff[0xffff];
			WORD mark = 0xFEFF;
			pRes->Write( &mark, 2 );
			// working folder
			str.Format( _T("Working_Folder	\"%s\"\r\n"), m_strWorkPath );
			_tcscpy_s( szBuff, str );
			pRes->Write( szBuff, _tcslen(szBuff), sizeof(TCHAR) );
			_tstring strstd = XE::Format(_T("bg\t\t\"%s\"\r\n"), m_pathBg.c_str() );
			pRes->Write( (void*)strstd.c_str(), strstd.length(), sizeof( TCHAR ) );
	//		pRes->Printf( _T("Working_Folder	\"%s\"\r\n"), m_strWorkPath );		// 워킹폴더를 ini에 저장함
			// lua glue
			if( XE::IsHave( XSprDat::s_cGlobalLua ) )
			{
				str.Format( _T("LuaGlue\r\n{\r\n%s\r\n}\r\n"), Convert_char_To_TCHAR( XSprDat::s_cGlobalLua ) );
				pRes->Write( (void*)((LPCTSTR)str), str.GetLength(), sizeof(TCHAR) );
			}
			str.Format( _T("grid_unit\t\t%d\r\n"), GetFrameView()->m_GridUnit );
			pRes->Write( (void*)((LPCTSTR)str), str.GetLength(), sizeof( TCHAR ) );

		}
		else {
			GetConsoleView()->Message( _T("%s 저장실패"), strIniName );
			return;
		}
		GetConsoleView()->Message( _T("%s 저장성공"), strIniName );
	}
}

void XTool::LoadINI()
{
	TCHAR szWork[1024];
	XE::GetCwd( szWork, sizeof(szWork) / sizeof(TCHAR)  );	// SE.exe 실행파일 폴더를 얻어냄
	CString strIniName = szWork;
	strIniName += _T("\\se.ini");
	CONSOLE( "GetCwd:%s", szWork );
	GetCurrentDirectory( 1024, szWork );		// spr파일을 직접 더블클릭해서 실행시킬땐 se.ini를 spr이 있는 폴더에서 찾아서 이방법 포기함. 현재디렉토리(작업디렉터리)를 얻어옴
	CONSOLE( "GetCurrentDirectory:%s", szWork );

	{
		CToken token;
		if( token.LoadFile( strIniName, TXT_UTF16 ) )		{
			while( token.GetToken() )		// "Working_Folder"
			{
				if( token == _T("Working_Folder") )	{
					m_strWorkPath = token.GetToken();
				} else
				if( token == _T("bg") ) {
					auto szToken = token.GetToken();
					if( XE::IsHave(szToken) )
						m_pathBg = szToken;
				} else
				if( token == _T( "grid_unit" ) ) {
					GetFrameView()->m_GridUnit = (CFrameView::xtGridUnit)token.GetNumber();
				} else
				if( token == _T("LuaGlue") )
				{
					token.GetToken();		// {
					char* cblock = token.CreateGetBlock();		// 블럭안의 코드를 모두 읽는다
					strcpy_s( XSprDat::s_cGlobalLua, cblock );
					SAFE_DELETE( cblock );
//					token.GetToken();		// lua본문을 읽는다.
//					strcpy_s( XSprDat::s_cGlobalLua, Convert_TCHAR_To_char( token.m_Token ) );
//					token.GetToken();		// }
				}
			}
			if( m_strWorkPath.IsEmpty() )
				XERROR( "Working_Folder항목을 찾을수 없습니다. 에러가 계속 되는경우 se.ini파일을 지우고 다시 실행하십시요." );

			GetConsoleView()->Message( _T("%s 로딩완료"), strIniName );
		} else
		{
			// 최초실행시 defineAct.h가 있는 워킹폴더를 지정하는 단계
			{
				TCHAR szDisplayName[MAX_PATH];
				BROWSEINFO bi;
				bi.hwndOwner = g_pMainFrm->GetSafeHwnd();
				bi.lParam = 0;
				bi.lpfn = nullptr;
				bi.lpszTitle = _T("리소스 폴더를 지정해주세요");
				bi.pidlRoot = nullptr;
				bi.pszDisplayName = szDisplayName;
				bi.ulFlags = 0;
				ITEMIDLIST *pidList = SHBrowseForFolder( &bi );
				if( pidList )
				{
					SHGetPathFromIDList( pidList, szDisplayName );
					m_strWorkPath = szDisplayName;
					// 폴더 선택후 곧바로 ini에 기록
					SaveINI( strIniName );
				}
			}
		}
	}

	GetConsoleView()->Message( _T("working folder: %s"), m_strWorkPath );
	XE::SetWorkDir( m_strWorkPath );

	// defineAct.h를 읽음
	CString str = m_strWorkPath + _T("\\defineAct.h");
	if( m_pDefineAct ) {
		XLOG( "" );
		SAFE_DELETE( m_pDefineAct );
	}
	m_pDefineAct = new CDefine;
	if( !m_pDefineAct->Load( str ) ) {
		GetConsoleView()->Message( _T("%s를 찾을 수 없습니다"), str );
		XALERT( "%s를 찾을 수 없습니다", str );
		SAFE_DELETE( m_pDefineAct );
	} else
		GetConsoleView()->Message( _T("%s 로딩완료"), str );
}

BOOL XTool::LoadSpr( LPCTSTR szFullPath )
{
	CString strFilename( XE::GetFileName( szFullPath ) );
	CString strPath( XE::GetFilePath( szFullPath ) );
	SetstrPath( strPath );
	UNDOMNG->Clear();
	Destroy();			// 로드하기전에 툴내에 sprobj를 파괴
	Init();					// 툴 정보 초기화
	Create( FALSE );				// sprobj 생성 Sprdat는 생성하지 않음
	BOOL retv = m_pSprObj->Load( strFilename );
//	m_pSprObj->SetAction( 0 );
	m_pSprObj->TraceActObj();
	if( retv ) {
		CONSOLE( "%s 로드성공.", strFilename );
//		if( m_pSprObj->GetAction() )		// 폰트류는 액션이 없음 
//			SetSelAct( m_pSprObj->GetAction()->GetActID() );
		SetSelActIdx( 0  );	// 첫번째 액션을 자동 선택함
		SetstrFilename( strFilename );
	}
	else
	{
		CONSOLE( "%s 로드실패.", strFilename );
		return FALSE;
	}
// 	if( GetThumbView() )
// 		GetThumbView()->Update();
// 	if( GetAdjustView() )
// 		GetAdjustView()->Update();
// 	if( GetFrameView() )
// 		GetFrameView()->Update();
// 	if( GetActionListView() )
// 		GetActionListView()->Update();
	XViews::sUpdateAllView();
	if( g_pMainFrm )
		g_pMainFrm->SetTitle();	// ->Update()로 바꿔서 일관되게 할것
	return TRUE;
}
// 언두에쓰는 액션추가함수
void XTool::AddAction( XSPAction spAction )
{
	SPROBJ->AddAction( spAction );
//	SPRDAT->AddAction( spAction );
	XViews::sUpdateActionListView();
// 	m_pSprObj->AddObjAct( pObjAct );
// 	GetActionListView()->AddAction( pObjAct->GetspAction()->GetszActName(), pObjAct->GetspAction()->GetActID() );
}

XSPActObj XTool::CreateAction( LPCTSTR szName )
{
	auto spActObj = m_pSprObj->CreateAction( szName );
	if( XASSERT(spActObj) ) {
		// 새 액션이 만들어지면 디폴트로 이미지 레이어를 하나 만든다.(이거 왜 없어졌지?)
		spActObj->CreateAddLayer( xLT_IMAGE, -1 );
	}
//	GetActionListView()->AddAction( szName, spAction->GetidAct() );
//	GetActionListView()->Update();
	XViews::sUpdateActionListView();
	return spActObj;
}

/**
 @brief id액션을 선택한다.
*/
void XTool::SetSelAct( DWORD idAct ) 
{ 
	auto spActObjCurr = m_pSprObj->GetspActObjCurr();
	if( spActObjCurr )
		spActObjCurr->SendLayerEvent_OnSelected(0);		// 이전액션의 모든레이어에 OnSlected-Leave이벤트를 보냄
	UNDOMNG->Clear();
	m_pSprObj->SetAction( idAct );
//	m_spSelAct = m_pSprObj->GetspAction();
	m_spSelActObj = m_pSprObj->GetspActObjCurr();
	if(m_spSelActObj)
		m_spSelActObj->SendLayerEvent_OnSelected(1);		// 액션내의 모든 레이어에 OnSelected-Enter이벤트를 보냄
	XViews::sUpdateFrameView();
}
void XTool::CopyAct( XSPActionConst spSrc )
{
// 	CString strName = spSrc->GetszActName();
// 	strName += " copy";
//	auto spNewAct = m_pSprObj->CopyAddAction( spSrc/*, strName*/ );
	auto spNewActObj = m_pSprObj->CreateAddCopyAction( spSrc/*, strName*/ );
//	GetActionListView()->AddAction( pNewAct->GetszActName(), pNewAct->GetActID() );
	XViews::sUpdateActionListView();
	XViews::sUpdateAnimationView();
	XViews::sUpdateFrameView();
}
// 인덱스로 액션을 찾는 버전. 툴에서만 사용함
void XTool::SetSelActIdx( int index )
{
//	if( m_pSprObj->GetpObjActCurr() )
//		m_pSprObj->GetpObjActCurr()->SendLayerEvent_OnSelected(0);		// 이전액션의 모든레이어에 OnSlected-Leave이벤트를 보냄
//	m_pSelAct = m_pSprObj->GetpSprDat()->GetActionIndex( index );		// index버전으로 액션을 찾음
//	XSPAction spAction = m_pSprObj->GetpSprDat()->GetspActionByIndex( index );
	auto spActObj = m_pSprObj->GetspActObjByIndex(index);
	if( spActObj ) {
		SetSelAct( spActObj->GetidAct() );
//		m_pSprObj->SetAction( m_pSelAct->GetActID() );
//		m_pSprObj->GetpObjActCurr()->SendLayerEvent_OnSelected(1);		// 액션내의 모든 레이어에 OnSelected-Enter이벤트를 보냄
	}
	if(spActObj)
		SetspSelLayer(spActObj->GetFirstLayer() );		// 새로선택안 액션의 첫번째 레이어를 자동으로 선택해준다
	else
		SetspSelLayer( nullptr );		// 새로선택안 액션의 첫번째 레이어를 자동으로 선택해준다
	TOOL->GetSelectKey().Clear();
	m_ShadowKey.Destroy();
//	UNDOMNG->Clear();		// 액션이 바뀔때 언두스택 클리어
	GetFrameView()->Update();
	GetAnimationView()->Update();
}

// idAct 액션을 삭제한다. 0이면 현재액션을 삭제한다. 언두때문에 실제로 삭제하는게 아니기때문에 그 액션의 포인터를 받아온다
XSPAction XTool::DelAct( ID idAct )
{
	XSPAction spAction = nullptr;
	if( idAct == 0 )
		spAction = m_pSprObj->GetspAction();
	else
		spAction = m_pSprObj->GetspAction( idAct );
	if( spAction ) {
		m_pSprObj->DelAction( idAct );
//		XObjAct *pObjAct = m_pSprObj->DelAction( spAction->GetidAct() );
		SetSelActIdx( 0 );		// 현재선택된 액션을 지웠으므로 현재액션을 바꿔줘야 한다
		XViews::sUpdateAllView();
	}
	return spAction;
}
// XObjAct* XTool::DelAct( ID idAct )
// {
// 	SPAction spAction = nullptr;
// 	if( idAct == 0 )
// 		spAction = m_pSprObj->GetAction();
// 	else
// 		spAction = m_pSprObj->GetAction( idAct );
// 	if( spAction )
// 	{
// 		XObjAct *pObjAct = m_pSprObj->DelAction( spAction->GetidAct() );
// 		SetSelActIdx( 0 );		// 현재선택된 액션을 지웠으므로 현재액션을 바꿔줘야 한다
// 		AllViewUpdate();
// 		return pObjAct;
// 	}
// 	return nullptr;
// }
// void XTool::AllViewUpdate()
// {
// 	GetThumbView()->Update();
// 	GetAdjustView()->Update();
// 	GetAnimationView()->Update();
// 	GetFrameView()->Update();
// 	GetActionListView()->Update();
// }

/*
. png를 로드한다.
. 32bpp인지 확인한다.
. 그림데이타를 메모리로 옮긴다. 
. 공백부분을 잘라내고 Adjust를 조정한후 XSprite에 넣는다.
*/
BOOL XTool::AddAniFrame( int nFileIdx, LPCTSTR szFilename, BOOL bCreateKey )
{
	DWORD *pImage;
	int w, h;
	GRAPHICS_D3DTOOL->LoadImg( szFilename, &w, &h, &pImage );		// png 로드하여 메모리에 올린다.
	if( XBREAK(pImage == nullptr) )
		return FALSE;
	if( XBREAK(w <= 0) )
		return FALSE;
	if( XBREAK(h <= 0) )
		return FALSE;

	return AddAniFrame( nFileIdx, w, h, pImage, szFilename, bCreateKey );
}

BOOL XTool::AddAniFrame( int idx, int w, int h, DWORD *pImage, LPCTSTR szFilename, BOOL bCreateKey )
{
	int adjx=0, adjy=0;

	CutAndAdjustImage( &w, &h, &adjx, &adjy, &pImage );			// 공백부분을 잘라내고 Adjust를 조정한다.
	// adjust를 중앙아래로 맞춘다.
	static int movex=0, movey=0;
	if( idx == 0 )
	{
		int newAdjx = -(w / 2);
		int newAdjy = (-h);
		movex = newAdjx - adjx;
		movey = newAdjy - adjy;
	}
	adjx += movex;
	adjy += movey;
	RECT rect;
	rect.left = rect.right = rect.top = rect.bottom = 0;		// 전체를 의미
	TCHAR szName[1024];
	memset( szName, 0, sizeof(szName) );
	if( szFilename ) {
		LPCTSTR sz = GET_FILE_NAME(szFilename);
		if( sz )
			_tcscpy_s( szName, sz );
	}
	auto spActObjCurr = m_pSprObj->GetspActObjCurr();
	if (spActObjCurr) {
		//	auto spActCurr = m_pSprObj->GetspActionCurr();
//		auto spLayerImage = spActCurr->GetspLayerImage(0);
//		auto spLayerImage = spActObjCurr->GetspLayerByNumber<XLayerImage>(0);
		// 현재 선택된 레이어에 프레임들을 넣고 선택된레이어가 없다면 첫번째 이미지레이어에 넣는다.
		auto spLayerImage = (m_spSelLayer)? m_spSelLayer : spActObjCurr->GetspLayerByIndex<XLayerImage>(0);
		auto pSpr = SPRDAT->IsExistSprInfo(szName, nullptr);		// 같은 원본파일명을 가지는 스프라이트가 있는가
		if (pSpr) {
			//		pSpr->SetImage( XHRS(w), XHRS(h), XHRS(adjx), XHRS(adjy), pImage );		// 해당스프라이트의 이미지정보를 교체함
			pSpr->SetImage(XHRS(w), XHRS(h), pSpr->GetAdjustX(), pSpr->GetAdjustY(), pImage);		// 해당스프라이트의 이미지정보를 교체함
			// 같은 이미지가 이미있다면 키만 저장해야한다
			if (bCreateKey ) {
				// 			auto spLayer = m_pSprObj->GetLayer( XBaseLayer::xIMAGE_LAYER, 0 );
				auto pNewKey = spActObjCurr->AddKeySprAtLast( spLayerImage->GetidLayer(), pSpr);
//				auto pNewKey = SPRDAT->AddKeySprAtLast(spActCurr, spLayerImage, pSpr);
				if (idx == 0)
					UNDOMNG->PushUndo(new XUndoCreateKey(m_pSprObj, m_pSprObj->GetspAction(), pNewKey));
				else
					UNDOMNG->PushUndo(new XUndoCreateKey(m_pSprObj, m_pSprObj->GetspAction(), pNewKey), TRUE);
			}
		} else {
			pSpr = SPRDAT->IsExistSpr(pImage, adjx, adjy);		// 이미 같은이미지의 스프라이트가 있는가? 이미지는 같아도 adj가 다른그림이라면 별개로 보고 저장해야 한다/
			if (pSpr) {
				// 같은 이미지가 이미있다면 키만 저장해야한다
				if (bCreateKey ) {
					auto pNewKey = spActObjCurr->AddKeySprAtLast( spLayerImage->GetidLayer(), pSpr);
					//					auto pNewKey = SPRDAT->AddKeySprAtLast(spActCurr, spLayerImage, pSpr);
					if (idx == 0)
						UNDOMNG->PushUndo(new XUndoCreateKey(m_pSprObj, m_pSprObj->GetspAction(), pNewKey));
					else
						UNDOMNG->PushUndo(new XUndoCreateKey(m_pSprObj, m_pSprObj->GetspAction(), pNewKey), TRUE);
				}
				if (XE::IsEmpty(pSpr->GetszSrcFile()))				// 소스파일명에 아무것도(nullptr)없을때만 새로 갱신해 넣는다. 만약 이미 이름이 있는상태라면 바꾸지 않는다.
					pSpr->SetSrcInfo(szName, rect);		// 소스이미지 정보만 갱신한다
			} else {
				// 같은 이미지나 이미지정보가 없으면 새로운 스프라이트로 등록하면서 키를 추가한다(옵션)
				const XE::VEC2 vSizeOrig(w, h);
				const XE::VEC2 vSize = vSizeOrig * 0.5f;
				const XE::VEC2 vAdj = XE::VEC2(adjx, adjy) * 0.5f;
				pSpr = m_pSprObj->AddAniFrame(vSize, vAdj, vSizeOrig, pImage, bCreateKey);
				//			pSpr = SPRDAT->AddAniFrame( m_pSprObj, XHRS(w), XHRS(h), XHRS(adjx), XHRS(adjy), w, h, pImage, bCreateKey );
				if( pSpr )
					pSpr->SetSrcInfo(szName, rect);
			}
		}
	}
	return TRUE;
}

void XTool::CutAndAdjustImage( int *pWidth, int *pHeight, int *pAdjustX, int *pAdjustY, DWORD **ppImage )
{
	RECT newRect = { 0, 0, (*pWidth) - 1, (*pHeight) - 1 };;
	// 공백탐지
	int x, y;
	int w, h;
	BYTE alpha;

	w = *pWidth;
	h = *pHeight;
	DWORD *pData = *ppImage;
	// top 시작위치 검색
	for( y = 0; y < h; y ++ ) {
		for( x = 0; x < w; x ++ ) {
			alpha = (pData[ y * w + x ] & 0xFF000000) >> 24;
			if( alpha != 0 ) {
				newRect.top = y;
				goto LP1;
			}
		}
	}
LP1:
	// bottom 시작위치 검색
	for( y = h-1; y >= 0 ; y -- ) {
		for( x = 0; x < w; x ++ ) {
			alpha = (pData[ y * w + x ] & 0xFF000000) >> 24;
			if( alpha != 0 ) {
				newRect.bottom = y;
				goto LP2;
			}
		}
	}
LP2:
	// left 시작위치 검색
	for( x = 0; x < w; x ++ )
	{
		for( y = 0; y < h; y ++ )
		{
			alpha = (pData[ y * w + x ] & 0xFF000000) >> 24;
			if( alpha != 0 ) {
				newRect.left = x;
				goto LP3;
			}
		}
	}
LP3:
	// right시작위치 검색
	for( x = w-1; x >= 0; x -- ) {
		for( y = 0; y < h ; y ++ ) {
			alpha = (pData[ y * w + x ] & 0xFF000000) >> 24;
			if( alpha != 0 ) {
				newRect.right = x;
				goto END;
			}
		}
	}
END:
	// 타이트하게 따낸 영역을 새메모리로 옮긴다.
	int newWidth = newRect.right - newRect.left + 1;
	int newHeight = newRect.bottom - newRect.top + 1;
	DWORD *pNewImage = new DWORD[ newWidth * newHeight ];
	DWORD *_pNewImage = pNewImage;
	for( y = newRect.top; y <= newRect.bottom; y ++ ) {
		for( x = newRect.left; x <= newRect.right; x ++ ) {
			*_pNewImage++ = pData[ y * w + x ];
		}
	}
	SAFE_DELETE_ARRAY( pData );
	// adjust 조정
	int adjx = *pAdjustX;
	int adjy = *pAdjustY;
	adjx += newRect.left;
	adjy += newRect.top;
	*pWidth = newWidth;	*pHeight = newHeight;
	*pAdjustX = adjx;	*pAdjustY = adjy;
	*ppImage = pNewImage;
}

void XTool::CutContinuosImage( LPCTSTR szFilename )
{
	DWORD *pImage = nullptr;
	int w, h;
	int adjx=0, adjy=0;
	XRect_List listRect;

	// png 로드하여 메모리에 올린다.
	if( GRAPHICS_D3DTOOL->LoadImg( szFilename, &w, &h, &pImage ) == FALSE )
	{
		XALERT( "%s 읽기 실패", szFilename );
		return;
	}
	// 세로공백 검사하여 RECT list를 만든다
	int x = 0, y = 0;
	RECT rect = {0,};
	rect.top = 0;
	rect.bottom = h-1;		// 세로영역은 전체를 쓴다
	while(1)
	{
		// 세로로 스캔해서 색이있는 세로라인을 찾는다
		for( ; x < w; x ++ ) {
			for( y = 0; y < h; y ++ )
			{
				if( XCOLOR_RGB_A( pImage[ y * w + x ] ) != 0 )	// 색이 있는라인을 찾았다
					goto LP1;
			}
		}
		break;		// 색이있는 라인을 못찾으면 루프 중단
LP1:
		rect.left = x++;
		// 세로로 스캔해서 투명 세로라인을 찾는다
		BOOL bTransLine;
		for( ; x < w; x ++ ) {
			bTransLine = TRUE;
			for( y = 0; y < h; y ++ )
			{
				if( XCOLOR_RGB_A( pImage[ y * w + x ] ) != 0 )	
				{
					bTransLine = FALSE;		// 투명라인이 아니다
					break;			// 색이 있으면 더이상 세로로 검사해볼필요 없다
				}
			}
			if( bTransLine )		// 투명라인을 찾았다
				break;
		}
		rect.right = x-1;
		listRect.push_back( rect );
		// 영역을 리스트에 추가한다
		if( x >= w )	// 가로끝까지 탐색했으면 중지
			break;
		x++;
	}
	if( listRect.size() == 0 )
	{
		XALERT( "잘라낼 이미지가 검출되지 않습니다" );
		return;
	}
	// 추출한 영역리스트를 기반으로 이미지를 잘라낸다
	CString strFilename = CString( GET_FILE_NAME(szFilename) );		// 패스 데어내고 파일명만 추출
	{
		int iw, ih;
		XRect_Itor itor;
		DWORD *pCutImage = nullptr;
		static int movex=0, movey=0;
		for( itor = listRect.begin(); itor != listRect.end(); itor ++ )
		{
			rect = (*itor);
			iw = (rect.right - rect.left) + 1;
			ih = (rect.bottom - rect.top) + 1;
			adjx = adjy = 0;
			DWORD *p = pCutImage = new DWORD[ iw * ih ];
			for( y = rect.top; y <= rect.bottom; y++ )
				for( x = rect.left; x <= rect.right; x++ )
					*p++ = pImage[ y * w + x ];
			CutAndAdjustImage( &iw, &ih, &adjx, &adjy, &pCutImage );		// 공백부분을 잘라내고 adjust를 조정한다
			if( itor == listRect.begin() )
			{
				int newAdjx = 0;	// 일단 폰트잘라내기 용도로 쓸것이기 때문에 중앙정렬은 하지 않았다
				int newAdjy = 0;
				movex = newAdjx - adjx;
				movey = newAdjy - adjy;
			}
			adjx += movex;
			adjy += movey;
			XE::VEC2 sizeSurface( XHRS(iw), XHRS(ih) );
			XSprite *pSpr = SPRDAT->AddSprite( sizeSurface.w, sizeSurface.h, XHRS(adjx), XHRS(adjy), iw, ih, pCutImage );
			pSpr->SetSrcInfo( strFilename, rect );
		}
		GetConsoleView()->Message( _T("%d개의 이미지를 스프라이트로 추가함"), listRect.size() );
	}
	SAFE_DELETE( pImage );
	GetThumbView()->Update();
	GetAdjustView()->Update();
}

// 일정크기의 이미지들이 바둑판처럼 배열되어있는 경우 차례로 잘라내기
#define CUT_ALIGNED_X		64
#define CUT_ALIGNED_Y		64
BOOL XTool::CutAlignedImage( LPCTSTR szFilename )
{
	DWORD *pImage = nullptr;
	int w, h;
	// png 로드하여 메모리에 올린다.
	if( GRAPHICS_D3DTOOL->LoadImg( szFilename, &w, &h, &pImage ) == FALSE )
	{
		XALERT( "%s 읽기 실패", szFilename );
		return FALSE;
	}
	if( (w % CUT_ALIGNED_X) != 0 )		// 옆에 여백이 남는다
		XALERT( "경고: image 가로크기=%d, 잘라낼크기는 %dx%d. 오른쪽끝에 %d만큼의 여백이 남습니다", w, CUT_ALIGNED_X, CUT_ALIGNED_Y, (w % CUT_ALIGNED_X) );
	if( (h % CUT_ALIGNED_Y) != 0 )		// 옆에 여백이 남는다
		XALERT( "경고: image 세로크기=%d, 잘라낼크기는 %dx%d. 아래쪽끝에 %d만큼의 여백이 남습니다", h, CUT_ALIGNED_X, CUT_ALIGNED_Y, (h % CUT_ALIGNED_Y) );
	int xcnt, ycnt;
	xcnt = w / CUT_ALIGNED_X;		// 가로세로로 그림이 몇장씩 있는가
	ycnt = h / CUT_ALIGNED_Y;
	int idx = 0;
	for( int i = 0; i < ycnt; i ++ )
	{
		for( int j = 0; j < xcnt; j++ )
		{
			DWORD *pDst = new DWORD[ CUT_ALIGNED_X * CUT_ALIGNED_Y ];
			memset( pDst, 0, sizeof(DWORD) * CUT_ALIGNED_X * CUT_ALIGNED_Y );
			DWORD *_pSrc = pImage + (i * CUT_ALIGNED_Y) * w + (j * CUT_ALIGNED_X);
			DWORD *_pDst = pDst;
			int skipx = w - CUT_ALIGNED_X;
			for( int ii = 0; ii < CUT_ALIGNED_Y; ii ++ )
			{
				for( int jj = 0; jj < CUT_ALIGNED_X; jj ++ )
				{
					*_pDst++ = *_pSrc++;
				}
				_pSrc += skipx;
			}
			// 잘라낸 낱장이미지를 스프라이트로 등록한다
			AddAniFrame( idx, CUT_ALIGNED_X, CUT_ALIGNED_Y, pDst, nullptr, TRUE );
			CONSOLE( "번호:%d/%d %dx%d 이미지를 스프라이트에 추가했습니다", idx, xcnt*ycnt, CUT_ALIGNED_X, CUT_ALIGNED_Y );
			idx++;
		}
	}

	GetThumbView()->Update();
	GetAdjustView()->Update();
	return TRUE;
}
// 현재선택한 키들을 몽땅 복사해서 쉐도우키로 만든다
void XTool::CreateShadowKey()
{
	m_SelectKey.GetNextClear();
	while( XBaseKey *pKey = (XBaseKey *)m_SelectKey.GetNext() ) {
// 		m_ShadowKey.Add( pKey->CopyKey() );
//		auto pKeyCopy = pKey->CopyDeep();
//		auto pKeyCopy = pKey->CreateCopyBySameLayer();
		auto pKeyCopy = pKey->CreateCopy();
		pKeyCopy->SetpShadowParent( pKey );
		m_ShadowKey.Add( pKeyCopy );

	}
}
// 쉐도우키의 정보를 원본에 반영한다
void XTool::ApplyShadowKey()
{
	BOOL bAppend = FALSE;
	XBaseKey *pKey;
	m_ShadowKey.GetNextClear();
	while( pKey = (XBaseKey *)m_ShadowKey.GetNext() ) {
		XBaseKey *pParent = pKey->GetpShadowParent();
//		XSPAction spAction = pParent->GetspAction();
		auto spActObj = m_pSprObj->GetspActObj(pKey->GetspAction()->GetidAct());
		const auto speedAct = spActObj->GetspAction()->GetSpeed();
		float frame = (int)(pKey->GetfFrame() / speedAct) * speedAct;		// 최소키단위로 정렬
		auto pUndo = new XUndoKeyChange( m_pSprObj, spActObj, pParent );
		pParent->SetfFrame( frame );
		pUndo->SetKeyAfter( pParent );		// 값이 변하고 난 후의 키값을 복사받음
		UNDOMNG->PushUndo( pUndo, bAppend );
		bAppend = TRUE;
	}
	m_pSprObj->GetspActObjCurr()->SortKey();						// 키리스트를 프레임순서에 따라 다시 소트한다
}
// 선택된 스프라이트들을 키로 넣기
BOOL XTool::AddKeyFromSelSpr( xSPLayerImage spLayer )
{
//	if( spLayer->GetType() == xSpr::xLT_IMAGE ) {	// 이미지레이어에만 된다
	if( spLayer->IsSameType( xSpr::xLT_IMAGE ) ) {	// 이미지레이어에만 된다
		XSPActionConst spAction = m_pSprObj->GetspAction();
		if( XBREAK( spAction == nullptr ) )
			return FALSE;
		float fFrame = m_pSprObj->GetFrmCurr();
		XSprite *pSpr;
		TOOL->GetSelSpr().GetNextClear();
		while( pSpr = static_cast<XSprite*>( TOOL->GetSelSpr().GetNext() ) )
		{
			auto pPrevKey = spAction->FindKey( spLayer, xSpr::xKT_IMAGE, xSpr::xKTS_MAIN, fFrame );	
			if( pPrevKey )
				m_pSprObj->DelKey( spAction, pPrevKey );		// 그자리에 이미 같은 종류의 키가 있으면 삭제
			m_pSprObj->AddKeySprWithFrame( spAction->GetidAct()
																		, spLayer->GetidLayer()
																		, fFrame, pSpr );
			fFrame += 1.0f;																	// 한꺼번에 드래그해서 넣으면 1.0프레임단위로 키가 들어간다
		}
		return TRUE;
	}	else {
		XLOG("spLayer가 이미지 레이어가 아니다");
	}
	return FALSE;
}

bool XTool::LoadBg( LPCTSTR szFullpath )
{
	m_psfcBg = GRAPHICS_D3DTOOL->CreateSurface( TRUE, szFullpath );
	m_pathBg = szFullpath;
	SaveINI( nullptr );
	CONSOLE( "%s Loaded.", szFullpath );
	return m_psfcBg != nullptr;
}

XSPLayerMove XTool::GetspSelLayerMove()
{
	if( m_spSelLayer && m_spSelLayer->IsTypeLayerMove() )
		return std::static_pointer_cast<XLayerMove>( m_spSelLayer );
	return XSPLayerMove();
}
xSPLayerImage XTool::GetspSelLayerImage()
{
	if( m_spSelLayer && m_spSelLayer->IsTypeLayerImage() )
		return std::static_pointer_cast<XLayerImage>( m_spSelLayer );
	return xSPLayerImage();
}
XSPLayerObject XTool::GetspSelLayerObject()
{
	if( m_spSelLayer && m_spSelLayer->IsTypeLayerObj() )
		return std::static_pointer_cast<XLayerObject>( m_spSelLayer );
	return XSPLayerObject();
}

void XTool::SetspSelLayer( XSPBaseLayer spLayer )
{
	m_spSelLayer = spLayer;
	auto spActObjCurr = m_pSprObj->GetspActObjCurr();
	if(spActObjCurr)
		spActObjCurr->UpdateLayersPos( CFrameView::sGet()->GetSizeKey() );
}
/**
 @brief 레이어를 추가하는 최상위 함수
*/
XSPBaseLayer XTool::AddLayer( xSpr::xtLayer type ) 
{
	auto spActObjCurr = m_pSprObj->GetspActObjCurr();
	if (!spActObjCurr)
		return nullptr;
	auto spLayer = spActObjCurr->CreateAddLayer( type );
	SetspSelLayer( spLayer );
	XViews::sUpdateFrameView();
	return spLayer;
}

//xSPLayerImage XTool::AddLayerImage()
//{
//	auto spAction = m_pSprObj->GetspActionCurr();
//	if( !spAction )
//		return xSPLayerImage();
//	auto spLayer = spAction->AddImgLayer();
//	SetspSelLayer( spLayer );
//	XViews::sUpdateFrameView();
//	return spLayer;
//}
//XSPLayerObject XTool::AddLayerObject()
//{
//	auto spAction = m_pSprObj->GetspActionCurr();
//	if( !spAction )
//		return XSPLayerObject();
//	auto spLayer = spAction->AddObjLayer();
//	SetspSelLayer( spLayer );
//	XViews::sUpdateFrameView();
//	return spLayer;
//}
//XSPLayerSound XTool::AddLayerSound()
//{
//	auto spAction = m_pSprObj->GetspActionCurr();
//	if( !spAction )
//		return XSPLayerSound();
//	auto spLayer = spAction->AddSndLayer();
//	SetspSelLayer( spLayer );
//	XViews::sUpdateFrameView();
//	return spLayer;
//}
//XSPLayerDummy XTool::AddLayerDummy()
//{
//	auto spAction = m_pSprObj->GetspActionCurr();
//	if( !spAction )
//		return XSPLayerDummy();
//	auto spLayer = spAction->AddDummyLayer();
//	SetspSelLayer( spLayer );
//	XViews::sUpdateFrameView();
//	return spLayer;
//}
//XSPLayerEvent XTool::AddLayerEvent()
//{
//	auto spAction = m_pSprObj->GetspActionCurr();
//	if( !spAction )
//		return XSPLayerEvent();
//	auto spLayer = spAction->AddEventLayer();
//	SetspSelLayer( spLayer );
//	XViews::sUpdateFrameView();
//	return spLayer;
//}
/**
 @brief spLayerSrc레이어를 카피해서 레이어를 하나더 만든다.
*/
XSPBaseLayer XTool::AddLayerFromCopy( XSPBaseLayer spLayerSrc )
{
	if( XBREAK( spLayerSrc == nullptr ) )
		return nullptr;
	if( m_pSprObj->IsPlaying() )
		return XSPBaseLayer();
//	auto spAction = m_pSprObj->GetspActionCurr();
	auto spActObj = m_pSprObj->GetspActObjCurr();
	if (spActObj == nullptr)
		return nullptr;
	XSPAction spAction = spActObj->GetspAction();
	// 레이어 카피
	auto spLayerCopyed = spActObj->CreateAddCopyLayer( spLayerSrc );
// 	auto spLayerCopy = spActObj->CreateLayerFromOtherLayer( spLayerSrc );
// 	spActObj->AddLayer( 0, spLayerCopy );
// 	spActObj->SortLayer( CFrameView::sGet()->GetSizeKey() );
	SetspSelLayer( spLayerCopyed );
// 	{
// 		// 레이어내 키 카피
// 		// 소스측 레이어에 속한 키들을 모두 꺼냄
// 		std::vector<XBaseKey*> ary;
// 		spAction->GetKeysToAryByLayer( spLayerSrc, &ary );
// 		for( auto pKeySrc : ary ) {
// 			auto pKeyCopy = pKeySrc->CopyDeep();
// 			pKeyCopy->MakeNewID();	// 새 아이디로 만든다.
// //			pKeyCopy->SetspLayer( spLayerCopy );	// 새 레이어소속으로 바꿔준다.
// 			pKeyCopy->SetidLayer( spLayerCopy->GetidLayer() );	// 새 레이어소속으로 바꿔준다.
// 			spAction->AddKey( pKeyCopy );
// 		}
// 		spActObj->SortKey();
// 	}
// 	auto spLayerCopy = spActObj->CreateLayerFromOtherLayer( spLayerSrc );
// 	spActObj->AddLayer( 0, spLayerCopy );
// 	spActObj->SortLayer( CFrameView::sGet()->GetSizeKey() );
// 	SetspSelLayer( spLayerCopy );
// 	{
// 		// 레이어내 키 카피
// 		// 소스측 레이어에 속한 키들을 모두 꺼냄
// 		std::vector<XBaseKey*> ary;
// 		spAction->GetKeysToAryByLayer( spLayerSrc, &ary );
// 		for( auto pKeySrc : ary ) {
// 			auto pKeyCopy = pKeySrc->CopyDeep();
// 			pKeyCopy->MakeNewID();	// 새 아이디로 만든다.
// //			pKeyCopy->SetspLayer( spLayerCopy );	// 새 레이어소속으로 바꿔준다.
// 			pKeyCopy->SetidLayer( spLayerCopy->GetidLayer() );	// 새 레이어소속으로 바꿔준다.
// 			spAction->AddKey( pKeyCopy );
// 		}
// 		spActObj->SortKey();
// 	}
	GetFrameView()->UpdateOfsYBySelectedLayer();
	GetFrameView()->Update();
	return spLayerCopyed;
}

XSPActionConst XTool::GetSelAct() const 
{
	if( !m_spSelActObj )
		return nullptr;
	return m_spSelActObj->GetspAction();
}

