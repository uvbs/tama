#include "stdafx.h"
#include "XLayout.h"
//#include "XWindow.h"
#include "XFramework/XConstant.h"
#include "XClientMain.h"
#include "etc/xLang.h"
#include "XAutoPtr.h"
#include "XSoundMng.h"
#include "XEContent.h"
#include "_Wnd2/XWndText.h"
#include "_Wnd2/XWndButton.h"
#include "_Wnd2/XWndButtonRadio.h"
#include "_Wnd2/XWndImage.h"
#include "_Wnd2/XWndProgressBar.h"
#include "_Wnd2/XWndScrollView.h"
#include "_Wnd2/XWndList.h"
#include "_Wnd2/XWndCtrls.h"
#include "_Wnd2/XWndPageSlide.h"
#include "_Wnd2/XWndEdit.h"
#include "_Wnd2/XWndPopup.h"
#include "XResMng.h"
#include "sprite/Sprdef.h"
#include "sprite/SprObj.h"
#include "_Wnd2/XWndSprObj.h"
#include "sprite/XActObj2.h"


//////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XLayout* XLayout::s_pMain = nullptr;		// 메인 레이아웃. 
XDelegateLayout* XLayout::s_pDelegateMain = nullptr;

XWndList* xGET_LIST_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndList*>( pWnd );
}

XWndView* XDelegateLayout::DelegateLayoutCreateViewCtrl( XWnd* pParent, const XLayout::xATTR_ALL& attr )
{
	return new XWndView();
}

//////////////////////////////////////////////////////////////////////////
XLayout* XLayout::sCreateMain( LPCTSTR szXml, XDelegateLayout* pDelegateMain )
{
	XBREAK( s_pMain != nullptr );
	s_pDelegateMain = pDelegateMain;
	s_pMain = new XLayout( szXml, nullptr, nullptr );
	return s_pMain;
}
XLayout::XLayout( LPCTSTR szXml, XDelegateLayout *pDelegate, XLayout *pParent ) 
{ 
	Init(); 
	m_pDelegate = pDelegate;
	m_pParent = pParent;
	TCHAR szFullpath[ 1024 ];
	// szRes의 풀패스를 얻어낸다.
	{
		_tstring strPath = XE::MakePathLang( DIR_LAYOUT, szXml );
		if( XE::IsExistFileInPackage( strPath.c_str() ) )		{
			XE::SetReadyRes( szFullpath, XE::MakePathLang( DIR_LAYOUT, szXml ) );
		} else		{
			XE::SetReadyRes( szFullpath, XE::MakePath( DIR_LAYOUT, szXml ) );
		}
	}
	// 각 언어폴더의 레이아웃 파일을 먼저 준비시켜보고 실패하면 루트폴더의 파일을 준비시킨다.
	if( XBREAK( XE::IsEmpty( szFullpath ) ) )
		return;
	m_strFullpath = SZ2C( szFullpath );
	//
	Load( m_strFullpath.c_str() );
}

BOOL XLayout::Load( const char *cFullpath )
{
	// 언어 폴더부터 찾아봐서 있으면 그걸 우선으로 읽는다.
	char cLangPath[ 1024 ];
	XE::LANG.ChangeToLangDir( m_strFullpath.c_str(), cLangPath );
	if( m_Layout.LoadFile( cLangPath ) == false )
	{
		if( m_Layout.LoadFile( m_strFullpath.c_str() ) == false )
		{
			XLOGXNA("failed open xml: %s\n%s", cFullpath, m_Layout.ErrorDesc() );
			return FALSE;
		}
	}
	// 루트 노드를 찾음.
	m_rootNode = m_Layout.FirstChild( "layout" );
	if( m_rootNode == nullptr ) {
		m_rootNode = m_Layout.FirstChild( "root" );		// 이제 이것만 쓸것.
		if( XBREAK( m_rootNode == NULL ) )
			return FALSE;
	}
	m_bLoad = TRUE;
	CONSOLE("%s load success", C2SZ(m_strFullpath.c_str()) );
	return TRUE;
}


BOOL XLayout::Reload( void )
{
	m_Layout.Clear();
	return Load( m_strFullpath.c_str() );
}


/**
 "main.select_theme.evol"같은 형식으로 바꾸자.
 포인트(.)단위로 파싱해서 키를 찾아들어가도록
 //
 일단은 cKeyGroup명은 전체 xml내에서 고유한 문자열이어야 한다.
 cKeyWnd이름은 cKeyGroup내에서 고유한 문자열이면 된다.
*/
BOOL XLayout::CreateLayout( const char *cKeyWnd, XWnd *pParent, const char *cKeyGroup )
{
	XLOAD_PROFILE1;
//	CONSOLE("layout create: key=%s", C2SZ(cKeyWnd) );
	TiXmlNode *nodeWnd = NULL;
	if( XE::IsHave( cKeyGroup ) ) {
		// 그룹명이 있을경우 그룹에서 cKeyWnd를 찾음.
		TiXmlNode *nodeGroup = m_rootNode->FirstChild( cKeyGroup );
		nodeWnd = GetNode( cKeyWnd, nodeGroup );	// 그룹하위의 자식들을 다뒤져서 찾아낸다.
	} else
		nodeWnd = GetNode( cKeyWnd, m_rootNode );

	XBREAK( nodeWnd == nullptr );
	if( nodeWnd == NULL ) {
		XLOGXN( "not found key: %s", C2SZ(cKeyWnd) );
		return FALSE;
	}
	m_nodeWnd = nodeWnd;
	//
	if( pParent ) {
		xATTR_ALL attr;
		GetAttrCommon( nodeWnd->ToElement(), &attr );
		auto vPosParent = pParent->GetPosLocal();
		auto sizeParent = pParent->GetSizeLocal();
		// 미리 위치와 크기가 지정되어있지 않은경우만 layout값을 사용한다.
		if( vPosParent.IsInvalid() && attr.vPos.IsValid() ) {
			// 부모의좌표가 0,-1이고 layout에서 >0으로 지정되었으면 위치를 지정한다.
			pParent->SetPosLocal( attr.vPos );
		}
		if( sizeParent.IsInvalid() && attr.vSize.IsValid() )
			pParent->SetSizeLocal( attr.vSize );
	}
	auto bRet = CreateLayout( nodeWnd->ToElement(), pParent );
	XLOAD_PROFILE2;
	_tstring strKey = C2SZ(cKeyWnd);
	XLOGP( "%s, %s, %llu", XE::GetFileName( C2SZ(m_strFullpath) ),  strKey.c_str(), __llPass );
	return bRet;
}

BOOL XLayout::CreateLayout( TiXmlElement *elemNode, XWnd *pParent )
{
	if( elemNode == NULL )
		return FALSE;
	//
	TiXmlElement *elemCtrl = elemNode->FirstChildElement();
	if( elemCtrl == NULL )
		return FALSE;
	do {
		// 윈도노드 안의 모든 컨트롤들을 꺼낸다.
		const char *cNameCtrl = elemCtrl->Value();
		// virtual 
		CreateControl( cNameCtrl, elemCtrl, pParent );

	} while(( elemCtrl = elemCtrl->NextSiblingElement() ));
	return TRUE;
}

/**
 elemNode에 정의된 윈도우를 하나의 윈도우로 생성해서 돌려준다.
 그러므로 노드의 바로밑 자식은 단 하나여야 한다. 여러개있을경우 첫번째것만 인식한다.
*/
XWnd* XLayout::CreateXWindow( TiXmlElement *elemNode )
{
	TiXmlElement *elemCtrl = elemNode->FirstChildElement();
	if( elemCtrl == NULL )
		return NULL;
	const char *cCtrlName = elemCtrl->Value();
	XWnd *pRoot = NULL;
	CreateControl( cCtrlName, elemCtrl, NULL, &pRoot );
	return pRoot;
}

// 바로 아랫단계 자식들 레벨을 우선적으로 검색하고 손자레벨로 검색을 확장하는 버전.
TiXmlNode* XLayout::FindNode( TiXmlNode *nodeWnd, const char *cKey )
{
	// 바로 아래 자식들 레벨부터 우선적으로 검사해본다.
	TiXmlNode *nodeChild = nodeWnd->FirstChild( cKey );
	if( nodeChild )
		return nodeChild;
	// 못찾았으면 각 각의 자식들을 돌면서 손자레벨까지 들여다본다.
	nodeChild = nodeWnd->FirstChild();
	if( nodeChild == NULL )
		return NULL;
	do 
	{
		// 자식들속에서 검색해본다.
		TiXmlNode *findChild = FindNode( nodeChild, cKey );
		if( findChild )
			return findChild;
	} while (( nodeChild = nodeChild->NextSibling() ));
	return NULL;
}


XLAYOUT_ELEM* XLayout::GetElement( const char* cKey, TiXmlNode *nodeRoot )
{
	if( nodeRoot == NULL )
		nodeRoot = m_rootNode;
	TiXmlNode *nodeWnd = 	GetNode( cKey, nodeRoot );
	if( nodeWnd )
		return nodeWnd->ToElement();
	return NULL;
}

XLAYOUT_NODE* XLayout::GetNode( const char* cKey, TiXmlNode *nodeRoot )
{
	if( nodeRoot == NULL )
		nodeRoot = m_rootNode;
	TiXmlNode *nodeWnd = 	FindNode( nodeRoot, cKey );
	return nodeWnd;
}

