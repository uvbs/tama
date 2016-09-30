#pragma once
#include "tinyxml/tinyxml.h"
#include "XBaseFontDat.h"
#include "XBaseDelegate.h"
#include "_Wnd2/XWnd.h"
#include "XFramework/XParamObj.h"

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
class XWndProgressBar2;
class XDelegateLayout;
class XWndTextString;
class XWndList;
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
	static XLayout* sCreateMain( LPCTSTR szXml, XDelegateLayout* pDelegateMain );
	static XDelegateLayout* s_pDelegateMain;		// 메인 델리게이트. 커스텀 컨트롤등을 만들때 사용
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
		int m_Debug = 0;
		XE::VEC2 vPos;
		XE::VEC2 vSize = XE::VEC2(-1);
		XE::VEC2 vAdjust;		// vPos의 보정값
		XE::VEC2 vAdjDraw;	// vPos는 건드리지 않고 최종 화면 draw할때 보정값.
		XE::VEC2 vScale = XE::VEC2(1);
		XE::VEC3 vRot;
		DWORD xfFlag;	// xF_시리즈들이나 버튼에서 쓰는 몇몇 플래그
		ID idActDown, idActDisable;
		union {
			ID idAct;
			ID idActUp;
		};
		union {
			ID idText;
//			ID idActUp;
//			ID idAct;
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
			XCOLOR col;
//			ID idActDown;
		};
		union {
			float sizeFont;
//			ID idActDisable;
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
		int m_Touchable = -1;
		XE::xAlign align = XE::xALIGN_NONE;
//		XE::xAlign alignpos;
//		bool bAlignByParent = false;	// 부모크기기준 실시간 정렬
		float lengthLine;
//		DWORD dwFlag;	 일단은 xfFlag를 같이쓰되 분리해야 할필요가 있다면 이걸 살릴것.
		_tstring strFrame;		// 3x3 타일프레임.
		std::string m_strcLayout;		// 다른 레이아웃파일(xml)내의 레이아웃 키
		float alpha = 1.f;
		XE::xtBlendFunc m_funcBlend = XE::xBF_MULTIPLY;
		bool m_bFlipX = false;
		bool m_bFlipY = false;
		XE::xtPixelFormat m_Format = XE::xPF_NONE;
		XParamObj m_Param;
		xATTR_ALL() {
			xfFlag = 0;
			idText = 0;
			colText = XCOLOR_WHITE;
			sizeFont = 20.f;
			style = xFONT::xSTYLE_NORMAL;
			idAct = idActUp = idActDown = idActDisable = 0;
			num = 1;
			idWnd = 0;
			bEnable = TRUE;
			bShow = TRUE;
//			alignpos = XE::xALIGN_LEFT;
			lengthLine = 0;
//			dwFlag = 0;
		}
		// "file" attribue의 값
		const _tstring& GetstrFile() const {
			if( !strSpr.empty() )
				return strSpr;
			if( !strImgUp.empty() )
				return strImgUp;
			return strSpr;
		}
		void SetstrFile( const _tstring& strFile ) {
			strSpr = strFile;
		}
	}; // xATTR_ALL
