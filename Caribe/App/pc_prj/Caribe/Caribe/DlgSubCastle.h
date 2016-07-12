#pragma once
#include "resource.h"
#include "XFramework/MFC/GenericChildDialog.h"


// CDlgSubCastle ��ȭ �����Դϴ�.

class CDlgSubCastle : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubCastle)

public:
	CDlgSubCastle(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSubCastle();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SUB_CASTLE };

	virtual int CRHGetDialogID() override {
		return IDD;
	}
  void SettypeDrop( XGAME::xtDropCastle type );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	std::vector<float> m_aryResource;
//  	float m_numWood;
//  	float m_numIron;
// 	float m_numJewel;
// 	float m_numSulfur;
// 	float m_numMandrake;
//   afx_msg void OnBnClickedRadio3();
//   afx_msg void OnBnClickedRadioNone();
//   afx_msg void OnBnClickedRadioMedal();
//   afx_msg void OnBnClickedRadioSphere();
  afx_msg void OnRadioControl( UINT id );
  XGAME::xtDropCastle m_typeDrop = XGAME::xDC_NO_DROP;
private:
  int m_idxRadio;
};
