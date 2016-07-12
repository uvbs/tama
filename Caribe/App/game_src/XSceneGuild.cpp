#include "StdAfx.h"
#include "XSceneGuild.h"
#include "XGame.h"
#include "XGameWnd.h"
#include "XSockGameSvr.h"
#include "Network/XPacket.h"
#include "XWndTemplate.h"
#include "_Wnd2/XWndList.h"
#include "_Wnd2/XWndButton.h"
#include "XFramework/client/XLayout.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

XSceneGuild *SCENE_GUILD = NULL;

XSceneGuild::XSceneGuild(XGame *pGame)
	: XSceneBase(pGame, XGAME::xSC_GUILD)
	, m_Layout(_T("layout_guild.xml"))
{
	XBREAK(SCENE_GUILD != NULL);
	SCENE_GUILD = this;
	Init();
	//------------------------------------------
	//이거 타임아웃되면 월드맵으로 화면 전환해줘야함
	//------------------------------------------
	// 길드씬 진입후 아직 길드가 없다면 동기화 시켜야 함.
	// 길드가 여전히 없는경우 => 길드리스트를 받아서 비회원 화면으로
	// 길드가 있는경우 => 회원 모드화면으로 전환.
	if (GAME->GetpGuild() == nullptr)
		GAMESVR_SOCKET->SendReqGuildInfo(GAME);
	else
		RecvGuildInfo();
	SCENE_GUILD->Create();
}

void XSceneGuild::Destroy()
{
	//SAFE_DELETE(m_pGuild);
	for( auto itor = m_listGuild.begin(); itor != m_listGuild.end(); ) {
		if( ( *itor ) ) {
			XGuild* pGuild = ( *itor );
			m_listGuild.erase( itor++ );
			SAFE_DELETE( pGuild );
		}
		else
			itor++;
	}
	XBREAK( SCENE_GUILD == NULL );
	XBREAK( SCENE_GUILD != this );
	SCENE_GUILD = NULL;
}


void XSceneGuild::Create(void)
{
	XSceneBase::Create();
}

void XSceneGuild::Update()
{
	XGAME::CreateUpdateTopGuildCoin( this );
	XSceneBase::Update();
}


int XSceneGuild::Process(float dt)
{
	return XSceneBase::Process(dt);
}

//
void XSceneGuild::Draw(void)
{
	XSceneBase::Draw();
	XSceneBase::DrawTransition();
}

void XSceneGuild::OnLButtonDown(float lx, float ly)
{
	XSceneBase::OnLButtonDown(lx, ly);
}
void XSceneGuild::OnLButtonUp(float lx, float ly) {
	XSceneBase::OnLButtonUp(lx, ly);
}
void XSceneGuild::OnMouseMove(float lx, float ly) {
	XSceneBase::OnMouseMove(lx, ly);
}

int XSceneGuild::OnBack(XWnd *pWnd, DWORD p1, DWORD p2)
{
	DoExit(XGAME::xSC_WORLD);
	return 1;
}

