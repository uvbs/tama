// ActionListView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "ActionListView.h"
#include "Tool.h"
#include "DlgRenameAction.h"
#include "MainFrm.h"
#include "UndoMng.h"
#include "XAniAction.h"
#include "XActObj.h"
static CActionListView *s_pActionListView = nullptr;
CActionListView *GetActionListView() { return s_pActionListView; }

// CActionListView

IMPLEMENT_DYNCREATE(CActionListView, CFormView)

CActionListView::CActionListView()
	: CFormView(CActionListView::IDD)
{
	s_pActionListView = this;
	m_nNumList = 0;
}

CActionListView::~CActionListView()
{
}

void CActionListView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
}

BEGIN_MESSAGE_MAP(CActionListView, CFormView)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST1, &CActionListView::OnLvnItemchangingList1)
	ON_COMMAND(ID_DEL_ACTION, &CActionListView::OnDelAction)
	ON_WM_RBUTTONUP()
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CActionListView::OnNMRClickList1)
	ON_COMMAND(ID_RENAME_ACTION, &CActionListView::OnRenameAction)
	ON_COMMAND(ID_ADD_ACTION, &CActionListView::OnAddAction)
	ON_COMMAND(ID_COPYADD_ACTION, &CActionListView::OnCopyaddAction)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// static
CActionListView *CActionListView::CreateOne( CWnd *pParent )
{
	CActionListView *pView = new CActionListView;
	CCreateContext *pContext = nullptr;

	if (!pView->Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE, CRect(0, 0, 500, 500), pParent, 0, pContext))
	{
		AfxMessageBox( _T("Failed in creating CActionListView") );
	}

	pView->OnInitialUpdate();

	return pView;
}

// CActionListView 진단입니다.

#ifdef _DEBUG
void CActionListView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CActionListView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CActionListView 메시지 처리기입니다.

void CActionListView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//	int w, minw, h, minh;		// WndSplitter::OnHScroll에서 자꾸 다운되서 추가한코드
	CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
//	pMainFrm->m_wndSplitter.GetRowInfo( 1, h, minh );	
//	pMainFrm->m_wndSplitter3.GetColumnInfo( 0, w, minw );		// 분할창의크기를 구함
//	MoveWindow( 0, 0, w, h );
	{
		CSize size;
		size = GetTotalSize();
		size.cx = 0;
		size.cy = 0;
		SetScrollSizes( MM_TEXT, size );
	}
	TCHAR *szLable[2] = 
	{
		_T("ID"), _T("이름")
	};
	int nColWidth[2] = { 30, 100 };
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	int		i;
	for( i = 0; i < 2; i ++ )
	{
		lvc.iSubItem = i;
		lvc.pszText = szLable[i];
		lvc.cx = nColWidth[i];
		lvc.fmt = LVCFMT_LEFT;
		m_ctrlList.InsertColumn( i, &lvc );
	}
	RECT	rect;
	GetClientRect( &rect );
	m_ctrlList.SetExtendedStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );
	m_ctrlList.MoveWindow( &rect );

}

void CActionListView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( m_ctrlList.GetSafeHwnd() )
		m_ctrlList.MoveWindow( 0, 0, cx, cy );
}

void CActionListView::AddAction( LPCTSTR szName, DWORD id )
{
	LV_ITEM lvi;

	lvi.mask = LVIF_TEXT;
	lvi.iItem = m_nNumList;

	lvi.iSubItem = 0;
	CString strNum;
	strNum.Format( _T("%d"), id );
	lvi.pszText = (LPTSTR)((LPCTSTR)strNum);
	m_ctrlList.InsertItem( &lvi );

	lvi.iSubItem = 1;
	lvi.pszText = (LPTSTR)szName;
	m_ctrlList.SetItem( &lvi );

	m_nNumList ++;
	Invalidate(0);
}
/**
 @brief 리스트를 싹 지우고 다시 갱신한다.
*/
void CActionListView::Update()
{
	m_ctrlList.DeleteAllItems();
	m_nNumList = 0;
	if( SPRDAT ) {
// 		SPRDAT->GetNextActionClear();
// 		while( XSPAction& spAction = SPRDAT->GetNextAction() ) {
// 			AddAction( spAction->GetszActName(), spAction->GetidAct() );
// 		}
		for( auto& spActObj : SPROBJ->GetlistActObj() ) {
			const auto spAction = spActObj->GetspAction();
			AddAction( spAction->GetszActName(), spAction->GetidAct() );
		}
	}
	Invalidate( FALSE ); 
}
void CActionListView::OnLvnItemchangingList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( pNMLV->uNewState & (LVIS_SELECTED | LVIS_FOCUSED) ) {
		TOOL->SetSelActIdx( pNMLV->iItem );
	}
	*pResult = 0;
}

void CActionListView::OnDelAction()
{
	VALID_ACTOBJ( spActObj );
	// shared_ptr의 사용으로 인해 지우는 순서가 바뀜
	auto spAction = spActObj->GetspAction();
	UNDOMNG->PushUndo( new XUndoDelAction( SPROBJ, spAction ) );
	TOOL->DelAct();		// 현재선택된 액션을 삭제
// 	XObjAct *pDelObjAct = TOOL->DelAct();		// 현재선택된 액션을 삭제
// 	UNDOMNG->PushUndo( new XUndoDelAction( SPROBJ, pDelObjAct ) );
}

void CActionListView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFormView::OnRButtonUp(nFlags, point);
}

void CActionListView::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( pNMItemActivate->iItem >= 0 )
	{
		CMenu		menu, *popup;
		POINT	p;
		
		menu.LoadMenu( IDR_POPUP_ACTIONLIST );
		popup = menu.GetSubMenu(0);
		
		GetCursorPos( &p );
		popup->TrackPopupMenu( TPM_CENTERALIGN | TPM_LEFTBUTTON, 
			p.x, p.y, this );
		
		menu.DestroyMenu();
	}
	*pResult = 0;
}

// 액션 이름 바꾸기창 
void CActionListView::OnRenameAction()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDlgRenameAction dlg;
	if( TOOL->GetSelAct() ) {
		dlg.m_dwID = TOOL->GetSelAct()->GetidAct();
		dlg.m_strName = TOOL->GetSelAct()->GetszActName();		// 디폴트 이름
		if( dlg.DoModal() == IDOK ) {
			TOOL->GetSelActObj()->SetidAct( dlg.m_dwID );
			TOOL->GetSelActObj()->SetszActName( dlg.m_strName );	// 바뀐이름으로 카피한다
			Update();
		}
	}
}


// 빈 액션 추가
void CActionListView::OnAddAction()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	auto spActObj = TOOL->CreateAction( _T("noname") );
	TOOL->SetSelAct( spActObj->GetidAct() );
}

// 선택한 액션을 복사하여 새로 추가
void CActionListView::OnCopyaddAction()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	auto spAction = TOOL->GetSelAct();
	if( !spAction )		return;
	TOOL->CopyAct( spAction );
}


void CActionListView::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
