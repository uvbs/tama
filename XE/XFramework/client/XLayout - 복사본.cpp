#include "stdafx.h"
#include "XLayout.h"
#include "XWindow.h"
//#include "XWindow.h"
#include "XConstant.h"
#include "XClientMain.h"
#include "etc/xLang.h"
#include "XAutoPtr.h"
#include "XSoundMng.h"



//////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XLayout* XLayout::s_pMain = nullptr;		// 메인 레이아웃. 

//////////////////////////////////////////////////////////////////////////
XLayout* XLayout::sCreateMain( LPCTSTR szXml )
{
	XBREAK( s_pMain != nullptr );
	s_pMain = new XLayout( szXml, nullptr, nullptr );
	return s_pMain;
}
XLayout::XLayout( LPCTSTR szXml, XBaseDelegate *pDelegate, XLayout *pParent ) 
{ 
	Init(); 
	m_pDelegate = pDelegate;
	m_pParent = pParent;
	TCHAR szFullpath[ 1024 ];
	// szRes의 풀패스를 얻어낸다.
//#ifdef _VER_ANDROID
	{
		_tstring strPath = XE::MakePathLang( DIR_LAYOUT, szXml );
//		XTRACE("Layout:LangPath:%s", strPath.c_str() );
		if( XE::IsExistFileInPackage( strPath.c_str() ) )		{
//			XTRACE("Layout:exist lang layout" );
			XE::SetReadyRes( szFullpath, XE::MakePathLang( DIR_LAYOUT, szXml ) );
		} else		{
//			XTRACE("Layout:is not exist lang layout" );
			XE::SetReadyRes( szFullpath, XE::MakePath( DIR_LAYOUT, szXml ) );
		}
	}
	// 각 언어폴더의 레이아웃 파일을 먼저 준비시켜보고 실패하면 루트폴더의 파일을 준비시킨다.
/*
	BOOL bResult = XE::SetReadyRes( szFullpath, XE::MakePathLang( DIR_SCRIPTW, szXml ) );
	if( bResult == FALSE )
		XE::SetReadyRes( szFullpath, XE::MakePath( DIR_SCRIPTW, szXml ) );
*/

//#else
//	XE::SetReadyRes( szFullpath, XE::MakePath( DIR_SCRIPTW, szXml ) );
//#endif
	XBREAK( XE::IsEmpty( szFullpath ) == TRUE );
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
	XLP1;
	CONSOLE("layout create: key=%s", C2SZ(cKeyWnd) );
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
	XLP2;
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
//	XTRACE( "CreateControl: %s", C2SZ( cCtrlName ) );
	//
	XWnd *pAddWnd = NULL;
	xATTR_ALL attrAll;
	GetAttrCommon( elemCtrl, &attrAll );
	// idWnd
//	ID idWnd = 0;
#ifdef _DEBUG
	{
		const char *cVal = elemCtrl->Attribute( "debug" );
		if( XE::IsHave(cVal) ) {
			int a = 0;
		}
	}
#endif // _DEBUG
	int nTouchable = -1;
	GetAttrWinID( elemCtrl, &attrAll.idWnd );
	{
		BOOL bTouchable = FALSE;
		if( GetAttrBool( elemCtrl, "touchable", &bTouchable ) )
			nTouchable = ( bTouchable ) ? 1 : 0;
		GetAttrPos( elemCtrl, "pos", &attrAll );
	}
	// 윈도우 key문자열
//	std::string strKey;
	{
		const char *cKey = elemCtrl->Attribute( "key" );
		if( XE::IsHave( cKey ) )
			attrAll.strKey = cKey;
	}
	//
//	BOOL bEnable = TRUE;
	{
		const char *cEnable = elemCtrl->Attribute( "enable" );
		if( XE::IsHave( cEnable ) )
			if( XE::IsSame( cEnable, "off" ) || XE::IsSame( cEnable, "false" ) || XE::IsSame( cEnable, "0" ) || XE::IsSame( cEnable, "no" ))
				attrAll.bEnable = FALSE;
	}
	//
//	BOOL bShow = TRUE;
	{
		const char *cShow = elemCtrl->Attribute( "show" );
		if( XE::IsHave( cShow ) )
			if( XE::IsSame( cShow, "off" ) || XE::IsSame( cShow, "false" ) || XE::IsSame( cShow, "0" ) || XE::IsSame( cShow, "no" ))
				attrAll.bShow = FALSE;
	}
	{
		// num sequence
		int num = 0;
		elemCtrl->Attribute( "num", &attrAll.num );
		// dist
		int distx=0, disty=0;
		elemCtrl->Attribute( "distx", &distx );
		elemCtrl->Attribute( "disty", &disty );
		if( distx )
			attrAll.vDist.x = (float) distx;
		if( disty )
			attrAll.vDist.y = (float) disty;
	}
	GetAttrScale( elemCtrl, &attrAll.vScale );
	GetAttrSize( elemCtrl, &attrAll.vSize );
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
		xATTR_BUTT attrButt;
		GetAttrCtrlButton( elemCtrl, &attrButt );
		if( attrButt.num > 1 ) {
			// 여러개 생성
			for( int i = 0; i < attrButt.num; ++i ) {
				XWnd *pButt = CreateButtonCtrl( cCtrlName, elemCtrl, pParent, attrAll, attrButt, i );
				if( pButt ) {
					CreateLayout( elemCtrl, pButt );
					pButt->OnFinishCreatedChildLayout( this );
					if( pParent )
						AddWndParent( pButt, pParent, attrAll, i );
				}
			}
		} else
			pAddWnd = CreateButtonCtrl( cCtrlName, elemCtrl, pParent, attrAll, attrButt );
	} // butt
	else
	if( XE::IsSame( cCtrlName, "radio_butt" ) ) {
		xATTR_BUTT attrButt;
		GetAttrCtrlButton( elemCtrl, &attrButt );
		int idGroup = 0;
		elemCtrl->Attribute( "group", &idGroup );
		if( XBREAK(idGroup == 0) )
			CONSOLE("node=%s:radio_butt:group is zero", szWndNode);
		//
		if( attrButt.num > 1 ) {
			// 여러개 생성
			for( int i = 0; i < attrButt.num; ++i ) {
				XWnd *pButt = CreateRadioButtonCtrl( cCtrlName, elemCtrl, pParent, (ID)idGroup, attrAll, attrButt, i );
				AddWndParent( pButt, pParent, attrAll, i );
			}
		} else
			pAddWnd = CreateRadioButtonCtrl( cCtrlName, elemCtrl, pParent, (ID)idGroup, attrAll, attrButt );
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
	if( XE::IsSame( cCtrlName, "tabview" ) ) {
		pAddWnd = CreateTabViewCtrl( cCtrlName, elemCtrl, pParent, attrAll );
	} else	
	if( XE::IsSame( cCtrlName, "tab" ) ) {
		AddTab( cCtrlName, elemCtrl, pParent, attrAll );
	} else
	if( XE::IsSame( cCtrlName, "wnd" ) ) {
		xATTR_ALL attrWnd;
		GetAttrCommon( elemCtrl, &attrWnd );
		if( attrWnd.num > 1 ) {
			// 여러개 생성
			XE::VEC2 v = attrWnd.vPos;
			for( int i = 0; i < attrWnd.num; ++i ) {
				XWnd *pWnd = new XWnd( v, attrWnd.vSize );
				if( pWnd ) {
					pWnd->SetScaleLocal( attrWnd.vScale );
					// 버추얼 커스텀에게 맡겼는데도 모르는게 나왔다면 이 노드를 키로 다시 리커시브로 들어가본다.
					CreateLayout( elemCtrl, pWnd );
					// pAddWnd의 자식들이 모두 생성이 끝나고 핸들러가 호출된다.
					pWnd->OnFinishCreatedChildLayout( this );
					if( pParent )
						AddWndParent( pWnd, pParent, attrWnd, i );
					v += XE::VEC2(attrWnd.vDist);
				}
			}
		} else {
	 		pAddWnd = new XWnd( attrWnd.vPos, attrWnd.vSize );
			pAddWnd->SetScaleLocal( attrWnd.vScale );
		}
	} else
	if( XE::IsSame( cCtrlName, "rect" ) ) {
		XE::VEC2 vPos;
		XE::xAlign alignpos =XE::xALIGN_LEFT;;
		GetAttrPos( elemCtrl, "pos", &vPos, &alignpos );
		//
		XE::VEC2 vSize;
		GetAttrSize( elemCtrl, &vSize );
		//
		XCOLOR col = 0;
		GetAttrColor( elemCtrl, "col", &col );
		if( col == 0 )
			GetAttrColor( elemCtrl, "color", &col );
		//
		pAddWnd = new XWndRect( vPos.x, vPos.y, vSize.w, vSize.h, col );
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
		xATTR_ALL attrSnd;
		GetAttrCommon( elemCtrl, &attrSnd );
		if( attrSnd.idActUp || attrSnd.strImgDown.empty() == false ) {
			if( attrSnd.idActUp )
				pAddWnd = new XWndPlaySound( attrSnd.idActUp );
			else
			if( !attrSnd.strImgDown.empty() )
				pAddWnd = new XWndPlayMusic( SZ2C(attrSnd.strImgDown) );
// 			if( XASSERT( m_pDelegate ) ) {
// 				auto pDelegate = dynamic_cast<XDelegateLayout*>( m_pDelegate );
// 				if( XASSERT( pDelegate ) ) {
// 					if( attrSnd.idActUp )
// 						pDelegate->OnDelegateLayoutPlaySound( attrSnd.idActUp );
// 					if( attrSnd.strImgDown.empty() == false )
// 						pDelegate->OnDelegateLayoutPlayMusic( attrSnd.strImgDown.c_str() );
// // 				SOUNDMNG->OpenPlayBGMusic( SZ2C( attrSnd.strImgDown.c_str() ) );
// // 				SOUNDMNG->PlaySound( attrSnd.idActUp );
// 				}
// 			}
		}
	} else
	if( XE::IsSame( cCtrlName, "layout" ) ) {
		const auto strtLayout = attrAll.GetstrFile();
		XLayoutObj layoutObj( strtLayout );
		XWnd *pRoot = new XWnd();
		if( layoutObj.CreateLayout( attrAll.m_strcLayout, pRoot ) ) {
			pRoot->SetPosLocal( attrAll.vPos );
			pRoot->SetScaleLocal( attrAll.vScale );
			pRoot->SetbShow( attrAll.bShow );
			pRoot->SetstrIdentifier( attrAll.strKey );
			pParent->Add( pRoot );
		} else {
			SAFE_DELETE( pRoot );
		}
	}	else {
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
		if( nTouchable >= 0 )
			pAddWnd->SetbTouchable( (nTouchable==1)? TRUE : FALSE );
		// 먼저 부모에 붙인다.
		if( pParent )
			AddWndParent( pAddWnd, pParent, attrAll );
		// 버추얼 커스텀에게 맡겼는데도 모르는게 나왔다면 이 노드를 키로 다시 리커시브로 들어가본다.
		CreateLayout( elemCtrl, pAddWnd );
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
 텍스트 컨트롤
*/
void XLayout::GetAttrCtrlText( TiXmlElement *elemCtrl, xATTR_ALL *pOut )
{
	// position
	GetAttrPos( elemCtrl, "pos", &pOut->vPos, &pOut->alignpos );
	// size
	GetAttrSize( elemCtrl, &pOut->vSize );
	// text id
	GetAttrText( elemCtrl, "text", &pOut->idText );
	// color
// 	if( !GetAttrColor( elemCtrl, "text_col", &pOut->colText ) )	// 이건 앞으로 사용하지 말것.
// 		GetAttrColor( elemCtrl, "col", &pOut->colText );
	GetAttrColor( elemCtrl, "text_col", &pOut->colText );
	// font 
	GetAttrFont( elemCtrl, "font", &pOut->strFont, &pOut->sizeFont, &pOut->style );
	// align
	if( pOut->bAlignByParent && pOut->align ) {
		XE::xAlign alignDummy;
		GetAttrTextAlign( elemCtrl, &alignDummy, &pOut->lengthLine );
	} else {
		GetAttrTextAlign( elemCtrl, &pOut->align, &pOut->lengthLine );
	}
	// 가로세로 중앙정렬옵션일때
	if( pOut->align == XE::xALIGN_CENTER ) {
		// 세로크기가 정해지지 않았다면 자동으로 hcenter한다.
		if( pOut->vSize.h < 1 )
			pOut->align = XE::xALIGN_HCENTER;
	}
	if( pOut->align == XE::xALIGN_HCENTER ) {
		if( pOut->vSize.w < 1 ) {
			pOut->align = XE::xALIGN_LEFT;
//			CONSOLE( "경고:node=%s 가로크기가 지정되지 않음.", m_strNodeWnd.c_str() );
		}
	}
	if( pOut->align == XE::xALIGN_VCENTER ) {
		if( pOut->vSize.h < 1 ) {
			pOut->align = XE::xALIGN_LEFT;
//			CONSOLE( "경고:node=%s 세로크기가 지정되지 않음.", m_strNodeWnd.c_str() );
		}			
	}
	// comma
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
}


XWnd *XLayout::CreateStaticText( const char *cCtrlName, 
										TiXmlElement *elemCtrl, 
										XWnd *pParent, 
										const xATTR_ALL& attrAll )
{
	xATTR_ALL attr;
	attr = attrAll;			// attrAll로 먼저 넣고
	GetAttrCtrlText( elemCtrl, &attr );		// 추가로 텍스트의 속성을 넣는다.

	XWnd *pAddWnd = NULL;
	//
	if( attr.num > 1 ) {
		// 여러개 생성
		for( int i = 0; i < attr.num; ++i ) {
			XWnd *pWnd = CreateStaticTextOnlyOne( cCtrlName, elemCtrl, pParent, attr, i );
			AddWndParent( pWnd, pParent, attrAll, i );
		}
	} else
		pAddWnd = CreateStaticTextOnlyOne( cCtrlName, elemCtrl, pParent, attr );

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
#ifdef _VER_IOS
	// 아이폰에선 팝업일때 외곽선찍기가 느려서 쉐도우로 바꿈
	if( pParent->GetwtWnd() == XE::WT_POPUP )
		if( style == xFONT::xSTYLE_STROKE )
			style = xFONT::xSTYLE_SHADOW;
#endif
	XBREAK( attr.sizeFont == 0 );
	// 세로사이즈가 있으면 영역 텍스트로 생성
	if( attr.vSize.h > 0 || attr.alignpos & XE::xALIGN_VCENTER ) {
		auto vSize = attr.vSize;
		if( attr.xfFlag & xF_COUNTER )
			CONSOLE( "node:%s: 카운터 텍스트속성에 세로크기는 사용할 수 없다.", m_strNodeWnd.c_str() );
		if( (attr.alignpos & XE::xALIGN_VCENTER) || (attr.alignpos & XE::xALIGN_HCENTER) ) {
			// "posx/y"에 center가 있으면 자동으로 부모의 크기를 사용한다.
			if( attr.vSize.h <= 0 )
				vSize.h = pParent->GetSizeValidNoTrans().h;
// 				vSize.h = pParent->GetSizeFinal().h;	// 사이즈가 지정되어 있지 않은경우 부모의 사이즈로 한다.
			if( attr.vSize.w <= 0 )
				vSize.w = pParent->GetSizeValidNoTrans().w;
//				vSize.w = pParent->GetSizeFinal().w;	// 사이즈가 지정되어 있지 않은경우 부모의 사이즈로 한다.
// 			if( vSize.w <= 0 )
// 				vSize.w = pParent->GetSizeValidNoTrans().w;
// 			if( vSize.h <= 0 )
// 				vSize.h = pParent->GetSizeValidNoTrans().h;
			XBREAK( vSize.w <= 0 );
			XBREAK( vSize.h <= 0 );
		}
		XWndTextString *pText = NULL;
		_tstring strFont = attr.strFont;
		float sizeFont = attr.sizeFont;
		if( strFont.empty() )
			strFont = FONT_SYSTEM;
		if( sizeFont == 0 )
			sizeFont = FONT_SIZE_DEFAULT;
		if( attr.xfFlag & xF_COUNTER ) {
			auto pTextCnt = new XWndTextNumberCounter( 
												vPos.x, vPos.y, 
												0,
												strFont.c_str(), 
												sizeFont, 
												attr.colText );
			if( attr.xfFlag & xF_COMMA )
				pTextCnt->SetbComma( TRUE );
			pText = pTextCnt;
		} else {
			pText = new XWndTextString( vPos, vSize, XTEXT(attr.idText),
										strFont.c_str(), 
										sizeFont, 
										attr.colText );
		}
		auto align = attr.align;
		if( attr.alignpos & XE::xALIGN_VCENTER ) {
			// "posy"에 center를 했을때. "align"은 무시됨.
			align = XE::xALIGN_VCENTER;
		}
		if( attr.alignpos & XE::xALIGN_HCENTER ) {
			// "posx"에 center를 했을때. "align"은 무시됨.
			if( align & XE::xALIGN_VCENTER )
				align = XE::xALIGN_CENTER;
			else
				align = XE::xALIGN_HCENTER;
		}
		pText->SetAlign( align );
		pText->SetStyle( style );
		// 아직 구현이 덜되서 이건 무조건 CENTER정렬이다.
		if( attr.lengthLine && vSize.w > 0 )
			pText->SetLineLength( vSize.w );
		if( attr.bAlignByParent )
			pText->SetbAlignParent( true );
		return pText;
	} else {
		XWndTextString *pText = NULL;
		_tstring strFont = attr.strFont;
		if( strFont.empty() )
			strFont = FONT_SYSTEM;
		if( attr.xfFlag & xF_COUNTER ) {
			auto pTextCnt = new XWndTextNumberCounter( 
												vPos.x, vPos.y, 
												0,
												strFont.c_str(), attr.sizeFont, 
												attr.colText );
			if( attr.xfFlag & xF_COMMA )
				pTextCnt->SetbComma( TRUE );
			pText = pTextCnt;
		} else
			pText = new XWndTextString( vPos, XTEXT(attr.idText), 
										strFont.c_str(), attr.sizeFont, attr.colText );
		if( attr.alignpos ) {
			auto alignpos = attr.alignpos;
			// "posy" center인데 세로크기를 지정해놓지 않으면 자동으로 hcenter로 바뀐다.
			if( alignpos & XE::xALIGN_VCENTER )
				if( attr.vSize.h <= 0 )	
					alignpos = XE::xALIGN_HCENTER;
			// pos에 있는 정렬값이 우선한다.
			pText->SetAlign( alignpos );
//			auto vSizeParent = pParent->GetSizeFinal();
			auto wSizeParent = pParent->GetWidthFinalValid();
			XBREAK( alignpos != XE::xALIGN_LEFT && wSizeParent <= 0 );
			pText->SetLineLength( wSizeParent );
			pText->SetSizeLocal( XE::VEC2( wSizeParent, 0 ) );
		} else {
			pText->SetSizeLocal( attr.vSize );
			auto align = attr.align;
			if( align && align != XE::xALIGN_LEFT ) {
				if( attr.vSize.w > 0 )
					pText->SetLineLength( attr.vSize.w );
				else
					pText->SetLineLength( pParent->GetSizeLocal().w );	// CENTER나 right일때 사이즈 값을 안넣었으면 부모의 사이즈로 한다.
				// "align" center(가로세로중앙정렬)로 했는데 세로크기가 지정이 안되어 있으면 자동으로 hcenter한다.
				if( align & XE::xALIGN_VCENTER )
					if( attr.vSize.h <= 0 )
						align = XE::xALIGN_HCENTER;
				pText->SetAlign( align );
			} else {
				if( attr.vSize.w > 0 )
					pText->SetLineLength( attr.vSize.w );
			}
		}
		pText->SetStyle( style );
		if( attr.bAlignByParent )
			pText->SetbAlignParent( true );
		return pText;
	}
	return NULL;
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
		if( m_pDelegate )
		{
			pDelegate = dynamic_cast<XWndPageSlideWithXMLDelegate*>( m_pDelegate );
			pSlide->SetpDelegate( pDelegate );
		}
		// xml에 페이지를 직접 구현했으면 여기서 생성한다.
		TiXmlElement *elemPage = elemCtrl->FirstChildElement( "page" );
		if( elemPage )
		{
			//
			int numPages = 0;
			do 
			{
				const char *cKeyPage = elemPage->Value();
				if( XE::IsSame( cKeyPage, "page" ) )
				{
//					int idxPage = pSlide->AddPageKey( cKeyPage );
					++numPages;
				}
			} while (( elemPage = elemPage->NextSiblingElement() ));
			// "page"개수를 세어 슬라이드뷰의 총페이지 수로 정함. 
			// 페이지 인덱스는 자동으로 0~ 부터 매겨진다.
			pSlide->SetnumPages( numPages );
			// for XML
		} else 
		{  
			// 페이지 레이아웃이 정의되어 있을때....
			if( strDefaultPageLayout.empty() == false )
			{
				if( m_pDelegate )
				{
					// 몇페이지나 만들건지 델리게이트에 물어본다.
					int numPages = 0;
					if( pDelegate )
						numPages = pDelegate->GetNumPagesForSlide( attr );
	//				XWndPageForSlide *pPage = new XWndPageForSlide( i, XE::VEC2(0), attr.vSize );
					pSlide->SetnumPages( numPages );
				}
			} else
			{
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
	const char *cFile = elemCtrl->Attribute( "file" );
//	XLOGXNA("%s: file=%s", cCtrlName, cFile );
	XE::VEC2 vPos = attrAll.vPos;
	//
	_tstring strRes;
	if( XE::IsHave( cFile ) )	{
		_tstring strPath = C2SZ(XE::GetFilePath(cFile));
		if( strPath.empty() )
			strRes = XE::MakePath( DIR_UI, C2SZ( cFile ) );
		else
			strRes = C2SZ(cFile);
		if( XE::IsEmpty(XE::GetFileExt(cFile)) )
			strRes += _T(".png");
	}
	const std::string strFormatSurface = elemCtrl->Attribute( "format_surface" );
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
	//
	int roty = 0, rotz = 0;
	elemCtrl->Attribute( "roty", &roty );
	elemCtrl->Attribute( "rotz", &rotz );
	XWndImage *pWnd = new XWndImage( strRes.c_str(), formatSurface, vPos );
	if( idxMulti ) {
		auto vDist = attrAll.vDist;
		if( vDist.w == 0 )
			vDist.w = 2.f;	// 사용자 실수를 방지하기위해 값이 지정되지 않으면 기본값으로.
		vPos += vDist * (float)idxMulti;
		pWnd->SetPosLocal( vPos );
	}
	pWnd->SetRotateY( (float)roty );
	pWnd->SetRotateZ( (float)rotz );
	double scale = 1.0;
	elemCtrl->Attribute( "scale", &scale );
	pWnd->SetScaleLocal( (float)scale );
	double scaley, scalex;
	scalex = -1.0;
	scaley = -1.0;
	elemCtrl->Attribute( "scale_x", &scalex );
	elemCtrl->Attribute( "scale_y", &scaley );
	if( scalex >= 0 )
		pWnd->SetScaleLocal( (float)scalex, pWnd->GetScaleLocal().y );
	if( scaley >= 0 )
		pWnd->SetScaleLocal( pWnd->GetScaleLocal().x, (float)scaley );
	// 그림을 부모크기에 맞춰서 중앙정렬.
	const XE::VEC2 vSize = pWnd->GetSizeLocal();
//	auto vCurr = pWnd->GetPosLocal();
//	auto vSizeParent = pParent->GetSizeFinal();
	auto vSizeParent = pParent->GetSizeValidNoTrans();
	if( attrAll.align & XE::xALIGN_VCENTER ) {
		float y = (vSizeParent.h * 0.5f) - (vSize.h * 0.5f);
		pWnd->SetY( y );
	}
	if( attrAll.align & XE::xALIGN_HCENTER ) {
		float x = ( vSizeParent.w * 0.5f ) - ( vSize.w * 0.5f );
		pWnd->SetX( x );
	}
	if( roty != 0 || rotz != 0 ) {
		auto size = vSize * 0.5f;
		pWnd->SetAdjustAxis( size );	// 회전축을 중앙으로
	}
// 	if( attrAll.align == XE::xALIGN_HCENTER )
// 	{
// //		XE::VEC2 vSizeParent = pParent->GetSizeLocal();
// 		XE::VEC2 vSizeImg = pWnd->GetSizeLocal();
// 		XE::VEC2 vPosLT = vSizeParent / 2.f - vSizeImg / 2.f;
// 		XE::VEC2 vPosLocal = pWnd->GetPosLocal();
// 		vPosLocal.x = vPosLT.x;
// 		pWnd->SetPosLocal( vPosLocal );
// 	}
	double alpha = 1.0;
	elemCtrl->Attribute( "alpha", &alpha );
	pWnd->SetAlphaLocal( (float)alpha );
	const char *cStr = elemCtrl->Attribute( "blend" );
	if( XE::IsHave(cStr) ) {
		std::string strBlend = cStr;
		if( strBlend == "add" )
			pWnd->SetblendFunc( XE::xBF_ADD );
		else
		if( strBlend == "sub" )
			pWnd->SetblendFunc( XE::xBF_SUBTRACT );
		else
		if( strBlend == "gray" )
			pWnd->SetblendFunc( XE::xBF_GRAY );
	}
	return pWnd;
}

/**
 SprObj컨트롤
*/
XWnd* XLayout::CreateSprCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll )
{
	XE::VEC2 vPos;
	XE::xAlign align = XE::xALIGN_NONE;
	GetAttrPos( elemCtrl, "pos", &vPos, &align );
	if( align == XE::xALIGN_HCENTER )
	{
// 		XE::VEC2 vSize = pParent->GetSizeLocal();
		XE::VEC2 vSize = pParent->GetSizeValidNoTrans();
		vPos.x = vSize.x / 2.f;
	}
	//
	const char *cSpr = elemCtrl->Attribute( "spr" );
	if( XE::IsEmpty(cSpr) )
		cSpr = elemCtrl->Attribute( "file" );
	_tstring strSpr;
	if( XE::IsHave( cSpr ) )
	{
		strSpr = C2SZ( cSpr );
		if( XE::IsEmpty( XE::GetFileExt( strSpr.c_str() ) ) )		// 확장자가 안붙어 있으면 붙인다.
			strSpr += _T(".spr");
	}
	//
	int roty = 0, rotz = 0;
	elemCtrl->Attribute( "roty", &roty );
	elemCtrl->Attribute( "rotz", &rotz );
	bool bFlip = false;
	GetAttrBool( elemCtrl, "flip", &bFlip );
	if( bFlip )
		roty = 180;
	int idAct = 0;
	elemCtrl->Attribute( "act", &idAct );
	if( idAct == 0 )
		idAct = 1;		// 지정안되어있으면 디폴트 1로 한다.
	// num sequence
	int num = 0;
	elemCtrl->Attribute( "num", &num );
	// dist
	int distx=0, disty=0;
	elemCtrl->Attribute( "distx", &distx );
	elemCtrl->Attribute( "disty", &disty );
	// playtype
	const char *cPlayType = elemCtrl->Attribute( "play_type" );
	xRPT_TYPE loopType = xRPT_LOOP;
	if( XE::IsHave( cPlayType ) )
	{
		if( XE::IsSame( cPlayType, "once" ) )
			loopType = xRPT_1PLAY;
		else if( XE::IsSame( cPlayType, "once_standby" ) )
			loopType = xRPT_1PLAY_CONT;
	}

	if( num > 0 ) {
		// 여러개 생성
		XE::VEC2 v = vPos;
		for( int i = 0; i < num; ++i ) {
			XWndSprObj *pWnd = NULL;
			if( strSpr.empty() ) {
				pWnd = new XWndSprObj( v.x, v.y );
				pWnd->SetloopType( loopType );
			} else
				pWnd = new XWndSprObj( strSpr.c_str(), (ID)idAct, v.x, v.y, loopType );
			if( pWnd ) {
				pWnd->SetRotateY( (float)roty );
				pWnd->SetRotateZ( (float)rotz );
				pWnd->SetScaleLocal( attrAll.vScale );
				if( !attrAll.vSize.IsMinus() )
					pWnd->SetSizeLocal( attrAll.vSize );
			}
			AddWndParent( pWnd, pParent, attrAll, i );
			v += XE::VEC2( distx, disty );
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
			pWnd->SetRotateY( (float)roty );
			pWnd->SetRotateZ( (float)rotz );
			pWnd->SetScaleLocal( attrAll.vScale );
			if( !attrAll.vSize.IsMinus() )
				pWnd->SetSizeLocal( attrAll.vSize );
		}
		return pWnd;
	}
	return NULL;
}

/**
 팝업프레임
*/
XWnd* XLayout::CreatePopupFrame( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll )
{
	XE::VEC2 vPos;
	XE::xAlign align = XE::xALIGN_HCENTER;
	GetAttrPos( elemCtrl, "pos", &vPos, &align );
	// size
	XE::VEC2 vSize;
	GetAttrSize( elemCtrl, &vSize );
	// frame
	const char *cFrame = elemCtrl->Attribute( "frame" );
	_tstring strFrame;
	if( XE::IsHave( cFrame ) )
		strFrame = C2SZ( cFrame );
	// img
	const char *cImg = elemCtrl->Attribute( "img" );
	_tstring strImg;
	if( XE::IsHave(cImg) )
		strImg = C2SZ(cImg);
	//
	xATTR_ALL attr;
	attr.vPos = vPos;
	attr.alignpos = align;
	attr.vSize = vSize;
	attr.strFrame = strFrame;
	attr.strImgUp = strImg;

	const std::string strFormatSurface = elemCtrl->Attribute( "format_surface" );
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
	// popup의 경우는 다른 컨트롤과달리 부모가 XWndPopup이어야 한다.
	if( XBREAK( pParent->GetwtWnd() != XE::WT_POPUP) )
		return nullptr;
	XWndPopup *pPopup = SafeCast<XWndPopup*, XWnd*>( pParent );
	if( XBREAK( pPopup == nullptr ) )
		return nullptr;
	if( strImg.empty() == false )	{
		// 팝업의 배경 이미지를 설정.
		pPopup->SetBgImg( strImg.c_str(), formatSurface );
	} else
	if( strFrame.empty() == false )	{
		pPopup->LoadRes( strFrame.c_str() );
	}
	// 팝업 하위의 레이아웃을 만든다.
	if( attrAll.strKey.empty() == false )
		pParent->SetstrIdentifier( attrAll.strKey.c_str() );
	CreateLayout( elemCtrl, pParent );
	return nullptr;
}

XWndPopup* XLayout::CreatePopupWnd( const xATTR_ALL& attr )
{
	XE::VEC2 vPos = attr.vPos;
	XE::VEC2 vSize = attr.vSize;
	if( vSize.IsMinus() )
		vSize.Set(0);
	XWndPopup *pFrame = nullptr;
	if( attr.strFrame.empty() == false )
	{
		pFrame = new XWndPopup( vPos, vSize, attr.strFrame.c_str() );
		pFrame->SetbModal( TRUE );
	} else
	if( attr.strImgUp.empty() == false )
	{
		pFrame = new XWndPopup( vPos.x, vPos.y, attr.strImgUp.c_str()  );
		pFrame->SetbModal( TRUE );
	}
	return pFrame;
}

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
	if( pTabView )
	{
		pTabView->SetidStartTab( (ID)idStartTab );
		if( attr.alignpos == XE::xALIGN_HCENTER )
		{
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
XWnd* XLayout::CreateButtonCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, 
								XWnd *pParent,
								const xATTR_ALL& attrAll, 
								const xATTR_BUTT& attrButt,
								int idxMulti )
{
	XWnd *pAddWnd = NULL;
	XE::VEC2 vPos = attrButt.vPos;		// 시작위치
	if( idxMulti > 0 )
		vPos += attrButt.vDist * (float)idxMulti;
	// sprite button
	if( attrButt.strSpr.empty() == false )
	{
		_tstring strSpr = attrButt.strSpr;
		{
			ID idActUp = attrButt.idActUp;
			ID idActDown = attrButt.idActDown;
			ID idActDisable = attrButt.idActDisable;
			if( idActUp == 0 )
			{
				XALERT( "node:%s, act_up is 0", C2SZ( m_nodeWnd->Value() ) );
				return 0;
			}
			// layout-"butt"에서 "text" 를 아예 넣지 않은 경우 idText=-1이 된다.
			// "text 0"과 구분하기 위해 만들었다. 
			// 이런 쓰임은 XWndbuttString타입으로 생성을 시키고 싶으나 일단 텍스트는 없을때 이렇게 사용한다.
			// 사용자가 불편할수 있으니 버튼은 모두 TextString타입으로 생성시키는게 좋을듯 하다.
			if( attrButt.idText >= 0 && attrButt.idText != (ID)-1 )
			{
				ID idText = attrButt.idText;
				XWndButtonString *pButt = NULL;
				if( attrButt.strFont.empty() )
				{
					// 폰트가 지정되지 않았다면 시스템 폰트를 사용
					pButt = new XWndButtonString( vPos.x, vPos.y, 
											XTEXT(idText), 
											attrButt.colText, 
											XE::GetMain()->GetSystemFontDat(), 
											strSpr.c_str(), 
											idActUp, idActDown, idActDisable );
				} else
					pButt = new XWndButtonString( vPos.x, vPos.y, 
											XTEXT(idText), 
											attrButt.colText,
											attrButt.strFont.c_str(), 
											attrButt.sizeFont, 
											attrButt.strSpr.c_str(), 
											idActUp, idActDown, idActDisable );
				pButt->SetStyle( attrButt.style );
				pAddWnd = pButt;
			}
			else
			{
				XWndButton *pButt = NULL;
				// 텍스트가 지정되지 않았다면 일반 이미지버튼.
				pButt = new XWndButton( vPos.x, vPos.y, 
										attrButt.strSpr.c_str(), 
										idActUp, idActDown, idActDisable );		
				pAddWnd = pButt;
			}
		} // not null spr
	} // strSpr
	else
	// image button
	if( attrButt.strImgUp.empty() == false )
	{
		XWndButton *pButt = NULL;
		{
			if( attrButt.idText >= 0 && attrButt.idText != (ID)-1 )
			{
				XWndButtonString *pButtStr;
				if( attrButt.strFont.empty() )
				{
					// 폰트가 지정되지 않았다면 시스템 폰트를 사용
					pButtStr = new XWndButtonString( vPos.x, vPos.y, 
												XTEXT(attrButt.idText), 
												attrButt.colText, 
												XE::GetMain()->GetSystemFontDat(), 
												attrButt.strImgUp.c_str(), 
												attrButt.strImgDown.c_str(), 
												attrButt.strImgDisable.c_str() );
				} else
					pButtStr = new XWndButtonString( vPos.x, vPos.y, 
												XTEXT(attrButt.idText), 
												attrButt.colText,
												attrButt.strFont.c_str(), 
												attrButt.sizeFont, 
												attrButt.strImgUp.c_str(), 
												attrButt.strImgDown.c_str(), 
												attrButt.strImgDisable.c_str() );
				pButt = pButtStr;
				pButtStr->SetStyle( attrButt.style );
			} else
				pButt = new XWndButton( vPos.x, vPos.y, 
										attrButt.strImgUp.c_str(), 
										attrButt.strImgDown.c_str(), 
										attrButt.strImgDisable.c_str() );
		}
		if( attrButt.xfFlag & xfALIGN_ALL )
			pButt->SetFlag( attrButt.xfFlag, pParent );
		pAddWnd = pButt;
	} else
	{
		// 파일이 지정되지 않았다면 투명버튼.
		XWndButton *pButt = new XWndButton( vPos.x, vPos.y,
											attrButt.vSize.w, attrButt.vSize.h );
		pAddWnd = pButt;
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
										const xATTR_BUTT& attrButt,
										int idxMulti )
{
	XWnd *pAddWnd = NULL;
	XE::VEC2 vPos = attrButt.vPos;		// 시작위치
	if( idxMulti > 0 )
		vPos += attrButt.vDist * (float)idxMulti;
	if( attrButt.strImgUp.empty() )	{
		CONSOLE("node=%s:radio_butt: img_up is null", m_strNodeWnd.c_str());
		return NULL;
	}
	XE::VEC2 vAdj2;
	GetAttrVector2( elemCtrl, "adj", &vAdj2, 2 );		// down이미지의 추가보정값
	// image button
	XWndButton *pButt = NULL;
	pButt = new XWndButtonRadio( idGroup,
															 vPos.x, vPos.y,
															 attrButt.strImgUp.c_str(),
															 attrButt.strImgDown.c_str(),
															 attrButt.strImgDisable.c_str() );
// 	if( attrButt.idText >= 0 && attrButt.idText != (ID)-1 ) {
// 		XWndButtonRadio *pButtRadio;
// 		LPCTSTR szFont = attrButt.strFont.c_str();
// 		if( attrButt.strFont.empty() )
// 			szFont = FONT_SYSTEM;	// 폰트가 지정되어있지 않다면 시스템 폰트를 사용.
// 		pButtRadio = new XWndButtonRadio( idGroup,
// 									vPos.x, vPos.y, 
// 									XTEXT(attrButt.idText), 
// 									attrButt.colText,
// 									szFont, 
// 									attrButt.sizeFont, 
// 									attrButt.strImgUp.c_str(), 
// 									attrButt.strImgDown.c_str(), 
// 									attrButt.strImgDisable.c_str() );
// 		pButt = pButtRadio;
// 		pButtRadio->SetStyle( attrButt.style );
// 	} else {
// 		pButt = new XWndButtonRadio( idGroup,
// 								vPos.x, vPos.y, 
// 								attrButt.strImgUp.c_str(), 
// 								attrButt.strImgDown.c_str(), 
// 								attrButt.strImgDisable.c_str() );
// 	}
	if( attrButt.xfFlag & xfALIGN_ALL )
		pButt->SetFlag( attrButt.xfFlag, pParent );
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
	int adjx=0, adjy=0;
	elemCtrl->Attribute( "adjx", &adjx );
	elemCtrl->Attribute( "adjy", &adjy );

	auto pBar = new XWndProgressBar( vPos.x, vPos.y, strImg1.c_str(), strImg2.c_str() );
	if( align == XE::xALIGN_HCENTER )
	{
		XE::VEC2 vLT = (pParent->GetSizeLocal() / 2.f) - (pBar->GetSizeLocal() / 2.f);
		pBar->SetPosLocal( vLT );
	}
	pBar->SetvAdjust( XE::VEC2( adjx, adjy ) );

	return pBar;
}

XWnd *XLayout::CreateProgressBarCtrl2( const char *cCtrlName, 
												TiXmlElement *elemCtrl, 
												XWnd *pParent, 
												const xATTR_ALL& attrAll )
{
// 	xATTR_ALL attr;
// 	attr = attrAll;			// attrAll로 먼저 넣고
	//
//	XE::VEC2 vPos;
	XE::xAlign align = XE::xALIGN_LEFT;
//	GetAttrPos( elemCtrl, "pos", &vPos, &align );
	//
	auto pBar = new XWndProgressBar2( attrAll.vPos , attrAll.GetstrFile() );
	if( pBar ) {
		// "layer"들을 읽음.
		LoadLayer( pBar, elemCtrl );
	}
	if( align == XE::xALIGN_HCENTER ) {
		XE::VEC2 vLT = (pParent->GetSizeLocal() / 2.f) - (pBar->GetSizeLocal() / 2.f);
		pBar->SetPosLocal( vLT );
	}

	return pBar;
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
			xATTR_ALL attr;
			GetAttrCommon( pNode, & attr );
			bool bFixed = false;
			GetAttrBool( pNode, "fixed", &bFixed );
			auto pLayer = pBar->AddLayer( attr.idWnd, attr.GetstrFile(), bFixed );
			if( pLayer ) {
				pLayer->m_vAdj = attr.vAdjust;
				pLayer->m_bShow = (attr.bShow != FALSE);
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
// 	XWndList *pWndList = new XWndList( strBg.c_str(),	// 배경이미지
// 													attr.vPos,			// 좌상귀 좌표
// 													XE::VEC2(0),	// 좌상귀에서 +되는 상대좌표
// 													attr.vSize,			// 리스트영역 크기
// 													type );			// 리스트 타입
	auto pWndList = new XWndList( attr.vPos,			// 좌상귀 좌표
																attr.vSize,			// 리스트영역 크기
																type );			// 리스트 타입
	pWndList->SetvSpace( XE::VEC2( wSpace, hSpace ) );
	// 리스트안에 들어갈 items
	int numItems = 0;
	elemCtrl->Attribute( "items_num", &numItems );
	const char *cItems = elemCtrl->Attribute( "items_node" );
	if( XE::IsHave( cItems ) ) {
		TiXmlElement *pElemItem = GetElement( cItems );
		if( pElemItem ) {
			pWndList->SetstrItemLayoutForXML( cItems );		// 아이템 레이아웃 이름을 저장.
			for( int i = 0; i < numItems; ++i ) {
				XWnd *pItem = CreateXWindow( pElemItem );
				pWndList->AddItem( i + 1, pItem );
			}
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
	XWndEdit *pWndEdit = CreateEditBoxWnd( attr );
	if( pWndEdit )	{
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
// 	pEdit = new XWndEdit( attr.vPos.x, attr.vPos.y, 
// 						attr.vSize.w, attr.vSize.h,
// 						szFont, attr.sizeFont,
// 						attr.colText );
	pEdit->SetvAdjust( attr.vAdjust );
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
//////////////////////////////////////////////////////////////////////////
// GetAttribute
int XLayout::GetAttrCommon( TiXmlElement *elemCtrl, xATTR_ALL *pOut )
{
	// 윈도우 아이디 읽기
	GetAttrWinID( elemCtrl, &pOut->idWnd );
	// x, y좌표 읽음 혹은 정렬
	GetAttrPos( elemCtrl, "pos", &pOut->vPos, &pOut->alignpos);
	// size
	GetAttrSize( elemCtrl, &pOut->vSize );
	// scale
	GetAttrScale( elemCtrl, &pOut->vScale );	
	//
	double alpha = 1.0;
	elemCtrl->Attribute( "alpha", &alpha );
	pOut->alpha = (float)alpha;
	int adjx = 0;
	int adjy = 0;
	elemCtrl->Attribute( "adjx", &adjx );
	elemCtrl->Attribute( "adjy", &adjy );
	pOut->vAdjust = XE::VEC2( adjx, adjy );
	// 텍스트
	int idText = 0;
	elemCtrl->Attribute( "text", &idText );
	pOut->idText = idText;
	if( idText )
		if( XE::IsEmpty( XTEXT(idText) ) )
			XALERT( "node=%s, text id %d not found", m_strNodeWnd.c_str(), idText );
	// 컬러
	XCOLOR colText = 0;				// 디폴트 컬러
	GetAttrColor( elemCtrl, "text_col", &colText );	// 컬러 태그가 있으면 그값을 쓴다.
	if( colText == 0 )
		colText = XCOLOR_WHITE;
	pOut->colText = colText;

// 	GetAttrColor( elemCtrl, "text_col", &pOut->colText );	// 컬러 태그가 있으면 그값을 쓴다.
//	GetAttrColor( elemCtrl, "col", &pOut->colText );	// 컬러 태그가 있으면 그값을 쓴다.
	// 폰트
	_tstring strFont;
	float sizeFont = 20;
	xFONT::xtStyle style = xFONT::xSTYLE_NORMAL;
	GetAttrFont( elemCtrl, "font", &pOut->strFont, &pOut->sizeFont, &pOut->style );
	// sprite
	const char *cSpr = elemCtrl->Attribute( "spr" );
	if( XE::IsEmpty( cSpr ) )
		cSpr = elemCtrl->Attribute( "file" );
	if( XE::IsHave( cSpr ) )
	{
		_tstring strSpr = C2SZ( cSpr );
		if( XE::IsEmpty( XE::GetFileExt( strSpr.c_str() ) ) )		// 확장자가 안붙어 있으면 붙인다.
			strSpr += _T(".spr");
		int idActUp=0, idActDown=0, idActDisable=0;
		elemCtrl->Attribute( "act_up", &idActUp );
		elemCtrl->Attribute( "act_down", &idActDown );
		elemCtrl->Attribute( "act_off", &idActDisable );
		pOut->strSpr = strSpr;
		pOut->idActUp = idActUp;
		pOut->idActDown = idActDown;
		pOut->idActDisable = idActDisable;

	}
	// image
	const char *cImg = elemCtrl->Attribute( "img" );
	if( XE::IsEmpty( cImg ) )
		cImg = elemCtrl->Attribute( "file" );
	if( XE::IsHave( cImg ) )
		pOut->strImgUp = C2SZ( cImg );		// img는 strImgUp과 공용으로 쓴다.
	// num sequence
	elemCtrl->Attribute( "num", &pOut->num );
	// dist
	{
		int distx=0, disty=0;
		elemCtrl->Attribute( "distx", &distx );
		elemCtrl->Attribute( "disty", &disty );
		if( distx )
			pOut->vDist.x = (float)distx;
		if( disty )
			pOut->vDist.y = (float)disty;
	}
	// sound
	{
		const char *cFile = elemCtrl->Attribute( "file" );
		if( XE::IsHave( cFile ) == FALSE )
			cFile = elemCtrl->Attribute( "file" );
		if( XE::IsHave( cFile ) )
		{
			pOut->strImgUp = C2SZ( cFile );
		}
		int idSnd = 0;
		elemCtrl->Attribute( "id_snd", &idSnd );
		if( idSnd == 0 )
			elemCtrl->Attribute( "id_sound", &idSnd );
		pOut->idActUp = (ID)idSnd;
		const char *cBgm = NULL;
		cBgm = elemCtrl->Attribute( "bgm" );
		if( XE::IsHave( cBgm ) )
		{
			pOut->strImgDown = C2SZ( cBgm );
		}
	}
	// layout
	pOut->m_strcLayout = elemCtrl->Attribute( "layout" );
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
		if( XE::IsEmpty(cColor) )
			return 0;
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
	if( XE::IsHave(cColor) )
	{
		// 상수에 있는지 찾아봄
		TiXmlElement *elemConst = GetElement( "constant" );
		if( elemConst )
		{
			TiXmlElement *elemValue = elemConst->FirstChildElement( cColor );
			if( elemValue )
				return GetAttrColor( elemValue, cAttrKey, pOutColor );
		}
		// 부모에 있는지 찾아봄
		if( m_pParent )
		{
			TiXmlElement *elemConst = m_pParent->GetElement( "constant" );
			if( elemConst )
			{
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
		if( XE::IsSame( cPosx, "center" ) )
			align |= (int)XE::xALIGN_HCENTER;
		else if( XE::IsSame( cPosx, "right" ) )
			align |= (int)XE::xALIGN_RIGHT;
	}
	strcpy_s( cKey, cAttrKey );
	strcat_s( cKey, "y" );
	const char *cPosy = GetAttrValueI( elemCtrl, cKey, &y );
	if( XE::IsSame( cPosy, "center" ) )
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
	XBREAK( pOut == nullptr );
	char cKey[ 64 ];
	strcpy_s( cKey, cAttrKey );
	strcat_s( cKey, "x" );
	int x = -0xffff, y = -0xffff;
	int align = 0;
	const char *cPosx = GetAttrValueI( elemCtrl, cKey, &x );
	if( cPosx ) {
		if( XE::IsSame( cPosx, "center" ) )
			align |= (int)XE::xALIGN_HCENTER;
		else
		if( XE::IsSame( cPosx, "center_parent" ) ) {
			align |= (int)XE::xALIGN_HCENTER;
			pOut->bAlignByParent = true;
		}
	}
	strcpy_s( cKey, cAttrKey );
	strcat_s( cKey, "y" );
	const char *cPosy = GetAttrValueI( elemCtrl, cKey, &y );
	if( XE::IsSame( cPosy, "center" ) )
		align |= (int)XE::xALIGN_VCENTER;
	if( XE::IsSame( cPosy, "center_parent" ) ) {
		align |= (int)XE::xALIGN_VCENTER;
		pOut->bAlignByParent = true;
	}
	pOut->align = ( XE::xAlign )align;
	if( x == -0xffff && y == -0xffff )
		return 0;
	if( x != -0xffff )
		pOut->vPos.x = (float)x;
	if( y != -0xffff )
		pOut->vPos.y = (float)y;
	return 1;
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
	*pOut = XE::VEC2( (float)scale );
	double scaley, scalex;
	scalex = -1.0;
	scaley = -1.0;
	elemCtrl->Attribute( "scale_x", &scalex );
	elemCtrl->Attribute( "scale_y", &scaley );
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
	if( idText >= 0 )
	{
		if( idText >0 && XE::IsEmpty( XTEXT(idText) ) )
		{
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
	int width = 0;
	elemCtrl->Attribute( "width", &width );
	if( width )
		*pOutWidth = (float) width;

	return 1;
}

int XLayout::GetAttrBool( TiXmlElement *elemCtrl, const char *cKey, BOOL *pOut )
{
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


int XLayout::GetAttrWinID( TiXmlElement *elemCtrl, ID *pOut )
{
	// 윈도우 아이디
	int idWnd = 0;
	elemCtrl->Attribute( "id", &idWnd );
	if( pOut )
		*pOut = idWnd;
// 	int idWnd = 0;
// 	if( XE::IsHave( cID ) )
// 	{
// 		idWnd = CONSTANT->GetValue( C2SZ( cID ) );
// // 		if( idWnd == XConstant::xERROR )
// // 		{
// // 			XALERT( "id=%s, not found", C2SZ( cID ) );
// // 			return 0;
// // 		}
// 		*pOut = (ID) idWnd;
// 		return 1;
// 	}
	return 0;
}

int XLayout::GetAttrFile( TiXmlElement *elemCtrl, const char *cAttrKey, _tstring *pOutStr )
{
	if( XE::IsHave( cAttrKey ) )
	{
		// 지정된 키가 있으면 그걸로 찾고 끝낸다.
		const char *cFile = elemCtrl->Attribute( cAttrKey );
		if( XE::IsHave( cFile ) )
		{
			*pOutStr = C2SZ( cFile );
			return 1;
		}
		return 0;
	}
	// 지정된 키가 없으면 예약된 키들로 찾는다.
	const char *cFile = elemCtrl->Attribute( "file" );
	if( XE::IsHave( cFile ) )
	{
		*pOutStr = C2SZ( cFile );
		return 1;
	}
	cFile = elemCtrl->Attribute( "img" );
	if( XE::IsHave( cFile ) )
	{
		*pOutStr = C2SZ( cFile );
		return 1;
	}
	return 0;
}

/**
 버튼을 위한 어트리뷰트를 읽어 구조체에 담아준다. 
*/
int XLayout::GetAttrCtrlButton( TiXmlElement *elemCtrl, xATTR_BUTT *pOut )
{
	// x, y좌표 읽음 혹은 정렬
	int x=0, y=0;
	DWORD xfFlag = 0;
	const char *cPosx = elemCtrl->Attribute( "posx" );
	const char *cPosy = elemCtrl->Attribute( "posy" );
	if( XE::IsSame( cPosx, "center" ) )
		xfFlag |= xfALIGN_HCENTER;
	else if( XE::IsSame( cPosx, "right" ) )
		xfFlag |= xfALIGN_RIGHT;
	else
		GetAttrValueI( elemCtrl, "posx", &x );
//		elemCtrl->Attribute( "posx", &x );
	if( XE::IsSame( cPosy, "center" ) )
		xfFlag |= xfALIGN_VCENTER;
	else if( XE::IsSame( cPosy, "bottom" ) )
		xfFlag |= xfALIGN_BOTTOM;
	else
		GetAttrValueI( elemCtrl, "posy", &y );
//		elemCtrl->Attribute( "posy", &y );
	pOut->vPos.x = (float) x;
	pOut->vPos.y = (float) y;
	pOut->xfFlag = xfFlag;
	// size
	GetAttrSize( elemCtrl, &pOut->vSize );
	// 텍스트
	int idText = -1;		// Layout에 "text 아이디" 를 아예 쓰지 않은 경우
	elemCtrl->Attribute( "text", &idText );
	if( idText )
		if( idText != -1 && XE::IsEmpty( XTEXT(idText) ) )
			XALERT( "node=%s, text id %d not found", m_strNodeWnd.c_str(), idText );
	pOut->idText = (ID)idText;
	// 컬러
	XCOLOR colText = XCOLOR_WHITE;				// 디폴트 컬러
	GetAttrColor( elemCtrl, "text_col", &colText );	// 컬러 태그가 있으면 그값을 쓴다.
//	GetAttrColor( elemCtrl, "col", &colText );	// 컬러 태그가 있으면 그값을 쓴다.
	pOut->colText = colText;
	// 폰트
	_tstring strFont;
	float sizeFont = 20;
	xFONT::xtStyle style = xFONT::xSTYLE_NORMAL;
	GetAttrFont( elemCtrl, "font", &strFont, &sizeFont, &style );
	pOut->strFont = strFont;
	pOut->sizeFont = sizeFont;
	pOut->style = style;
	//
	// sprite button
	const char *cSpr = elemCtrl->Attribute( "spr" );
	if( XE::IsHave( cSpr ) )
	{
		_tstring strSpr = C2SZ( cSpr );
		if( XE::IsEmpty( XE::GetFileExt( strSpr.c_str() ) ) )		// 확장자가 안붙어 있으면 붙인다.
			strSpr += _T(".spr");
		int idActUp=0, idActDown=0, idActDisable=0;
		elemCtrl->Attribute( "act_up", &idActUp );
		elemCtrl->Attribute( "act_down", &idActDown );
		elemCtrl->Attribute( "act_off", &idActDisable );
		pOut->strSpr = strSpr;
		pOut->idActUp = idActUp;
		pOut->idActDown = idActDown;
		pOut->idActDisable = idActDisable;
	}
	// image button
	const char *cImgUp = elemCtrl->Attribute( "img_up" );
	if( XE::IsEmpty(cImgUp) )
		cImgUp = elemCtrl->Attribute( "file" );
	if( XE::IsHave( cImgUp ) )
	{
		const char *cImgDown = elemCtrl->Attribute( "img_down" );
		const char *cImgDisable = elemCtrl->Attribute( "img_off" );
		_tstring strImgUp, strImgDown, strImgDisable;
		strImgUp = C2SZ( cImgUp );
		if( XE::IsHave( cImgDown ) )
			strImgDown = C2SZ( cImgDown );
		if( XE::IsHave( cImgDisable ) )
			strImgDisable = C2SZ( cImgDisable );
		pOut->strImgUp = strImgUp;
		pOut->strImgDown = strImgDown;
		pOut->strImgDisable = strImgDisable;
	}
	// num sequence
	elemCtrl->Attribute( "num", &pOut->num );
	// dist
	{
		int distx=0, disty=0;
		elemCtrl->Attribute( "distx", &distx );
		elemCtrl->Attribute( "disty", &disty );
		if( distx )
			pOut->vDist.x = (float)distx;
		if( disty )
			pOut->vDist.y = (float)disty;
	}

	return 1;
}

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
	if( cStr )
	{
		BOOL bRead = FALSE;
		// 첫문자가 숫자나 +-가 아니면 문자열상수로 본다
		if( _tcschr(_T("-+0123456789"), cStr[0]) == NULL )	
		{
//			if( strchr( cStr, '-' ) || strchr( cStr, '+' ) || strchr( cStr, '*' ) || strchr( cStr, '/' ))
			{
				CToken token;
				token.LoadString( C2SZ(cStr) );
				token.GetToken();
				float valConst = 0;
				if( token == _T("scr_height") ) 
				{
					valConst = GRAPHICS->GetLogicalScreenSize().h;
					bRead = TRUE;
				} else
				if( token == _T("scr_width") )
				{
					valConst = GRAPHICS->GetLogicalScreenSize().w;
					bRead = TRUE;
				} else
				if( token == _T("game_height") ) 
				{
					valConst = XE::GetGameHeight();
					bRead = TRUE;
				} else
				if( token == _T("game_width") )
				{
					valConst = XE::GetGameWidth();
					bRead = TRUE;
				}
				*pOut = valConst;	// 현재까지의 결과를 저장
				if( token.GetToken() ) 	// +,-,/,*를 읽음
				{
					if( _tcschr(_T("+-*/"), token.m_Token[0]) )	// 사칙연산 기호가 있느냐
					{
						switch( token.m_Token[0] )
						{
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
		if( bRead == FALSE )
		{
			double dVal = -0xffff;
			elem->Attribute( cKey, &dVal );
			if( dVal != -0xffff )
				*pOut = (float)dVal;
		}
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





