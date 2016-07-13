#include "stdafx.h"
#ifdef _xIN_TOOL
#include "XTool.h"
#include "XSceneWorld.h"
#include "XSceneTech.h"
#include "XGame.h"
#include "XGameWnd.h"
#include "XPropBgObj.h"
#include "XWorld.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XBaseTool* XBaseTool::s_pCurr = nullptr;

int XBaseTool::Process( float dt ) 
{
	if( m_timerAutoSave.IsOver() )	{
		Save();		// virtual
		m_timerAutoSave.Off();
	}
	return 1;
}

/**
 @brief 툴모드를 바꾼다.
*/
void XBaseTool::sDoChangeMode( xtToolMode modeNew )
{
	const auto modeOld = (s_pCurr)? s_pCurr->GetToolMode() : xTM_NONE;
	if( s_pCurr )
		s_pCurr->OnLeaveMode();
//	auto modeCurr = xTM_NONE;
	switch( modeNew )	{
	case xTM_NONE:
		s_pCurr = nullptr;
		break;
	case xTM_CLOUD:
		s_pCurr = XToolCloud::sGet().get();
		break;
	case xTM_TECH:
		s_pCurr = XToolTech::sGet().get();
		break;
	case xTM_SPOT:
		s_pCurr = XToolSpot::sGet().get();
		break;
	case xTM_OBJ:
		s_pCurr = XToolBgObj::sGet().get();
		break;
	default:
		break;
	}
	if( s_pCurr )
		s_pCurr->OnEnterMode();
	const auto modeCurr = (s_pCurr)? s_pCurr->GetToolMode() : xTM_NONE;
// 	if( s_pCurr )
// 		modeCurr = s_pCurr->GetToolMode();
	if( GAME->GetpScene() )
		GAME->GetpScene()->DelegateChangeToolMode( modeOld, modeCurr );
}
///////////////////////////////////////////////////////////////
std::shared_ptr<XToolCloud> XToolCloud::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XToolCloud>& XToolCloud::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XToolCloud>( new XToolCloud );
	return s_spInstance;
}
////////////////////////////////////////////////////////////////
XToolCloud::XToolCloud()
  : XBaseTool( xTM_CLOUD )
{
	Init();
}

void XToolCloud::Destroy()
{
//	SCENE_WORLD->UpdateCloud( FALSE );
}

// BOOL XToolCloud::OnCreate()
// {
// // 	// 툴모드로 전환될때는 모든 구름을 다시 읽어서 업데이트 한다.
// // 	SCENE_WORLD->UpdateCloudSpotList( true );	
// 	return TRUE;
// }

void XToolCloud::OnEnterMode()
{
	// 툴모드로 전환될때는 모든 구름을 다시 읽어서 업데이트 한다.
	SCENE_WORLD->UpdateCloudSpotList( true );
}
void XToolCloud::OnLeaveMode()
{

}

bool XToolCloud::Save()
{
	BOOL bRet = PROP_CLOUD->Save( _T( "propCloud2.xml" ) );
	return (bRet)? true : false;
}

////////////////////////////////////////////////////////////////
std::shared_ptr<XToolSpot> XToolSpot::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XToolSpot>& XToolSpot::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XToolSpot>( new XToolSpot );
	return s_spInstance;
}
////////////////////////////////////////////////////////////////
XToolSpot::XToolSpot()
  : XBaseTool( xTM_SPOT )
{
	Init();
}

void XToolSpot::Destroy()
{
	SAFE_DELETE( m_pDeleted );
}

bool XToolSpot::Save()
{
	BOOL bRet = PROP_CLOUD->Save( _T( "propCloud2.xml" ) );
	if( bRet == FALSE )
		return false;
	return PROP_WORLD->Save(_T("propWorld.xml"));
}

void XToolSpot::OnEnterMode()
{
// 	auto pWndScrl = SCENE_WORLD->Find( "scroll.view" );
// 	XBREAK( pWndScrl == nullptr );
// 	auto pCloudLayer = pWndScrl->Find( "scroll.cloud.view" );
// 	if( pCloudLayer )
// 		pCloudLayer->SetbTouchable( false );
}
void XToolSpot::OnLeaveMode()
{
// 	if( SCENE_WORLD ) {
// 		auto pWndScrl = SCENE_WORLD->Find( "scroll.view" );
// 		XBREAK( pWndScrl == nullptr );
// 		auto pCloudLayer = pWndScrl->Find( "scroll.cloud.view" );
// 		if( pCloudLayer )
// 			pCloudLayer->SetbTouchable( true );
// 		// 게임모드 스팟루트를 다시 나타나게함.
// 		xSET_SHOW( SCENE_WORLD, "root.spot", true );
// 		SCENE_WORLD->GetpUIRoot()->SetbShow( true );
// 	}
}

void XToolSpot::DelSelected()
{
	if( XWndSpotForTool::s_pSelected )
	{
		ID idSpot = XWndSpotForTool::s_pSelected->GetpBaseProp()->idSpot;
		XWndSpotForTool::s_pSelected->SetbDestroy( TRUE );
		m_pDeleted = PROP_WORLD->DelSpot( idSpot );
		XWndSpotForTool::s_pSelected = nullptr;
		//sGetpWorld()->DestroySpot( idSpot );
		auto pBaseSpot = sGetpWorld()->GetSpot( idSpot );
		if( pBaseSpot )
			pBaseSpot->SetbDestroy( true );
		if( SCENE_WORLD )
			SCENE_WORLD->UpdateCloudSpotList( true );
		UpdateAutoSave();
	}
}