/**
 
*/
int XLayout::CreateControl( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, XWnd **ppOutCreated )
{
	TCHAR szWndNode[ 64 ];
	_tcscpy_s( szWndNode, C2SZ( m_nodeWnd->Value() ) );
	m_strNodeWnd = szWndNode;
	//
	XWnd *pAddWnd = NULL;
	xATTR_ALL attrAll;
	GetAttrCommon( elemCtrl, &attrAll );
	int debug = 0;
#ifdef _DEBUG
	{
		elemCtrl->Attribute( "debug", &debug );
		if( debug ) {
			attrAll.m_Debug = debug;
		}
	}
#endif // _DEBUG
	//////////////////////////////////////////////////////////////////////////
	// XWndImage 컨트롤
	if( XE::IsSame( cCtrlName, "img" ) ) {
		if( attrAll.num > 1 ) {
			// 여러개 생성
			XE::VEC2 v = attrAll.vPos;
			for( int i = 0; i < attrAll.num; ++i ) {
				XWnd *pImg = CreateImgCtrl( cCtrlName, elemCtrl, pParent, attrAll, i );
				if( pImg ) {
					// 버추얼 커스텀에게 맡겼는데도 모르는게 나왔다면 이 노드를 키로 다시 리커시브로 들어가본다.
					CreateLayout( elemCtrl, pImg );
					// pAddWnd의 자식들이 모두 생성이 끝나고 핸들러가 호출된다.
					pImg->OnFinishCreatedChildLayout( this );
					//
					if( pParent )
						AddWndParent( pImg, pParent, attrAll, i );
					v += attrAll.vDist;
				}
			}
		} else
			pAddWnd = CreateImgCtrl( cCtrlName, elemCtrl, pParent, attrAll );

	} else
	if( XE::IsSame( cCtrlName, "spr" ) ) {
		pAddWnd = CreateSprCtrl( cCtrlName, elemCtrl, pParent, attrAll );
	} else
	//////////////////////////////////////////////////////////////////////////
	if( XE::IsSame( cCtrlName, "pbar2" ) ) {
		pAddWnd = CreateProgressBarCtrl2( cCtrlName, elemCtrl, pParent, attrAll );
	} else
	//////////////////////////////////////////////////////////////////////////
	if( XE::IsSame( cCtrlName, "pbar" ) ) {
		pAddWnd = CreateProgressBarCtrl( cCtrlName, elemCtrl, pParent, attrAll );
	} else
	//////////////////////////////////////////////////////////////////////////
	if( XE::IsSame( cCtrlName, "butt" ) ) {
		if( attrAll.num > 1 ) {
			// 여러개 생성
			for( int i = 0; i < attrAll.num; ++i ) {
				XWnd *pButt = CreateButtonCtrl( cCtrlName, elemCtrl, pParent, attrAll, i );
				if( pButt ) {
					CreateLayout( elemCtrl, pButt );
					pButt->OnFinishCreatedChildLayout( this );
					if( pParent )
						AddWndParent( pButt, pParent, attrAll, i );
				}
			}
		} else {
			pAddWnd = CreateButtonCtrl( cCtrlName, elemCtrl, pParent, attrAll );
		}
	} // butt
	else
	if( XE::IsSame( cCtrlName, "radio_butt" ) ) {
		int idGroup = 0;
		elemCtrl->Attribute( "group", &idGroup );
		if( XBREAK(idGroup == 0) )
			CONSOLE("node=%s:radio_butt:group is zero", szWndNode);
		//
		if( attrAll.num > 1 ) {
			// 여러개 생성
			for( int i = 0; i < attrAll.num; ++i ) {
				XWnd *pButt = CreateRadioButtonCtrl( cCtrlName, elemCtrl, pParent, (ID)idGroup, attrAll/*, attrButt*/, i );
				if( pButt ) {
					CreateLayout( elemCtrl, pButt );
					pButt->OnFinishCreatedChildLayout( this );
					if( pParent )
						AddWndParent( pButt, pParent, attrAll, i );
				}
			}
		} else {
			pAddWnd = CreateRadioButtonCtrl( cCtrlName, elemCtrl, pParent, (ID)idGroup, attrAll/*, attrButt*/ );
		}
	} else
	//////////////////////////////////////////////////////////////////////////
	if( XE::IsSame( cCtrlName, "text" ) ) {
		pAddWnd = CreateStaticText( cCtrlName, elemCtrl, pParent, attrAll );
	} else
	//////////////////////////////////////////////////////////////////////////
	if( XE::IsSame( cCtrlName, "list" ) ) {
		pAddWnd = CreateListCtrl( cCtrlName, elemCtrl, pParent, attrAll );
	} else
	if( XE::IsSame( cCtrlName, "slide" ) ) {
		pAddWnd = CreateSlideCtrl( cCtrlName, elemCtrl, pParent, attrAll );
	} else
	if( XE::IsSame( cCtrlName, "popup" ) ) {
		pAddWnd = CreatePopupFrame( cCtrlName, elemCtrl, pParent, attrAll );
	} else
	if( XE::IsSame( cCtrlName, "view" ) ) {
		pAddWnd = CreateViewFrame( cCtrlName, elemCtrl, pParent, attrAll );
	} else
	if( XE::IsSame( cCtrlName, "tabview" ) ) {
		pAddWnd = CreateTabViewCtrl( cCtrlName, elemCtrl, pParent, attrAll );
	} else	
	if( XE::IsSame( cCtrlName, "tab" ) ) {
		AddTab( cCtrlName, elemCtrl, pParent, attrAll );
	} else
	if( XE::IsSame( cCtrlName, "wnd" ) ) {
		if( attrAll.num > 1 ) {
			// 여러개 생성
			XE::VEC2 v = attrAll.vPos;
			for( int i = 0; i < attrAll.num; ++i ) {
				XWnd *pWnd = new XWnd( v, attrAll.vSize );
				if( pWnd ) {
					pWnd->SetScaleLocal( attrAll.vScale );
					// 버추얼 커스텀에게 맡겼는데도 모르는게 나왔다면 이 노드를 키로 다시 리커시브로 들어가본다.
					CreateLayout( elemCtrl, pWnd );
					// pAddWnd의 자식들이 모두 생성이 끝나고 핸들러가 호출된다.
					pWnd->OnFinishCreatedChildLayout( this );
					if( pParent )
						AddWndParent( pWnd, pParent, attrAll, i );
					v += attrAll.vDist;
				}
			}
		} else {
			pAddWnd = new XWnd( attrAll.vPos, attrAll.vSize );
			pAddWnd->SetScaleLocal( attrAll.vScale );
		}
	} else
	if( XE::IsSame( cCtrlName, "rect" ) ) {
		pAddWnd = new XWndRect( attrAll.vPos, attrAll.vSize, attrAll.col );
		pAddWnd->SetAlphaLocal( attrAll.alpha );
	} else
	if( XE::IsSame( cCtrlName, "slide_down" ) )
	{
		pAddWnd = CreateSlideDownCtrl( cCtrlName, elemCtrl, pParent, attrAll );
	} else
	if( XE::IsSame( cCtrlName, "edit_box" ) || XE::IsSame( cCtrlName, "edit" ))
	{
		pAddWnd = CreateEditBoxCtrl( cCtrlName, elemCtrl, pParent, attrAll );
	} else
	if(	XE::IsSame( cCtrlName, "scroll_view" ) )
	{
		pAddWnd = CreateScrollViewCtrl( cCtrlName, elemCtrl, pParent, attrAll );
	} else
	if( XE::IsSame( cCtrlName, "sound" ) || XE::IsSame( cCtrlName, "snd" ) ) {
//		xATTR_ALL attrSnd;
//		GetAttrCommon( elemCtrl, &attrSnd );
		if( attrAll.idActUp )
			pAddWnd = new XWndPlaySound( attrAll.idActUp );
		else
		if( !attrAll.GetstrFile().empty() )
			pAddWnd = new XWndPlayMusic( SZ2C( attrAll.GetstrFile() ) );
// 				pAddWnd = new XWndPlayMusic( SZ2C(attrSnd.strImgDown) );
	} else
	if( XE::IsSame( cCtrlName, "layout" ) ) {
		auto strtLayout = attrAll.GetstrFile();
		if( strtLayout.empty() ) {
			const _tstring strtFile = C2SZ( m_strFullpath );
			strtLayout = XE::GetFileName( strtFile );
		}
		XBREAK( strtLayout.empty() );
		XLayoutObj layoutObj( strtLayout );
		const int num = attrAll.num;
		auto vPos = attrAll.vPos;
		//
		for( int i = 0; i < num; ++i ) {
			XWnd *pRoot = new XWnd();
			if( layoutObj.CreateLayout( attrAll.m_strcLayout, pRoot ) ) {
				pRoot->SetPosLocal( vPos );
				pRoot->SetAutoSize();
				AlignCtrl( "layout", elemCtrl, pParent, pRoot, attrAll );
				pRoot->SetScaleLocal( attrAll.vScale );
				pRoot->SetbShow( attrAll.bShow );
				if( num > 1 )
					pRoot->SetstrIdentifierf( "%s%d", attrAll.strKey.c_str(), i + 1 );
				else
					pRoot->SetstrIdentifier( attrAll.strKey );
				pParent->Add( pRoot );
				CreateLayout( elemCtrl, pRoot );
			} else {
				SAFE_DELETE( pRoot );
			}
			vPos += attrAll.vDist;
		}
	}	else 
	if( XE::IsSame( cCtrlName, "click" ) ) {
		// click event
		xATTR_ALL attrClick;
		GetAttrCommon( elemCtrl, &attrClick );
// 		ID idTooltip = 0;
// 		GetAttrDWORD( elemCtrl, "tooltip", &idTooltip );
		// 클릭이벤트를 할당해야할 실제 컨트롤을 얻음.(아직은 구조적인 문제로 click이벤트가 해당 컨트롤부터 아래에 있어야 함.
		auto pWndClick = pParent->Find( attrClick.strKey );
		if( pWndClick ) {
			pWndClick->SetEvent( XWM_CLICKED, pParent, &XWnd::__OnClickLayoutEvent );
		}
		if( attrClick.GetstrFile().empty() ) {
			std::string strcFile = XE::GetFileName( m_strFullpath );
			attrClick.SetstrFile( C2SZ(strcFile) );
		}
		xnWnd::xClickEvent event;
		event.m_xmlClick = attrClick.GetstrFile();
		event.m_strNodeClick = attrClick.m_strcLayout;
		event.m_vPopupByLayout = attrClick.vPos;
		if( attrClick.align )
			event.m_AlignClick = attrClick.align;
		event.m_strKeyTarget = attrClick.strKey;
		event.m_strType = elemCtrl->Attribute( "wnd_type" );
		pParent->AddClickEvent( event );
	} else
	if( XE::IsSame( cCtrlName, "tooltip" ) ) {
		xnWnd::xTooltip event;
		event.m_xml = attrAll.GetstrFile();
		event.m_strNode = attrAll.m_strcLayout;
		event.m_idText = attrAll.idText;
		DWORD length = 0;
		if( !GetAttrDWORD( elemCtrl, "len", &length ) ) {
			GetAttrDWORD( elemCtrl, "length", &length );
		}
		event.m_Length = (float)length;
		pParent->SetdatTooltip( event );
		if( pParent )
			pParent->SetEvent( XWM_TOOLTIP, XEContent::sGet(), &XEContent::__OnClickTooltip );
		
	} else {
		// 해석이 안되는 custom 컨트롤
		if( attrAll.num > 1 ) {
			xATTR_ALL attrCustom = attrAll;
			GetAttrCommon( elemCtrl, &attrCustom );
			for( int ic = 0; ic < attrAll.num; ++ic ) {
				// virtual
				XWnd *pCtrl = CreateCustomControl( cCtrlName, elemCtrl, pParent, attrCustom );		
				if( pCtrl ) {
					attrCustom.vPos += attrAll.vDist;
					AddWndParent( pCtrl, pParent, attrCustom, ic );
				}
			}
		} else
			// virtual
			pAddWnd = CreateCustomControl( cCtrlName, elemCtrl, pParent, attrAll );		
	}

	// 만들어진 윈도우 컨트롤을 부모윈도우에 붙인다.
	if( pAddWnd ) {
		pAddWnd->SetDebug( debug );
//		pAddWnd->SetvAdjDrawLocal( attrAll.vAdjust );
		pAddWnd->SetvAdjPos( attrAll.vAdjust );
		if( attrAll.m_Touchable >= 0 )
			pAddWnd->SetbTouchable( (attrAll.m_Touchable == 1)? TRUE : FALSE );
		// 먼저 부모에 붙인다.
		if( pParent )
			AddWndParent( pAddWnd, pParent, attrAll );
		// 버추얼 커스텀에게 맡겼는데도 모르는게 나왔다면 이 노드를 키로 다시 리커시브로 들어가본다.
		CreateLayout( elemCtrl, pAddWnd );
		// 좌표 정렬(자식들의 생성이 끝난후 실시)
		if( attrAll.align && attrAll.align != XE::xALIGN_LEFT ) {
			pAddWnd->SetAlign( attrAll.align );
			pAddWnd->SetbUpdate( true );		// 
///<		이걸 여기서 먼저 해버리면 pAddWnd의 상위 부모가 있을때 그 상위부모의 크기가 아직 정해지지 않은 시점이면
			// pAddWnd의 vPos이 autoLayout으로 인해 잘못정해지고 부모측에서 차일드전체 크기를 구할때 역시 크기가 잘못 구해진다.
			// layout파일에 직접 pos값을 명시한거랑 "center"로 자동으로 구해진거랑 구분할 필요가 있다.
			// 바로 상위부모가 크기가 정해지지 않았고 "wnd"같은게 아니라 "view"처럼 잠시 보류된거면 자식도 정렬을 하면 안될것 같다.
//			pAddWnd->AutoLayoutByAlign( pParent, attrAll.align );
		}
		// pAddWnd의 자식들이 모두 생성이 끝나고 핸들러가 호출된다.
		pAddWnd->OnFinishCreatedChildLayout( this );
		//
// 		if( pParent )
// 			AddWndParent( pAddWnd, pParent, attrAll );
		if( ppOutCreated )
			*ppOutCreated = pAddWnd;
		return 1;
	}

	return 0;
}

// 부모윈도우에 추가시킬 컨트롤의 공통함수.
void XLayout::AddWndParent( XWnd *pAddWnd, 
							XWnd *pParent, 
							const xATTR_ALL& attrAll, 
							int idxMulti )
{
	// 윈도우 키 문자열이 있으면 세팅
	if( attrAll.strKey.empty() == false ) {
		if( idxMulti >= 0 ) {
			// 멀티생성일경우 키문자열 끝에 1~번호를 매김./
			char cKey[ 256 ];
			sprintf_s( cKey, "%s%d", attrAll.strKey.c_str(), idxMulti + 1 );
			pAddWnd->SetstrIdentifier( cKey );
		} else
			pAddWnd->SetstrIdentifier( attrAll.strKey.c_str() );
	}
	if( pAddWnd->GetbModal() )
		pParent->AddWndTop( pAddWnd );
	else
		pParent->Add( pAddWnd );
	if( attrAll.bEnable == FALSE )
		pAddWnd->SetbEnable( FALSE );
	if( attrAll.bShow == FALSE )
		pAddWnd->SetbShow( FALSE );
}


//////////////////////////////////////////////////////////////////////////
// Create Control

/**
 @brief 
*/
XWnd *XLayout::CreateStaticText( const char *cCtrlName, 
										TiXmlElement *elemCtrl, 
										XWnd *pParent, 
										const xATTR_ALL& attrAll )
{
// 	xATTR_ALL attr;
// 	attr = attrAll;			// attrAll로 먼저 넣고
// 	GetAttrCtrlText( elemCtrl, &attr );		// 추가로 텍스트의 속성을 넣는다.
	XWnd *pAddWnd = NULL;
	//
	if( attrAll.num > 1 ) {
		// 여러개 생성
		for( int i = 0; i < attrAll.num; ++i ) {
			XWnd *pWnd = CreateStaticTextOnlyOne( cCtrlName, elemCtrl, pParent, attrAll, i );
			AddWndParent( pWnd, pParent, attrAll, i );
		}
	} else {
		pAddWnd = CreateStaticTextOnlyOne( cCtrlName, elemCtrl, pParent, attrAll );
//		pAddWnd = CreateStaticTextOnlyOne( cCtrlName, elemCtrl, pParent, attr );
	}

	return pAddWnd;
}

