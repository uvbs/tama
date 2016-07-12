#pragma once
#include "tinyxml/tinyxml.h"
#include "XBaseFontDat.h"
#include "XBaseDelegate.h"
#include "_Wnd2/XWnd.h"
#include "_Wnd2/XWndText.h"
#include "_Wnd2/XWndView.h"
#include "_Wnd2/XWndButton.h"
#include "_Wnd2/XWndImage.h"
#include "_Wnd2/XWndSprObj.h"

#define XLAYOUT_ELEM	TiXmlElement
#define XLAYOUT_NODE	TiXmlNode

class XBaseDelegate;
class XWndPopup;
class XWndTab2;
class XWndSlideDown;
class XWndPageSlideWithXML;
class XWndEdit;
class XWndScrollView;
class XLayout;
class XWndSprObj;
class XWndButton;
class XWndImage;
class XWndView;
////////////////////////////////////////////////////////////////
class XDelegateLayout : public XBaseDelegate
{
	void Init() {}
	void Destroy() {}
public:
	XDelegateLayout() { Init(); }
	virtual ~XDelegateLayout() { Destroy(); }
	//
	virtual void OnDelegateLayoutPlaySound( ID idSnd ) {}
	virtual void OnDelegateLayoutPlayMusic( LPCTSTR szBgm ) {}
};
//////////////////////////////////////////////////////////////////////////
/**
 커스텀 컨트롤을 위해 XLayout을 상속받는 현 방식은 최초 레이아웃 객체 생성시에
 항상 상속받은 객체를 수동으로 생성시켜야 한다는 단점이 있다.
 메인 레이아웃은 싱글톤 형태로 수동으로 생성시키고 커스텀 컨트롤은 델리게이트
 형태로 생성시키는게 좀더 편할듯 하다.
*/
class XLayout
{
public:
	static XLayout* sGetMain() { return s_pMain; }
	static XLayout* sCreateMain( LPCTSTR szXml );
	template<typename T>
	static T sGetSeqCtrl( XWnd *pRoot, const char *cKey, int numSeq ) {
		char cBuff[ 256 ];
		sprintf_s( cBuff, "%s%d", cKey, numSeq );
		XWnd *pFind = pRoot->Find( cBuff );
		if( pFind )
			return SafeCast<T, XWnd*>( pFind );
		return nullptr;
	}
	template<typename T>
	static T sGetCtrl( XWnd *pRoot, const char *cKey ) {
		XWnd *pFind = pRoot->Find( cKey );
		if( pFind )
			return SafeCast<T, XWnd*>( pFind );
		return nullptr;
	}
	// 메인 레이아웃 수동삭제.
	static void sDestroyMain() {
		SAFE_DELETE( s_pMain );
	}
private:
	/**
	 레이아웃은 여러객체가 생성될수 있으나 편의상 메인이 되는 레이어 객체가 있고 다른 객체는 이를 상속받는다.
	 메인 레이아웃에는 컬러 상수등이 있다.
	*/
	static XLayout *s_pMain;		// 메인 레이아웃. 
//	static XAutoPtr<XLayout*> s_autoDestroy;	// 메인 레이아웃 자동 파괴 객체
public:
	enum {	// 버튼들에서 XWndButton에 xf시리즈 플래그를 중복해서 쓰는게 있어서 임시로 앞쪽 비트를 쓰게 함.
			xF_COMMA	=0x10000000,
			xF_COUNTER	=0x20000000,	// 숫자텍스트를 표시하는데 카운트가 되면서 올라가는 버전
	};
	struct xATTR_ALL {
		XE::VEC2 vPos;
		XE::VEC2 vSize = XE::VEC2(-1);
		XE::VEC2 vAdjust;
		XE::VEC2 vScale = XE::VEC2(1);
		XE::VEC2 vRot;
		DWORD xfFlag;	// xF_시리즈들이나 버튼에서 쓰는 몇몇 플래그
		union {
			ID idText;
			ID idActUp;
			struct {
				BYTE b[4];
			};
			struct {
				BYTE dir;
				BYTE dummy[3];
			};
		};
		union {
			XCOLOR colText;
			ID idActDown;
		};
		union {
			float sizeFont;
			ID idActDisable;
		};
		_tstring strFont;
		xFONT::xtStyle style;
		_tstring strSpr;
		_tstring strImgUp, strImgDown, strImgDisable;
		int num;		// 버튼 연속생성 개수
		XE::VEC2 vDist;		// 연속생성시 건너갈 간격
		ID idWnd;
		std::string strKey;
		BOOL bEnable;
		BOOL bShow;
		XE::xAlign align;
//		XE::xAlign alignpos;
		bool bAlignByParent = false;	// 부모크기기준 실시간 정렬
		float lengthLine;
//		DWORD dwFlag;	 일단은 xfFlag를 같이쓰되 분리해야 할필요가 있다면 이걸 살릴것.
		_tstring strFrame;		// 3x3 타일프레임.
		std::string m_strcLayout;		// 다른 레이아웃파일(xml)내의 레이아웃 키
		float alpha = 1.f;
		xATTR_ALL() {
			xfFlag = 0;
			idText = 0;
			colText = XCOLOR_WHITE;
			sizeFont = 20.f;
			style = xFONT::xSTYLE_NORMAL;
			idActUp = idActDown = idActDisable = 0;
			num = 1;
			idWnd = 0;
			bEnable = TRUE;
			bShow = TRUE;
			align = XE::xALIGN_LEFT;
			alignpos = XE::xALIGN_LEFT;
			lengthLine = 0;
//			dwFlag = 0;
		}
		// "file" attribue의 값
		const _tstring& GetstrFile() const {
			return strSpr;
		}
	};
	struct xATTR_BUTT {
		XE::VEC2 vPos;
		XE::VEC2 vSize;
		DWORD xfFlag;
		ID idText;
		XCOLOR colText;
		_tstring strFont;
		float sizeFont;
		xFONT::xtStyle style;
		_tstring strSpr;
		ID idActUp, idActDown, idActDisable;
		_tstring strImgUp, strImgDown, strImgDisable;
		int num;		// 버튼 연속생성 개수
		XE::VEC2 vDist;		// 연속생성시 건너갈 간격
		xATTR_BUTT() {
			xfFlag = 0;
			idText = 0;
			colText = XCOLOR_WHITE;
			sizeFont = 20.f;
			style = xFONT::xSTYLE_NORMAL;
			idActUp = idActDown = idActDisable = 0;
			num = 1;
		}
	};
private:
	std::string m_strFullpath;
	TiXmlDocument m_Layout;
	TiXmlNode *m_rootNode;
	TiXmlNode *m_nodeWnd;
	_tstring m_strNodeWnd;
	BOOL m_bLoad;
	XBaseDelegate *m_pDelegate;
	XLayout *m_pParent;
	void Init() {
		m_bLoad = FALSE;
		m_rootNode = NULL;
		m_nodeWnd = NULL;
		m_pDelegate = NULL;
		m_pParent = NULL;
	}
	void Destroy() {}
public:
	XLayout( LPCTSTR szXml, XBaseDelegate *pDelegate=NULL, XLayout *pParent=NULL );
	virtual ~XLayout() { Destroy(); }
	//
	BOOL GetError() {
		return (m_bLoad == FALSE);
	}
	GET_SET_ACCESSOR( XBaseDelegate*, pDelegate );
	GET_ACCESSOR( std::string&, strFullpath );
	BOOL Load( const char *cFullpath );
	BOOL Reload( void );
	BOOL CreateLayout( const char *cKey, XWnd *pParent, const char *cKeyGroup = NULL );
	BOOL CreateLayout( TiXmlElement *elemNode, XWnd *pParent );
	XWnd* CreateXWindow( TiXmlElement *elemNode );
	float GetLeftPosButt( _tstring& strSpr, int idActUp, XWnd *pParent );
	XLAYOUT_ELEM* GetElement( const char* cKey, TiXmlNode *nodeRoot=NULL );
	XLAYOUT_NODE* GetNode( const char* cKey, TiXmlNode *nodeRoot=NULL );
	TiXmlNode* FindNode( TiXmlNode *nodeWnd, const char *cKey );
	void AddWndParent( XWnd *pWnd, XWnd *pParent, const xATTR_ALL& attrAll, int idxMulti=-1 );
	// control
	XWnd* CreateStaticText( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll );
	XWnd* CreateStaticTextOnlyOne( const char *cCtrlName, 
								TiXmlElement *elemCtrl, 
								XWnd *pParent, 
								const xATTR_ALL& attr, 
								int idxMulti=0 );
	XWnd* CreateSlideCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll );
	XWnd* CreateImgCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll, int idxMulti=0 );
	XWnd* CreateSprCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll );
	XWnd* CreatePopupFrame( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll );
	XWnd* CreateButtonCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent,
							const xATTR_ALL& attrAll, const xATTR_BUTT& attrButt,
							int idxMulti=0 );
	XWnd* CreateRadioButtonCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, 
								XWnd *pParent,
								const ID idGroup,
								const xATTR_ALL& attrAll, 
								const xATTR_BUTT& attrButt,
								int idxMulti=0 );
	XWnd* CreateProgressBarCtrl( const char *cCtrlName, 
								TiXmlElement *elemCtrl, 
								XWnd *pParent, 
								const xATTR_ALL& attrAll );
	XWnd* CreateProgressBarCtrl2( const char *cCtrlName, 
								TiXmlElement *elemCtrl, 
								XWnd *pParent, 
								const xATTR_ALL& attrAll );
	int LoadLayer( XWndProgressBar2 *pBar, TiXmlElement *pRoot );
	XWnd* CreateTabViewCtrl( const char *cCtrlName, 
							TiXmlElement *elemCtrl, 
							XWnd *pParent,
							const xATTR_ALL& attrAll );
	int AddTab( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll );
	XWnd* CreateListCtrl( const char *cCtrlName, 
						TiXmlElement *elemCtrl, 
						XWnd *pParent, 
						const xATTR_ALL& attrAll );
	XWnd* CreateSlideDownCtrl( const char *cCtrlName, 
								TiXmlElement *elemCtrl, 
								XWnd *pParent, 
								const xATTR_ALL& attrAll );
	XWnd* CreateEditBoxCtrl( const char *cCtrlName, 
							TiXmlElement *elemCtrl, 
							XWnd *pParent, 
							const xATTR_ALL& attrAll );
	XWnd* CreateScrollViewCtrl( const char *cCtrlName, 
								TiXmlElement *elemCtrl, 
								XWnd *pParent, 
								const xATTR_ALL& attrAll );
	// Wnd
	virtual XWndPopup* CreatePopupWnd( const xATTR_ALL& attr );
	virtual XWndTab2* CreateTabViewWnd( const xATTR_ALL& attr );
	virtual XWndSlideDown* CreateSlideDownWnd( const xATTR_ALL& attr );
	virtual XWndPageSlideWithXML* CreateSlideWnd( const xATTR_ALL& attr, TiXmlElement *elemCtrl );
	virtual XWndEdit* CreateEditBoxWnd( const xATTR_ALL& attr );
	virtual XWndScrollView* CreateScrollViewWnd( const xATTR_ALL& attr );
	
	// attribute
	int GetAttrCommon( TiXmlElement *elemCtrl, xATTR_ALL *pOut );
	int GetAttrColor( TiXmlElement *elemCtrl, const char *cAttrKey, XCOLOR *pOutColor );
	int GetAttrRGBA( TiXmlElement *elemCtrl, const char *cAttrKey, BYTE *pColElem );
	int GetAttrFont( TiXmlElement *elemCtrl, const char *cAttrKey, _tstring *pOutStr, float *pOutSize, xFONT::xtStyle *pOutStyle );
	int GetAttrPos( TiXmlElement *elemCtrl, const char *cAttrKey, XE::VEC2 *pOut, XE::xAlign *pOutAlign=NULL );
	int GetAttrPos( TiXmlElement *elemCtrl, const char *cAttrKey, xATTR_ALL *pOut );
	int GetAttrSize( TiXmlElement *elemCtrl, XE::VEC2 *pOut );
	int GetAttrScale( TiXmlElement *elemCtrl, XE::VEC2 *pOut );
	int GetAttrText( TiXmlElement *elemCtrl, const char *cAttrKey, ID *pOut );
	int GetAttrTextAlign( TiXmlElement *elemCtrl, XE::xAlign *pOut, float *pOutWidth );
	int GetAttrWinID( TiXmlElement *elemCtrl, ID *pOut );
	int GetAttrFile( TiXmlElement *elemCtrl, const char *cAttrKey, _tstring *pOutStr );
	int GetAttrBool( TiXmlElement *elemCtrl, const char *cKey, BOOL *pOut );
	inline bool GetAttrBool( TiXmlElement *elemCtrl, const char *cKey, bool *pOut ) {
		BOOL bFlag = FALSE;
		GetAttrBool( elemCtrl, cKey, &bFlag );
		*pOut = (bFlag != FALSE);
		return *pOut;
	}
	int GetAttrNum( TiXmlElement *elemCtrl );
	XE::VEC2 GetAttrDist( TiXmlElement *elemCtrl );
	//
	int GetAttrCtrlButton( TiXmlElement *elemCtrl, xATTR_BUTT *pOut );
	void GetAttrCtrlText( TiXmlElement *elemCtrl, xATTR_ALL *pOut );
	//
	int CreateControl( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, XWnd **ppOutCreated=NULL );
	virtual XWnd* CreateCustomControl( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll ) { return NULL; }
	//
	int GetNextCtrlFromKeyClear( void ) { return 1; }
	template<int _Size>
	XWnd* GetNextCtrlFromKey( XWnd *pRoot, const char *cKey, int *pOutIdx, char (&pOutSeqKey)[_Size] ) {
		return GetNextCtrlFromKey( pRoot, cKey, pOutIdx, pOutSeqKey, _Size );
	}
	XWnd* GetNextCtrlFromKey( XWnd *pRoot, const char *cKey, int *pOutIdx, char *pOutSeqKey, int lenSeqKey );
	const char* GetAttrValueF( TiXmlElement *elem, const char *cKey, float *pOut );
	const char* GetAttrValueI( TiXmlElement *elem, const char *cKey, int *pOut );
};

