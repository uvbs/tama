// DlgCloseArea.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgCloseArea.h"
#include "afxdialogex.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// CDlgCloseArea ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgCloseArea, CDialogEx)

CDlgCloseArea::CDlgCloseArea(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCloseArea::IDD, pParent)
	, m_strArea( _T( "" ) )
{

}

CDlgCloseArea::~CDlgCloseArea()
{
}

void CDlgCloseArea::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_strArea );
}


BEGIN_MESSAGE_MAP(CDlgCloseArea, CDialogEx)
END_MESSAGE_MAP()


// CDlgCloseArea �޽��� ó�����Դϴ�.