/**
 @brief 
*/
XWnd *XLayout::CreateStaticTextOnlyOne( const char *cCtrlName, 
																				TiXmlElement *elemCtrl, 
																				XWnd *pParent, 
																				const xATTR_ALL& attr, 
																				int idxMulti )
{
	XE::VEC2 vPos = attr.vPos;
	if( idxMulti > 0 )
		vPos += attr.vDist * (float)idxMulti;
	xFONT::xtStyle style = attr.style;
// #ifdef _VER_IOS
// 	// 아이폰에선 팝업일때 외곽선찍기가 느려서 쉐도우로 바꿈
// 	if( pParent->GetwtWnd() == XE::WT_POPUP )
// 		if( style == xFONT::xSTYLE_STROKE )
// 			style = xFONT::xSTYLE_SHADOW;
// #endif
//	XBREAK( attr.sizeFont == 0 );
	int length = 0;
	if( !GetAttrInt( elemCtrl, "len", &length ) ) {
		GetAttrInt( elemCtrl, "length", &length );
	}
	const _tstring strFont = (attr.strFont.empty())? FONT_SYSTEM : attr.strFont;
	const float sizeFont = (attr.sizeFont > 0)? attr.sizeFont : FONT_SIZE_DEFAULT;
	const auto vSize = attr.vSize;
	//
	XWndTextString *pText;
	if( attr.xfFlag & xF_COUNTER ) {
		auto pTextCnt = new XWndTextNumberCounter( vPos
																							, vSize
																							, strFont.c_str()
																							, sizeFont
																							, attr.colText );
		pTextCnt->SetbComma( (attr.xfFlag & xF_COMMA) != 0 );
		pText = pTextCnt;
	} else {
		const ID idText = (attr.idText != 0xffffffff)? attr.idText : 0;
		pText = new XWndTextString( vPos, vSize
															, XTEXT(idText)
															, strFont.c_str()
															, sizeFont
															, attr.colText );
		pText->SetbComma( ( attr.xfFlag & xF_COMMA ) != 0 );
	}
	if( length > 0 ) {
		// 문단 폭
		pText->SetLineLength( (float)length );
	}
	if( attr.style )
		pText->SetStyle( attr.style );
	if( attr.align )
		pText->SetAlign( attr.align );
	return pText;
}
/**
페이지 슬라이드 컨트롤
 elemCtrl: 슬라이드노드
*/
XWnd* XLayout::CreateSlideCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll )
{
	xATTR_ALL attr;
	attr = attrAll;
#ifdef _DEBUG
	const char *_cName = NULL;
#endif
	XWndPageSlideWithXML *pSlide = NULL;
	// 페이지가 있을경우에만 컨트롤윈도우를 생성.
	// position
	GetAttrPos( elemCtrl, "pos", &attr.vPos );
	// size
	GetAttrSize( elemCtrl, &attr.vSize );
	//
	// 현재 페이지의 위치를 보여주는 포인트이미지 파일명. 파일명.png의 형식
	// 으로 넣으면 파일명_on/off.png형태로 자동으로 바꿔준다.
	const char *cImgPoint = elemCtrl->Attribute( "img_point" );
//	_tstring strImgPoint;
	if( XE::IsHave( cImgPoint ) )
		attr.strImgDisable = C2SZ( cImgPoint );
//		strImgPoint = C2SZ( cImgPoint );
	std::string strDefaultPageLayout;
	{
		const char *cAttr = elemCtrl->Attribute( "default_page_layout" );
		if( XE::IsHave( cAttr ) )
			strDefaultPageLayout = cAttr;
	}
	//
//	pSlide = new XWndPageSlideWithXML( vPos, vSize, this, elemCtrl, strImgPoint.c_str() );
	pSlide = CreateSlideWnd( attr, elemCtrl );
	if( pSlide )
	{
		if( pSlide->GetstrPageLayout().empty() == true )	// 상속쪽에서 임의로 지정했을수도 있으므로 지정안했을때만 xml값으로 쓴다.
			pSlide->SetstrPageLayout( strDefaultPageLayout );
		//
		XWndPageSlideWithXMLDelegate *pDelegate = NULL;
		if( m_pDelegate )	{
			XBREAK(1);		// 델리게이트가 XDelegateLayout으로 바껴서 이부분 다시 프로그래밍 해야함.
			pDelegate = dynamic_cast<XWndPageSlideWithXMLDelegate*>( m_pDelegate );
			pSlide->SetpDelegate( pDelegate );
		}
		// xml에 페이지를 직접 구현했으면 여기서 생성한다.
		TiXmlElement *elemPage = elemCtrl->FirstChildElement( "page" );
		if( elemPage ) {
			//
			int numPages = 0;
			do {
				const char *cKeyPage = elemPage->Value();
				if( XE::IsSame( cKeyPage, "page" ) ) {
					++numPages;
				}
			} while( ( elemPage = elemPage->NextSiblingElement() ) );
			// "page"개수를 세어 슬라이드뷰의 총페이지 수로 정함. 
			// 페이지 인덱스는 자동으로 0~ 부터 매겨진다.
			pSlide->SetnumPages( numPages );
			// for XML
		} else
		{
			// 페이지 레이아웃이 정의되어 있을때....
			if( strDefaultPageLayout.empty() == false ) {
				if( m_pDelegate ) {
					// 몇페이지나 만들건지 델리게이트에 물어본다.
					int numPages = 0;
					if( pDelegate )
						numPages = pDelegate->GetNumPagesForSlide( attr );
					//				XWndPageForSlide *pPage = new XWndPageForSlide( i, XE::VEC2(0), attr.vSize );
					pSlide->SetnumPages( numPages );
				}
			} else {
				// 일단은 "page1,2,3"과 default_page_layout은 같이 쓰지 못하는것으로...
				// page레이아웃을 페이지별로 따로 지정했으면 여기서...
			}
			// 페이지가 하나라도 있을때.
		}
		// 몇번페이지부터 나타나게 할건가.
		{
			int idxPage = 0;
			if( pDelegate )
				idxPage = pDelegate->GetStartPageIndexForSlide( attr );
			pSlide->SetCurrPage( idxPage );
		}
	}
	//
	return pSlide;
}

XWndPageSlideWithXML* XLayout::CreateSlideWnd( const xATTR_ALL& attr, TiXmlElement *elemCtrl )
{
	XWndPageSlideWithXML *pSlide = new XWndPageSlideWithXML( attr.vPos, 
													attr.vSize, 
													this, 
													elemCtrl, 
													attr.strImgDisable.c_str() );
	return pSlide;
}

/**
 XWndImage컨트롤
*/
XWnd* XLayout::CreateImgCtrl( const char *cCtrlName, 
							TiXmlElement *elemCtrl, 
							XWnd *pParent, 
							const xATTR_ALL& attrAll,
							int idxMulti )
{
	XE::VEC2 vPos = attrAll.vPos;
	//
	// 파일의 패스를 붙인다. 확장자가 없다면 디폴트 확장자도 붙인다.
	_tstring strRes = attrAll.GetstrFile();
	if( !strRes.empty() )	{
		const _tstring strPath = XE::GetFilePath( strRes );
		if( strPath.empty() ) {
			// 패스가 지정되어있지 않으면 디폴트로 ui폴더를 사용한다.
			strRes = XE::MakePath( DIR_UI, strRes );
		}
		if( XE::IsEmpty(XE::GetFileExt(strRes)) )
			strRes += _T(".png");
	}
	//
	auto pWnd = new XWndImage( strRes, attrAll.m_Format, vPos );
	if( idxMulti ) {
		auto vDist = attrAll.vDist;
		if( vDist.w == 0 )
			vDist.w = 2.f;	// 사용자 실수를 방지하기위해 값이 지정되지 않으면 기본값으로.
		vPos += vDist * (float)idxMulti;
		pWnd->SetPosLocal( vPos );
	}
	// 사이즈가 명시되어 있으면 명시된 사이즈로 고친다.
	if( attrAll.vSize.w > 0 || attrAll.vSize.h > 0 ) {
		auto sizeThis = pWnd->GetSizeLocalNoTrans();
		if( attrAll.vSize.w > 0 )
			sizeThis.w = attrAll.vSize.w;
		if( attrAll.vSize.h > 0)
			sizeThis.h = attrAll.vSize.h;
		pWnd->SetSizeLocal( sizeThis );
	}

	pWnd->SetRotateY( attrAll.vRot.y );
	pWnd->SetRotateZ( attrAll.vRot.z );
	pWnd->SetScaleLocal( attrAll.vScale );
	// 그림을 부모크기에 맞춰서 중앙정렬.
	const XE::VEC2 vSize = pWnd->GetSizeLocal();
	pWnd->SetAlign( attrAll.align );
	if( attrAll.vRot.y != 0 || attrAll.vRot.z != 0 ) {
		auto size = vSize * 0.5f;
		pWnd->SetAdjustAxis( size );	// 회전축을 중앙으로
	}
	pWnd->SetAlphaLocal( attrAll.alpha );
	pWnd->SetblendFunc( attrAll.m_funcBlend );
	return pWnd;
}

/**
 SprObj컨트롤
*/
XWnd* XLayout::CreateSprCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll )
{
	XE::VEC2 vPos = attrAll.vPos;
	if( attrAll.align == XE::xALIGN_HCENTER ) {
		// 중앙정렬로 되어있으면 좌표를 부모의 중앙으로 한다.
		const XE::VEC2 vSize = pParent->GetSizeValidNoTrans();
		if( vSize.w > 0 )
			vPos.x = vSize.w / 2.f;
	}
	// 확장자가 없으면 붙인다.
	_tstring strSpr = attrAll.GetstrFile();
	if( !strSpr.empty() ) {
		if( XE::IsEmpty( XE::GetFileExt( strSpr ) ) ) {
			strSpr += _T( ".spr" );
		}
	}
	//
	int idAct = attrAll.idAct;
	if( idAct <= 0 )
		idAct = 1;		// 지정안되어있으면 디폴트 1로 한다.
	// playtype
	const char *cPlayType = elemCtrl->Attribute( "play_type" );
	xRPT_TYPE loopType = xRPT_LOOP;
	if( XE::IsHave( cPlayType ) ) {
		if( XE::IsSame( cPlayType, "once" ) )
			loopType = xRPT_1PLAY;
		else if( XE::IsSame( cPlayType, "once_standby" ) )
			loopType = xRPT_1PLAY_CONT;
	}

	if( attrAll.num > 1 ) {
		// 여러개 생성
		auto v = vPos;
		for( int i = 0; i < attrAll.num; ++i ) {
			XWndSprObj *pWnd = NULL;
			if( strSpr.empty() ) {
				pWnd = new XWndSprObj( v.x, v.y );
				pWnd->SetloopType( loopType );
			} else
				pWnd = new XWndSprObj( strSpr.c_str(), (ID)idAct, v.x, v.y, loopType );
			if( pWnd ) {
				pWnd->SetRotateY( attrAll.vRot.y );
				pWnd->SetRotateZ( attrAll.vRot.z );
				pWnd->SetFlipHoriz( attrAll.m_bFlipX );
				pWnd->SetFlipVert( attrAll.m_bFlipY );
				pWnd->SetScaleLocal( attrAll.vScale );
				if( !attrAll.vSize.IsMinus() )
					pWnd->SetSizeLocal( attrAll.vSize );
			}
			AddWndParent( pWnd, pParent, attrAll, i );
//			v += XE::VEC2( distx, disty );
			v += attrAll.vDist;
		}
	} else {
		// 한개생성
		XWndSprObj *pWnd = NULL;
		if( strSpr.empty() ) {
			pWnd = new XWndSprObj( vPos.x, vPos.y );
			pWnd->SetloopType( loopType );
		}	else
			pWnd = new XWndSprObj( strSpr.c_str(), (ID)idAct, vPos.x, vPos.y, loopType );
		if( pWnd ) {
			pWnd->SetRotateY( attrAll.vRot.y );
			pWnd->SetRotateZ( attrAll.vRot.z );
			pWnd->SetFlipHoriz( attrAll.m_bFlipX );
			pWnd->SetFlipVert( attrAll.m_bFlipY );
			pWnd->SetScaleLocal( attrAll.vScale );
			if( !attrAll.vSize.IsMinus() )
				pWnd->SetSizeLocal( attrAll.vSize );
		}
		return pWnd;
	}
	return nullptr;
}

/**
 팝업프레임
*/
XWnd* XLayout::CreatePopupFrame( const char *cCtrlName
																, TiXmlElement *elemCtrl
																, XWnd *pParent
																, const xATTR_ALL& attrAll )
{
	XE::xAlign align = attrAll.align;
	if( !align )
		align = XE::xALIGN_HCENTER;
	// size
	const auto strImg = attrAll.GetstrFile();
	// frame
	const char *cFrame = elemCtrl->Attribute( "frame" );
	_tstring strFrame;
	if( XE::IsHave( cFrame ) )
		strFrame = C2SZ( cFrame );
	// img
	bool bNcClose = true;
	GetAttrBool( elemCtrl, "nc_close", &bNcClose );
	//
	// popup의 경우는 다른 컨트롤과달리 부모가 XWndPopup이어야 한다.
	if( XBREAK( pParent->GetwtWnd() != XE::WT_POPUP) )
		return nullptr;
	auto pPopup = SafeCast<XWndPopup*>( pParent );
	if( XBREAK( pPopup == nullptr ) )
		return nullptr;
	if( strImg.empty() == false )	{
		// 팝업의 배경 이미지를 설정.
		pPopup->SetBgImg( strImg.c_str(), attrAll.m_Format );
	} else
	if( strFrame.empty() == false )	{
		pPopup->LoadRes( strFrame.c_str() );
	}
	pPopup->SetEnableNcEvent( xboolToBOOL(bNcClose) );
	// 팝업 하위의 레이아웃을 만든다.
	if( attrAll.strKey.empty() == false )
		pParent->SetstrIdentifier( attrAll.strKey.c_str() );
	CreateLayout( elemCtrl, pParent );
	return nullptr;
}

