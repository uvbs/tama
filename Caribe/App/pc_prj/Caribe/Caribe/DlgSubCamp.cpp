// DlgSubCamp.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgSubCamp.h"
#include "afxdialogex.h"


// CDlgSubCamp ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgSubCamp, CDialogEx)

CDlgSubCamp::CDlgSubCamp(CWnd* pParent /*=NULL*/)
//	: CDialogEx(CDlgSubCamp::IDD, pParent)
: m_strCamp( _T( "" ) )
{

}

CDlgSubCamp::~CDlgSubCamp()
{
}

void CDlgSubCamp::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_IDS_CAMP, m_strCamp );
}


BEGIN_MESSAGE_MAP(CDlgSubCamp, CDialogEx)
END_MESSAGE_MAP()


// CDlgSubCamp �޽��� ó�����Դϴ�.