int XSceneGuild::OnClickListGuild(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if( ACCOUNT->GetidGuild() ) {
		// 이미 길드에 가입되어있으면 무시.
		return 1;
	}
	XWndList *pList = dynamic_cast<XWndList*>(pWnd);
	XWndGuildElem *pElem = SafeCast<XWndGuildElem*, XWnd*>(pList->Find(p2));

	XWnd *pScene = Find("scene.search");
	if( pScene ) {
		XWnd *pPopup = new XWndPopupGuildJoin( pElem, m_Layout.GetpLayout(), "popup_join", nullptr );
		pPopup->SetstrIdentifier( "popup.join");
		pScene->Add( pPopup );
	}
	const ID idGuild = pElem->GetpGuild()->GetGuildIndex();
//	if( pElem->GetpGuild()->IsJoinReqerUser( ACCOUNT->GetidAccount()) ) {
	if( ACCOUNT->IsReqJoinGuild( idGuild ) ) {
		// 이미 가입신청이 되어있으면 버튼 비활성
		xSET_ENABLE( this, "butt.guild.join", false );
	} else {
		xSET_BUTT_HANDLER_PARAM( this, "butt.guild.join", this, &XSceneGuild::OnClickJoinGuild, idGuild );
	}
// 	if (ACCOUNT->GetGuildIndex() != 0)
// 		xSET_ENABLE(this, "butt.guild.join", FALSE);
	xSET_BUTT_HANDLER(this, "butt.guild.cancel", &XSceneGuild::OnClickPopupClose);
	return 1;
}
int XSceneGuild::OnClickSearchBox(XWnd *pWnd, DWORD p1, DWORD p2)
{
	m_SelInputBox = 3;
	XWnd *pBox = Find("img.search.box");
	if (pBox)	{
		XWndEdit *pEdit = new XWndEdit( XE::VEC2(0), XE::VEC2(308,88), FONT_SYSTEM, 20.f, XCOLOR_WHITE );
		pEdit->SetbShow(FALSE);
		pEdit->SetpDelegate(this);
		pEdit->OnLButtonUp(pEdit->GetPosFinal().x, pEdit->GetPosFinal().y);
		pBox->Add(pEdit);
	}
	return 1;
}
int XSceneGuild::OnClickSearch(XWnd *pWnd, DWORD p1, DWORD p2)
{
	XWndList *pWndList = SafeCast<XWndList*, XWnd*>(Find("list.search.result"));
	if( pWndList ) {
		pWndList->DestroyAllItem();
		XWndTextString *pText = dynamic_cast<XWndTextString*>( Find( "text.search" ) );
		LPCTSTR szName = pText->GetszString();
		XGuild *pGuild = GetpGuildBystrName( szName );
		if( pGuild ) {
			bool bFlag = FALSE;
			if( ACCOUNT->IsReqJoinGuild( pGuild->GetGuildIndex() ) )
				bFlag = TRUE;
			XWndGuildElem *pElem = new XWndGuildElem( pGuild, bFlag );
			pWndList->AddItem( 1, pElem );
		}
		//pWndList->SetAutoSize();
	}
	return 1;
}
int XSceneGuild::OnClickJoinGuild(XWnd *pWnd, DWORD p1, DWORD p2)
{
	GAMESVR_SOCKET->SendReqJoinGuild(this, p1);
	return 1;
}
int XSceneGuild::OnClickCreateGuild(XWnd *pWnd, DWORD p1, DWORD p2)
{
	XWnd *pScene = Find( "scene.search" );
	if( pScene ) {
		XWnd *pPopup = new XWndPopup( m_Layout.GetpLayout(), "popup_create", nullptr );
		pPopup->SetstrIdentifier( "popup.create" );
		pPopup->SetbModal( TRUE );
		pScene->Add( pPopup );
	}
	XWnd *pBox = Find( "img.input.name" );
	if( pBox )
		pBox->SetEvent( XWM_CLICKED, this, &XSceneGuild::OnClickInputNameCreateGuild );

	pBox = Find( "img.input.desc" );
	if( pBox )
		pBox->SetEvent( XWM_CLICKED, this, &XSceneGuild::OnClickInputDescCreateGuild );

	//xSET_BUTT_HANDLER(this, "butt.duplicate.name", &XSceneGuild::OnClickDuplicateName);
	xSET_BUTT_HANDLER( this, "butt.guild.create", &XSceneGuild::OnDecideCreateGuild );
	xSET_BUTT_HANDLER( this, "butt.guild.cancel", &XSceneGuild::OnClickPopupClose );
	return 1;
}
int XSceneGuild::OnClickInputNameCreateGuild(XWnd *pWnd, DWORD p1, DWORD p2)
{
	m_SelInputBox = 1;
	XWnd *pBox = Find("img.input.name");
	if( pBox ) {
		auto pEdit = new XWndEdit( XE::VEC2( 0 ), XE::VEC2( 185, 35 ), FONT_SYSTEM, 20.f, XCOLOR_WHITE );
		pEdit->SetbShow( FALSE );
		pEdit->SetpDelegate( this );
		pEdit->OnLButtonUp( pEdit->GetPosFinal().x, pEdit->GetPosFinal().y );
		pBox->Add( pEdit );
	}
	return 1;
}
int XSceneGuild::OnClickInputDescCreateGuild(XWnd *pWnd, DWORD p1, DWORD p2)
{
	m_SelInputBox = 2;
	XWnd *pBox = Find("img.input.desc");
	if( pBox ) {
		auto pEdit = new XWndEdit( XE::VEC2( 0 ), XE::VEC2( 308, 88 ), FONT_SYSTEM, 20.f, XCOLOR_WHITE );
		pEdit->SetbShow( FALSE );
		pEdit->SetpDelegate( this );
		pEdit->OnLButtonUp( pEdit->GetPosFinal().x, pEdit->GetPosFinal().y );
		pBox->Add( pEdit );
	}
	return 1;
}
int XSceneGuild::OnDecideCreateGuild(XWnd *pWnd, DWORD p1, DWORD p2)
{
	auto pName = dynamic_cast<XWndTextString*>(Find("text.guild.name"));
	auto pDesc = dynamic_cast<XWndTextString*>(Find("text.guild.desc"));
	if( pName && pDesc ) {
		auto szName = pName->GetszString();
		auto szDesc = pDesc->GetszString();
		if( _tcscmp( szName, _T( "" ) ) != 0 
			&& _tcscmp( szDesc, _T( "" ) ) != 0 )
			GAMESVR_SOCKET->SendReqCreateGuild( this, szName, szDesc );
		else 
		if( _tcscmp( szName, _T( "" ) ) == 0 ) {
			XWnd *pScene = Find( "scene.search" );
			if( pScene ) {
				XWnd *pPopup = new XWndPopup( m_Layout.GetpLayout(), "popup_notice", nullptr );
				xSET_TEXT( pPopup, "text.notice", XTEXT( 80157 ) );
				pPopup->SetbModal( TRUE );
				pScene->Add( pPopup );
			}
 			xSET_BUTT_HANDLER( this, "butt.cancel", &XSceneGuild::OnClickPopupClose );
		} else
		if( _tcscmp( szDesc, _T( "" ) ) == 0 ) {
			XWnd *pScene = Find( "scene.search" );
			if( pScene ) {
				XWnd *pPopup = new XWndPopup( m_Layout.GetpLayout(), "popup_notice", nullptr );
				pPopup->SetbModal( TRUE );
				pScene->Add( pPopup );
			}
			xSET_TEXT( this, "text.notice", XTEXT( 80157 ) );
 			xSET_BUTT_HANDLER( this, "butt.cancel", &XSceneGuild::OnClickPopupClose );
		}
	}
	return 1;
}
int XSceneGuild::OnClickPopupClose(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if (pWnd->GetpParent())
		pWnd->GetpParent()->SetbDestroy(TRUE);
	return 1;
}

//void XSceneGuild::UpdateList(std::list<XGuild*> &listGuild)
//{
//
//}

/**
 @brief 길드 리스트 갱신
*/
void XSceneGuild::RecvGuildList(XList4<XGuild*>& listGuild)
{
	// 길드씬내의 모든 윈도우를 다 날림?
	if( Find( "scene.guild" ) ) {
		DestroyAllWnd();		// ????
	}
	if( Find( "scene.search" ) == nullptr ) {
		m_Layout.CreateLayout( "search", this );
	}
	// 기존리스트를 백업하고 새 리스트를 카피
	auto listOld = m_listGuild;
	m_listGuild = listGuild;
	XWnd *pImg = Find("img.search.box");
	if (pImg)
		pImg->SetEvent(XWM_CLICKED, this, &XSceneGuild::OnClickSearchBox);

	xSET_BUTT_HANDLER(this, "butt.back", &XSceneGuild::OnBack);
	xSET_BUTT_HANDLER(this, "butt.search", &XSceneGuild::OnClickSearch);

	auto pWndList = SafeCast<XWndList*>(Find("list.search.result"));
	if( pWndList ) {
		// 길드 리스트 갱신
		pWndList->DestroyAllItem();
		pWndList->SetScrollDir( XE::xVERT );
		pWndList->SetEvent( XWM_SELECT_ELEM, this, &XSceneGuild::OnClickListGuild );
		std::list<XGuild*> listReqJoin;
		for( auto itor = listGuild.begin(); itor != listGuild.end(); ) {
			XGuild* pGuild = (*itor);
			if( pGuild ) {
				// 가입신청 낸 길드면 리스트에서 제외하고 가입신청 리스트에 별도로 올림
				if( ACCOUNT->IsReqJoinGuild( pGuild->GetGuildIndex() ) ) {
					listReqJoin.push_back( pGuild );
					listGuild.erase( itor++ );
				} else {
					itor++;
				}
			} else {
				itor++;
			}
		}
		// 가입신청중인 길드를 먼저 표시
		for( auto pGuild : listReqJoin ) {
			if( pGuild ) {
				auto pElem = new XWndGuildElem( pGuild, TRUE );
				pWndList->AddItem( pGuild->GetGuildIndex(), pElem );
			}
		}
		// 그 외 길드리스트를 추가
		for( auto pGuild : listGuild ) {
			if( pGuild ) {
				auto pElem = new XWndGuildElem( pGuild );
				pWndList->AddItem( pGuild->GetGuildIndex(), pElem );
			}
		}
		// 길드생성버튼
		xSET_BUTT_HANDLER( this, "butt.create", &XSceneGuild::OnClickCreateGuild );
		// 현재 가입된 길드가 없으면 길드생성버튼 표시
		xSET_SHOW( this, "butt.create", (ACCOUNT->GetGuildIndex() == 0) );
		// 기존 길드객체들 파괴
		for( auto pGuild : listOld ) {
			SAFE_DELETE( pGuild );
		}
		listOld.clear();
	}
}