/**
 @brief 뷰 컨트롤
 (뷰랑 팝업의 차이가 뭐여? 버튼이 디폴트로 있는거? 모달창인거?)
*/
XWnd* XLayout::CreateViewFrame( const char *cCtrlName
																, TiXmlElement *elemCtrl
																, XWnd *pParent
																, const xATTR_ALL& _attrAll )
{
	xATTR_ALL attrAll = _attrAll;
	XE::xAlign align = attrAll.align;
	if( !align )
		align = XE::xALIGN_HCENTER;
	const auto strImg = attrAll.GetstrFile();
	// frame
	const char *cFrame = elemCtrl->Attribute( "frame" );
	_tstring strFrame;
	if( XE::IsHave( cFrame ) ) {
		strFrame = C2SZ( cFrame );
		attrAll.strFrame = strFrame;
	}
	bool bNcClose = true;
	GetAttrBool( elemCtrl, "nc_close", &bNcClose );
	// view의 경우는 다른 컨트롤과달리 부모가 XWndView이어야 한다.
// 	if( XBREAK( pParent->GetwtWnd() != XE::WT_VIEW) )
// 		return nullptr;
// 	auto pView = SafeCast<XWndView*>( pParent );
	XWndView* pView = nullptr;
	if( m_pDelegate )
		pView = m_pDelegate->DelegateLayoutCreateViewCtrl( pParent, attrAll );
	else {
		const auto v = attrAll.vPos;
		const auto size = attrAll.vSize;
		pView = new XWndView( v, size );
	}
	if( XBREAK( pView == nullptr ) )
		return nullptr;
	if( strImg.empty() == false )	{
		// 팝업의 배경 이미지를 설정.
		pView->SetBgImg( strImg.c_str(), attrAll.m_Format );
	} else
	if( strFrame.empty() == false )	{
		pView->LoadRes( strFrame.c_str() );
	}
	pView->SetbNcClose( bNcClose );
//		pView->SetEnableNcEvent( TRUE );
	// 하위 레이아웃을 만든다.
	if( attrAll.strKey.empty() == false )
		pParent->SetstrIdentifier( attrAll.strKey.c_str() );
//	CreateLayout( elemCtrl, pParent );
	return pView;
}

// XWndPopup* XLayout::CreateViewWnd( const xATTR_ALL& attr )
// {
// 	XE::VEC2 vPos = attr.vPos;
// 	XE::VEC2 vSize = attr.vSize;
// 	if( vSize.IsMinus() )
// 		vSize.Set(0);
// 	XWndPopup *pFrame = nullptr;
// 	if( attr.strFrame.empty() == false ) {
// 		pFrame = new XWndPopup( vPos, vSize, attr.strFrame.c_str() );
// 		pFrame->SetbModal( TRUE );
// 	} else
// 		if( attr.strImgUp.empty() == false ) {
// 			pFrame = new XWndPopup( vPos.x, vPos.y, attr.strImgUp.c_str() );
// 			pFrame->SetbModal( TRUE );
// 		}
// 	return pFrame;
// }

/**
 탭뷰  컨트롤 
*/
XWnd* XLayout::CreateTabViewCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll )
{
	xATTR_ALL attr;
	attr = attrAll;
	//
	GetAttrCommon( elemCtrl, &attr );
	int idStartTab = 0;
	elemCtrl->Attribute( "start_tab", &idStartTab );
	//
	XWndTab2 *pTabView = CreateTabViewWnd( attr );
	if( pTabView ) {
		pTabView->SetidStartTab( (ID)idStartTab );
		if( attr.align == XE::xALIGN_HCENTER ) {
			XE::VEC2 vLT = (pParent->GetSizeLocal() / 2.f) - (pTabView->GetSizeLocal() / 2.f);
			XE::VEC2 v = pTabView->GetPosLocal();
			v.x = vLT.x;
			pTabView->SetPosLocal( v );
			attr.vPos = v;
		}
	}
	return pTabView;
}

// 
XWndTab2* XLayout::CreateTabViewWnd( const xATTR_ALL& attr )
{
	XE::VEC2 vPos = attr.vPos;
//	XE::VEC2 vSize = attr.vSize;
	XWndTab2 *pTabView = NULL;
	if( attr.strFont.empty() )
		pTabView = new XWndTab2( vPos.x, vPos.y, attr.strImgUp.c_str(), FONT_SYSTEM, this );
	else
		pTabView = new XWndTab2( vPos.x, vPos.y, attr.strImgUp.c_str(), attr.strFont.c_str(), this );
	return pTabView;
}

int XLayout::AddTab( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll )
{
	XWndTab2 *pTabView = dynamic_cast<XWndTab2*>( pParent );
	if( pTabView == NULL )
	{
		CONSOLE( "node:%s: attached tab where non tabview", m_strNodeWnd.c_str() );
		return 0;
	}
	xATTR_ALL attrTab;
	attrTab = attrAll;
	GetAttrCommon( elemCtrl, &attrTab );
	//
	int idTab = 0;
	elemCtrl->Attribute( "idtab", &idTab );
	if( idTab == 0 )
		CONSOLE( "node:%s: idtab=0", m_strNodeWnd.c_str() );
	//
	_tstring strImgTabOn;
	_tstring strImgTabOff;
	const char *cImgTabOn = elemCtrl->Attribute( "img_tab_on" );
	if( XE::IsHave( cImgTabOn ) )
		strImgTabOn = C2SZ( cImgTabOn );
	const char *cImgTabOff = elemCtrl->Attribute( "img_tab_off" );
	if( XE::IsHave( cImgTabOff ) )
		strImgTabOff = C2SZ( cImgTabOff );
	std::string cstrLayoutKey;
	const char *cLayoutKey = elemCtrl->Attribute( "layout" );
	if( XE::IsHave( cLayoutKey ) )
		cstrLayoutKey = cLayoutKey;

	//
	pTabView->AddTab( idTab, 
						XTEXT(attrTab.idText), 
						strImgTabOn.c_str(), 
						strImgTabOff.c_str(),
						attrTab.vPos.x, attrTab.vPos.y,
						attrTab.vSize.w, attrTab.vSize.h, cLayoutKey );
	return 1;
}



/**
 버튼 컨트롤 생성함수.
*/
XWnd* XLayout::CreateButtonCtrl( const char *cCtrlName, TiXmlElement* /*elemCtrl*/, 
								XWnd *pParent,
								const xATTR_ALL& attrAll, 
//								const xATTR_BUTT& attrButt,
								int idxMulti )
{
	XWnd *pAddWnd = NULL;
	XE::VEC2 vPos = attrAll.vPos;		// 시작위치
	if( idxMulti > 0 )
		vPos += attrAll.vDist * (float)idxMulti;
	XWndButton* pBaseButt = nullptr;
	// sprite button
///< spr버튼은 당분간 지원하지 않는다. 버튼의 이미지파일을 "file"로 쓰는경우가 많아 spr과 구분이 안됨.
// 	if( attrAll.strSpr.empty() == false ) {	
// 		_tstring strSpr = attrAll.strSpr;
// 		{
// 			ID idActUp = attrAll.idActUp;
// 			ID idActDown = attrAll.idActDown;
// 			ID idActDisable = attrAll.idActDisable;
// 			if( idActUp == 0 ) {
// 				XALERT( "node:%s, act_up is 0", C2SZ( m_nodeWnd->Value() ) );
// 				return 0;
// 			}
// 			// layout-"butt"에서 "text" 를 아예 넣지 않은 경우 idText=-1이 된다.
// 			// "text 0"과 구분하기 위해 만들었다. 
// 			// 이런 쓰임은 XWndbuttString타입으로 생성을 시키고 싶으나 일단 텍스트는 없을때 이렇게 사용한다.
// 			// 사용자가 불편할수 있으니 버튼은 모두 TextString타입으로 생성시키는게 좋을듯 하다.
// 			if( attrAll.idText >= 0 && attrAll.idText != (ID)-1 ) {
// 				ID idText = attrAll.idText;
// 				XWndButtonString *pButt = NULL;
// 				if( attrAll.strFont.empty() ) {
// 					// 폰트가 지정되지 않았다면 시스템 폰트를 사용
// 					pButt = new XWndButtonString( vPos.x, vPos.y,
// 																				XTEXT( idText ),
// 																				attrAll.colText,
// 																				XE::GetMain()->GetSystemFontDat(),
// 																				strSpr.c_str(),
// 																				idActUp, idActDown, idActDisable );
// 					pBaseButt = pButt;
// 				} else {
// 					pButt = new XWndButtonString( vPos.x, vPos.y,
// 																				XTEXT( idText ),
// 																				attrAll.colText,
// 																				attrAll.strFont.c_str(),
// 																				attrAll.sizeFont,
// 																				attrAll.strSpr.c_str(),
// 																				idActUp, idActDown, idActDisable );
// 				}
// 				pButt->SetStyle( attrAll.style );
// 				pAddWnd = pButt;
// 				pBaseButt = pButt;
// 			} else {
// 				// 텍스트가 지정되지 않았다면 일반 이미지버튼.
// 				auto pButt = new XWndButton( vPos.x, vPos.y,
// 																			attrAll.strSpr.c_str(),
// 																			idActUp, idActDown, idActDisable );
// 				pAddWnd = pButt;
// 				pBaseButt = pButt;
// 			}
// 		} // not null spr
// 	} // strSpr
// 	else
	// image button
	if( attrAll.strImgUp.empty() == false ) {
		{
			if( attrAll.idText != 0xffffffff ) {
				XWndButtonString *pButtStr;
				const _tstring strFont = attrAll.strFont;
				pButtStr = new XWndButtonString( vPos.x, vPos.y,
																				XTEXT( attrAll.idText ),
																				attrAll.colText,
																				strFont.c_str(),
																				attrAll.sizeFont,
																				attrAll.strImgUp.c_str(),
																				attrAll.strImgDown.c_str(),
																				attrAll.strImgDisable.c_str() );
				pBaseButt = pButtStr;
				pButtStr->SetStyle( attrAll.style );
				pBaseButt = pButtStr;
			} else {
				pBaseButt = new XWndButton( vPos.x, vPos.y,
																attrAll.strImgUp.c_str(),
																attrAll.strImgDown.c_str(),
																attrAll.strImgDisable.c_str() );
			}
		}
		pAddWnd = pBaseButt;
	} else {
		// 파일이 지정되지 않았다면 투명버튼.
		pBaseButt = new XWndButton( vPos.x, vPos.y,
																attrAll.vSize.w, attrAll.vSize.h );
		pAddWnd = pBaseButt;
	}
	if( pAddWnd ) {
		pAddWnd->SetScaleLocal( attrAll.vScale );
		pAddWnd->SetAlphaLocal( attrAll.alpha );
		pAddWnd->SetblendFunc( attrAll.m_funcBlend );
	}
	return pAddWnd;
} // createButtonCtrl

/**
 라디오 버튼 컨트롤 생성함수.
*/
XWnd* XLayout::CreateRadioButtonCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, 
										XWnd *pParent,
										const ID idGroup,
										const xATTR_ALL& attrAll, 
										int idxMulti )
{
	XWnd *pAddWnd = NULL;
	XE::VEC2 vPos = attrAll.vPos;		// 시작위치
	if( idxMulti > 0 )
		vPos += (attrAll.vDist * attrAll.vScale) * (float)idxMulti;
	if( attrAll.strImgUp.empty() ) {
		CONSOLE("node=%s:radio_butt: img_up is null", m_strNodeWnd.c_str());
		return NULL;
	}
	// image button
	auto pButt = new XWndButtonRadio( idGroup, vPos );
	if( pButt ) {
		// up버튼 지정
		if( !attrAll.strImgUp.empty() ) {
			pButt->SetpSurface( 0, XE::MakePath(DIR_UI,attrAll.strImgUp) );
		} 
		if( attrAll.idActUp ) {
			XBREAK( attrAll.strSpr.empty() );
			pButt->SetSprButton( 0, attrAll.strSpr.c_str(), attrAll.idActUp );
		}
		// down버튼 지정
		if( !attrAll.strImgDown.empty() ) {
			pButt->SetpSurface( 1, XE::MakePath( DIR_UI, attrAll.strImgDown) );
		}
		if( attrAll.idActDown ) {
			XBREAK( attrAll.strSpr.empty() );
			pButt->SetSprButton( 1, attrAll.strSpr.c_str(), attrAll.idActDown );
		}
		// disable 버튼 지정
		if( !attrAll.strImgDisable.empty() ) {
			pButt->SetpSurface( 2, XE::MakePath( DIR_UI, attrAll.strImgDisable) );
		}
		if( attrAll.idActDisable ) {
			XBREAK( attrAll.strSpr.empty() );
			pButt->SetSprButton( 2, attrAll.strSpr.c_str(), attrAll.idActDisable );
		}
		if( pButt ) {
			pButt->SetScaleLocal( attrAll.vScale );
			pButt->SetAlphaLocal( attrAll.alpha );
			pButt->SetblendFunc( attrAll.m_funcBlend );
		}
	}

	pAddWnd = pButt;
	return pAddWnd;
} // createRadioButtonCtrl