private:
	std::string m_strFullpath;
	TiXmlDocument m_Layout;
	TiXmlNode *m_rootNode;
	TiXmlNode *m_nodeWnd;
	_tstring m_strNodeWnd;
	BOOL m_bLoad;
	XDelegateLayout *m_pDelegate;
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
	XLayout( LPCTSTR szXml, XDelegateLayout *pDelegate=NULL, XLayout *pParent=NULL );
	virtual ~XLayout() { Destroy(); }
	//
	BOOL GetError() {
		return (m_bLoad == FALSE);
	}
	bool IsError() const {
		return !m_bLoad;
	}
	GET_SET_ACCESSOR( XDelegateLayout*, pDelegate );
	GET_ACCESSOR_CONST( const std::string&, strFullpath );
	BOOL Load( const char *cFullpath );
	inline BOOL Load( const std::string& strcFullpath ) {
		return Load( strcFullpath.c_str() );
	}
	BOOL Reload( void );
	BOOL CreateLayout( const char *cKey, XWnd *pParent, const char *cKeyGroup = NULL );
	inline BOOL CreateLayout( const std::string& strcKey, XWnd *pParent, const char *cKeyGroup = NULL ) {
		return CreateLayout( strcKey.c_str(), pParent, cKeyGroup );
	}
	BOOL CreateLayout( TiXmlElement *elemNode, XWnd *pParent );
	XWnd* CreateXWindow( TiXmlElement *elemNode );
	float GetLeftPosButt( _tstring& strSpr, int idActUp, XWnd *pParent );
	XLAYOUT_ELEM* GetElement( const char* cKey, TiXmlNode *nodeRoot=NULL ) const;
	XLAYOUT_NODE* GetNode( const char* cKey, TiXmlNode *nodeRoot=NULL ) const;
	TiXmlNode* FindNode( TiXmlNode *nodeWnd, const char *cKey ) const;
	void AddWndParent( XWnd *pWnd, XWnd *pParent, const xATTR_ALL& attrAll, int idxMulti=-1 );
	// control
//	XWnd* CreateStaticText( const char *cCtrlName, TiXmlElement *elemCtrl, const XWnd *pParent, const xATTR_ALL& attrAll ) const;
	XWnd* CreateStaticTextOnlyOne( const char *cCtrlName, 
								TiXmlElement *elemCtrl, 
								const XWnd *pParent, 
								const xATTR_ALL& attr, 
								int idxMulti=0 ) const;
	XWnd* CreateSlideCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, const XWnd *pParent, const xATTR_ALL& attrAll ) const;
	XWnd* CreateImgCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, const XWnd* pParent, const xATTR_ALL& attrAll, int idxMulti=0 ) const;
	XWnd* CreateSprCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll );
	XWnd* CreatePopupFrame( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll );
	XWnd* CreateButtonCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, const XWnd *pParent
							, const xATTR_ALL& attrAll
							, int idxMulti=0 ) const;
	XWnd* CreateRadioButtonCtrl( const char *cCtrlName, TiXmlElement *elemCtrl, 
								const XWnd *pParent,
								const ID idGroup,
								const xATTR_ALL& attrAll, 
								int idxMulti=0 ) const;
	XWnd* CreateProgressBarCtrl( const char *cCtrlName, 
								TiXmlElement *elemCtrl, 
								const XWnd *pParent, 
								const xATTR_ALL& attrAll ) const;
	XWnd* CreateProgressBarCtrl2( const char *cCtrlName, 
								TiXmlElement *elemCtrl, 
								const XWnd *pParent, 
								const xATTR_ALL& attrAll ) const;
	int LoadLayer( XWndProgressBar2 *pBar, TiXmlElement *pRoot ) const;
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
								const XWnd *pParent, 
								const xATTR_ALL& attrAll );
	XWnd* CreateEditBoxCtrl( const char *cCtrlName, 
							TiXmlElement *elemCtrl, 
							const XWnd *pParent, 
							const xATTR_ALL& attrAll ) const;
	XWnd* CreateScrollViewCtrl( const char *cCtrlName, 
								TiXmlElement *elemCtrl, 
								const XWnd *pParent, 
								const xATTR_ALL& attrAll ) const;
	// Wnd
