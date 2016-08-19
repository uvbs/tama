/*
 *  XWndMng.h
 *  Game
 *
 *  Created by xuzhu on 11. 1. 13..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */
#ifndef __XWNDMNG_H__
#define __XWNDMNG_H__

#include "XWnd.h"
#include "XWndButton.h"
#include "SprObj.h"
#include "XFont.h"

struct XWND_RESOURCE
{
	XWND_RES_FRAME frameDialog;		// 다이얼로그창의 프레임리소스
	XWND_RES_FRAME frameButtonUp;		// 버튼의 프레임리소스
	XWND_RES_FRAME frameButtonDown;		
	XWND_RES_FRAME frameButtonDiable;		
	XWND_RES_FRAME frameView;		//뷰창의 프레임리소스
	XWND_RESOURCE() {
		_tcscpy_s( frameDialog.szFrame, _T("baseDialog.png") );
		_tcscpy_s( frameButtonUp.szFrame, _T("baseButton_up.png") );
		_tcscpy_s( frameButtonDown.szFrame, _T("baseButton_down.png") );
		_tcscpy_s( frameView.szFrame, _T("baseWindow.png") );
	}
};

class XWndMng;

class XWndMng
{
public:
	static ID s_idGlobal;
	static ID GenerateID( void ) { return s_idGlobal++; }
private:
	XSprObj *m_pSprObj;
	XWND_RESOURCE m_Res;
	XWnd *m_pWnd;
	ID m_idSndPushDown;		// 클릭시 소리
	ID m_idSndPushUp;			// 뗄때 소리
	XE::VEC2 m_vScale;			// 각 컨트롤들에 스케일값이 들어갈때 매니저의 기준스케일과 주어진 스케일이 곱해져서 들어감
	float m_dRotateZ;
	void _Init() {
		m_pSprObj = NULL;
		m_pWnd = NULL;
		m_vScale.Set( 1.0f, 1.0f );
		m_dRotateZ = 0;
		m_idSndPushDown = m_idSndPushUp = 0;
	}
	void _Destroy() {
		SAFE_DELETE( m_pSprObj );
		SAFE_DELETE( m_pWnd );
	}
public:
	XWndMng() { _Init(); Create( NULL ); }
	// 최상위 m_pWnd는 스케일을 사용하지 않는다. m_pWndMng에 스케일이 있기때문? 그리고 최상위 바로아래 컨트롤들은 스케일에 적용받지 않는 좌표를 쓰고싶어서다
	XWndMng( LPCTSTR szSprObj ) { _Init(); Create( szSprObj ); /*m_pWnd->SetScaleLocal( 1.0f, 1.0f );*/ } 
	virtual ~XWndMng() { _Destroy(); }
	