XWnd *XLayout::CreateProgressBarCtrl( const char *cCtrlName, 
												TiXmlElement *elemCtrl, 
												XWnd *pParent, 
												const xATTR_ALL& attrAll )
{
	xATTR_ALL attr;
	attr = attrAll;			// attrAll로 먼저 넣고
	//
	XE::VEC2 vPos;
	XE::xAlign align = XE::xALIGN_LEFT;
	GetAttrPos( elemCtrl, "pos", &vPos, &align );
/*	if( align == XE::xALIGN_HCENTER )
	{
		XE::VEC2 vSize = pParent->GetSizeLocal();
		vPos.x = vSize.x / 2.f;
	} */
	// 
	const char *cImg = elemCtrl->Attribute( "img1" );
	_tstring strImg1;
	if( XE::IsHave( cImg ) )
		strImg1 = C2SZ( cImg );
	cImg = elemCtrl->Attribute( "img2" );
	_tstring strImg2;
	if( XE::IsHave( cImg ) )
		strImg2 = C2SZ( cImg );
	//
	double adjx=0, adjy=0;
	elemCtrl->Attribute( "adjx", &adjx );
	elemCtrl->Attribute( "adjy", &adjy );

	auto pBar = new XWndProgressBar( vPos.x, vPos.y, strImg1.c_str(), strImg2.c_str() );
	if( align == XE::xALIGN_HCENTER )
	{
		XE::VEC2 vLT = (pParent->GetSizeLocal() / 2.f) - (pBar->GetSizeLocal() / 2.f);
		pBar->SetPosLocal( vLT );
	}
	pBar->SetvAdjBar( XE::VEC2( (float)adjx, (float)adjy ) );

	return pBar;
}

XWnd *XLayout::CreateProgressBarCtrl2( const char *cCtrlName, 
												TiXmlElement *elemCtrl, 
												XWnd *pParent, 
												const xATTR_ALL& attrAll )
{
	//
//	XE::xAlign align = XE::xALIGN_LEFT;
	//
	auto pBar = new XWndProgressBar2( attrAll.vPos , attrAll.GetstrFile() );
	if( pBar ) {
		// "layer"들을 읽음.
		LoadLayer( pBar, elemCtrl );
	}
	auto bReverse = false;
	GetAttrBool( elemCtrl, "reverse", &bReverse );
	pBar->SetbReverse( bReverse );
	// align옵션에 맞춰 좌표 정렬
	AlignCtrl( cCtrlName, elemCtrl, pParent, pBar, attrAll );
	float lerp = 0.f;
	if( GetAttrFloat( elemCtrl, "lerp", &lerp ) )
		pBar->SetLerp( lerp );	
	
	return pBar;
}

/**
 @brief pCtrl윈도우를 attrAll의 align옵션에 맞춰 정렬한다.
*/
void XLayout::AlignCtrl( const char* cCtrlName
												, TiXmlElement* pElemCtrl
												, XWnd* pParent
												, XWnd* pCtrl
												, const xATTR_ALL& attrAll )
{
	pCtrl->AutoLayoutByAlign( pParent, attrAll.align );
}

/**
 @brief XWndProgressBar2의 "layer" 로더
*/
int XLayout::LoadLayer( XWndProgressBar2 *pBar, TiXmlElement *pRoot )
{
	auto pNode = pRoot->FirstChildElement();
	if( pNode == nullptr )
		return 0;
	int numLayer = 0;
	do {
		std::string strNodeName = pNode->Value();
		if( XE::IsSame( strNodeName.c_str(), "layer" ) ) {
// 			xATTR_ALL attr;
// 			GetAttrCommon( pNode, & attr );
			XE::VEC2 vAdj;
			GetAttrVector2( pNode, "adj", &vAdj );
			_tstring strFile;
			GetAttrFile( pNode, nullptr, &strFile );
			XBREAK( strFile.empty() );
			ID idWnd = 0;
			GetAttrWinID( pNode, &idWnd );
			bool bFixed = false;
			GetAttrBool( pNode, "fixed", &bFixed );
			bool bShow = true;
			GetAttrBool( pNode, "show", &bShow );
			XE::xtBlendFunc funcBlend = XE::xBF_NONE;
			GetAttrBlendFunc( pNode, &funcBlend );
			ID idAct = 1;
			GetAttridAct( pNode, &idAct );
			//
			auto pLayer = pBar->AddLayer( idWnd, strFile, bFixed );
			if( pLayer ) {
				pLayer->m_vAdj = vAdj;
				pLayer->m_bShow = bShow; //(attr.bShow != FALSE);
				pLayer->m_idAct = idAct;
				if( pLayer->m_psoTail && idAct ) {
					pLayer->m_psoTail->SetAction( idAct );
				}
				int idTail = 0;
				pNode->Attribute( "tail", &idTail );
				pLayer->m_idTraceTail = (int)idTail;
				if( funcBlend )
					pLayer->m_blendFunc	= funcBlend;
				bool bFlipH = false, bFlipV = false;
				if( !GetAttrBool( pNode, "flip", &bFlipH ) )
					GetAttrBool( pNode, "flip_x", &bFlipH );
				GetAttrBool( pNode, "flip_y", &bFlipV );
				pLayer->m_bFlipH = bFlipH;
				pLayer->m_bFlipV = bFlipV;
			}
				
			++numLayer;
		}
	} while( (pNode = pNode->NextSiblingElement()) );
	return numLayer;
}

XWnd *XLayout::CreateListCtrl( const char *cCtrlName, 
									TiXmlElement *elemCtrl, 
									XWnd *pParent, 
									const xATTR_ALL& attrAll )
{
	xATTR_ALL attr;
	attr = attrAll;			// attrAll로 먼저 넣고
	// position
	GetAttrPos( elemCtrl, "pos", &attr.vPos );
	// size
	XE::VEC2 vSize;
	GetAttrSize( elemCtrl, &attr.vSize );
	// list type
	const char *cType = elemCtrl->Attribute( "type" );
	XWndList::xtType type = XWndList::xLT_VERT;
	if( XE::IsHave( cType ) )
	{
		if( XE::IsSame( cType, "thumb" ) )
			type = XWndList::xLT_THUMB;
		else if( XE::IsSame( cType, "list_horiz" ) )	// 가로형 리스트
			type = XWndList::xLT_HORIZ;
		else if( XE::IsSame( cType, "list_vert" ) )		// 세로형 리스트
			type = XWndList::xLT_VERT;
		else if( XE::IsSame( cType, "list" ) )		// 세로형 리스트
			type = XWndList::xLT_VERT;
	}
	// 리스트간 간격
	int wSpace=0, hSpace=0;
	elemCtrl->Attribute( "spacew", &wSpace );
	elemCtrl->Attribute( "spaceh", &hSpace );
	// img
	_tstring strBg;
	GetAttrFile( elemCtrl, NULL, &strBg );
	//
	auto pWndList = new XWndList( attr.vPos,			// 좌상귀 좌표
																attr.vSize,			// 리스트영역 크기
																type );			// 리스트 타입
	pWndList->SetvSpace( XE::VEC2( wSpace, hSpace ) );
	// 리스트안에 들어갈 items
	int numItems = -1;
	elemCtrl->Attribute( "items_num", &numItems );
	if( numItems < 0 )
		numItems = 0;
		// 테스트때만 쓰기위해 0이면 객체를 생성하지 않는다.
		const char *cItems = elemCtrl->Attribute( "items_node" );
		if( XE::IsHave( cItems ) ) {
			TiXmlElement *pElemItem = GetElement( cItems );
			if( pElemItem ) {
				pWndList->SetstrItemLayoutForXML( cItems );		// 아이템 레이아웃 이름을 저장.
				if( numItems > 0 ) {
					// 개수가 지정되지 아낳으면 노드이름만 저장하고 객체를 생성하지 않음.
					for( int i = 0; i < numItems; ++i ) {
						XWnd *pItem = CreateXWindow( pElemItem );
						pWndList->AddItem( i + 1, pItem );
					}
				}
			} else {
				XBREAKF( 1, "not found %s", C2SZ(cItems) );
			}
		}
	{
		const char *cStr = elemCtrl->Attribute( "dir" );
		if( XE::IsHave( cStr ) ) {
			if( XE::IsSame( cStr, "vert" ) )
				pWndList->SetScrollVertOnly();
			else if( XE::IsSame( cStr, "horiz" ) )
				pWndList->SetScrollHorizOnly();
			else if( XE::IsSame( cStr, "all" ) )
				pWndList->SetScrollAllDir();
			else
				XBREAKF( 1, "경고:list의 dir토큰이 잘못되었습니다.: %s", C2SZ(cStr));
		}
	}
	XE::VEC2 sizeElemFixed;
	if( attr.vDist.w > 0 )
		sizeElemFixed.w = attr.vDist.w;
	if( attr.vDist.h > 0 )
		sizeElemFixed.h = attr.vDist.h;
	pWndList->SetsizeFixed( sizeElemFixed );
// 	if( dir == XE::xHORIZ )
// 		pWndList->SetScrollLockHoriz();
// 	else if( dir == XE::xVERT )
// 		pWndList->SetScrollLockVert();

	return pWndList;
}

/**
 슬라이드 다운 컨트롤
*/
XWnd *XLayout::CreateSlideDownCtrl( const char *cCtrlName, 
												TiXmlElement *elemCtrl, 
												XWnd *pParent, 
												const xATTR_ALL& attrAll )
{
	xATTR_ALL attr;
	attr = attrAll;			// attrAll로 먼저 넣고
	//
	GetAttrCommon( elemCtrl, &attr );
	//
	const char *cImgBody = elemCtrl->Attribute( "img_body" );
	if( XE::IsHave( cImgBody ) )
		attr.strImgUp = C2SZ( cImgBody );
	const char *cImgSlider = elemCtrl->Attribute( "img_slider" );
	if( XE::IsHave( cImgSlider ) )
		attr.strImgDown = C2SZ( cImgSlider );
	//
	int adj_slideY = 0;
	elemCtrl->Attribute( "adj_slidey", &adj_slideY );
	//
	std::string strSliderLayout;
	{
		const char *cAttr = elemCtrl->Attribute( "slider_layout" );
		if( XE::IsHave( cAttr ) )
			strSliderLayout = cAttr;
	}
	XE::VEC2 vPosSliderLayout;
	GetAttrPos( elemCtrl, "slider_layout_pos", &vPosSliderLayout );
	double secSlide = 0;
	elemCtrl->Attribute( "sec_slide", &secSlide );
	//
	XWndSlideDown *pSlide = CreateSlideDownWnd( attr );
	if( attr.idText )
		pSlide->SetTextBody( XTEXT( attr.idText ), attr.strFont.c_str(), attr.sizeFont, attr.colText, attr.style );
	pSlide->SetpLayout( this );
	pSlide->SetvAdjSlide( XE::VEC2( 0, adj_slideY ) );
	if( strSliderLayout.empty() == false )
		pSlide->SetstrSliderLayout( strSliderLayout );
	pSlide->SetvSliderLayout( vPosSliderLayout );
	if( secSlide > 0 )
		pSlide->SetsecSlide( (float)secSlide );
	return pSlide;
}

//
XWndSlideDown* XLayout::CreateSlideDownWnd( const xATTR_ALL& attr )
{
	XWndSlideDown *pSlide = new XWndSlideDown( attr.vPos.x, attr.vPos.y, attr.strImgUp.c_str(), attr.strImgDown.c_str() );
	return pSlide;
}

XWnd *XLayout::CreateEditBoxCtrl( const char *cCtrlName, 
								TiXmlElement *elemCtrl, 
								XWnd *pParent, 
								const xATTR_ALL& attrAll )
{
	xATTR_ALL attr;
	attr = attrAll;			// attrAll로 먼저 넣고
	// 일반적인 attribute를 읽어본다.
	GetAttrCommon( elemCtrl, &attr );
	//
	XE::xAlign align = XE::xALIGN_LEFT;
	float width = 0;
	GetAttrTextAlign( elemCtrl, &align, &width );
	//
	BOOL bPassword = FALSE;
	GetAttrBool( elemCtrl, "password", &bPassword );
	//
	const auto sizeParent = pParent->GetSizeLocalNoTrans();
	XWndEdit *pWndEdit = CreateEditBoxWnd( attr );
	if( pWndEdit )	{
		auto sizeEdit = pWndEdit->GetSizeLocalNoTrans();
		if( attrAll.vSize.w > 0 )
			sizeEdit.w = attrAll.vSize.w;
		else
			sizeEdit.w = sizeParent.w;
		if( attrAll.vSize.h > 0 )
			sizeEdit.h = attrAll.vSize.h;
		else
			sizeEdit.h = sizeParent.h;
		pWndEdit->SetSizeLocal( sizeEdit );
		pWndEdit->SetbPassword( bPassword );
		pWndEdit->SetAlign( align );
		if( width > 0 )
			pWndEdit->SetAlignWidth( width );
	}
	return pWndEdit;
}

