#pragma once
#include "resource.h"


// CDlgBattleOption ��ȭ �����Դϴ�.

class CDlgBattleOption : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgBattleOption)

public:
	CDlgBattleOption(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgBattleOption();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_BATTLE_OPTION };

	BOOL m_bCheckFace = FALSE;		// �ʻ�ȭ ���̱�/���߱�
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
