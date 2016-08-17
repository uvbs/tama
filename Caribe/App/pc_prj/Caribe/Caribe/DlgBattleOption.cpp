// DlgBattleOption.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgBattleOption.h"
#include "afxdialogex.h"
#include "client/XAppMain.h"


// CDlgBattleOption 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgBattleOption, CDialogEx)

CDlgBattleOption::CDlgBattleOption(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgBattleOption::IDD, pParent)
	, m_bHeroInfoToConsole( FALSE )
{
	XCLEAR_ARRAY( m_bNoDraw );
}

CDlgBattleOption::~CDlgBattleOption()
{
}

void CDlgBattleOption::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Check( pDX, IDC_CHECK_PLAYER_HERO, m_bCheckPlayerHero );
	DDX_Check( pDX, IDC_CHECK_PLAYER_UNIT, m_bCheckPlayerUnit );
	DDX_Check( pDX, IDC_CHECK_ENEMY_HERO, m_bCheckEnemyHero );
	DDX_Check( pDX, IDC_CHECK_ENEMY_UNIT, m_bCheckEnemyUnit );
	DDX_Check( pDX, IDC_CHECK_FACE, XAPP->m_bShowFace );
	DDX_Check( pDX, IDC_CHECK_WAIT_AFTER_WIN, XAPP->m_bWaitAfterWin );
	DDX_Check( pDX, IDC_CHECK_HERO_INFO_TO_CONSOLE, m_bHeroInfoToConsole );
	DDX_Check( pDX, IDC_CHECK_FLUSH_IMG, m_bCheckFlushImg );
	DDX_Check( pDX, IDC_CHECK_FLUSH_SPR, m_bCheckFlushSpr );
	DDX_Check( pDX, IDC_CHECK_NO_DRAW_DMG_NUM, m_bNoDraw[0] );
	DDX_Check( pDX, IDC_CHECK_NO_DRAW_HIT_SFX, m_bNoDraw[1] );
	DDX_Check( pDX, IDC_CHECK_NO_DRAW_SKILL_SFX, m_bNoDraw[2] );
	DDX_Check( pDX, IDC_CHECK_NO_DRAW_BUFF_ICON, m_bNoDraw[3] );
	DDX_Check( pDX, IDC_CHECK_NO_CREATE_DMG_NUM2, m_bNoDraw[8] );
	DDX_Check( pDX, IDC_CHECK_NO_CREATE_HIT_SFX2, m_bNoDraw[9] );
	DDX_Check( pDX, IDC_CHECK_NO_CREATE_SKILL_SFX2, m_bNoDraw[10] );
	DDX_Check( pDX, IDC_CHECK_NO_CREATE_BUFF_ICON2, m_bNoDraw[11] );
	DDX_Text( pDX, IDC_EDIT_FONT_DMG, m_strFontDmg );
}


BEGIN_MESSAGE_MAP(CDlgBattleOption, CDialogEx)
END_MESSAGE_MAP()


// CDlgBattleOption 메시지 처리기입니다.