////////////////////////////////////////////////////////////////
/**
 @brief 레이아웃 객체의 자동삭제를 위한 클래스
 보통 씬 레이아웃에 사용한다.
*/
// class XLayoutObj
// {
// 	XLayout *m_pLayout;
// 	void Init() {
// 		m_pLayout = NULL;
// 	}
// 	void Destroy() {
// 		SAFE_DELETE( m_pLayout );
// 	}
// public:
// //	XLayoutObj() { Init(); }
// 	XLayoutObj( LPCTSTR szXml, XDelegateLayout *pDelegate = nullptr );
// 	virtual ~XLayoutObj() { Destroy(); }
// 	//
// 	GET_ACCESSOR( XLayout*, pLayout );
// //	BOOL CreateLayout( XLayout *pLayout, const char *cKey, XWnd *pParent, const char *cKeyGroup = NULL );
// 	BOOL CreateLayout( const char *cKey, XWnd *pParent, const char *cKeyGroup = nullptr );
// };
//////////////////////////////////////////////////////////////////////////
/*
template<typename RET, typename F, typename T>
RET xSetButtHandler( XWnd* pRoot, T This, const char *cKey, F func ) 
{
	XWnd *pButt; 
	pButt = pRoot->Find( pKey ); 
	if( pButt ) 
		pButt->SetEvent( XWM_CLICKED, This, func ); 
	return pButt;
} */