//	virtual XWndPopup* CreatePopupWnd( const xATTR_ALL& attr );
	virtual XWndTab2* CreateTabViewWnd( const xATTR_ALL& attr );
	virtual XWndSlideDown* CreateSlideDownWnd( const xATTR_ALL& attr ) const;
	virtual XWndPageSlideWithXML* CreateSlideWnd( const xATTR_ALL& attr, TiXmlElement *elemCtrl ) const;
	virtual XWndEdit* CreateEditBoxWnd( const xATTR_ALL& attr ) const;
	virtual XWndScrollView* CreateScrollViewWnd( const xATTR_ALL& attr ) const;
	
	// attribute
	int GetAttrCommon( TiXmlElement *elemCtrl, xATTR_ALL *pOut ) const;
	int GetAttrColor( TiXmlElement *elemCtrl, const char *cAttrKey, XCOLOR *pOutColor ) const;
	int GetAttrRGBA( TiXmlElement *elemCtrl, const char *cAttrKey, BYTE *pColElem ) const;
	int GetAttrFont( TiXmlElement *elemCtrl, const char *cAttrKey, _tstring *pOutStr, float *pOutSize, xFONT::xtStyle *pOutStyle ) const;
	int GetAttrPos( TiXmlElement *elemCtrl, const char *cAttrKey, XE::VEC2 *pOut, XE::xAlign *pOutAlign=NULL ) const;
	int GetAttrPos( TiXmlElement *elemCtrl, const char *cAttrKey, xATTR_ALL *pOut ) const;
	int GetAttrSize( TiXmlElement *elemCtrl, XE::VEC2 *pOut ) const;
	int GetAttrScale( TiXmlElement *elemCtrl, XE::VEC2 *pOut ) const;
	int GetAttrText( TiXmlElement *elemCtrl, const char *cAttrKey, ID *pOut ) const;
	int GetAttrTextAlign( TiXmlElement *elemCtrl, XE::xAlign *pOut, float *pOutWidth ) const;
	int GetAttrWinID( TiXmlElement *elemCtrl, ID *pOut ) const;
	int GetAttrFile( TiXmlElement *elemCtrl, const char *cAttrKey, _tstring *pOutStr ) const;
	int GetAttrBool( TiXmlElement *elemCtrl, const char *cKey, BOOL *pOut ) const;
	bool GetAttrBool( TiXmlElement *elemCtrl, const char *cKey, bool *pOut ) const;
	int GetAttrNum( TiXmlElement *elemCtrl ) const;
	XE::VEC2 GetAttrDist( TiXmlElement *elemCtrl ) const;
	//
	int CreateControl( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, XWnd **ppOutCreated=NULL );
	XWnd* CreateCustomControl( const char *cCtrlName, TiXmlElement *elemCtrl, const XWnd *pParent, const xATTR_ALL& attrAll ) const;
	//
	int GetNextCtrlFromKeyClear( void ) { return 1; }
	template<int _Size>
	XWnd* GetNextCtrlFromKey( XWnd *pRoot, const char *cKey, int *pOutIdx, char (&pOutSeqKey)[_Size] ) {
		return GetNextCtrlFromKey( pRoot, cKey, pOutIdx, pOutSeqKey, _Size );
	}
	XWnd* GetNextCtrlFromKey( XWnd *pRoot, const char *cKey, int *pOutIdx, char *pOutSeqKey, int lenSeqKey );
	const char* GetAttrValueF( TiXmlElement *elem, const char *cKey, float *pOut ) const;
	const char* GetAttrValueI( TiXmlElement *elem, const char *cKey, int *pOut ) const;
	XWnd* CreateWnd( const std::string& strNode, const XWnd* pParent ) const;