/**
 @brief 플레이어가 길드가 있을경우 소속 길드정보 업데이트
*/
void XSceneGuild::RecvGuildInfo()
{
	// 비회원일때 씬이 있으면 먼저 삭제
	if( Find( "scene.search" ) ) {
		DestroyAllWnd();
	}
	// 길드소속되어있을때 레이아웃으로 읽음.
	if( Find( "scene.guild" ) == nullptr ) {
		m_Layout.CreateLayout( "guild", this );
		xSET_BUTT_HANDLER( this, "butt.back", &XSceneGuild::OnBack );
	}
	// 만약 여기서 길드가 없다면 이건 에러
	auto pGuild = GAME->GetpGuild();
	if( XBREAK(pGuild == nullptr) ) {
		XBREAK( 1 );
		DoExit( XGAME::xSC_WORLD );
		return;
	}
	xSET_TEXT( this, "text.member.name", XTEXT( 80102 ) );
	xSET_TEXT( this, "guild.name", pGuild->GetstrName() );
	xSET_TEXT( this, "guild.master", pGuild->GetstrMasterName() );
	xSET_TEXT( this, "guild.raid.point", XE::NumberToMoneyString( ACCOUNT->GetptGuild() ) );
	xSET_TEXT( this, "guild.num.member", XE::Format( _T( "%d" ), pGuild->GetMemberCount() ) );

	xSET_SHOW( this, "text.grade.member", TRUE );
	xSET_SHOW( this, "text.last.conn.member", TRUE );
	{
		auto pButt = xSET_SHOW( this, "butt.guild.shop", TRUE );
		if( pButt )
			pButt->SetEvent( XWM_CLICKED, this, &XSceneGuild::OnClickShop );
	}
	xSET_SHOW(this, "butt.guild.desc", false);
	//xSET_SHOW(this, "butt.guild.ok", false);
	// 권한에 따라 버튼들 on/off
	if( XGAME::xGGL_LEVEL4 <= ACCOUNT->GetGuildgrade() 
		&& ACCOUNT->GetGuildgrade() < XGAME::xGGL_MAX ) {
		xSET_SHOW( this, "butt.guild.join.manager", TRUE );
		xSET_SHOW( this, "butt.guild.desc", TRUE );
		xSET_BUTT_HANDLER( this, "butt.guild.join.manager", &XSceneGuild::OnClickMngJoin );
		xSET_BUTT_HANDLER( this, "butt.guild.desc", &XSceneGuild::OnClickAdjustDesc );
	}
	// 길드 멤버 리스트 출력
// 	std::list<XGuild::SGuildMember*> listMember;
	auto pWndList = SafeCast<XWndList*>( Find( "list.guild.member" ) );
	if( pWndList ) {
		pWndList->SetScrollDir( XE::xVERT );
		pWndList->DestroyAllItem();
		pWndList->SetEvent( XWM_SELECT_ELEM, this, &XSceneGuild::OnClickListMember );
		int i = 1;
		//관리모드일때는 가입요청자의 리스트로 출력
		if( Find( "wnd.mng.join" ) ) {
			auto& listMember = pGuild->GetListJoinReq();
// 			auto itor = listMember.begin();
// 			for( ; itor != listMember.end(); ++itor ) {
			for( auto pUser : listMember ) {
				if( pUser ) {
					auto pElem = new XWndGuildJoinReqMember( pUser );
					pWndList->AddItem( i++, pElem );
				}
			}
			xSET_TEXT( this, "text.member.name", XTEXT( 80162 ) );
			xSET_SHOW( this, "butt.guild.shop", FALSE );
			xSET_SHOW( this, "text.grade.member", FALSE );
			xSET_SHOW( this, "text.last.conn.member", FALSE );
			pWndList->ClearEvent( XWM_SELECT_ELEM );
		} else {
			// 관리모드가 아닐때는 우리 멤버들 리스트 출력
			auto& listMember = pGuild->GetListMember();
// 			std::list<XGuild::SGuildMember*>::iterator itor = listMember.begin();
// 			for( ; itor != listMember.end(); ++itor ) {
			for( auto pMember : listMember ) {
				if( pMember ) {
					auto pElem = new XWndGuildMember( pMember );
					pWndList->AddItem( pMember->m_idAcc, pElem );
				}
			}
		}
	}
}

void XSceneGuild::OnDelegateEnterEditBox(XWndEdit *pWndEdit, LPCTSTR szString, const _tstring& strOld)
{
	switch (m_SelInputBox)
	{
	case 1:			//길드 생성 이름
		xSET_TEXT(this, "text.guild.name", szString);
		break;
	case 2:			//길드 생성 설명
		xSET_TEXT(this, "text.guild.desc", szString);
		break;
	case 3:			//길드 검색
		xSET_TEXT(this, "text.search", szString);
		break;
	case 4:
		xSET_TEXT(this, "text.desc", szString);
		break;
	}
	pWndEdit->SetbDestroy(TRUE);
}

void XSceneGuild::RecvCreateGuild()
{
	//UpdateAll();
	ClosePopup();
	UpdateChange();
}