#define xSET_BUTT_HANDLER2	xSET_BUTT_HANDLER_RET

#define xSET_BUTT_HANDLER_RET( RET, ROOT, KEY, CALL ) \
XWnd *RET; \
{ \
	XWnd *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = ROOT->Find( KEY ); \
	if( _pButt ) 	\
		_pButt->SetEvent( XWM_CLICKED, this, CALL ); \
	else \
		XLOGXN( "xSET_BUTT_HANDLER2: %s not found", C2SZ( KEY ) ); \
	RET = _pButt; \
}

#define xSET_BUTT_HANDLER2_OWNER( RET, ROOT, KEY, OWNER, CALL ) \
	XWnd *RET; \
{ \
	XWnd *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = ROOT->Find( KEY ); \
	if( _pButt ) 	\
		_pButt->SetEvent( XWM_CLICKED, OWNER, CALL ); \
	else \
		XLOGXNA( "xSET_BUTT_HANDLER2: %s not found", KEY ); \
	RET = _pButt; \
}

#define xSET_BUTT_HANDLER2_PARAM( RET, ROOT, KEY, CALL, P1, P2 ) \
	XWnd *RET; \
{ \
	XWnd *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = ROOT->Find( KEY ); \
	if( _pButt ) 	\
		_pButt->SetEvent( XWM_CLICKED, this, CALL, P1, P2 ); \
	else \
		XLOGXNA( "xSET_BUTT_HANDLER2: %s not found", KEY ); \
	RET = _pButt; \
}

