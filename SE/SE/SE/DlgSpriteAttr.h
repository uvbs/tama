#pragma once


// CDlgSpriteAttr 대화 상자입니다.

class CDlgSpriteAttr : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSpriteAttr)

public:
	CDlgSpriteAttr(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSpriteAttr();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ATTR_SPRITE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	BOOL OnInitDialog();
	void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboFormatSurface;
	XE::xtPixelFormat m_formatSurface = XE::xPF_ARGB4444;
};