int XSceneGuild::OnClickListMember(XWnd *pWnd, DWORD p1, DWORD p2)
{
	XWndGuildMember *pMember = SafeCast<XWndGuildMember*, XWnd*>(pWnd->Find(p2));
	if( pMember ) {
		m_SelMemberIdAcc = pMember->getpMemberInfo()->m_idAcc;
		//m_idAccSelMember = pMember->getpMemberInfo()->s_idaccount;
		XWnd *pScene = Find( "scene.guild" );
		if( pScene ) {
			XWnd *pPopup = new XWndPopup( m_Layout.GetpLayout(), "popup_member", nullptr );
			pPopup->SetstrIdentifier( "popup.member" );
			pPopup->SetbModal( true );
			pScene->Add( pPopup );

			xSET_TEXT( this, "text.member.info", XE::Format( _T( "Lv%d %s" ),
				pMember->getpMemberInfo()->m_lvAcc,
				pMember->getpMemberInfo()->m_strName.c_str() ) );
			xSET_TEXT( this, "text.member.power", XE::Format( XTEXT( 80110 ), 0 ) );
			xSET_TEXT( this, "text.member.ladder", XE::Format( XTEXT( 80111 ), 0 ) );
			xSET_TEXT( this, "text.member.class", XE::Format( XTEXT( 80112 ), XTEXT( 80038 ) ) );

			xSET_IMG( this, "img.hero.icon", XE::MakePath( DIR_IMG, _T( "hero_unluny.png" ) ) );
			xSET_TEXT( this, "text.hero.info", XE::Format( _T( "Lv%d %s" ), 1, XTEXT( 10002 ) ) );
			xSET_SHOW( this, "img.star1", TRUE );
			xSET_SHOW( this, "img.star2", TRUE );
			xSET_SHOW( this, "img.star3", TRUE );
			xSET_SHOW( this, "img.star4", TRUE );

			xSET_BUTT_TEXT( this, "butt.guild.grade1", XE::Format( XTEXT( 80158 ), 1 ) );
			xSET_BUTT_TEXT( this, "butt.guild.grade2", XE::Format( XTEXT( 80158 ), 2 ) );
			xSET_BUTT_TEXT( this, "butt.guild.grade3", XE::Format( XTEXT( 80158 ), 3 ) );
			xSET_BUTT_TEXT( this, "butt.guild.grade4", XE::Format( XTEXT( 80158 ), 4 ) );
			xSET_BUTT_TEXT( this, "butt.guild.grade5", XE::Format( XTEXT( 80158 ), 5 ) );

			if( ACCOUNT->GetGuildgrade() >= XGAME::xGGL_LEVEL4 &&
				ACCOUNT->GetidAccount() != pMember->getpMemberInfo()->m_idAcc &&
				( ( ACCOUNT->GetGuildgrade() == XGAME::xGGL_LEVEL4 &&
				pMember->getpMemberInfo()->m_Grade < XGAME::xGGL_LEVEL4 )
				|| ACCOUNT->GetGuildgrade() == XGAME::xGGL_LEVEL5 ) ) {
				xSET_ENABLE( this, "butt.guild.grade1", true );
				xSET_ENABLE( this, "butt.guild.grade2", true );
				xSET_ENABLE( this, "butt.guild.grade3", true );

				xSET_BUTT_HANDLER_PARAM( this, "butt.guild.grade1", this, &XSceneGuild::OnClickMemberGrade, 1 );
				xSET_BUTT_HANDLER_PARAM( this, "butt.guild.grade2", this, &XSceneGuild::OnClickMemberGrade, 2 );
				xSET_BUTT_HANDLER_PARAM( this, "butt.guild.grade3", this, &XSceneGuild::OnClickMemberGrade, 3 );

				if( ACCOUNT->GetGuildgrade() == XGAME::xGGL_LEVEL5 ) {
					xSET_ENABLE( this, "butt.guild.grade4", true );
					xSET_BUTT_HANDLER_PARAM( this, "butt.guild.grade4", this, &XSceneGuild::OnClickMemberGrade, 4 );

					xSET_ENABLE( this, "butt.guild.grade5", true );
					xSET_BUTT_HANDLER_PARAM( this, "butt.guild.grade5", this, &XSceneGuild::OnClickMemberGrade, 5 );
				}
			}

			xSET_BUTT_HANDLER( this, "butt.guild.cancel", &XSceneGuild::OnClickPopupClose );
			if( pMember->getpMemberInfo()->m_idAcc == ACCOUNT->GetidAccount() ) {
				xSET_BUTT_HANDLER( this, "butt.guild.out", &XSceneGuild::OnClickGuildOut );
			} else
				if( ( ACCOUNT->GetGuildgrade() == XGAME::xGGL_LEVEL4 && pMember->getpMemberInfo()->m_Grade <= XGAME::xGGL_LEVEL3 ) ||
					ACCOUNT->GetGuildgrade() == XGAME::xGGL_LEVEL5 ) {
					XWndButtonString *pButt = dynamic_cast<XWndButtonString*>( Find( "butt.guild.out" ) );
					if( pButt ) {
						m_strSelMemberName = pMember->getpMemberInfo()->m_strName;
						pButt->SetText( XTEXT( 80109 ) );
						pButt->SetEvent( XWM_CLICKED, this, &XSceneGuild::OnClickGuildKick );
					}
				} else {
					XWndButtonString *pButt = dynamic_cast<XWndButtonString*>( Find( "butt.guild.cancel" ) );
					if( pButt ) {
						pButt->SetPosLocal( 160.f, pButt->GetPosLocal().y );
					}
					xSET_SHOW( this, "butt.guild.out", FALSE );
				}
		}
	}
	return 1;
}

void XSceneGuild::RecvReqJoinGuild()
{
	ClosePopup();
// 	XWnd *pWnd = Find("popup.join");
// 	if (pWnd && pWnd->GetpParent())
// 		pWnd->GetpParent()->SetbDestroy(true);
	if (GAME->GetpGuild() == nullptr)
		UpdateGuildList();
}

void XSceneGuild::UpdateGuildList()
{
	if( Find( "scene.guild" ) ) {
		SCENE_GUILD->DestroyAllWnd();
	}
	if( Find( "scene.search" ) == nullptr ) {
		m_Layout.CreateLayout( "search", this );
		xSET_BUTT_HANDLER( this, "butt.back", &XSceneGuild::OnBack );
	}
	XWnd *pImg = Find("img.search.box");
	if (pImg)
		pImg->SetEvent(XWM_CLICKED, this, &XSceneGuild::OnClickSearchBox);

	xSET_BUTT_HANDLER(this, "butt.back", &XSceneGuild::OnBack);
	xSET_BUTT_HANDLER(this, "butt.search", &XSceneGuild::OnClickSearch);

	auto pWndList = SafeCast<XWndList*>(Find("list.search.result"));
	if( pWndList ) {
		pWndList->DestroyAllItem();
		pWndList->SetScrollDir( XE::xVERT );
		pWndList->SetEvent( XWM_SELECT_ELEM, this, &XSceneGuild::OnClickListGuild );
		std::list<XGuild*> listReqJoin;
		std::list<XGuild*> listGuild;
		auto itor = m_listGuild.begin();
		for( ; itor != m_listGuild.end(); ) {
			if( ( *itor ) ) {
				if( ACCOUNT->IsReqJoinGuild( ( *itor )->GetGuildIndex() ) ) {
					listReqJoin.push_back( ( *itor++ ) );
					//m_listGuild.erase(itor++);
				} else {
					listGuild.push_back( ( *itor++ ) );
					//itor++;
				}
			} else
				itor++;
		}
		int i = 1;
		itor = listReqJoin.begin();
		for( ; itor != listReqJoin.end(); ++itor ) {
			if( ( *itor ) ) {
				XWndGuildElem *pElem = new XWndGuildElem( ( *itor ), TRUE );
				pWndList->AddItem( i++, pElem );
			}
		}
		itor = listGuild.begin();
		for( ; itor != listGuild.end(); ++itor ) {
			if( ( *itor ) ) {
				XWndGuildElem *pElem = new XWndGuildElem( ( *itor ) );
				pWndList->AddItem( i++, pElem );
			}
		}
		xSET_BUTT_HANDLER( this, "butt.create", &XSceneGuild::OnClickCreateGuild );
		if( ACCOUNT->GetGuildIndex() != 0 )
			xSET_SHOW( this, "butt.create", FALSE );
	}
}



