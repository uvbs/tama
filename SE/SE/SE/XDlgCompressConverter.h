#pragma once


// XDlgCompressConverter ��ȭ �����Դϴ�.

class XDlgCompressConverter : public CDialogEx
{
	DECLARE_DYNAMIC(XDlgCompressConverter)

public:
	XDlgCompressConverter(CWnd* pParent = nullptr);   // ǥ�� �������Դϴ�.
	virtual ~XDlgCompressConverter();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_COMP_CONVERT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