XWndEdit* XLayout::CreateEditBoxWnd( const xATTR_ALL& attr )
{
	XWndEdit *pEdit;
	LPCTSTR szFont = attr.strFont.c_str();
	if( XE::IsEmpty(szFont) )
		szFont = FONT_SYSTEM;
	pEdit = new XWndEdit( attr.vPos, attr.vSize, szFont, attr.sizeFont, attr.colText );

	pEdit->SetvAdjPos( attr.vAdjust );		// 실제 좌표를 보정하는게 나을거 같아 바꿈.
	return pEdit;
}


XWnd *XLayout::CreateScrollViewCtrl( const char *cCtrlName, 
									TiXmlElement *elemCtrl, 
									XWnd *pParent, 
									const xATTR_ALL& attrAll )
{
	xATTR_ALL attr;
	attr = attrAll;			// attrAll로 먼저 넣고
	// 일반적인 attribute를 읽어본다.
	GetAttrCommon( elemCtrl, &attr );
	//
	XWndScrollView *pScrlView = CreateScrollViewWnd( attr );
	return pScrlView;
}

XWndScrollView* XLayout::CreateScrollViewWnd( const xATTR_ALL& attr )
{
	XWndScrollView *pScrlView =
		new XWndScrollView( attr.vPos, attr.vSize );
	if( attr.dir == XE::xVERT )
		pScrlView->SetScrollVertOnly();	// SetScrollDir( (XE::xtCrossDir)attr.dir );
	else if( attr.dir == XE::xHORIZ )
		pScrlView->SetScrollHorizOnly(); // SetScrollDir( (XE::xtCrossDir)attr.dir );
	else
		pScrlView->SetScrollAllDir();
	return pScrlView;
}


// Create Control
//////////////////////////////////////////////////////////////////////////
void XLayout::GetAttrAdj( TiXmlElement* pElemCtrl, XE::VEC2* pOut )
{
	double adjx = -9999.;
	double adjy = -9999.;
	pElemCtrl->Attribute( "adjx", &adjx );
	pElemCtrl->Attribute( "adjy", &adjy );
	if( adjx != -9999.)
		pOut->x = (float)adjx;
	if( adjy != -9999.)
		pOut->y = (float)adjy;
}
/**
 @brief 
 @param idx "adjx2"와 같은식으로 읽고 싶을때 인덱스값을 지정한다. 
*/
void XLayout::GetAttrVector2( TiXmlElement* pElemCtrl, const char* cHeader, XE::VEC2* pOut, int idx )
{
	if( cHeader == nullptr )
		return;
	double x = -9999.;
	double y = -9999.;
	std::string strKeyx = std::string(cHeader) + "x";
	std::string strKeyy = std::string(cHeader) + "y";
	if( idx > 0 ) {
		strKeyx += XE::Format("%d", idx );
		strKeyy += XE::Format("%d", idx );
	}
	pElemCtrl->Attribute( strKeyx.c_str(), &x );
	pElemCtrl->Attribute( strKeyy.c_str(), &y );
	// 실제로 값을 명시적으로 입력했을때만 넣는다. 디폴트값이 필요하다면 호출전에 이미 채워져 있어야 한다.
	if( x != -9999. )
		pOut->x = (float)x;
	if( y != -9999. )
		pOut->y = (float)y;
}
//////////////////////////////////////////////////////////////////////////
// GetAttribute
int XLayout::GetAttrCommon( TiXmlElement *elemCtrl, xATTR_ALL *pOut )
{
#ifdef _DEBUG
	int debug;
	elemCtrl->Attribute( "debug", &debug );
	if( debug ) {
		pOut->m_Debug = debug;
	}
#endif // _DEBUG
	// 윈도우 아이디 읽기
	GetAttrWinID( elemCtrl, &pOut->idWnd );
	// x, y좌표 읽음 혹은 정렬
	GetAttrPos( elemCtrl, "pos", &pOut->vPos, &pOut->align);
	// size
	GetAttrSize( elemCtrl, &pOut->vSize );
	if( pOut->vSize.w < 0 )
		pOut->vSize.w = 0;
	if( pOut->vSize.h < 0 )
		pOut->vSize.h = 0;
	// scale
	GetAttrScale( elemCtrl, &pOut->vScale );	
	//
	double alpha = 1.0;
	elemCtrl->Attribute( "alpha", &alpha );
	pOut->alpha = (float)alpha;
	GetAttrAdj( elemCtrl, &pOut->vAdjust );
	// 텍스트
	int idText = -1;		// -1은 "text"가 명시되지 않음. 일반 XWndButton()으로 생성되어야 함.
	elemCtrl->Attribute( "text", &idText );
	if( idText != -1 ) {
		// "text"가 명시됨
		if( idText > 0 && XE::IsEmpty( XTEXT(idText) ) ) {
			XALERT( "node=%s, text id %d not found", m_strNodeWnd.c_str(), idText );
		}
	}
	pOut->idText = idText;		// 디폴트값은 -1이되어야 함.
	// 컬러
	XCOLOR colText = 0;				// 디폴트 컬러
	GetAttrColor( elemCtrl, "text_col", &colText );	// 컬러 태그가 있으면 그값을 쓴다.
	if( colText == 0 )
		pOut->colText = XCOLOR_WHITE;
	else
		pOut->colText = colText;
	// 폰트
	GetAttrFont( elemCtrl, "font", &pOut->strFont, &pOut->sizeFont, &pOut->style );
	// align
	if( !pOut->align ) {
		GetAttrTextAlign( elemCtrl, &pOut->align, &pOut->lengthLine );
		pOut->lengthLine = pOut->vSize.w;
	}
	BOOL bFlag = FALSE;
	GetAttrBool( elemCtrl, "comma", &bFlag );
	if( bFlag )
		pOut->xfFlag |= xF_COMMA;
	else
		pOut->xfFlag &= ~xF_COMMA;
	// counter
	bFlag = FALSE;
	GetAttrBool( elemCtrl, "counter", &bFlag );
	if( bFlag )
		pOut->xfFlag |= xF_COUNTER;
	else
		pOut->xfFlag &= ~xF_COUNTER;
	// file
	
	const char *cImg = elemCtrl->Attribute( "file" );
	if( XE::IsHave( cImg ) ) {
		// "file"로 암시적으로 지정했다면 img와 spr양쪽에 다 넣는다.
		pOut->strImgUp = C2SZ( cImg );
		pOut->strSpr = pOut->strImgUp;
	} else {
		cImg = elemCtrl->Attribute( "img" );
		if( XE::IsHave( cImg ) ) {
			pOut->strImgUp = C2SZ( cImg );		// "img"로 명시를 했으면 ImgUp에만 넣어야 함.
		} else {
			cImg = elemCtrl->Attribute( "img_up" );
			if( XE::IsHave( cImg ) ) {
				pOut->strImgUp = C2SZ( cImg );		// "img"로 명시를 했으면 ImgUp에만 넣어야 함.
			} else {
			}
		}
	}
	// sprite(버튼이 이제 spr과 img를 함께 쓸수 있어 별도로 분리함. spr로 명시하면 spr에 읽음)
	cImg = elemCtrl->Attribute( "spr" );
	if( XE::IsHave( cImg ) ) {
		pOut->strSpr = C2SZ( cImg );		// "spr"로 명시를 했으면 ImgUp에만 넣어야 함.
	}

	pOut->idActUp = 0;
	pOut->idActDown = 0;
	pOut->idActDisable = 0;
	GetAttrDWORD( elemCtrl, "act_up", &pOut->idActUp );
	GetAttridAct( elemCtrl, &pOut->idActUp );
	GetAttrDWORD( elemCtrl, "act_down", &pOut->idActDown );
	GetAttrDWORD( elemCtrl, "act_off", &pOut->idActDisable );
//	XBREAK( pOut->idActUp == 0 );
	// button
	{
		const char *cImgDown = elemCtrl->Attribute( "img_down" );
		const char *cImgDisable = elemCtrl->Attribute( "img_off" );
		_tstring strImgDown, strImgDisable;
		if( XE::IsHave( cImgDown ) )
			strImgDown = C2SZ( cImgDown );
		if( XE::IsHave( cImgDisable ) )
			strImgDisable = C2SZ( cImgDisable );
		pOut->strImgDown = strImgDown;
		pOut->strImgDisable = strImgDisable;
	}
	// num sequence
	elemCtrl->Attribute( "num", &pOut->num );
	// dist
	GetAttrVector2( elemCtrl, "dist", &pOut->vDist );
	// sound
	{
		int idSnd = 0;
		elemCtrl->Attribute( "id_snd", &idSnd );
		if( idSnd == 0 )
			elemCtrl->Attribute( "id_sound", &idSnd );
		if( idSnd )
			pOut->idActUp = (ID)idSnd;
		const char *cBgm = NULL;
		cBgm = elemCtrl->Attribute( "bgm" );
		if( XE::IsHave( cBgm ) ) {
			pOut->SetstrFile( C2SZ( cBgm ) );
		}
	}
	// layout
	pOut->m_strcLayout = elemCtrl->Attribute( "layout" );
	// 방향
	{
		const char *cStr = elemCtrl->Attribute( "dir" );
		if( XE::IsHave( cStr ) )	{
			if( XE::IsSame( cStr, "vert" ) )
				pOut->dir = (BYTE)XE::xVERT;
			else if( XE::IsSame( cStr, "horiz" ) )
				pOut->dir = (BYTE)XE::xHORIZ; 
			else
				pOut->dir = (BYTE)XE::xVERT_HORIZ_ALL;
		}
	}	{
		const char *cKey = elemCtrl->Attribute( "key" );
		if( XE::IsHave( cKey ) )
			pOut->strKey = cKey;
	}	{
		const char *cEnable = elemCtrl->Attribute( "enable" );
		if( XE::IsHave( cEnable ) )
			if( XE::IsSame( cEnable, "off" ) || XE::IsSame( cEnable, "false" ) || XE::IsSame( cEnable, "0" ) || XE::IsSame( cEnable, "no" ))
				pOut->bEnable = FALSE;
	}	{
		const char *cShow = elemCtrl->Attribute( "show" );
		if( XE::IsHave( cShow ) )
			if( XE::IsSame( cShow, "off" ) || XE::IsSame( cShow, "false" ) || XE::IsSame( cShow, "0" ) || XE::IsSame( cShow, "no" ) )
				pOut->bShow = FALSE;
	}
	// blend func
	GetAttrBlendFunc( elemCtrl, &pOut->m_funcBlend );
	// rotate
//	int roty = 0, rotz = 0;
	double roty=0, rotz=0;
	elemCtrl->Attribute( "roty", &roty );
	elemCtrl->Attribute( "rotz", &rotz );
	pOut->vRot.y = (float)roty;
	pOut->vRot.z = (float)rotz;
	// flip
	if( !GetAttrBool( elemCtrl, "flip", &pOut->m_bFlipX ) )
		GetAttrBool( elemCtrl, "flip_x", &pOut->m_bFlipX );
	GetAttrBool( elemCtrl, "flip_y", &pOut->m_bFlipY );
	//
	{
		std::string strFormatSurface = elemCtrl->Attribute( "format_surface" );
		if( strFormatSurface.empty() )
			strFormatSurface = elemCtrl->Attribute( "format" );
		if( strFormatSurface.empty() )
			strFormatSurface = elemCtrl->Attribute( "surface_format" );
		auto formatSurface = XE::xPF_ARGB4444;
		if( !strFormatSurface.empty() ) {
			if( strFormatSurface == "argb8888" )
				formatSurface = XE::xPF_ARGB8888;
			else if( strFormatSurface == "argb4444" )
				formatSurface = XE::xPF_ARGB4444;
			else if( strFormatSurface == "argb1555" )
				formatSurface = XE::xPF_ARGB1555;
			else if( strFormatSurface == "rgb565" )
				formatSurface = XE::xPF_RGB565;
			else {
				XBREAKF( 1, "unknown format_surface:%s", C2SZ( strFormatSurface ) );
			}
		}
		pOut->m_Format = formatSurface;
	}
	BOOL bTouchable = FALSE;
	if( GetAttrBool( elemCtrl, "touchable", &bTouchable ) )
		pOut->m_Touchable = ( bTouchable ) ? 1 : 0;
	return 1;
} // GetAttrCommon

