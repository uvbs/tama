// DlgSubSulfur.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgSubSulfur.h"
#include "afxdialogex.h"


// CDlgSubSulfur ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgSubSulfur, CDialogEx)

CDlgSubSulfur::CDlgSubSulfur(CWnd* pParent /*=NULL*/)
//	: CDialogEx(CDlgSubSulfur::IDD, pParent)
: m_Produce( 0 )
, m_secRegen( 0 )
{

}

CDlgSubSulfur::~CDlgSubSulfur()
{
}

void CDlgSubSulfur::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_PRODUCE, m_Produce );
	DDX_Text( pDX, IDC_SEC_REGEN, m_secRegen );
}


BEGIN_MESSAGE_MAP(CDlgSubSulfur, CDialogEx)
END_MESSAGE_MAP()


// CDlgSubSulfur �޽��� ó�����Դϴ�.