// #define xSET_BUTT_HANDLER( ROOT, KEY, CALL ) { \
// 	XWnd *_pButt; \
// 	XBREAK( ROOT == NULL ); \
// 	_pButt = ROOT->Find( KEY ); \
// 	if( _pButt ) \
// 		_pButt->SetEvent( XWM_CLICKED, this, CALL ); \
// 	else \
// 		XLOGXNA( "xSET_BUTT_HANDLER: %s not found", KEY ); \
// }

template<typename T>
inline XWndButton* xSET_BUTT_HANDLER_NEW( XWnd *pRoot, const char *cKey, XWnd *pCallback, T func, DWORD param1=0 ) {
	typedef int ( XWnd::*CALLBACK_FUNC )( XWnd *, DWORD, DWORD );
	if( XBREAK( pRoot == NULL ) )
		return nullptr;
	auto pButt = pRoot->Find( cKey );
	if( pButt )
		pButt->SetEvent( XWM_CLICKED, pCallback, func, param1 );	
	return dynamic_cast<XWndButton*>( pButt );
};

#define xSET_BUTT_HANDLER( ROOT, KEY, CALL ) \
	xSET_BUTT_HANDLER_NEW( ROOT, KEY, this, CALL )

#define xSET_BUTT_TEXT( ROOT, KEY, TXT ) { \
	XWndButtonString *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = dynamic_cast<XWndButtonString*>( ROOT->Find( KEY ) ); \
	if( _pButt ) \
		_pButt->SetText( TXT ); \
	else \
		XLOGXNA( "xSET_BUTT_TEXT: %s not found", KEY ); \
}

#define xSET_BUTT_HANDLER_OWNER( ROOT, KEY, OWNER, CALL ) { \
	XWnd *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = ROOT->Find( KEY ); \
	if( _pButt ) \
		_pButt->SetEvent( XWM_CLICKED, OWNER, CALL ); \
	else \
		XLOGXNA( "xSET_BUTT_HANDLER: %s not found", KEY ); \
}