int XSceneGuild::OnClickMngJoin(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if( !( XGAME::xGGL_LEVEL4 <= ACCOUNT->GetGuildgrade() && ACCOUNT->GetGuildgrade() < XGAME::xGGL_MAX ) ) {
		XBREAK( 1 );
		return 1;
	}
	if (Find("wnd.mng.join"))
		return 1;
	XGuild *pGuild = GAME->GetpGuild();
	xSET_SHOW(this, "butt.guild.shop", FALSE);
	xSET_SHOW(this, "text.grade.member", FALSE);
	xSET_SHOW(this, "text.last.conn.member", FALSE);
	xSET_TEXT(this, "text.member.name", XTEXT(80162));

	XWnd *pScene = Find("scene.guild");
	if( pScene ) {
		XWnd *pJoinMng = new XWnd( m_Layout.GetpLayout(), "mng_join" );
		pJoinMng->SetPosLocal( 386, 241 );
		pScene->Add( pJoinMng );

		xSET_SHOW( this, "img.option1.check", pGuild->GetGuildOption().s_bAutoAccept );
		xSET_SHOW( this, "img.option2.check", pGuild->GetGuildOption().s_bBlockReqJoin );
	}
	XWnd *pBox = Find("img.option1");
	if (pBox)
		pBox->SetEvent(XWM_CLICKED, this, &XSceneGuild::OnClickAutoAccept);

	pBox = Find("img.option2");
	if (pBox)
		pBox->SetEvent(XWM_CLICKED, this, &XSceneGuild::OnClickBlockReqJoin);

	xSET_BUTT_HANDLER(this, "butt.guild.decide", &XSceneGuild::OnClickMngJoinOK);
	auto pWndList = SafeCast<XWndList*>(Find("list.guild.member"));
	pWndList->ClearEvent(XWM_SELECT_ELEM);
	if( pWndList && pGuild ) {
		pWndList->DestroyAllItem();
		int i = 1;
		auto listJoinReq = pGuild->GetListJoinReq();
		auto itor = listJoinReq.begin();
		for( ; itor != listJoinReq.end(); ++itor ) {
			if( ( *itor ) ) {
				auto pElem = new XWndGuildJoinReqMember( ( *itor ) );
				pWndList->AddItem( i++, pElem );
			}
		}
	}

	return 1;
}

int XSceneGuild::OnClickMngGuild(XWnd *pWnd, DWORD p1, DWORD p2)
{
	//if (!(XGAME::xGGL_LEVEL4 <= ACCOUNT->GetGuildgrade() && ACCOUNT->GetGuildgrade() < XGAME::xGGL_MAX))
	//{
	//	XBREAK(1);
	//	return 1;
	//}

	//XWnd *pOld = Find("wnd.mng.join");
	//if (pOld)
	//	pOld->GetpParent()->SetbDestroy(true);

	//RecvGuildInfo();

	////xSET_SHOW(this, "butt.guild.manager", false);
	//xSET_SHOW(this, "butt.guild.shop", false);

	//if (ACCOUNT->GetGuildgrade() >= XGAME::xGGL_LEVEL4)
	//{
	//	xSET_SHOW(this, "butt.guild.desc", true);
	//	//xSET_SHOW(this, "butt.guild.ok", true);

	//	xSET_BUTT_HANDLER(this, "butt.guild.desc", &XSceneGuild::OnClickAdjustDesc);
	//	//xSET_BUTT_HANDLER(this, "butt.guild.ok", &XSceneGuild::OnClickGuildMngOK);
	//}


	return 1;
}

int XSceneGuild::OnClickMngJoinOK(XWnd *pWnd, DWORD p1, DWORD p2)
{
	XGuild *pGuild = GAME->GetpGuild();
	if (XBREAK(!pGuild))
		return 1;
	bool bOption1 = false, bOption2 = false;
	XWnd *pCheck = Find("img.option1.check");
	if (pCheck)
		bOption1 = (pCheck->GetbShow() != FALSE);
	pCheck = Find("img.option2.check");
	if (pCheck)
		bOption2 = (pCheck->GetbShow() != FALSE);

	if( bOption1 == pGuild->GetGuildOption().s_bAutoAccept &&
		bOption2 == pGuild->GetGuildOption().s_bBlockReqJoin ) {
		XWnd *pWnd = Find( "wnd.mng.join" );
		if( pWnd && pWnd->GetpParent() )
			pWnd->GetpParent()->SetbDestroy( true );
		RecvGuildInfo();
	} else {
		GAMESVR_SOCKET->SendReqChangeOption( this, bOption1, bOption2 );
	}
	//pWnd->GetpParent()->SetbDestroy(true);
	//RecvGuildInfo(pGuild);
	//SetbUpdate(true);
	return 1;
}

void XSceneGuild::UpdateOption(/*BOOL bAutoAccept, BOOL bBlockJoin*/)
{
	XWnd *pWnd = Find("wnd.mng.join");
	if (pWnd && pWnd->GetpParent())
		pWnd->GetpParent()->SetbDestroy(true);
// 	XGuild *pGuild = GAME->GetpGuild();
// 	if (pGuild)
// 		pGuild->SetGuildOption(bAutoAccept, bBlockJoin);
	RecvGuildInfo();
}

int XSceneGuild::OnClickAutoAccept(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if (XBREAK(!GAME->GetpGuild()))
		return 1;
	XWnd *pCheck = Find("img.option1.check");
	if( pCheck ) {
		BOOL bOption = !pCheck->GetbShow();
		pCheck->SetbShow( bOption );
		if( bOption ) {
			pCheck = Find( "img.option2.check" );
			if( pCheck->GetbShow() == TRUE )
				pCheck->SetbShow( false );
		}
	}
	return 1;
}

int XSceneGuild::OnClickBlockReqJoin(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if (XBREAK(!GAME->GetpGuild()))
		return 1;
	XWnd *pCheck = Find("img.option2.check");
	if( pCheck ) {
		BOOL bOption = !pCheck->GetbShow();
		pCheck->SetbShow( bOption );
		//m_pGuild->SetGuildOption(m_pGuild->GetGuildOption().s_bAutoAccept, bOption);

		if( bOption ) {
			pCheck = Find( "img.option1.check" );
			if( pCheck->GetbShow() == TRUE )
				pCheck->SetbShow( false );
		}
	}
	return 1;
}

