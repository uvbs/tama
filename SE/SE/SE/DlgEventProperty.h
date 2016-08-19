#pragma once
#include "afxwin.h"
#include "XDlgBaseKey.h"

// CDlgEventProperty 대화 상자입니다.

class CDlgEventProperty : public CDialog, public XDlgBaseKey
{
	int m_LoopType;

	DECLARE_DYNAMIC(CDlgEventProperty)

public:
	CDlgEventProperty(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgEventProperty();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROP_EVENT };

	void SetLoopType( xRPT_TYPE typeLoop, float sec=0 ) {
		switch( typeLoop ) {
		case xRPT_1PLAY:	m_LoopType = 1;	break;
		case xRPT_LOOP:		
			if( sec == -1 )	// 무한반복
				m_LoopType = 3;	
			else {
				m_LoopType = 2;
				m_secLifeTime = sec;
			}
			break;
		}
	}
	xRPT_TYPE GetLoopType( float *pOutSec ) {
		switch( m_LoopType ) {
		case 1:	*pOutSec = 0;	return xRPT_1PLAY;
		case 2:	*pOutSec = m_secLifeTime;	return xRPT_LOOP;	// 유한반복
		case 3: *pOutSec = -1.f;		return xRPT_LOOP;	// 무한반복
		}
		XBREAKF( 1, "알수없는 루핑타입:%d", m_LoopType );
		return xRPT_1PLAY;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_idObj;		// DDX_TEXT매크로에 WORD형이 없어서...
	CComboBox m_ctrlEventType;
	xSpr::xtEventKey m_EventType;

	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedLuaDialog();
	CString m_strSpr;
	DWORD m_idAct;
	CComboBox m_ctrlLoopType;
	float m_secLifeTime;
	BOOL m_bTraceParent;
	float m_Scale;
	CString m_strParam[4];
};