// 이제 이 매크로는 쓰지 말것. xSET_BUTT_HANDLER_PARAM2를 사용할것.
#define __xSET_BUTT_HANDLER_PARAM( ROOT, KEY, CALL, P1, P2 ) { \
	XWnd *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = ROOT->Find( KEY ); \
	if( _pButt ) \
		_pButt->SetEvent( XWM_CLICKED, this, CALL, P1, P2 ); \
	else \
		XLOGXNA( "xSET_BUTT_HANDLER: %s not found", KEY ); \
}

#define xSET_BUTT_HANDLER_PARAM2( ROOT, KEY, OWNER, CALL, P1, P2 ) { \
	XWnd *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = ROOT->Find( KEY ); \
	if( _pButt ) \
		_pButt->SetEvent( XWM_CLICKED, OWNER, CALL, P1, P2 ); \
	else \
		XLOGXNA( "xSET_BUTT_HANDLER: %s not found", KEY ); \
}
#define xSET_BUTT_HANDLER_PARAM( ROOT, KEY, OWNER, CALL, P1 ) { \
	XWnd *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = ROOT->Find( KEY ); \
	if( _pButt ) \
		_pButt->SetEvent( XWM_CLICKED, OWNER, CALL, P1 ); \
		else \
		XLOGXNA( "xSET_BUTT_HANDLER: %s not found", KEY ); \
}

/**
 @brief 
*/
inline XWndTextString* xSET_TEXT( XWnd *pRoot, const char *cKey, LPCTSTR szText ) {
	if( pRoot == nullptr )	// if( pRoot ) 이런거 안쓰고 간편하게 하려고 넣음.
		return nullptr;
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd ) {
		auto pText = dynamic_cast<XWndTextString*>( pWnd );
		if( pText )
			pText->SetText( szText );
		return pText;
	}
	else
		XLOGXNA( "xSET_TEXT: %s not found. ", cKey );
	return nullptr;
}

inline XWndTextString* xSET_TEXT( XWnd *pRoot, const char *cKey, const _tstring& strText ) {
	return xSET_TEXT( pRoot, cKey, strText.c_str() );
}

inline XWndTextString* xSET_TEXT( XWnd *pRoot, const char *cKey, int num ) {
	if( pRoot == nullptr )	// if( pRoot ) 이런거 안쓰고 간편하게 하려고 넣음.
		return nullptr;
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd ) {
		auto pText = dynamic_cast<XWndTextString*>( pWnd );
		if( pText ) {
			pText->SetText( XFORMAT("%s", XE::NtS(num)) );
		}
		return pText;
	}
	else
		XLOGXNA( "xSET_TEXT: %s not found. ", cKey );
	return nullptr;
}

#define xSET_TEXT_RET( RET, ROOT, KEY, TXT )  \
	XWndTextString *RET = dynamic_cast<XWndTextString*>( ROOT->Find( KEY ) ); \
	if( RET ) \
		RET->SetText( TXT ); \
	else { \
		XLOGXNA( "xSET_TEXT_RET: %s not found", KEY ); } \

inline XE::VEC2 xGET_POS_LOCAL( XWnd *pRoot, const char *cKey ) {
	XWnd *pWnd = pRoot->Find( cKey );
	return pWnd->GetPosLocal();
}

inline XWndTextString* xGET_TEXT_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	return SafeCast<XWndTextString*>( pRoot->Find( cKey ) );
}

inline XWnd* xSET_SHOWF( XWnd *pRoot, bool bFlag, const char *cKeyFormat, ... ) {
	XBREAK( pRoot == nullptr );
	char cKey[256];
	va_list vl;
	va_start(vl, cKeyFormat);
	vsprintf_s( cKey, cKeyFormat, vl );
	va_end(vl);
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd )
		pWnd->SetbShow( bFlag );
	else
		XLOGXNA( "xSET_SHOW: %s not found. ", cKey );
	return pWnd;
};

template<typename T>
inline XWndTextString* xSET_TEXTF( XWnd *pRoot, T text, const char *cKeyFormat, ... ) {
	XBREAK( pRoot == nullptr );
	char cKey[ 256 ];
	va_list vl;
	va_start( vl, cKeyFormat );
	vsprintf_s( cKey, cKeyFormat, vl );
	va_end( vl );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd ) {
		XWndTextString *pText = dynamic_cast<XWndTextString*>( pWnd );
		if( pText )
			pText->SetText( text );
		return pText;
	}
	else
		XLOGXNA( "xSET_SHOW: %s not found. ", cKey );
	return nullptr;
}

