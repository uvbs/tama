/********************************************************************
@date:
@file:
@author:	xuzhu

@brief:
*********************************************************************/
#pragma once
#include "XSceneBase.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"
#include "XGuild.h"
#include "_Wnd2/XWndEdit.h"

class XGame;
class XPacket;
class XSceneGuild : public XSceneBase,
	public XDelegateWndEdit
{
	XLayoutObj m_Layout;
	//XGuild *m_pGuild = nullptr;
	XList4<XGuild*> m_listGuild;
	//ID m_idAccSelMember = 0;
private:
	_tstring m_strSelMemberName;
	int m_SelInputBox;
	int m_SelMemberIdAcc = 0;
	//
	void Init()  {
		m_SelInputBox = 0;
	}
	void Destroy();

	XGuild* GetpGuildBystrName(_tstring strName);

protected:
public:
	XSceneGuild(XGame *pGame);
	virtual ~XSceneGuild(void) { Destroy(); }
	//
	// virtual
	virtual void Create(void);
	virtual int Process(float dt);
	virtual void Draw(void);
	virtual void OnLButtonDown(float lx, float ly);
	virtual void OnLButtonUp(float lx, float ly);
	virtual void OnMouseMove(float lx, float ly);

	int OnBack(XWnd *pWnd, DWORD p1, DWORD p2);

	// 길드 없을때
	//int OnClickDuplicateName(XWnd *pWnd, DWORD p1, DWORD p2); //중복 체크
	int OnClickSearchBox(XWnd *pWnd, DWORD p1, DWORD p2); //검색
	int OnClickSearch(XWnd *pWnd, DWORD p1, DWORD p2); //검색
	int OnClickListGuild(XWnd *pWnd, DWORD p1, DWORD p2); //길드 선택
	int OnClickJoinGuild(XWnd *pWnd, DWORD p1, DWORD p2); //길드 가입
	int OnClickCreateGuild(XWnd *pWnd, DWORD p1, DWORD p2); //길드 설립 버튼
	int OnDecideCreateGuild(XWnd *pWnd, DWORD p1, DWORD p2); //길드 설립 결정
	int OnClickPopupClose(XWnd *pWnd, DWORD p1, DWORD p2); //팝업 종료
	int OnClickInputNameCreateGuild(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickInputDescCreateGuild(XWnd *pWnd, DWORD p1, DWORD p2);

	// 길드 있을때
	int OnClickMngJoin(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickMngGuild(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickMngJoinOK(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickAutoAccept(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickBlockReqJoin(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickShop(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickAcceptJoin(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickListMember(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickGuildOut(XWnd *pWnd, DWORD p1, DWORD p2);	//길드 탈퇴
	int OnClickGuildOutConfirm(XWnd *pWnd, DWORD p1, DWORD p2);	//길드 탈퇴
	int OnClickGuildKick(XWnd *pWnd, DWORD p1, DWORD p2);	//길드 추방
	int OnClickGuildKickConfirm(XWnd *pWnd, DWORD p1, DWORD p2);	//길드 추방
	int OnClickAdjustDesc(XWnd *pWnd, DWORD p1, DWORD p2);	//연방 관리 설명변경
	int OnClickGuildMngOK(XWnd *pWnd, DWORD p1, DWORD p2);	//연방 관리 확인
	int OnClickInputDescAdjust(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickAdjustDescOk(XWnd *pWnd, DWORD p1, DWORD p2);
	int OnClickMemberGrade(XWnd *pWnd, DWORD p1, DWORD p2);

	void UpdateGuildList();

	//void UpdateList(std::list<XGuild*> &listGuild);		// 길드 가입 신청/취소 했을때 길드 리스트 업데이트
	// Recv
	void RecvGuildList(XList4<XGuild*> &listGuild);	//길드 없음(리스트 리시브)
	void RecvGuildInfo();	//길드 있음(가입된 길드 정보 리시브)
	void RecvCreateGuild();
	void RecvReqJoinGuild();
	//void RecvReqAccept();
	void RecvReqOutGuild( XGAME::xtGuildError errCode );
	void RecvReqKickGuild();
	void RecvAdjustDesc( XGAME::xtGuildError errCode );
	void RecvMemberUpdate(XPacket& p, bool bGrade = false);
	void RecvListReqUpdate(/*XPacket& p*/);
	void RecvAcceptMember(/*XPacket& p*/);

	//길드 에러 팝업
	void RecvErrorPopup(XGAME::xtGuildError sResult);
	//파라미터가 필요한 팝업 or 팝업이 아닌 에러
	//void RecvErrorAcceptMember(_tstring strName);
	//void RecvErrorHaveGuild();

	//
	//void UpdateGuild(XGuild *pGuild);
	void Update(void);
	void UpdateChange(void);	// 길드의 정보를 최신으로 업데이트함
	void UpdateDesc(_tstring strDesc);	//길드 설명
	void UpdateOption(/*BOOL bAutoAccept, BOOL bBlockJoin*/);	//길드 옵션
	void UpdateMemberlist();	//멤버가 추가되거나 숫자나 가입 신청한사람이 있을경우
	void UpdateUI();			//등급 변경이나 여러 이유로 인해 버튼 등 UI에 업데이트가 필요할때

	void OnDelegateEnterEditBox(XWndEdit *pWndEdit, LPCTSTR szString, const _tstring& strOld) override;
	void RecvUpdateGuildUser( XGAME::xtGuildEvent event, XArchive& arParam );
private:
	void ClosePopup();
};

extern XSceneGuild *SCENE_GUILD;

