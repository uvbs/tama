#pragma once
#include "resource.h"


// CDlgBattleOption 대화 상자입니다.

class CDlgBattleOption : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgBattleOption)

public:
	CDlgBattleOption(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgBattleOption();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_BATTLE_OPTION };

	BOOL m_bCheckFace = FALSE;		// 초상화 보이기/감추기
	BOOL m_bCheckPlayerHero = FALSE;
	BOOL m_bCheckPlayerUnit = FALSE;
	BOOL m_bCheckEnemyHero = FALSE;
	BOOL m_bCheckEnemyUnit = FALSE;
	BOOL m_bCheckFlushImg = FALSE;
	BOOL m_bCheckFlushSpr = FALSE;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bHeroInfoToConsole;
};