inline XWndTextString* xGET_TEXT_CTRLF( XWnd *pRoot, const char *cKeyFormat, ... ) {
	XBREAK( pRoot == nullptr );
	char cKey[ 256 ];
	va_list vl;
	va_start( vl, cKeyFormat );
	vsprintf_s( cKey, cKeyFormat, vl );
	va_end( vl );
	return dynamic_cast<XWndTextString*>( pRoot->Findf( cKey ) );
}

inline XWndImage* xGET_IMAGE_CTRLF( XWnd *pRoot, const char *cKeyFormat, ... ) {
	XBREAK( pRoot == nullptr );
	char cKey[ 256 ];
	va_list vl;
	va_start( vl, cKeyFormat );
	vsprintf_s( cKey, cKeyFormat, vl );
	va_end( vl );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndImage*, XWnd*>( pWnd );
}


inline XWndView* xGET_VIEW_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndView*, XWnd*>( pWnd );
}


inline XWndScrollView* xGET_SCROLLVIEW_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndScrollView*,XWnd*>( pWnd );
}

inline XWndButton* xGET_BUTT_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return dynamic_cast<XWndButton*>( pWnd );
}

inline XWndList* xGET_LIST_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndList*>( pWnd );
}

inline XWndImage* xGET_IMAGE_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndImage*>( pWnd );
}

inline XWndSprObj* xGET_SPROBJ_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndSprObj*>( pWnd );
}
inline XWndSprObj* xGET_SPROBJ_CTRLF( XWnd *pRoot, const char *cKeyFormat, ... ) {
	XBREAK( pRoot == nullptr );
	char cKey[ 256 ];
	va_list vl;
	va_start( vl, cKeyFormat );
	vsprintf_s( cKey, cKeyFormat, vl );
	va_end( vl );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndSprObj*>( pWnd );
}

inline XWndPopup* xGET_POPUP_CTRL( XWnd *pRoot, const char *cKey ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd == nullptr )
		return nullptr;
	return SafeCast<XWndPopup*, XWnd*>( pWnd );
}

// #define xSET_TEXT_FORMAT( ROOT, KEY, ... ) { \
// 	XWnd *__pWnd = ROOT->Find(KEY); \
// 	if( __pWnd ) { \
// 		XWndTextString *__pText = dynamic_cast<XWndTextString*>( __pWnd ); \
// 			if( __pText )	{ \
// 				_tstring __szFormat = __pText->GetszString(); \
// 				__pText->SetText( XE::Format( __szFormat.c_str(), ##__VA_ARGS__ ) ); \
// 		} \
// 	} \
// }

inline XWndTextString* xSET_TEXT_FORMAT( XWnd *pRoot, const char *cKey, LPCTSTR strFormat, ... ) {
	XBREAK( pRoot == NULL );
	TCHAR szStr[ 1024 ] = {0,};
	va_list vl;
	va_start( vl, strFormat );
	_vstprintf_s( szStr, strFormat, vl );
	va_end( vl );
	auto pText = dynamic_cast<XWndTextString*>( pRoot->Find( cKey ) );
	if( pText )
		pText->SetText( szStr );
	else
		XLOGXN( "xSET_TEXT_FORMAT: %s not found.", C2SZ( cKey ) );
	return pText;
};

#define xSET_SPR( ROOT, KEY, SPRNAME, ACT ) { \
	XBREAK( ROOT == NULL ); \
	XWndSprObj *_pWndSpr = SafeCast<XWndSprObj*, XWnd*>( ROOT->Find( KEY ) ); \
	if( _pWndSpr ) \
		_pWndSpr->SetSprObj( SPRNAME, ACT ); \
	else \
		XLOGXN( "xSET_SPR: %s not found. spr file=%s", C2SZ( KEY ), SPRNAME ); \
}

#define xSET_SPR2( RET, ROOT, KEY, SPRNAME, ACT ) \
	XBREAK( ROOT == NULL ); \
	XWndSprObj *RET = SafeCast<XWndSprObj*, XWnd*>( ROOT->Find( KEY ) ); \
	if( RET ) \
		RET->SetSprObj( SPRNAME, ACT ); \
	else \
		XLOGXN( "xSET_SPR2: %s not found. spr file=%s", C2SZ( KEY ), SPRNAME ); \

#define xSET_SPR_IDX( ROOT, KEY, IDX, SPRNAME, ACT ) { \
	XBREAK( ROOT == NULL ); \
	char __cKey[ 256 ]; \
	sprintf_s( __cKey, "%s%d", KEY, IDX ); \
	XWndSprObj *__pWndSpr = SafeCast<XWndSprObj*, XWnd*>( ROOT->Find( __cKey ) ); \
	if( __pWndSpr ) \
		__pWndSpr->SetSprObj( SPRNAME, ACT ); \
	else \
		XLOGXN( "xSET_SPR_IDX: %s not found. spr file=%s", C2SZ( KEY ), SPRNAME ); \
}

