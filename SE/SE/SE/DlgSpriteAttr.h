#pragma once


// CDlgSpriteAttr ��ȭ �����Դϴ�.

class CDlgSpriteAttr : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSpriteAttr)

public:
	CDlgSpriteAttr(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSpriteAttr();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ATTR_SPRITE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	BOOL OnInitDialog();
	void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboFormatSurface;
	XE::xtPixelFormat m_formatSurface = XE::xPF_ARGB4444;
};
