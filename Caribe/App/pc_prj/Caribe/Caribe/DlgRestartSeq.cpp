// DlgRestartSeq.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgRestartSeq.h"
#include "afxdialogex.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// CDlgRestartSeq ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgRestartSeq, CDialogEx)

CDlgRestartSeq::CDlgRestartSeq(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRestartSeq::IDD, pParent)
	, m_strIds( _T( "" ) )
{

}

CDlgRestartSeq::~CDlgRestartSeq()
{
}

void CDlgRestartSeq::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_strIds );
}


BEGIN_MESSAGE_MAP(CDlgRestartSeq, CDialogEx)
END_MESSAGE_MAP()


// CDlgRestartSeq �޽��� ó�����Դϴ�.