// #define xSET_IMG( ROOT, KEY, RES ) { \
// 	XBREAK( ROOT == NULL ); \
// 	XWndImage *_pImg = SafeCast<XWndImage*, XWnd*>( ROOT->Find( KEY ) ); \
// 	if( _pImg ) \
// 		_pImg->SetSurfaceRes( RES ); \
// 	else \
// 		XLOGXN( "xSET_IMG: %s not found. img file=%s", C2SZ( KEY ), RES ); \
// }

inline XWndImage* xSET_IMG( XWnd *pRoot, const char *cKey, LPCTSTR szRes ) {
	XBREAK( pRoot == NULL );
	auto pImg = SafeCast<XWndImage*>( pRoot->Find( cKey ) );
	if( pImg )
		pImg->SetSurfaceRes( szRes );
	else
		XLOGXN( "xSET_IMG: %s not found. img file=%s", C2SZ( cKey ), szRes );
	return pImg;
};

inline XWndImage* xSET_IMG( XWnd *pRoot, const char *cKey, const _tstring& strRes ) {
	return xSET_IMG( pRoot, cKey, strRes.c_str() );
}

inline XWndImage* xSET_IMGF( XWnd *pRoot, LPCTSTR szRes, const char *cKeyFormat, ... ) {
	XBREAK( pRoot == NULL );
	char cKey[ 256 ];
	va_list vl;
	va_start( vl, cKeyFormat );
	vsprintf_s( cKey, cKeyFormat, vl );
	va_end( vl );
	auto pImg = SafeCast<XWndImage*>( pRoot->Find( cKey ) );
	if( pImg )
		pImg->SetSurfaceRes( szRes );
	else
		XLOGXN( "xSET_IMGF: %s not found. img file=%s", C2SZ( cKey ), szRes );
	return pImg;
};

#define xSET_IMG_SEQ( ROOT, KEY, SEQ, RES ) { \
	XBREAK( ROOT == NULL ); \
	char __cBuff[ 256 ]; \
	sprintf_s( __cBuff, "%s%d", KEY, SEQ ); \
	XWndImage *_pImg = SafeCast<XWndImage*, XWnd*>( ROOT->Find( __cBuff ) ); \
	if( _pImg ) \
		_pImg->SetSurfaceRes( RES ); \
		else \
		XLOGXN( "xSET_IMG_SEQ: %s not found. img file=%s", C2SZ( __cBuff ), RES ); \
}

#define xSET_IMG2	xSET_IMG_RET
#define xSET_IMG_RET( RET, ROOT, KEY, RES ) \
	XBREAK( ROOT == NULL ); \
	XWndImage *RET = SafeCast<XWndImage*, XWnd*>( ROOT->Find( KEY ) ); \
	if( RET ) \
		RET->SetSurface( RES ); \
	else \
		XLOGXN( "xSET_IMG2: %s not found. img file=%s", C2SZ( KEY ), RES ); \
	

inline XWnd* xSET_SHOW( XWnd *pRoot, const char *cKey, BOOL bFlag ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd )
		pWnd->SetbShow( bFlag );
	else
		XLOGXNA( "xSET_SHOW: %s not found. ", cKey );
	return pWnd;
};
inline XWnd* xSET_SHOW( XWnd *pRoot, const char *cKey, bool bFlag ) {
	return xSET_SHOW( pRoot, cKey, (bFlag)? TRUE : FALSE );
}

// #define xSET_SHOW( ROOT, KEY, FLAG ) { \
// 	XBREAK( ROOT == NULL ); \
// 	XWnd *_pWnd = ROOT->Find( KEY );  \
// 	if( _pWnd ) \
// 		_pWnd->SetbShow( FLAG ); \
// 	else \
// 		XLOGXNA( "xSET_SHOW: %s not found. ", KEY ); \
// }

// #define xSET_ENABLE( ROOT, KEY, FLAG ) { \
// 	XBREAK( ROOT == NULL ); \
// 	XWnd *_pWnd = ROOT->Find( KEY );  \
// 	if( _pWnd ) \
// 		_pWnd->SetbEnable( FLAG ); \
// 	else \
// 		XLOGXNA( "xSET_ENABLE: %s not found. ", KEY ); \
// }

