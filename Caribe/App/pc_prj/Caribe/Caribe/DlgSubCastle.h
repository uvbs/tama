#pragma once
#include "resource.h"
#include "XFramework/MFC/GenericChildDialog.h"


// CDlgSubCastle 대화 상자입니다.

class CDlgSubCastle : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CDlgSubCastle)

public:
	CDlgSubCastle(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSubCastle();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SUB_CASTLE };

	virtual int CRHGetDialogID() override {
		return IDD;
	}
  void SettypeDrop( XGAME::xtDropCastle type );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
