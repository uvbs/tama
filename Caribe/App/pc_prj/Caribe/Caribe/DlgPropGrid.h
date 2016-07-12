#pragma once
#include "afxdialogex.h"
class CDlgPropGrid :
	public CDialog
{
public:
	static CDlgPropGrid *s_pInstance;
	static CDlgPropGrid* sGet() {
		return s_pInstance;
	}
private:
	CFont m_fntPropList;
	CMFCPropertyGridCtrl m_wndPropList;
	//
	CMFCPropertyGridProperty *m_pCurrGrid = nullptr;
public:
	CDlgPropGrid();
	virtual ~CDlgPropGrid();
	virtual BOOL OnInitDialog();
	//
	void InitPropList();
	void UpdateData( int type );
	void CreatePropType1();
	void CreatePropType2();
		//
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg LRESULT OnPropertyChanged( WPARAM wparam, LPARAM lparam );
};

