#pragma once


// CDlgEditAni 대화 상자입니다.

class CDlgEditAni : public CDialog
{
	DECLARE_DYNAMIC(CDlgEditAni)

public:
	CDlgEditAni(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgEditAni();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EDIT_ANI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	float m_fMaxFrame;
	float m_scaleX;
	float m_scaleY;
	float m_rotateZ;
	float m_RotateY;
};
