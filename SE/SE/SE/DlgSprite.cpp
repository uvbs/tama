// DlgSprite.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "DlgSprite.h"


// CDlgSprite 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSprite, CDialog)

CDlgSprite::CDlgSprite(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSprite::IDD, pParent)
{

}

CDlgSprite::~CDlgSprite()
{
}

void CDlgSprite::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSprite, CDialog)
END_MESSAGE_MAP()


// CDlgSprite 메시지 처리기입니다.
