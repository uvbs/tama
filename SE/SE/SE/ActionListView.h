#pragma once
#include "afxcmn.h"
#include "resource.h"
//#include "EditListCtrl.h"


// CActionListView 폼 뷰입니다.

class CActionListView : public CFormView
{
	DECLARE_DYNCREATE(CActionListView)
	int m_nNumList;
protected:
	CActionListView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CActionListView();

public:
	enum { IDD = IDD_ACTIONLIST };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	void Update();

	static CActionListView* CreateOne( CWnd *pParent );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctrlList;
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnItemchangingList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDelAction();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRenameAction();
	afx_msg void OnAddAction();
	afx_msg void OnCopyaddAction();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
private:
	void AddAction( LPCTSTR szName, DWORD id );
};


CActionListView *GetActionListView();