	// get/set
//	GET_ACCESSOR( XSprObj*, pSprObj );
	XSprObj* GetpSprObj( void ) {	
		XBREAK( m_pSprObj == NULL ); 
		XBREAK( m_pWnd == NULL ); 
		m_pSprObj->SetScale( m_pWnd->GetScaleFinal() );	// ui는 sprobj를 하나를 쓰기땜에 모든 draw마다 setscale()을 불러줘야 하는단점이 있어 이렇게 함
		return m_pSprObj; 
	}	
	GET_ACCESSOR( XWnd*, pWnd );
	GET_SET_ACCESSOR( ID, idSndPushDown );
	GET_SET_ACCESSOR( ID, idSndPushUp );
	GET_ACCESSOR( XWND_RESOURCE&, Res );
	void SetScale( float sx, float sy ) { m_vScale.Set( sx, sy ); }
	const XE::VEC2& GetScale( void ) { return m_vScale; }		// 
	float GetRotate( void ) { return m_dRotateZ; }
	//
	void Create( LPCTSTR szSprObj ) {
		if( szSprObj && XE::IsHave( szSprObj ) )
			m_pSprObj = new XSprObj( szSprObj, NULL );
		LoadResAll();		// UI 리소스들을 읽음
		m_pWnd = new XWnd( this );		// 루트 윈도를 생성
	}
	virtual BOOL LoadResAll( void );				// UI에 필요한 모든 리소스 
	BOOL LoadResFrame( LPCTSTR szRes, XWND_RES_FRAME *pFrame, int surfaceBlockSize=0 );	// 프레임 리소스를 읽음. surfaceBlockSize=0은 자동계산
	void Process( float dt ) { m_pWnd->Process( dt ); }
	void Draw( void ) { m_pWnd->Draw(); }
	
//	XWnd* GetTouch( void ) { return m_pWnd->GetTouch(); }
//	int GetTouchID( void ) { return m_pWnd->GetTouchID(); }
	XWnd* AddWnd( int id, XWnd *pWnd ) { return m_pWnd->Add( id, pWnd ); }
	XWnd* AddWnd( XWnd *pWnd ) { return m_pWnd->Add( GenerateID(), pWnd ); }
	void DestroyWnd( int id ) { m_pWnd->DestroyID( id ); }
	// control
	XWndButton* AddButton( int id, XWndButton *pButton ) {
		XSprObj *pSprObj = (pButton->GetpSprObj())? pButton->GetpSprObj() : GetpSprObj();
		if( pButton->GetbSprNum()==FALSE ) {
			pSprObj->SetAction( pButton->GetidActUp() );
			pSprObj->FrameMove(0);
			pButton->SetSizeLocal( pSprObj->GetWidth(), pSprObj->GetHeight() );		
		} else
		{
			XSprite *pSpr = pSprObj->GetpSprDat()->GetSprite(pButton->GetidActUp() );
			pButton->SetSizeLocal( pSpr->GetWidth(), pSpr->GetHeight() );
//			pButton->SetSizeLocal( pSpr->GetWidth() * GRAPHICS->GetGScale().x, pSpr->GetHeight() * GRAPHICS->GetGScale().y );
		}
#ifdef _XDEBUG
		XBREAKF( m_pWnd->Find( id ), "이미 ID:%d가 있습니다", id );
#endif
		AddWnd( id, pButton );
		return pButton;
	}
	// 치트버튼 용도외엔 쓰지 말것.
	template<typename T>
	XWndButton* AddButton( int id, T x, T y, DWORD idActUp, DWORD idActDown = 0, BOOL bSprNum=FALSE ) {
		XWndButton *pButton = new XWndButton( x, y, GetpSprObj()->GetszFilename(), idActUp, idActDown, bSprNum );
		return (XWndButton *)AddButton( id, pButton );
	} 
//	template<typename T>
//	XWndButtonCheck* AddButtonCheck( int id, T x, T y, DWORD idActUp, DWORD idActDown = 0, BOOL bPush=FALSE ) {
//		XWndButtonCheck *pButtonCheck = new XWndButtonCheck( this, x, y, idActUp, idActDown, bPush );
//		return (XWndButtonCheck *)AddButton( id, pButtonCheck );
//	}
	// 
	BOOL GetShow( void ) { return m_pWnd->GetbShow(); }
	void SetShow( BOOL bShow ) { m_pWnd->SetbShow( bShow ); }		// 이 매니저에 속한 모든 버튼을 켜거나 감춤	
	XWnd* FindWnd( ID id ) { return m_pWnd->Find( id ); }
	void SetModalDialog( XWnd* pModalDlg ) { m_pWnd->SetModalDlg( pModalDlg ); }
    XWnd* GetModalDialog( void ) { return m_pWnd->GetpModalDlg(); }
	void DestroyModalDialog( void ) { m_pWnd->DestroyModalDlg(); }
	// handler
	void OnLButtonDown( float lx, float ly ) { m_pWnd->OnLButtonDown( lx, ly ); }
	void OnMouseMove( float lx, float ly ) { m_pWnd->OnMouseMove( lx, ly ); }
	ID OnLButtonUp( float lx, float ly ) { 
		ID id = m_pWnd->OnLButtonUp( lx, ly );
		return  id;
	}
	
};

#endif // __XWNDMNG_H__