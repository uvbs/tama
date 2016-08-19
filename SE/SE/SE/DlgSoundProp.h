#pragma once


// CDlgSoundProp 대화 상자입니다.

class CDlgSoundProp : public CDialog
{
	DECLARE_DYNAMIC(CDlgSoundProp)

public:
	CDlgSoundProp(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSoundProp();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROP_SOUND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_idSound;
};