void XToolSpot::CopySelected()
{
	if( XWndSpotForTool::s_pSelected ) {
		m_idSpotInClipboard = XWndSpotForTool::s_pSelected->GetpBaseProp()->idSpot;
	}
}

void XToolSpot::PasteSelected()
{
	if( m_idSpotInClipboard ) {
		auto pProp = PROP_WORLD->GetpProp( m_idSpotInClipboard );
		if( pProp ) {
			auto pNewSpot = PROP_WORLD->CreateSpot( pProp->type );
			auto idSpot = pNewSpot->idSpot;
			auto ids = pNewSpot->strIdentifier;
			PROP_WORLD->CopyProp( pNewSpot, pProp );
			pNewSpot->idSpot = idSpot;
			pNewSpot->strIdentifier = ids;
			PROP_WORLD->AddSpot( pNewSpot );
			if( XASSERT(SCENE_WORLD) ) {
				SCENE_WORLD->UpdateSpotForTool();
				SCENE_WORLD->UpdateCloudSpotList( true );
				pNewSpot->vWorld = SCENE_WORLD->GetvMouseWorld();
				pNewSpot->idArea = PROP_CLOUD->GetidAreaHaveSpot( pNewSpot->idSpot );
				SCENE_WORLD->UpdateAutoSave();
			}
		}
	}
}

void XToolSpot::Undo()
{
	bool bUndo = false;
	if( m_pDeleted ) {
		PROP_WORLD->AddSpot( m_pDeleted );
		m_pDeleted = nullptr;
		bUndo = true;
	}
	if( XWndSpotForTool::s_idUndoSpot )
	{
		auto pProp = PROP_WORLD->GetpProp( XWndSpotForTool::s_idUndoSpot );
		if( pProp )
		{
			pProp->vWorld = XWndSpotForTool::s_vUndo;
			bUndo = true;
		}
		XWndSpotForTool::s_idUndoSpot = 0;
	}
	if( bUndo )
	{
		if( SCENE_WORLD )
			SCENE_WORLD->SetbUpdate( TRUE );
		UpdateAutoSave();
	}
}

////////////////////////////////////////////////////////////////
std::shared_ptr<XToolBgObj> XToolBgObj::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XToolBgObj>& XToolBgObj::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XToolBgObj>( new XToolBgObj );
	return s_spInstance;
}
////////////////////////////////////////////////////////////////
XToolBgObj::XToolBgObj()
	: XBaseTool( xTM_OBJ )
{
	Init();
}

void XToolBgObj::Destroy()
{
}

// BOOL XToolBgObj::OnCreate()
// {
// 	return TRUE;
// }

void XToolBgObj::OnEnterMode()
{
	// 월드씬을 구름을 안보이게 함.
// 	if( SCENE_WORLD ) {
// 		auto pWndScrl = SCENE_WORLD->Find( "scroll.view" );
// 		XBREAK( pWndScrl == nullptr );
// 		auto pCloudLayer = pWndScrl->Find( "scroll.cloud.view" );
// 		if( XASSERT(pCloudLayer) )
// 			pCloudLayer->SetbShow( false );
// 	}
}

void XToolBgObj::OnLeaveMode()
{
	if( !SCENE_WORLD )
		return;
	// 월드씬의 구름을 다시 나타나게 함.
// 	if( SCENE_WORLD ) {
// 		auto pWndScrl = SCENE_WORLD->Find( "scroll.view" );
// 		XBREAK( pWndScrl == nullptr );
// 		auto pCloudLayer = pWndScrl->Find( "scroll.cloud.view" );
// 		if( XASSERT(pCloudLayer) )
// 			pCloudLayer->SetbShow( false );
// 	}
}

bool XToolBgObj::Save()
{
	XPropBgObj::sGet()->SaveProp( _T("propObj.xml") );
	return true;
}
void XToolBgObj::DelSelected()
{

}
void XToolBgObj::CopySelected()
{
}

void XToolBgObj::PasteSelected()
{

}
void XToolBgObj::Undo()
{

}

////////////////////////////////////////////////////////////////
std::shared_ptr<XToolTech> XToolTech::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XToolTech>& XToolTech::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XToolTech>( new XToolTech );
	return s_spInstance;
}
////////////////////////////////////////////////////////////////
XToolTech::XToolTech()
	: XBaseTool( xTM_TECH )
{
	Init();
}

void XToolTech::Destroy()
{
}

void XToolTech::OnEnterMode()
{
}

void XToolTech::OnLeaveMode()
{
}

bool XToolTech::Save()
{
	XPropTech::sGet()->Save( _T( "propTech2.xml" ) );
	return true;
}
void XToolTech::DelSelected()
{

}
void XToolTech::CopySelected()
{
}

void XToolTech::PasteSelected()
{

}
void XToolTech::Undo()
{

}


#endif // _xIN_TOOL