//////////////////////////////////////////////////////////////////////////
private:
	void AlignCtrl( const char* cCtrlName
								, TiXmlElement* pElemCtrl
								, const XWnd* pParent
								, XWnd* pCtrl
								, const xATTR_ALL& attrAll ) const;
	void GetAttrAdj( TiXmlElement* pElemCtrl, XE::VEC2* pOut ) const;
	void GetAttrVector2( TiXmlElement* pElemCtrl, const char* cHeader, XE::VEC2* pOut, int idx = 0 ) const;
	bool GetAttrBlendFunc( TiXmlElement* pElemCtrl, XE::xtBlendFunc* pOut, const std::string& strKey = std::string() ) const;
	bool GetAttridAct( TiXmlElement* pElemCtrl, ID* pOut ) const;
	bool GetAttrDWORD( TiXmlElement *elemCtrl, const char* cKey, DWORD *pOut ) const;
	bool GetAttrInt( TiXmlElement *elemCtrl, const char* cKey, int *pOut ) const;
	XWnd* CreateViewFrame( const char *cCtrlName, TiXmlElement *elemCtrl, XWnd *pParent, const xATTR_ALL& attrAll );
	bool GetAttrFloat( TiXmlElement *elemCtrl, const char* cKey, float *pOut ) const;
	XWnd* CreateControl2( TiXmlElement *elemCtrl, const XWnd *pParent ) const;
	XWnd* CreateSprCtrlWnd( TiXmlElement *elemCtrl, const XWnd *pParent, const xATTR_ALL& attrAll ) const;
}; // XLayout
////////////////////////////////////////////////////////////////
class XDelegateLayout : public XBaseDelegate
{
	void Init() {}
	void Destroy() {}
public:
	XDelegateLayout() {
		Init();
	}
	virtual ~XDelegateLayout() {
		Destroy();
	}
	//
	virtual void OnDelegateLayoutPlaySound( ID idSnd ) {}
	virtual void OnDelegateLayoutPlayMusic( LPCTSTR szBgm ) {}
	virtual XWndView* DelegateLayoutCreateViewCtrl( const XWnd* pParent, const XLayout::xATTR_ALL& attr ) const;
	virtual XWnd* DelegateCreateCustomCtrl( const std::string& strcCtrl
																				, TiXmlElement* pElemCtrl
																				, const XWnd *pParent
																				, const XLayout::xATTR_ALL& attrAll) const { return nullptr; }
};


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

#define xSET_BUTT_TEXT( ROOT, KEY, TXT ) { \
	XWndButtonString *_pButt; \
	XBREAK( ROOT == NULL ); \
	_pButt = dynamic_cast<XWndButtonString*>( ROOT->Find( KEY ) ); \
	if( _pButt ) \
		_pButt->SetText( TXT ); \
	else \
		XLOGXNA( "xSET_BUTT_TEXT: %s not found", KEY ); \
}


inline XE::VEC2 xGET_POS_LOCAL( XWnd *pRoot, const char *cKey ) {
	XWnd *pWnd = pRoot->Find( cKey );
	return pWnd->GetPosLocal();
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

// XWndImage* xGET_IMAGE_CTRLF( XWnd *pRoot, const char *cKeyFormat, ... );
XWndView* xGET_VIEW_CTRL( XWnd *pRoot, const char *cKey );
XWndScrollView* xGET_SCROLLVIEW_CTRL( XWnd *pRoot, const char *cKey );
XWndButton* xGET_BUTT_CTRL( XWnd *pRoot, const char *cKey );
// XWndList* xGET_LIST_CTRL( XWnd *pRoot, const char *cKey );
// XWndImage* xGET_IMAGE_CTRL( XWnd *pRoot, const char *cKey );
XWndSprObj* xGET_SPROBJ_CTRL( XWnd *pRoot, const char *cKey );
XWndSprObj* xGET_SPROBJ_CTRLF( XWnd *pRoot, const char *cKeyFormat, ... );
XWndPopup* xGET_POPUP_CTRL( XWnd *pRoot, const char *cKey );


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

inline XWnd* xSET_ACTIVE( XWnd *pRoot, const char *cKey, BOOL bFlag ) {
	XBREAK( pRoot == nullptr );
	XWnd *pWnd = pRoot->Find( cKey );
	if( pWnd )
		pWnd->SetbActive( bFlag );
	else
		XLOGXNA( "xSET_ACIVE: %s not found. ", cKey );
	return pWnd;
};

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