inline XWnd* xSET_ENABLE( XWnd *pRoot, const char *cKey, BOOL bFlag ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd )
		pWnd->SetbEnable( bFlag );
	else
		XLOGXNA( "xSET_ENABLE: %s not found. ", cKey );
	return pWnd;
};
inline XWnd* xSET_ENABLE( XWnd *pRoot, const char *cKey, bool bFlag ) {
	return xSET_ENABLE( pRoot, cKey, ( bFlag ) ? TRUE : FALSE );
}
inline XWnd* xSET_ENABLE( XWnd *pRoot, const std::string& strKey, bool bFlag ) {
	return xSET_ENABLE( pRoot, strKey.c_str(), ( bFlag ) ? TRUE : FALSE );
}
inline XWnd* xSET_ENABLEF( XWnd *pRoot, bool bFlag, const char *cKeyFormat, ... ) {
	XBREAK( pRoot == nullptr );
	char cKey[ 256 ];
	va_list vl;
	va_start( vl, cKeyFormat );
	vsprintf_s( cKey, cKeyFormat, vl );
	va_end( vl );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd )
		pWnd->SetbEnable( bFlag!=0 );
	else
		XLOGXNA( "xSET_ENABLEF: %s not found. ", cKey );
	return pWnd;
};

// #define xSET_ACTIVE( ROOT, KEY, FLAG ) { \
// 	XBREAK( ROOT == NULL ); \
// 	XWnd *_pWnd = ROOT->Find( KEY );  \
// 	if( _pWnd ) \
// 		_pWnd->SetbActive( FLAG ); \
// 				else \
// 		XLOGXNA( "xSET_ACTIVE: %s not found. ", KEY ); \
// }

inline XWnd* xSET_ACTIVE( XWnd *pRoot, const char *cKey, BOOL bFlag ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd )
		pWnd->SetbActive( bFlag );
	else
		XLOGXNA( "xSET_ACIVE: %s not found. ", cKey );
	return pWnd;
};

#define xSET_EDITBOX_HANDLER( ROOT, KEY, CALL ) { \
	XWnd *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = ROOT->Find( KEY ); \
	if( _pButt ) \
		_pButt->SetEvent( XWM_ENTER, this, CALL ); \
	else \
	XLOGXNA( "xSET_EDITBOX_HANDLER: %s not found", KEY ); \
}

#define xSET_EDITBOX_DELEGATER( ROOT, KEY, DELEGATER ) { \
	XWnd *_pWnd; \
	XBREAK( ROOT == NULL ); \
	_pWnd = ROOT->Find( KEY ); \
	if( _pWnd ) { \
		XWndEdit *_pEdit = SafeCast<XWndEdit*, XWnd*>( _pWnd ); \
		if( _pEdit ) \
			_pEdit->SetpDelegate( DELEGATER ); \
	} else \
		XLOGXNA( "xSET_EDITBOX_HANDLER: %s not found", KEY ); \
}

#define xLAYOUT_SPR_LOOP( _LAYOUT, WNDSPR, ROOT, KEY, cSEQKEY ) { \
	int _idx = _LAYOUT->GetNextCtrlFromKeyClear(); \
	char cSEQKEY[ 256 ]; \
	XWnd *__pWnd; \
	while( __pWnd = _LAYOUT->GetNextCtrlFromKey( ROOT, KEY, &_idx, cSEQKEY ) ) \
	{ \
		XWndSprObj *WNDSPR = SafeCast<XWndSprObj*, XWnd*>( __pWnd );
	
#define xLAYOUT_LOOP_SEQ_CTRL( _LAYOUT, TYPE, VAL, ROOT, KEY, cSEQKEY ) { \
	int _idx = _LAYOUT->GetNextCtrlFromKeyClear(); \
	char cSEQKEY[ 256 ]; \
	XWnd *__pWnd; \
	while( __pWnd = _LAYOUT->GetNextCtrlFromKey( ROOT, KEY, &_idx, cSEQKEY ) ) \
	{ \
		TYPE *VAL = dynamic_cast<TYPE*>( __pWnd );

/**
 @brief 순차적으로 배열되어 있는 윈도우들을 루프돈다.
 @param ROOT 검색할 루트 윈도우
 @param KEY 기본 키 문자열(const char*). 기본키+인덱스(1~) 형태의 문자열키가 완성된다.
 @param IDX 인덱스값을 받아야 할때 사용할 변수이름
 @param NUM 총 배열수
 @param TYPE 윈도우들의 dynamic_cast형
 @param VAL 꺼낸 윈도우의 변수이름.
 @note 예제
 img.star1~5 로 생성되어 있는 경우.
 xCTRL_LOOP( this, "img.star", i, 5, XWndImage*, pImg )
 {
	if( i < numStar )
		pImg->SetbShow( TRUE );
	else
		pImg->SetbShow( FALSE );
 } END_LOOP;
*/
#define xCTRL_LOOP( ROOT, KEY, IDX, NUM, TYPE, VAL ) \
	for( int IDX = 0; IDX < NUM; ++IDX ) { \
		char _cKeyBuff[256]; \
		sprintf_s( _cKeyBuff, "%s%d", KEY, IDX+1 ); \
		auto VAL = dynamic_cast<TYPE>( ROOT->Find( _cKeyBuff ) ); \
		if( VAL ) {