int XLayout::GetAttrColor( TiXmlElement *elemCtrl, const char *cAttrKey, XCOLOR *pOutColor )
{
	char cRgbKey[ 64 ];
	BOOL bRgba = FALSE;
	BYTE r=255, g=255, b=255, a=255;
	//
	strcpy_s( cRgbKey, cAttrKey );
	strcat_s( cRgbKey, "_r" );
	if( GetAttrRGBA( elemCtrl, cRgbKey, &r ) )
		bRgba = TRUE;
	//
	strcpy_s( cRgbKey, cAttrKey );
	strcat_s( cRgbKey, "_g" );
	if( GetAttrRGBA( elemCtrl, cRgbKey, &g ) )
		bRgba = TRUE;
	strcpy_s( cRgbKey, cAttrKey );
	strcat_s( cRgbKey, "_b" );
	if( GetAttrRGBA( elemCtrl, cRgbKey, &b ) )
		bRgba = TRUE;
	strcpy_s( cRgbKey, cAttrKey );
	strcat_s( cRgbKey, "_a" );
	if( GetAttrRGBA( elemCtrl, cRgbKey, &a ) )
		bRgba = TRUE;
	// r,g,b,a값은 colText값보다 우선한다.
	if( bRgba )
	{
		*pOutColor = XCOLOR_RGBA( r, g, b, a );
		return 1;
	}
	// 
	const char *cColor = elemCtrl->Attribute( cAttrKey );
	if( XE::IsEmpty(cColor) )	{ // 키를 못찾음.
		cColor = elemCtrl->Attribute( "color" );
		if( XE::IsEmpty(cColor) ) {
			cColor = elemCtrl->Attribute( "col" );
			if( XE::IsEmpty( cColor ) ) {
				return 0;
			}
		}
	}
	// 예약 색깔.
	*pOutColor = XCOLOR_WHITE;		// 디폴트 컬러
	if( XE::IsSame( cColor, "white" ) )
		*pOutColor = XCOLOR_WHITE;
	else if( XE::IsSame( cColor, "black" ) )
		*pOutColor = XCOLOR_BLACK;
	else if( XE::IsSame( cColor, "blue" ) )
		*pOutColor = XCOLOR_BLUE;
	else if( XE::IsSame( cColor, "red" ) )
		*pOutColor = XCOLOR_RED;
	else if( XE::IsSame( cColor, "green" ) )
		*pOutColor = XCOLOR_GREEN;
	else if( XE::IsSame( cColor, "yellow" ) )
		*pOutColor = XCOLOR_YELLOW;
	else if( XE::IsSame( cColor, "purple" ) )
		*pOutColor = XCOLOR_PURPLE;
	else if( XE::IsSame( cColor, "gray" ) )
		*pOutColor = XCOLOR_GRAY;
	else 
	if( XE::IsHave( cColor ) ) {
		// 상수에 있는지 찾아봄
		TiXmlElement *elemConst = GetElement( "constant" );
		if( elemConst ) {
			TiXmlElement *elemValue = elemConst->FirstChildElement( cColor );
			if( elemValue )
				return GetAttrColor( elemValue, cAttrKey, pOutColor );
		}
		// 부모에 있는지 찾아봄
		if( m_pParent ) {
			TiXmlElement *elemConst = m_pParent->GetElement( "constant" );
			if( elemConst ) {
				TiXmlElement *elemValue = elemConst->FirstChildElement( cColor );
				if( elemValue )
					return GetAttrColor( elemValue, cAttrKey, pOutColor );
			}
		}
	}
// 	// 상수에도 없으면 디폴트 컬러로 리턴.
// 	*pOutColor = XCOLOR_WHITE;
	return 0;
}

// RGBA각각의 바이트 값을 읽어서 pColElem에 담는다.
int XLayout::GetAttrRGBA( TiXmlElement *elemCtrl, const char *cAttrKey, BYTE *pColElem )
{
	int val = -1;
	elemCtrl->Attribute( cAttrKey, &val );
	*pColElem = (BYTE) val;
	return (val == -1)? 0 : 1;
}

int XLayout::GetAttrFont( TiXmlElement *elemCtrl, const char *cAttrKey, _tstring *pOutStr, float *pOutSize, xFONT::xtStyle *pOutStyle )
{
	const char *cFontKey = elemCtrl->Attribute( cAttrKey );
	if( XE::IsHave( cFontKey ) )
	{
		TiXmlElement *rootElem = m_rootNode->ToElement();
		const char *cFont = rootElem->Attribute( cFontKey );
		if( XE::IsEmpty( cFont ) )
		{
			if( m_pParent )
			{	// 부모가 있으면 부모쪽에도 찾아본다.
				rootElem = m_pParent->m_rootNode->ToElement();
				cFont = rootElem->Attribute( cFontKey );
				if( XE::IsEmpty( cFont ) )
				{
					XALERT( "node=%s, font:%s key not found", m_strNodeWnd.c_str(), C2SZ( cFontKey ) );
					return 0;
				}
			}
		}
		if( XE::IsEmpty(cFont) )
			cFont = "";
		*pOutStr = C2SZ( cFont );
	}
	char cKey[ 64 ];
	strcpy_s( cKey, cAttrKey );
	strcat_s( cKey, "_size" );
	int size = (int)FONT_SIZE_DEFAULT;
	elemCtrl->Attribute( cKey, &size );
	if( size )
		*pOutSize = (float) size;
	//
	strcpy_s( cKey, cAttrKey );
	strcat_s( cKey, "_style" );
	const char *cStyle = elemCtrl->Attribute( cKey );
	if( XE::IsEmpty(cStyle) )
		cStyle = elemCtrl->Attribute( "style" );
	if( XE::IsHave( cStyle ) )
	{
		if( XE::IsSame( cStyle, "stroke" ) )
			*pOutStyle = xFONT::xSTYLE_STROKE;
		else if( XE::IsSame( cStyle, "shadow" ) )
			*pOutStyle = xFONT::xSTYLE_SHADOW;
		else
			*pOutStyle = xFONT::xSTYLE_NORMAL;
	} else
		*pOutStyle = xFONT::xSTYLE_NORMAL;
	return 1;
}

int XLayout::GetAttrPos( TiXmlElement *elemCtrl, const char *cAttrKey, XE::VEC2 *pOut, XE::xAlign *pOutAlign )
{
	char cKey[ 64 ];
	strcpy_s( cKey, cAttrKey );
	strcat_s( cKey, "x" );
	int x=-0xffff, y=-0xffff;
	int align = 0;
	const char *cPosx = GetAttrValueI( elemCtrl, cKey, &x );
	if( cPosx ) {
		if( XE::IsSame( cPosx, "center" ) || XE::IsSame( cPosx, "hcenter" ) )
			align |= (int)XE::xALIGN_HCENTER;
		else if( XE::IsSame( cPosx, "right" ) )
			align |= (int)XE::xALIGN_RIGHT;
	}
	strcpy_s( cKey, cAttrKey );
	strcat_s( cKey, "y" );
	const char *cPosy = GetAttrValueI( elemCtrl, cKey, &y );
	if( XE::IsSame( cPosy, "center" ) || XE::IsSame( cPosy, "vcenter" ))
		align |= (int)XE::xALIGN_VCENTER;
	else if( XE::IsSame( cPosy, "bottom" ) )
		align |= (int)XE::xALIGN_BOTTOM;
	if( pOutAlign )
		*pOutAlign = (XE::xAlign)align;
	if( x == -0xffff && y == -0xffff )
		return 0;
	if( x != -0xffff )
		pOut->x = (float)x;
	if( y != -0xffff )
		pOut->y = (float)y;
	return 1;
}

int XLayout::GetAttrPos( TiXmlElement *elemCtrl, const char *cAttrKey, xATTR_ALL *pOut )
{
	return GetAttrPos( elemCtrl, cAttrKey, &pOut->vPos, &pOut->align );
}

int XLayout::GetAttrSize( TiXmlElement *elemCtrl, XE::VEC2 *pOut )
{
	int w=-1, h=-1;
	GetAttrValueI( elemCtrl, "width", &w );
	GetAttrValueI( elemCtrl, "height", &h );
	if( w < 0 && h < 0 )
		return 0;
	if( w != -1 )
		pOut->w = (float)w;
	if( h != -1 )
		pOut->h = (float)h;
	return 1;
}

int XLayout::GetAttrScale( TiXmlElement *elemCtrl, XE::VEC2 *pOut )
{
	double scale = 1.0;
	elemCtrl->Attribute( "scale", &scale );
	*pOut = XE::VEC2( (float)scale, (float)scale );
	double scaley, scalex;
	scalex = -1.0;
	scaley = -1.0;
	elemCtrl->Attribute( "scale_x", &scalex );
	if( scalex == -1.0 )
		elemCtrl->Attribute( "scalex", &scalex );
	elemCtrl->Attribute( "scale_y", &scaley );
	if( scaley == -1.0 )
		elemCtrl->Attribute( "scaley", &scaley );
	if( scalex > 0 )
		pOut->x = (float)scalex;
	if( scaley > 0 )
		pOut->y = (float)scaley;
	return 1;
}
int XLayout::GetAttrText( TiXmlElement *elemCtrl, const char *cAttrKey, ID *pOut )
{
	int idText = -1;
	elemCtrl->Attribute( cAttrKey, &idText );
	if( idText != -1 )	{
		if( idText > 0 && XE::IsEmpty( XTEXT(idText) ) )	{
			XALERT( "node=%s, text id %d not found", m_strNodeWnd.c_str(), idText );
			return 0;
		}
		*pOut = idText;
		return 1;
	}
	return 0;
}

int XLayout::GetAttrTextAlign( TiXmlElement *elemCtrl, XE::xAlign *pOut, float *pOutWidth )
{
	const char *cAlign = elemCtrl->Attribute( "align" );
	if( XE::IsEmpty(cAlign) )
		return 0;
	if( XE::IsSame( cAlign, "center" ) )
		*pOut = XE::xALIGN_CENTER;
	else
	if( XE::IsSame( cAlign, "hcenter" ) )
		*pOut = XE::xALIGN_HCENTER;
	else
	if( XE::IsSame( cAlign, "vcenter" ) )
		*pOut = XE::xALIGN_VCENTER;
	else
	if( XE::IsSame( cAlign, "right" ) )
		*pOut = XE::xALIGN_RIGHT;
	else
		*pOut = XE::xALIGN_LEFT;
// 	int width = 0;
// 	elemCtrl->Attribute( "width", &width );
// 	if( width )
// 		*pOutWidth = (float) width;

	return 1;
}

int XLayout::GetAttrBool( TiXmlElement *elemCtrl, const char *cKey, BOOL *pOut )
{
	XBREAK( pOut == nullptr );
	const char *cBool = elemCtrl->Attribute( cKey );
	BOOL bRet = FALSE;
	if( XE::IsHave( cBool ) ) {
		if( XE::IsSame( cBool, "off" ) 
			|| XE::IsSame( cBool, "false" ) 
			|| XE::IsSame( cBool, "0" ) 
			|| XE::IsSame( cBool, "no" )) {
			*pOut = FALSE;
			bRet = TRUE;
		} else
		if( XE::IsSame( cBool, "on" ) 
			|| XE::IsSame( cBool, "true" ) 
			|| XE::IsSame( cBool, "1" ) 
			|| XE::IsSame( cBool, "yes" )) {
			*pOut = TRUE;
			bRet = TRUE;
		}
	}
	return bRet;
}

/**
 @brief 리턴값은 실제 값을 읽었는지 생략되었는지를 나타내는것에 주의
*/
bool XLayout::GetAttrBool( TiXmlElement *elemCtrl, const char *cKey, bool *pOut ) 
{
	XBREAK( pOut == nullptr );
	BOOL bFlag = FALSE;
	// 값이 생략되어있으면 pOut에 값을 바꾸지 않는다
	if( GetAttrBool( elemCtrl, cKey, &bFlag ) ) {
		*pOut = ( bFlag != FALSE );
		return true;
	}
	return false;
}

bool XLayout::GetAttrDWORD( TiXmlElement *elemCtrl, const char* cKey, DWORD *pOut )
{
	int val = 0x7fffffff;
	elemCtrl->Attribute( cKey, &val );
	if( val != 0x7fffffff ) {
		if( pOut ) {
			*pOut = (DWORD)val;
		}
		return true;
	}
	return false;
}

bool XLayout::GetAttrInt( TiXmlElement *elemCtrl, const char* cKey, int *pOut )
{
	int val = 0x7fffffff;
	elemCtrl->Attribute( cKey, &val );
	if( val != 0x7fffffff ) {
		if( pOut ) {
			*pOut = val;
		}
		return true;
	}
	return false;
}

bool XLayout::GetAttrFloat( TiXmlElement *elemCtrl, const char* cKey, float *pOut )
{
	double val = 9999.0;
	elemCtrl->Attribute( cKey, &val );
	if( val != 9999.0 ) {
		if( pOut ) {
			*pOut = (float)val;
		}
		return true;
	}
	return false;
}


int XLayout::GetAttrWinID( TiXmlElement *elemCtrl, ID *pOut )
{
	// 윈도우 아이디
	int idWnd = 0;
	elemCtrl->Attribute( "id", &idWnd );
	if( pOut )
		*pOut = idWnd;
	return 0;
}

