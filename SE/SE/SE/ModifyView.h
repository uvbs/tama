#pragma once



// CModifyView 폼 뷰입니다.

class CModifyView : public CFormView
{
	DECLARE_DYNCREATE(CModifyView)

protected:
	CModifyView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CModifyView();

public:
	enum { IDD = IDD_MODIFYVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	float m_fPosX;
	float m_fPosY;
	float m_fRotZ;
	float m_fScaleX;
	float m_fScaleY;
	afx_msg void OnEnChangeEditPosx();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnSetfocusEditPosx();
	afx_msg void OnEnKillfocusEditPosx();
};

CModifyView *GetModifyView();