int XSceneGuild::OnClickShop(XWnd *pWnd, DWORD p1, DWORD p2)
{
	DoExit( XGAME::xSC_GUILD_SHOP );
	return 1;
}

XGuild* XSceneGuild::GetpGuildBystrName(_tstring strName)
{
	for( auto itor = m_listGuild.begin(); itor != m_listGuild.end(); itor++ ) {
		if( ( *itor ) && _tcscmp( ( *itor )->GetstrName(), strName.c_str() ) == 0 ) {
			return ( *itor );
		}
	}
	return nullptr;
}

int XSceneGuild::OnClickAcceptJoin(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if (pWnd->GetpParent())
	{
		XWndGuildJoinReqMember *pElem = SafeCast<XWndGuildJoinReqMember*, XWnd*>(pWnd->GetpParent());
		if (pElem)
			GAMESVR_SOCKET->SendReqJoinGuildAccept(this, pElem->GetUserID(), p1);
	}
	return 1;
}

//void XSceneGuild::RecvReqAccept()
//{
//	XWnd *pScene = Find("scene.guild");
//	if (pScene)
//	{
//		XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_duplicate", nullptr);
//		pPopup->SetbModal(TRUE);
//		Add(pPopup);
//		xSET_TEXT(this, "text.notice", XTEXT(80178));
//		xSET_BUTT_HANDLER(this, "butt.popup.cancel", &XSceneGuild::OnClickPopupClose);
//	}
//	/*if (Find("wnd.mng.join"))
//	{
//	OnClickMngJoin(this, 0, 0);
//	}*/
//	//GAMESVR_SOCKET->SendReqGuildInfo(this);
//	//UpdateAll();
//}

//void XSceneGuild::UpdateGuild(XGuild *pGuild)
//{
//	//가입 신청을 받음, 멤버 가입 수락 두 경우만 사용할 것
//	//XWnd *pFlag = Find("wnd.mng.join");
//	//XWndList *pWndList = SafeCast<XWndList*, XWnd*>(Find("list.guild.member"));
//
//	//if (pWndList)
//	//{
//	//	//매니저 씬이였다면
//	//	if (pFlag)
//	//	{
//
//	//	}
//	//	else
//	//	{
//
//	//	}
//	//}
//}
//
/**
 @brief 길드 탈퇴가 승인됨.
*/
void XSceneGuild::RecvReqOutGuild( xtGuildError errCode )
{
	//UpdateAll();
	if( errCode == xGE_SUCCESS ) {
		// 모든 관련창 닫음.
		ClosePopup();
		SetbUpdate( true );
		UpdateChange();		// 다시 서버로 길드리스트를 요청해서 최신 길드리스트를 받아온다.
	}
}

int XSceneGuild::OnClickGuildOut(XWnd *pWnd, DWORD p1, DWORD p2)
{
	pWnd->SetbEnable( FALSE );		// 탈퇴버튼 비활성시킴.
	XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_warn", nullptr);
	pPopup->SetstrIdentifier( "popup.warning" );
	pPopup->SetbModal(TRUE);
	Add(pPopup);

	xSET_TEXT(pPopup, "text.notice", XTEXT(80187));

	xSET_BUTT_HANDLER(this, "butt.notice.confirm", &XSceneGuild::OnClickGuildOutConfirm);
 	xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneGuild::OnClickPopupClose);

	return 1;
}

int XSceneGuild::OnClickGuildOutConfirm(XWnd *pWnd, DWORD p1, DWORD p2)
{
	pWnd->SetbEnable( FALSE );
	XGuild *pGuild = GAME->GetpGuild();
	if( pGuild == nullptr ) {
		UpdateChange();
		return 1;
	}
	if( pGuild->GetnumMembers() == 1 || ACCOUNT->GetGuildgrade() < XGAME::xGGL_LEVEL5 ) {
		GAMESVR_SOCKET->SendReqOutGuild( this );
	} else {
		RecvErrorPopup( XGAME::xGE_ERROR_MASTER_OUT );
	}
	return 1;
}

int XSceneGuild::OnClickGuildKick(XWnd *pWnd, DWORD p1, DWORD p2)
{
	XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_warn", nullptr);
	pPopup->SetstrIdentifier( "popup.warning" );
	pPopup->SetbModal(TRUE);
	Add(pPopup);

	xSET_TEXT(this, "text.notice", XE::Format(XTEXT(80204), m_strSelMemberName.c_str()));

	SetButtHander(this, "butt.notice.confirm", &XSceneGuild::OnClickGuildKickConfirm, m_SelMemberIdAcc );
	SetButtHander(this, "butt.cancel", &XSceneGuild::OnClickPopupClose);

	return 1;
}

int XSceneGuild::OnClickGuildKickConfirm(XWnd *pWnd, DWORD p1, DWORD p2)
{
	pWnd->SetbEnable( FALSE );		// kick버튼 비활성화
	GAMESVR_SOCKET->SendReqKickGuild(this, p1);
	return 1;
}

void XSceneGuild::RecvReqKickGuild()
{
	ClosePopup();
	SetbUpdate( true );
	GAMESVR_SOCKET->SendReqGuildInfo( GAME );
// 	XWnd *pWnd = Find("popup.member");
// 	if ( pWnd && pWnd->GetpParent())
// 		pWnd->GetpParent()->SetbDestroy(true);
	//SetbUpdate(true);
}

int XSceneGuild::OnClickAdjustDesc(XWnd *pWnd, DWORD p1, DWORD p2)
{
	XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_desc", nullptr);
	pPopup->SetstrIdentifier( "popup.desc" );
	pPopup->SetbModal(TRUE);
	Add(pPopup);
	xSET_BUTT_HANDLER(this, "butt.desc.adjust", &XSceneGuild::OnClickAdjustDescOk);
	xSET_BUTT_HANDLER(this, "butt.popup.cancel", &XSceneGuild::OnClickPopupClose);
	xSET_TEXT(this, "text.desc", GAME->GetpGuild()->GetstrGuildContext());

	pPopup = Find("img.box.desc");
	if (pPopup)
		pPopup->SetEvent(XWM_CLICKED, this, &XSceneGuild::OnClickInputDescAdjust);

	return 1;
}

int XSceneGuild::OnClickGuildMngOK(XWnd *pWnd, DWORD p1, DWORD p2)
{
	xSET_SHOW(this, "butt.guild.desc", false);
	//xSET_SHOW(this, "butt.guild.ok", false);
	xSET_SHOW(this, "butt.guild.shop", true);
	return 1;
}

