// DlgSubMandrake.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgSubMandrake.h"
#include "afxdialogex.h"


// CDlgSubMandrake ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgSubMandrake, CDialogEx)

CDlgSubMandrake::CDlgSubMandrake(CWnd* pParent /*=NULL*/)
//	: CDialogEx(CDlgSubMandrake::IDD, pParent)
: m_Idx( 0 )
, m_Produce( 0 )
{

}

CDlgSubMandrake::~CDlgSubMandrake()
{
}

void CDlgSubMandrake::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_IDX, m_Idx );
	DDX_Text( pDX, IDC_PRODUCE, m_Produce );
}


BEGIN_MESSAGE_MAP(CDlgSubMandrake, CDialogEx)
END_MESSAGE_MAP()


// CDlgSubMandrake �޽��� ó�����Դϴ�.