int XLayout::GetAttrFile( TiXmlElement *elemCtrl, const char *cAttrKey, _tstring *pOutStr )
{
	if( XE::IsHave( cAttrKey ) ) {
		// 지정된 키가 있으면 그걸로 찾고 끝낸다.
		const char *cFile = elemCtrl->Attribute( cAttrKey );
		if( XE::IsHave( cFile ) ) {
			*pOutStr = C2SZ( cFile );
			return 1;
		}
		return 0;
	}
	// 지정된 키가 없으면 예약된 키들로 찾는다.
	const char *cImg = elemCtrl->Attribute( "file" );
	if( XE::IsHave( cImg ) ) {
		*pOutStr = C2SZ( cImg );
		return 1;
	} else {
		cImg = elemCtrl->Attribute( "img" );
		if( XE::IsHave( cImg ) ) {
			*pOutStr = C2SZ( cImg );		// "img"로 명시를 했으면 ImgUp에만 넣어야 함.
			return 1;
		} else {
			cImg = elemCtrl->Attribute( "img_up" );
			if( XE::IsHave( cImg ) ) {
				*pOutStr = C2SZ( cImg );		// "img"로 명시를 했으면 ImgUp에만 넣어야 함.
				return 1;
			} else {
				cImg = elemCtrl->Attribute( "spr" );
				if( XE::IsHave( cImg ) ) {
					*pOutStr = C2SZ( cImg );		// "spr"로 명시를 했으면 ImgUp에만 넣어야 함.
					return 1;
				}
			}
		}
	}
// 	const char *cFile = elemCtrl->Attribute( "file" );
// 	if( XE::IsHave( cFile ) ) {
// 		*pOutStr = C2SZ( cFile );
// 		return 1;
// 	}
// 	cFile = elemCtrl->Attribute( "img" );
// 	if( XE::IsHave( cFile ) ) {
// 		*pOutStr = C2SZ( cFile );
// 		return 1;
// 	}
	return 0;
}

bool XLayout::GetAttrBlendFunc( TiXmlElement* pElemCtrl, XE::xtBlendFunc* pOut, const std::string& strKey )
{
	const char *cStr = pElemCtrl->Attribute( (strKey.empty())? "blend" : strKey.c_str() );
	if( XE::IsHave(cStr) ) {
		const std::string strBlend = cStr;
		if( strBlend == "normal" )
			*pOut = XE::xBF_MULTIPLY;		// 앞으로는 multiply가 아니고 normal이 일반찍기가 됨
		else
		if( strBlend == "multiply" )
			*pOut = XE::xBF_MULTIPLY;		// multiply속성은 앞으로 normal과 다름.
		else
		if( strBlend == "add" )
			*pOut = XE::xBF_ADD;
		else
		if( strBlend == "sub" )
			*pOut = XE::xBF_SUBTRACT;
		else
		if( strBlend == "gray" )
			*pOut = XE::xBF_GRAY;
		//
		return true;
	}
	return false;
}

bool XLayout::GetAttridAct( TiXmlElement* pElemCtrl, ID* pOut )
{
	int idAct = 0;
	pElemCtrl->Attribute( "act", &idAct );
	if( idAct == 0 )
		pElemCtrl->Attribute( "idact", &idAct );
	if( idAct == 0 )
		pElemCtrl->Attribute( "id_act", &idAct );
	if( idAct )
		*pOut = (ID)idAct;
	return idAct != 0;
}
/**
 버튼을 위한 어트리뷰트를 읽어 구조체에 담아준다. 
*/
// int XLayout::GetAttrCtrlButton( TiXmlElement *elemCtrl, xATTR_BUTT *pOut )
// {
// 	// x, y좌표 읽음 혹은 정렬
// 	int x=0, y=0;
// 	DWORD xfFlag = 0;
// 	const char *cPosx = elemCtrl->Attribute( "posx" );
// 	const char *cPosy = elemCtrl->Attribute( "posy" );
// 	if( XE::IsSame( cPosx, "center" ) )
// 		xfFlag |= xfALIGN_HCENTER;
// 	else if( XE::IsSame( cPosx, "right" ) )
// 		xfFlag |= xfALIGN_RIGHT;
// 	else
// 		GetAttrValueI( elemCtrl, "posx", &x );
// //		elemCtrl->Attribute( "posx", &x );
// 	if( XE::IsSame( cPosy, "center" ) )
// 		xfFlag |= xfALIGN_VCENTER;
// 	else if( XE::IsSame( cPosy, "bottom" ) )
// 		xfFlag |= xfALIGN_BOTTOM;
// 	else
// 		GetAttrValueI( elemCtrl, "posy", &y );
// //		elemCtrl->Attribute( "posy", &y );
// 	pOut->vPos.x = (float) x;
// 	pOut->vPos.y = (float) y;
// 	pOut->xfFlag = xfFlag;
// 	// size
// 	GetAttrSize( elemCtrl, &pOut->vSize );
// 	// 텍스트
// 	int idText = -1;		// Layout에 "text 아이디" 를 아예 쓰지 않은 경우
// 	elemCtrl->Attribute( "text", &idText );
// 	if( idText )
// 		if( idText != -1 && XE::IsEmpty( XTEXT(idText) ) )
// 			XALERT( "node=%s, text id %d not found", m_strNodeWnd.c_str(), idText );
// 	pOut->idText = (ID)idText;
// 	// 컬러
// 	XCOLOR colText = XCOLOR_WHITE;				// 디폴트 컬러
// 	GetAttrColor( elemCtrl, "text_col", &colText );	// 컬러 태그가 있으면 그값을 쓴다.
// //	GetAttrColor( elemCtrl, "col", &colText );	// 컬러 태그가 있으면 그값을 쓴다.
// 	pOut->colText = colText;
// 	// 폰트
// 	_tstring strFont;
// 	float sizeFont = 20;
// 	xFONT::xtStyle style = xFONT::xSTYLE_NORMAL;
// 	GetAttrFont( elemCtrl, "font", &strFont, &sizeFont, &style );
// 	pOut->strFont = strFont;
// 	pOut->sizeFont = sizeFont;
// 	pOut->style = style;
// 	//
// 	// sprite button
// 	const char *cSpr = elemCtrl->Attribute( "spr" );
// 	if( XE::IsHave( cSpr ) )
// 	{
// 		_tstring strSpr = C2SZ( cSpr );
// 		if( XE::IsEmpty( XE::GetFileExt( strSpr.c_str() ) ) )		// 확장자가 안붙어 있으면 붙인다.
// 			strSpr += _T(".spr");
// 		int idActUp=0, idActDown=0, idActDisable=0;
// 		elemCtrl->Attribute( "act_up", &idActUp );
// 		elemCtrl->Attribute( "act_down", &idActDown );
// 		elemCtrl->Attribute( "act_off", &idActDisable );
// 		pOut->strSpr = strSpr;
// 		pOut->idActUp = idActUp;
// 		pOut->idActDown = idActDown;
// 		pOut->idActDisable = idActDisable;
// 	}
// 	// image button
// 	const char *cImgUp = elemCtrl->Attribute( "img_up" );
// 	if( XE::IsEmpty(cImgUp) )
// 		cImgUp = elemCtrl->Attribute( "file" );
// 	if( XE::IsHave( cImgUp ) )
// 	{
// 		const char *cImgDown = elemCtrl->Attribute( "img_down" );
// 		const char *cImgDisable = elemCtrl->Attribute( "img_off" );
// 		_tstring strImgUp, strImgDown, strImgDisable;
// 		strImgUp = C2SZ( cImgUp );
// 		if( XE::IsHave( cImgDown ) )
// 			strImgDown = C2SZ( cImgDown );
// 		if( XE::IsHave( cImgDisable ) )
// 			strImgDisable = C2SZ( cImgDisable );
// 		pOut->strImgUp = strImgUp;
// 		pOut->strImgDown = strImgDown;
// 		pOut->strImgDisable = strImgDisable;
// 	}
// 	// num sequence
// 	elemCtrl->Attribute( "num", &pOut->num );
// 	// dist
// 	{
// 		int distx=0, disty=0;
// 		elemCtrl->Attribute( "distx", &distx );
// 		elemCtrl->Attribute( "disty", &disty );
// 		if( distx )
// 			pOut->vDist.x = (float)distx;
// 		if( disty )
// 			pOut->vDist.y = (float)disty;
// 	}
// 
// 	return 1;
// }

int XLayout::GetAttrNum( TiXmlElement *elemCtrl )
{
	int num = 0;
	elemCtrl->Attribute( "num", &num );
	return num;
}

XE::VEC2 XLayout::GetAttrDist( TiXmlElement *elemCtrl )
{
	int distx=0, disty=0;
	elemCtrl->Attribute( "distx", &distx );
	elemCtrl->Attribute( "disty", &disty );
	XE::VEC2 vDist;
	if( distx )
		vDist.x = (float)distx;
	if( disty )
		vDist.y = (float)disty;
	return vDist;
}
//////////////////////////////////////////////////////////////////////////
XWnd* XLayout::GetNextCtrlFromKey( XWnd *pRoot, const char *cKey, int *pOutIdx, char *pOutSeqKey, int lenSeqKey )
{
	char cKeyBuff[ 256 ];
	sprintf_s( cKeyBuff, "%s%d", cKey, *pOutIdx );
	XWnd *pWnd = pRoot->Find( cKeyBuff );
	strcpy_s( pOutSeqKey, lenSeqKey, cKeyBuff );
	++(*pOutIdx);
	return pWnd;
}

// 어트리뷰트의 수식형 값을 실수형으로 읽음
const char* XLayout::GetAttrValueF( TiXmlElement *elem, const char *cKey, float *pOut )
{
	const char *cStr = elem->Attribute( cKey );
	if( cStr ){
		BOOL bRead = FALSE;
		// 첫문자가 숫자나 +-가 아니면 문자열상수로 본다
		if( _tcschr(_T("-+0123456789"), cStr[0]) == NULL )	{
//			if( strchr( cStr, '-' ) || strchr( cStr, '+' ) || strchr( cStr, '*' ) || strchr( cStr, '/' ))
			{
				CToken token;
				token.LoadStr( C2SZ(cStr) );
				token.GetToken();
				float valConst = 0;
				if( token == _T("scr_height") ) {
					valConst = GRAPHICS->GetLogicalScreenSize().h;
					bRead = TRUE;
				} else
				if( token == _T("scr_width") ) {
					valConst = GRAPHICS->GetLogicalScreenSize().w;
					bRead = TRUE;
				} else
				if( token == _T("game_height") ) {
					valConst = XE::GetGameHeight();
					bRead = TRUE;
				} else
				if( token == _T("game_width") ) {
					valConst = XE::GetGameWidth();
					bRead = TRUE;
				}
				*pOut = valConst;	// 현재까지의 결과를 저장
				if( token.GetToken() ) {	// +,-,/,*를 읽음
					if( _tcschr(_T("+-*/"), token.m_Token[0]) )	{ // 사칙연산 기호가 있느냐
						switch( token.m_Token[0] ) {
						case '+':	valConst += token.GetNumberF();	break;
						case '-':	valConst -= token.GetNumberF();	break;
						case '*':	valConst *= token.GetNumberF();	break;
						case '/':	valConst /= token.GetNumberF();	break;
						}
						*pOut = valConst;
					}
					bRead = TRUE;
				}
			}
		}
		if( bRead == FALSE ) {
			double dVal = -0xffff;
			elem->Attribute( cKey, &dVal );
			if( dVal != -0xffff )
				*pOut = (float)dVal;
		}
	} else {
		cStr = "";
	}

		
	return cStr;
}

// 어트리뷰트의 수식형 값을 정수형으로 읽음
const char* XLayout::GetAttrValueI( TiXmlElement *elem, const char *cKey, int *pOut )
{
	float val = (float)*pOut;
	const char *cStr = GetAttrValueF( elem, cKey, &val );
	*pOut = (int)val;
	return cStr;
}

//////////////////////////////////////////////////////////////////////////

XWndView* xGET_VIEW_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndView*, XWnd*>( pWnd );
}


XWndScrollView* xGET_SCROLLVIEW_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndScrollView*, XWnd*>( pWnd );
}

//XWndList* xGET_LIST_CTRL( XWnd *pRoot, const char *cKey );

XWndImage* xGET_IMAGE_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndImage*>( pWnd );
}

XWndImage* xGET_IMAGE_CTRLF( XWnd *pRoot, const char *cKeyFormat, ... ) {
	XBREAK( pRoot == nullptr );
	char cKey[256];
	va_list vl;
	va_start( vl, cKeyFormat );
	vsprintf_s( cKey, cKeyFormat, vl );
	va_end( vl );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndImage*, XWnd*>( pWnd );
}

XWndSprObj* xGET_SPROBJ_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndSprObj*>( pWnd );
}
XWndSprObj* xGET_SPROBJ_CTRLF( XWnd *pRoot, const char *cKeyFormat, ... ) {
	XBREAK( pRoot == nullptr );
	char cKey[256];
	va_list vl;
	va_start( vl, cKeyFormat );
	vsprintf_s( cKey, cKeyFormat, vl );
	va_end( vl );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndSprObj*>( pWnd );
}

XWndPopup* xGET_POPUP_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndPopup*, XWnd*>( pWnd );
}

XWnd* XLayout::CreateCustomControl( const char *cCtrlName
																	, TiXmlElement *elemCtrl
																	, XWnd *pParent
																	, const xATTR_ALL& attrAll ) 
{
	if( s_pDelegateMain )
		return s_pDelegateMain->DelegateCreateCustomCtrl( std::string(cCtrlName), elemCtrl, pParent, attrAll );
	return nullptr;
}