int XSceneGuild::OnClickInputDescAdjust(XWnd *pWnd, DWORD p1, DWORD p2)
{
	XWnd *pBox = Find("img.box.desc");
	if (pBox)	{
		m_SelInputBox = 4;
		XWndEdit *pEdit = new XWndEdit( XE::VEC2(0), XE::VEC2(308,88), FONT_SYSTEM, 20.f, XCOLOR_WHITE );
// 		XWndEdit *pEdit = new XWndEdit(0, 0, 308, 88, GAME->GetpfdSystem(), XCOLOR_WHITE, 0);
		pEdit->SetbShow(FALSE);
		pEdit->SetpDelegate(this);
		pEdit->OnLButtonUp(pEdit->GetPosFinal().x, pEdit->GetPosFinal().y);
		pBox->Add(pEdit);
	}

	return 1;
}

int XSceneGuild::OnClickAdjustDescOk(XWnd *pWnd, DWORD p1, DWORD p2)
{
	XWndTextString *pText = dynamic_cast<XWndTextString*>(Find("text.desc"));
	LPCTSTR szDesc = pText->GetszString();
	GAMESVR_SOCKET->SendReqChangeDesc(this, szDesc);
	return 1;
}

void XSceneGuild::UpdateDesc(_tstring strDesc)
{
	/*if (XBREAK(!m_pGuild))
		return;*/
}

void XSceneGuild::RecvAdjustDesc( xtGuildError errCode )
{
	ClosePopup();
	if( errCode != xGE_SUCCESS ) {
		RecvErrorPopup( errCode );
	}
// 	XWnd *pWnd = Find("img.popup.adjust.desc");
// 	if (pWnd && pWnd->GetpParent())
// 		pWnd->GetpParent()->SetbDestroy(true);
}

void XSceneGuild::RecvErrorPopup(XGAME::xtGuildError sResult)
{
	/*XWnd *pScene = Find("scene.guild");
	if (pScene)
	{*/
	if (sResult == XGAME::xGE_SUCCESS)
		return;
	XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_notice", nullptr);
	pPopup->SetbModal(TRUE);
	Add(pPopup);
	switch (sResult)
	{
	case XGAME::XGE_ERROR_GUILD_NOT_FOUND:		// 길드가 없다
		UpdateChange();
		xSET_TEXT(this, "text.notice", XTEXT(80186));
		break;
	case XGAME::XGE_ERROR_MEMBER_NOT_FOUND:		// 멤버가 없다
		UpdateChange();
		xSET_TEXT(this, "text.notice", XTEXT(80178));
		break;
	case XGAME::XGE_ERROR_GUILD_INFO:			// 길드 정보 불러오기 에러
		//현재 사용 안함
		UpdateChange();
		//xSET_TEXT(this, "text.notice", XTEXT(80187));
		break;
	case XGAME::XGE_ERROR_GUILD_CREATE:			// 길드 생성 에러
		xSET_TEXT(this, "text.notice", XTEXT(80184));
		break;
	case XGAME::xGE_ERROR_ACCEPT_MEMBER:		// 멤버 승인 실패.
		xSET_TEXT(this, "text.notice", XTEXT(80179));
		break;
	case XGAME::xGE_ERROR_JOIN_MEMBER:			// 가입멤버 실패.
		xSET_TEXT(this, "text.notice", XTEXT(80156));
		break;
	case XGAME::xGE_ERROR_NAME_DUPLICATE:		// 이름 중복.
		xSET_TEXT(this, "text.notice", XTEXT(80153));
		break;
	case XGAME::xGE_ERROR_ALREADY_HAVE_GUILD:	//이미 길드 가입 되어 있음
		UpdateChange();
		xSET_TEXT(this, "text.notice", XTEXT(80126));
		break;
	case XGAME::xGE_ERROR_MAX_MEMBER_OVER:		// 최대인원수 초과.
		xSET_TEXT(this, "text.notice", XTEXT(80181));
		break;
	case XGAME::xGE_ERROR_NO_AUTHORITY:			// 권한 없음.
		UpdateChange();
		xSET_TEXT(this, "text.notice", XTEXT(80180));
		break;
	case XGAME::xGE_ERROR_BLOCK_JOIN_REQ:		// 가입 신청 제한
		xSET_TEXT(this, "text.notice", XTEXT(80182));
		break;
	case XGAME::xGE_ERROR_MASTER_OUT:			// 연방장이 탈퇴하려함
		xSET_TEXT(this, "text.notice", XTEXT(80185));
		break;
// 	case XGAME::xGE_ERROR_ALREADY_JOIN_REQ:
// 		// 재 동기화를 요청
// 		GAMESVR_SOCKET->SendReqGuildInfo(GAME);
// 		break;
	default:
		XBREAK(1);
		break;
	}

	xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneGuild::OnClickPopupClose);
}

//가입 시키려는 유저가 이미 다른 길드에 가입되어 있을경우
//void XSceneGuild::RecvErrorAcceptMember(_tstring strName)
//{
//	XWnd *pScene = Find("scene.guild");
//	if (pScene)
//	{
//		XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_duplicate", nullptr);
//		pPopup->SetbModal(TRUE);
//		Add(pPopup);
//		xSET_TEXT(this, "text.notice", XE::Format(XTEXT(80179), strName.c_str()));
//		xSET_BUTT_HANDLER(this, "butt.popup.cancel", &XSceneGuild::OnClickPopupClose);
//	}
//}

void XSceneGuild::UpdateMemberlist()
{
	RecvGuildInfo();
}

void XSceneGuild::UpdateChange()
{
	GAMESVR_SOCKET->SendReqGuildInfo(GAME);
}

void XSceneGuild::RecvUpdateGuildUser( xtGuildEvent event, XArchive& arParam )
{
	// 일단 편의상 이걸로 업데이트
	GAMESVR_SOCKET->SendReqGuildInfo( GAME );
	//
// 	switch( event ) {
// 	case xGEV_OUT:
// 	case xGEV_KICK: {
// 		// 길드정보 동기화 요청(모든 길드의 리스트)z
// 		GAMESVR_SOCKET->SendReqGuildInfo( GAME );
// 		break;
// 	}
// 	default:
// 		break;
// 	}
}

void XSceneGuild::RecvMemberUpdate(XPacket &p, bool bGrade)
{
	XGuild *pGuild = GAME->GetpGuild();
	if (XBREAK(pGuild == nullptr))
		return;

	XWndList *pWndList = SafeCast<XWndList*, XWnd*>(Find("list.guild.member"));
	//pWndList->SetScrollDir(XE::xVERT);
	pWndList->DestroyAllItem();
	pGuild->DeSerializeGuildMemberlist(p);
	//pWndList->SetEvent(XWM_SELECT_ELEM, this, &XSceneGuild::OnClickListMember);
	int i = 1;
	if( Find( "wnd.mng.join" ) ) {
		auto listMember = pGuild->GetListJoinReq();
		auto itor = listMember.begin();
		for( ; itor != listMember.end(); ++itor ) {
			if( ( *itor ) ) {
				XWndGuildJoinReqMember *pElem = new XWndGuildJoinReqMember( ( *itor ) );
				pWndList->AddItem( i++, pElem );
			}
		}
	} else {
		auto listMember = pGuild->GetListMember();
		auto itor = listMember.begin();
		for( ; itor != listMember.end(); ++itor ) {
			if( ( *itor ) ) {
				XWndGuildMember *pElem = new XWndGuildMember( ( *itor ) );
				pWndList->AddItem( ( *itor )->m_idAcc, pElem );
			}
		}
	}
	//UpdateUI();
}

