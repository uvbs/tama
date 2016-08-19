#pragma once


// XDlgCompressConverter 대화 상자입니다.

class XDlgCompressConverter : public CDialogEx
{
	DECLARE_DYNAMIC(XDlgCompressConverter)

public:
	XDlgCompressConverter(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~XDlgCompressConverter();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_COMP_CONVERT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