void XSceneGuild::RecvListReqUpdate(/*XPacket& ar*/)
{
	XGuild *pGuild = GAME->GetpGuild();
	if (XBREAK(pGuild == nullptr))
		return;
	int i = 1;
	if( Find( "wnd.mng.join" ) ) {
		XWndList *pWndList = SafeCast<XWndList*, XWnd*>( Find( "list.guild.member" ) );
		//pWndList->SetScrollDir(XE::xVERT);
		pWndList->DestroyAllItem();
		// 		pGuild->DeSerializeGuildReqMemberlist(ar);

		auto listMember = pGuild->GetListJoinReq();
		auto itor = listMember.begin();
		for( ; itor != listMember.end(); ++itor ) {
			if( ( *itor ) ) {
				auto pElem = new XWndGuildJoinReqMember( ( *itor ) );
				pWndList->AddItem( i++, pElem );
			}
		}
		xSET_TEXT( this, "text.member.name", XTEXT( 80162 ) );

		xSET_SHOW( this, "butt.guild.shop", FALSE );
		xSET_SHOW( this, "text.grade.member", FALSE );
		xSET_SHOW( this, "text.last.conn.member", FALSE );
		pWndList->ClearEvent( XWM_SELECT_ELEM );
	}
// 	else
// 		pGuild->DeSerializeGuildReqMemberlist(ar);
}

void XSceneGuild::RecvAcceptMember(/*XPacket& ar*/)
{
	XGuild *pGuild = GAME->GetpGuild();
	if (XBREAK(pGuild == nullptr))
		return;
	auto pWndList = SafeCast<XWndList*>(Find("list.guild.member"));
	//pWndList->SetScrollDir(XE::xVERT);
	pWndList->DestroyAllItem();
	int i = 1;
//	auto& listMember = pGuild->GetListJoinReq();
	////가입자 관리창이 존재할때 다르게 초기화 해줘야함
	if( Find( "wnd.mng.join" ) ) {
		auto& listMember = pGuild->GetListJoinReq();
		auto itor = listMember.begin();
		for( ; itor != listMember.end(); ++itor ) {
			if( ( *itor ) ) {
				auto pElem = new XWndGuildJoinReqMember( ( *itor ) );
				pWndList->AddItem( i++, pElem );
			}
		}
		xSET_TEXT( this, "text.member.name", XTEXT( 80162 ) );
		xSET_SHOW( this, "butt.guild.shop", FALSE );
		xSET_SHOW( this, "text.grade.member", FALSE );
		xSET_SHOW( this, "text.last.conn.member", FALSE );
		pWndList->ClearEvent( XWM_SELECT_ELEM );
	} else {
		auto& listMember = pGuild->GetListMember();
		auto itor = listMember.begin();
		for( ; itor != listMember.end(); ++itor ) {
			if( ( *itor ) ) {
				auto pElem = new XWndGuildMember( ( *itor ) );
				pWndList->AddItem( ( *itor )->m_idAcc, pElem );
			}
		}
	}
}

int XSceneGuild::OnClickMemberGrade(XWnd *pWnd, DWORD p1, DWORD p2)
{
	auto pWndList = SafeCast<XWndList*>(Find("list.guild.member"));
	auto pMember = SafeCast<XWndGuildMember*>(pWndList->Find(m_SelMemberIdAcc));

	if( pWnd && pWnd->GetpParent() )
		pWnd->GetpParent()->SetbDestroy( true );

	if( pMember == nullptr ) {
		RecvErrorPopup( XGAME::XGE_ERROR_MEMBER_NOT_FOUND );
		return 1;
	} else 
	if( ACCOUNT->GetGuildgrade() < XGAME::xGGL_LEVEL4 ||
		( ACCOUNT->GetGuildgrade() == XGAME::xGGL_LEVEL4 && pMember->getpMemberInfo()->m_Grade >= XGAME::xGGL_LEVEL4 ) ) {
		RecvErrorPopup( XGAME::xGE_ERROR_NO_AUTHORITY );
		return 1;
	}

	GAMESVR_SOCKET->SendReqChangeMemberGrade( this, pMember->getpMemberInfo()->m_idAcc,
		pMember->getpMemberInfo()->m_lvAcc,
		pMember->getpMemberInfo()->m_strName,
		( XGAME::xtGuildGrade )p1 );

	return 1;
}

void XSceneGuild::UpdateUI()
{
	XGuild *pGuild = GAME->GetpGuild();
	if (pGuild == nullptr)	//길드가 있을때만 업데이트 가능
		return;
	if( XGAME::xGGL_LEVEL4 <= ACCOUNT->GetGuildgrade() 
		&& ACCOUNT->GetGuildgrade() < XGAME::xGGL_MAX ) {
		xSET_SHOW( this, "butt.guild.join.manager", TRUE );
		xSET_SHOW( this, "butt.guild.desc", TRUE );

		xSET_BUTT_HANDLER( this, "butt.guild.join.manager", &XSceneGuild::OnClickMngJoin );
		xSET_BUTT_HANDLER( this, "butt.guild.desc", &XSceneGuild::OnClickAdjustDesc );
	} else {
		xSET_SHOW( this, "butt.guild.join.manager", false );
		xSET_SHOW( this, "butt.guild.desc", false );

		XWnd *pWnd = Find( "wnd.mng.join" );
		if( pWnd && pWnd->GetpParent() ) {
			pWnd->GetpParent()->SetbDestroy( true );
			UpdateMemberlist();
		}
	}
	xSET_TEXT(this, "guild.master", pGuild->GetstrMasterName());
	//xSET_TEXT(this, "guild.raid.point", XE::Format(_T("%d"), );
	xSET_TEXT(this, "guild.num.member", XE::Format(_T("%d"), pGuild->GetMemberCount()));
}

void XSceneGuild::ClosePopup()
{
	DestroyWndByIdentifier( "popup.create" );
	DestroyWndByIdentifier( "popup.join" );
	DestroyWndByIdentifier( "popup.member" );
	DestroyWndByIdentifier( "popup.warning" );
	DestroyWndByIdentifier( "popup.desc" );
}
