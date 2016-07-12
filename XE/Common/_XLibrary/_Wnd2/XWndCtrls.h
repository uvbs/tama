#pragma once

#include "XWnd.h"
//#include "XWndButton.h"
#include "XFont.h"
#include "XWndView.h"
#include "XWndText.h"
#include "XParticleMng.h"
#include "XFramework/client/XLayoutObj.h"

class XWndAlert;
class XWndButtonString;
class XWndProgressBar2;
class XParticleMng;
class XParticleDelegate;
class XLayout;
//class XWndTextStringArea;
class XWndAlertDelegate
{
	void Init() {}
	void Destroy() {}
public:
	XWndAlertDelegate() { Init(); }
	virtual ~XWndAlertDelegate() { Destroy(); }
	//
	virtual LPCTSTR OnDelegateAlertGetTextLable( XWndAlert *pAlert, ID idText ) { return _T(""); }
};


class XWndAlert : public XWndView
{
	XWndAlertDelegate *m_pDelegate;
	XBaseFontDat *m_pFDText;
	XWndTextString *m_pTextTitle;
	XWndTextString *m_pTextDesc;
	XWndButtonString *m_pButton1;
	XWndButtonString *m_pButton2;
	XE::VEC2 m_vLTMargin;	// 왼쪽, 위쪽 마진
	XE::VEC2 m_vRBMargin;	// 오른쪽 아래 마진
	XList<XWndButton*> m_listButtons;
	float m_sizeFont = 30.f;
	XWnd::xtAlert m_Type = XWnd::xOK;
	XCOLOR m_Color = XCOLOR_WHITE;
	XLayoutObj m_Layout;
	_tstring m_strTitle;
	_tstring m_strText;
	_tstring m_strFrameImg = _T("popup01.png");
	_tstring m_strImgUp;
	
	void Init() {
		m_pDelegate = NULL;
		m_pFDText = NULL;
		m_pTextTitle = NULL;
		m_pTextDesc = NULL;
		m_pButton1 = m_pButton2 = NULL;
		m_vLTMargin = XE::VEC2( 17, 6 );
		m_vRBMargin = XE::VEC2( 17, 0 );
	}
	void Destroy();
private:
	XWndButtonString* CreateButtonAndAdd( LPCTSTR szLabel, 
										LPCTSTR szFont,
										float sizeFont,
										LPCTSTR szImgUp,
										LPCTSTR szImgDown,
										LPCTSTR szImgOff );
protected:
	GET_ACCESSOR( XWndTextString*, pTextTitle );
	GET_ACCESSOR( XWndTextString*, pTextDesc );
	GET_ACCESSOR( XBaseFontDat*, pFDText );
	GET_SET_ACCESSOR( XE::VEC2&, vLTMargin );
	GET_SET_ACCESSOR( XE::VEC2&, vRBMargin );
public:
	XWndAlert( float w, float h );
	XWndAlert( LPCTSTR szLayout, const char *cKey );
	XWndAlert( XWndAlertDelegate *pDelegate,
				float x, float y, float w, float h, 
				LPCTSTR szFrameImg,		// 3x3타일형태로 만들어진 프레임이미지
				LPCTSTR szImgUp,		// 버튼이미지
				LPCTSTR szImgDown,
				LPCTSTR szImgDisable,
				LPCTSTR szText, LPCTSTR szTitle=NULL, XWnd::xtAlert type=XWnd::xOK, XCOLOR col=XCOLOR_WHITE );
	XWndAlert( XWndAlertDelegate *pDelegate,
				float w, float h, 
				LPCTSTR szFrameImg,		// 3x3타일형태로 만들어진 프레임이미지
				LPCTSTR szImgUp,		// 버튼이미지
				LPCTSTR szImgDown,
				LPCTSTR szImgDisable,
				LPCTSTR szText, LPCTSTR szTitle=NULL, XWnd::xtAlert type=XWnd::xOK, XCOLOR col=XCOLOR_WHITE );
	virtual ~XWndAlert() { Destroy(); }
	//
	GET_ACCESSOR( XWndButtonString*, pButton1 );
	GET_ACCESSOR( XWndButtonString*, pButton2 );
	GET_SET_ACCESSOR( float, sizeFont );
	GET_SET_ACCESSOR( const _tstring&, strTitle );
	GET_SET_ACCESSOR( const _tstring&, strText );
//	GET_SET_ACCESSOR( const _tstring&, strFrameImg );
	GET_SET_ACCESSOR( const _tstring&, strImgUp );
	GET_SET_ACCESSOR( XCOLOR, Color );
	GET_SET_ACCESSOR( XWnd::xtAlert, Type );
	void SetSizePopup( float w, float h );
	inline void SetSizePopup( const XE::VEC2& vSize ) {
		SetSizePopup( vSize.w, vSize.h );
	}
	void SetstrFrameImg( LPCTSTR szFrameImg ) {
		m_strFrameImg = szFrameImg;
		LoadRes( szFrameImg );
	}
	XWnd* GetButtOk() {
//		return Find("butt.ok");
		return Find("butt.ok");
	}
	XWnd* GetButtCancel() {
		return Find( "butt.cancel" );
	}
	XWnd* GetButtYes() {
		return Find( "butt.yes" );
	}
	XWnd* GetButtNo() {
		return Find( "butt.no" );
	}
	XWndTextString* GetpWndTextDesc() {
		return m_pTextDesc;
	}
	XWndTextString* GetpWndTextTitle() {
		return m_pTextDesc;
	}
	//
	void CreateWndAlert( XWndAlertDelegate *pDelegate = nullptr );
	void CreateWndAlert( XWndAlertDelegate *pDelegate,
						float x, float y, float w, float h, 
						LPCTSTR szFrameImg,		// 3x3타일형태로 만들어진 프레임이미지
						LPCTSTR szImgUp,		// 버튼이미지
						LPCTSTR szImgDown,
						LPCTSTR szImgDisable,
						LPCTSTR _szText, 
						LPCTSTR _szTitle, 
						XWnd::xtAlert type, 
						XCOLOR col );
	int OnOk( XWnd *pWnd, DWORD p1, DWORD p2 );
	int OnCancel( XWnd *pWnd, DWORD p1, DWORD p2 );
	int OnYes( XWnd *pWnd, DWORD p1, DWORD p2 );
	int OnNo( XWnd *pWnd, DWORD p1, DWORD p2 );
	//
	LPCTSTR GetText( ID idText );
	XWndTextString* SetText( LPCTSTR szText );
	XWndButtonString* AddButtonDirectly( const char *cIdentifier, 
								LPCTSTR szLabel, 
								LPCTSTR szFont,
								float sizeFont, 
								LPCTSTR szImgUp,
								LPCTSTR szImgDown,
								LPCTSTR szImgOff = nullptr );
	XWndButtonString* AddButtonDirectly( XWnd::xtAlert type, 
								LPCTSTR szLabel, 
								LPCTSTR szFont,
								float sizeFont, 
								LPCTSTR szImgUp,
								LPCTSTR szImgDown,
								LPCTSTR szImgOff = nullptr );
	XWndButtonString* AddButtonWithHeader( const char *cIdentifier, 
								LPCTSTR szLabel,
								LPCTSTR szFont,
								float sizeFont, 
								LPCTSTR szPngHeader );
	XWndButtonString* AddButtonWithHeader( XWnd::xtAlert type, 
								LPCTSTR szLabel,
								LPCTSTR szFont,
								float sizeFont,
								LPCTSTR szPngHeader );
	void AdjustButtons( void );
	void OnLButtonUp( float lx, float ly ) {
		if( m_pButton1 == nullptr )	// 닫는버튼이 없으면 터치만으로 꺼진다.
			SetbDestroy( TRUE );
		XWndView::OnLButtonUp( lx, ly );
	}
	void SetCancelButton( const char *cKey );
	BOOL OnKeyUp( int keyCode ) override;
	void Update() override;
};


//////////////////////////////////////////////////////////////////////////
// 숫자를 받으면 카운터가 올라가면서 표시가 되고 끝까지 올라가면 이벤트 핸들러가 호출되는 컨트롤러
class XWndTextNumberCounter : public XWndTextString
{
	int m_numMax;
	float m_Divide;
	int m_numCurr;
	int m_numStart;
	BOOL m_bSwitch;
	BOOL m_bComma;
	bool m_bSymbol = false;		// 큰숫자를 K,M,B단위로 보여주기
	void Init() {
		m_numStart = 0;
		m_numCurr = 0;
		m_numMax = 0;
		m_Divide = 40.f;
		m_bSwitch = FALSE;
		m_bComma = FALSE;
	}
	void Destroy() {}
public:
	XWndTextNumberCounter( float x, float y, 
												 float w, float h,
												int numMax, 
												LPCTSTR szFont, float sizeFont,
												XCOLOR colText ) 
			: XWndTextString( XE::VEC2( x, y ), XE::VEC2(w,h), _T(""), szFont, sizeFont, colText ) {
				Init(); 
				m_numCurr = 0;
				m_numMax = numMax;
				UpdateText();
	}
	XWndTextNumberCounter( float x, float y, 
												int numMax, 
												LPCTSTR szFont, float sizeFont,
												XCOLOR colText ) 
			: XWndTextNumberCounter( x, y, 0, 0, numMax, szFont, sizeFont, colText ) { }
	XWndTextNumberCounter( int x, int y, 
												int numMax, 
												LPCTSTR szFont, float sizeFont,
												XCOLOR colText ) 
			: XWndTextNumberCounter( (float)x, (float)y, 0, 0, numMax, szFont, sizeFont, colText ) {}
	XWndTextNumberCounter( const XE::VEC2& vPos
												, int numMax
												, LPCTSTR szFont = FONT_NANUM
												, float sizeFont = 20.f
												, XCOLOR colText = XCOLOR_WHITE ) 
		: XWndTextNumberCounter( vPos.x, vPos.y, 0, 0, numMax, szFont, sizeFont, colText ) {}
	XWndTextNumberCounter( const XE::VEC2& vPos
												, LPCTSTR szFont = FONT_NANUM
												, float sizeFont = 20.f
												, XCOLOR colText = XCOLOR_WHITE ) 
		: XWndTextNumberCounter( vPos.x, vPos.y, 0, 0, 0, szFont, sizeFont, colText ) {}
	XWndTextNumberCounter( const XE::VEC2& vPos
												, const XE::VEC2& vSize
												, LPCTSTR szFont = FONT_NANUM
												, float sizeFont = 20.f
												, XCOLOR colText = XCOLOR_WHITE ) 
		: XWndTextNumberCounter( vPos.x, vPos.y, vSize.w, vSize.h, 0, szFont, sizeFont, colText ) {}
	virtual ~XWndTextNumberCounter() { Destroy(); }
	//
	GET_SET_ACCESSOR_CONST( float, Divide );
	GET_SET_ACCESSOR_CONST( int, numMax );
	GET_SET_ACCESSOR_CONST( BOOL, bComma );
	GET_SET_ACCESSOR_CONST( bool, bSymbol );
	// 스위치가 켜져야 작동을 시작한다.
	void SetOn( int numStart ) {
		m_bSwitch = TRUE;
		m_numStart = numStart;
		m_numCurr = numStart;
	}
	BOOL IsOn( void ) const {
		return m_bSwitch;
	}
	/// 직접 현재값과 맥스값을 수정한다. 동기화 업데이트시에 사용한다.
	void SetCurrNum( int num ) {
		m_numCurr = num;
		m_numMax = num;
		UpdateText();
	}
	void UpdateText( void );
	//
	int Process( float dt ) override;
};

// iOS방식의 노티피케이션 알림창
class XWndTextString;
class XWndNotification : public XWndView
{
	CTimer m_Timer;
	void Init() {
	}
	void Destroy();
	template<typename T>
	void SetSize( T w, T h ) {
		float nw = (float)w, nh = (float)h;
		if( m_nBlockSize > 0 )
		{
			nw = (float)( m_nBlockSize * ( (int)w / m_nBlockSize ) );
			nh = (float)( m_nBlockSize * ( (int)h / m_nBlockSize ) );	// 블럭사이즈에 떨어지도록 크기를 조정한다
		}
		SetSizeLocal( nw, nh );
		SetPosLocal( ( XE::GetGameWidth() / 2.f ) - nw / 2.f, 5.f );
	}
public:
	XWndNotification() { Init(); }
	XWndNotification( float w, float h, LPCTSTR szTitle, LPCTSTR szMsg, XBaseFontDat *pFontDat );
	virtual ~XWndNotification() { Destroy(); }

	virtual int Process( float dt ) {
		if( m_Timer.IsOver() )
			SetbDestroy( TRUE );
		return XWndView::Process( dt );
	}
	virtual void Draw( void );
};

//////////////////////////////////////////////////////////////////
// 등장 이펙트

// 점점 커지거나 작아지는 이펙트
class XWndEffectGradually : virtual public XWnd
{
	BOOL m_bToBig;		// 1:커지기모드 0:작아지기모드
//	XE::VEC2 m_vPos;		// 이펙트중 좌표
	XE::VEC2 m_vPosStart, m_vPosEnd;	// 출발좌표 목표좌표
	CTimer m_Timer;			// 이펙트 보간 타이머
	float m_fSec;
	int m_State;				// 0:아직시작전 1:플레이중 2:끝나서멈춰있음
	void Init() {
		m_fSec = 0;
		m_State = 0;
		m_bToBig = 1;
	}
public:
	XWndEffectGradually() { Init(); }
	virtual ~XWndEffectGradually() {}
	//
	GET_ACCESSOR( float, fSec );
	//
	BOOL IsPlaying( void ) { return (m_State==1)? TRUE : FALSE; }	// 이펙트 중인가
	BOOL IsFinish( void ) { return (m_State==2)? TRUE : FALSE; }	// 이펙트가 끝났나
	BOOL IsReady( void ) { return (m_State==0)? TRUE : FALSE; }	// 아직 이펙트 시작전임
	void SetReady( void ) { m_State = 0; }		// 시작전 대기상태로 만든다
	void SetRestart( void ) {
		SetStart( m_bToBig, m_vPosStart, m_vPosEnd, m_fSec );
	}
	void SetStart( BOOL bToBig, float sec ) {
		SetStart( bToBig, GetPosLocal(), GetPosLocal(), sec );
	}
	virtual void SetStart( BOOL bToBig, const XE::VEC2& vStart, const XE::VEC2& vEnd, float sec ) {	// bOn:0작아기지 1커지기
		m_Timer.Set( sec );
		m_fSec = sec;
		m_vPosStart = vStart;
		m_vPosEnd = vEnd;
		m_bToBig = bToBig;
		m_State = 1;		// 커지기 시작함
		SetPosLocal( vStart );
		if( bToBig ) {
			SetAlphaLocal( 0 );
			SetScaleLocal( 0 );			// 첨엔 작아져있다
		}
		else {
			SetAlphaLocal( 1.0f );
			SetScaleLocal( 1.0f );
		}
	}
	virtual int Process( float dt );
};

//////////////////////////////////////////////////////////////////
// 슬라이더 컨트롤
class XWndSlider : public XWnd
{
	int m_Push;
	void Init() {
		m_Min = m_Max = 0;
		m_Curr = 0;
		m_Push = FALSE;
	}
	void Destroy() {}
protected:
	float m_Min, m_Max;		// 슬라이더 최소 최대값
	float m_Curr;
public:
	template<typename T>
	XWndSlider( T x, T y, T w, T h, float min, float max, float curr=0 ) 
		: XWnd( x, y, w, h ) { 
			Init(); 
			m_Min = min;
			m_Max = max;
			if( curr == 0 )
				m_Curr = min;
			else
				m_Curr = curr;
	}
	virtual ~XWndSlider() { Destroy(); }
	GET_ACCESSOR( float, Curr );

	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
//	virtual void OnNCMouseMove( float lx, float ly );
//	virtual void OnNCLButtonUp( float lx, float ly );
	virtual void Draw( void ) {}
};

// menuitem base class
class XWndMenuItem : public XWnd
{
	void Init() {}
	void Destroy() {}
public:
	XWndMenuItem() {}
	virtual ~XWndMenuItem() {}
	virtual void OnLButtonDown( float lx, float ly ) {}
	virtual void OnLButtonUp( float lx, float ly ) {}
};

//#include "XList.h"
class XWndTab : public XWnd
{
protected:
	struct TAB {
		ID idTab;
		_tstring strImg;
		_tstring strLabel;
		float x, y;
		float w, h;
		XSurface *psfcImg;
		TAB() {
			idTab = 0;
			x = y = 0;
			w = h = 0;
			psfcImg = NULL;
		}
	};
private:
	XList<TAB*> m_listTab;
	TAB *m_pSelectedTab;
	XBaseFontObj *m_pFontObj;
	void Init() {
		m_pSelectedTab = NULL;
		m_pFontObj = NULL;
	}
	void Destroy();
public:
	XWndTab( float x, float y, float w, float h, LPCTSTR szFont );
	virtual ~XWndTab() { Destroy(); }
	//
	GET_ACCESSOR( TAB*, pSelectedTab );
	TAB* GetTab( ID idTab );
	void AddTab( ID idTab, LPCTSTR szLabel, LPCTSTR szImg, float x, float y, float w, float h );
	//
	virtual void DrawTabLabel( TAB *pTab );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnClickedTab( TAB *pTab ) {}
};

////////////////////////////////////////////////////////////////
class XDelegateWndTab;
class XWndTab2 : public XWnd
{
public:
	struct TAB {
		ID idRootLayout;	// 탭에 레아아웃이 있을때 레이아웃의 루트 윈도우 아이디
		ID idTab;
		_tstring strImgOn;
		_tstring strImgOff;
		_tstring strLabel;
		std::string cstrLayoutKey;	// 탭안에 들어갈 레이아웃 키
		float x, y;
		float w, h;
		XSurface *psfcImgOn;		// 탭부분 이미지
		XSurface *psfcImgOff;		// 탭부분 이미지
		TAB() {
			idRootLayout = 0;
			idTab = 0;
			x = y = 0;
			w = h = 0;
			psfcImgOn = NULL;
			psfcImgOff = NULL;
		}
	};
protected:
private:
	XDelegateWndTab *m_pDelegate;
	XList<TAB*> m_listTab;
	TAB *m_pSelectedTab;
	XBaseFontObj *m_pFontObj;
	XSurface *m_psfcBg;
	XLayout *m_pLayout;		// 탭에 레이아웃이 지정되어있을경우 레이아웃을 찾을 객체
	ID m_idStartTab;		// 시작탭 아이디
	void Init() {
		m_pDelegate = NULL;
		m_pSelectedTab = NULL;
		m_pFontObj = NULL;
		m_psfcBg = NULL;
		m_pLayout = NULL;
		m_idStartTab = 1;
	}
	void Destroy();
public:
	XWndTab2( float x, float y, LPCTSTR szImg, LPCTSTR szFont, XLayout *pLayout=NULL );
	XWndTab2( float x, float y, float w, float h, LPCTSTR szFont, XLayout *pLayout=NULL );
	virtual ~XWndTab2() { Destroy(); }
	//
	GET_ACCESSOR( TAB*, pSelectedTab );
	GET_SET_ACCESSOR( XDelegateWndTab*, pDelegate );
	GET_SET_ACCESSOR( int, idStartTab );
	TAB* GetTab( ID idTab );
	void AddTab( ID idTab, LPCTSTR szLabel, LPCTSTR szImgOn, LPCTSTR szImgOff, float x, float y, float w, float h, const char *cLayoutKey=NULL );
	//
	virtual void DrawTabLabel( TAB *pTab );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	// 이제 이거 사용하지 말고 델리게이트를 쓸것.
	virtual void OnClickedTab( TAB *pTab ) {}
	virtual BOOL OnCreate( void ) {
		CallEvent( "OnEventAddWnd" );
		return TRUE;
	}
	int DoSelectTab( ID idTab );
};
////////////////////////////////////////////////////////////////
class XDelegateWndTab : public XBaseDelegate
{
	void Init() {}
	void Destroy() {}
public:
	XDelegateWndTab() { Init(); }
	virtual ~XDelegateWndTab() { Destroy(); }
	//
	virtual void OnDelegateClickedTab( XWndTab2 *pWndTab, XWndTab2::TAB *pTab ) {}
};
//////////////////////////////////////////////////////////////////////////
class XWndRect : public XWnd
{
	XCOLOR m_Color;
public:
	XWndRect( float x, float y, float w, float h, XCOLOR col ) 
		: XWnd( x, y, w, h )	{
			m_Color = col;
	}
	XWndRect( const XE::VEC2& v, const XE::VEC2& vSize, XCOLOR col )
		: XWnd( v, vSize )	{
		m_Color = col;
	}
	virtual ~XWndRect() {}
	void Draw( void ) {
		XE::VEC2 vPos = GetPosFinal();
		XE::VEC2 vSize = GetSizeFinal();
		BYTE r = XCOLOR_RGB_R( m_Color );
		BYTE g = XCOLOR_RGB_G( m_Color );
		BYTE b = XCOLOR_RGB_B( m_Color );
		BYTE a = XCOLOR_RGB_A( m_Color );
		float fa = a / 255.f;
		fa *= GetAlphaLocal();
		a = (BYTE)(fa * 255.f);
		XCOLOR col = XCOLOR_RGBA( r, g, b, a );
		GRAPHICS->FillRectSize( vPos, vSize, col );
		XWnd::Draw();
	}
};
//////////////////////////////////////////////////////////////////////////
/**
 기본 바디가 있고 누르면 기본바디보다 더큰 슬라이드가 아래로 내려와서 더 많은 정보를 보여줄수 있는 컨트롤.
 */
class XWndSlideDown : public XWnd
{
	XWndImage *m_pBody;
	XWndImage *m_pSlider;
	int m_State;
	XInterpolationObjDeAccel m_ipoSlider;
	//	XInterpolationObjCatmullrom m_ipoSlider;
	XE::VEC2 m_vAdjSlide;		// 슬라이더가 내려왔을때 어디까지 내려올것인지 위치보정값.
	float m_secSlide;				// 슬라이더가 올라갔다 내려갔다 하는 시간.
	XLayout *m_pLayout;		// 레이아웃 객체가 연결되어있을때.
	std::string m_strSliderLayout;		// 슬라이더뷰에 올려질 레이아웃의 노드이름.
	XE::VEC2 m_vSliderLayout;			// 슬라이더 레이아웃의 좌상귀 좌표
	XE::VEC2 m_vAdjustLT;		// 부모뷰포트와의 걸침으로 인해 발생하는 보정치.
	void Init() {
		m_pBody = NULL;
		m_pSlider = NULL;
		m_State = 0;
		m_secSlide = 0.3f;
		m_pLayout = NULL;
	}
	void Destroy() {}
public:
	// 이미지 패스는 항상 UI폴더
	XWndSlideDown( float x, float y,
				  LPCTSTR szImgBody,	// 기본 바디가 될 이미지
				  LPCTSTR szImgSlide );
	virtual ~XWndSlideDown() { Destroy(); }
	//
	GET_SET_ACCESSOR( const XE::VEC2&, vAdjSlide );
	GET_SET_ACCESSOR( float, secSlide );
	GET_SET_ACCESSOR( XLayout*, pLayout );
	GET_SET_ACCESSOR( std::string&, strSliderLayout );
	GET_SET_ACCESSOR( const XE::VEC2&, vSliderLayout );
	//
	virtual int Process( float dt );
	virtual void Draw( void );
	XE::VEC2 GetPosFinal( void ) const {
		return XE::VEC2(0) + m_vAdjustLT;		// 뷰포트 객체이므로 자식들을 위해 항상 0을 돌려줘야 한다.
	}
	virtual void OnLButtonDown( float lx, float ly );
	//	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnStartSlideOpen( void ) {}
	virtual void OnStartSlideClose( void ) {}
	virtual void OnFinishSlideOpen( void ) {}
	virtual void OnFinishSlideClose( void ) {}
	//
	void SetTextBody( LPCTSTR szText, LPCTSTR szFont, float sizeFont, XCOLOR color, xFONT::xtStyle style );
};


/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/12/04 18:22
*****************************************************************/
class XWndParticleLayer : public XWnd
{
public:
	XWndParticleLayer( const char *cKeyParticle, const XE::VEC2& vPos, float secLife, XParticleDelegate *pDelegate );
	XWndParticleLayer( float secLife, XParticleDelegate *pDelegate ) 
		: XWndParticleLayer( nullptr, XE::VEC2(0), secLife, pDelegate ) {}
	virtual ~XWndParticleLayer() { Destroy(); }
	// get/setter
	// public member
	void AddEmitter( const char *cKeyParticle, const XE::VEC2& vPos );
	/**
	 @brief
	 @param sec 
		>0:sec시간후에 this파괴, 
		0:파티클한번 방출하고 파티클이 다 없어지면 객체 자동삭제
		-1: this의 파괴는 외부에서 수동으로 설정.
	*/
	void SetsecLife( float sec ) {
		XASSERT( sec > 0 || sec < 0 );		// 일단은 생존시간을 무조건 정해주도록.
		m_timerLife.Set( sec );
	}
	void DestroyEmittersAndParticles();
	void SetpDelegate( XParticleDelegate *pDelegate );
private:
	// private member
	XParticleMng *m_pMng = nullptr;
	CTimer m_timerLife;
private:
	// private method
	void Init() {}
	void Destroy();
	int Process( float dt ) override;
	void Draw() override;
	BOOL RestoreDevice() override;
}; // class XWndParticleLayer

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/23 18:44
*****************************************************************/
class XWndPlaySound : public XWnd
{
public:
	XWndPlaySound( ID idSnd, bool bOneSec = false ) { 
		Init(); 
		XBREAK( idSnd == 0 );
		m_idSnd = idSnd;
	}
	virtual ~XWndPlaySound() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	ID m_idSnd = 0;
	bool m_bOneSec = false;		// OpenPlaySoundOneSec()로 사운드플 플레이함.
private:
	// private method
	void Init() {}
	void Destroy() {}
	int Process( float dt ) override;
}; // class XWndPlaySound

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/23 18:46
*****************************************************************/
class XWndPlayMusic : public XWnd
{
public:
	XWndPlayMusic( const std::string& strBgm ) { 
		Init(); 
		XBREAK( strBgm.empty() );
		m_strBgm = strBgm;
	}
	virtual ~XWndPlayMusic() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	std::string m_strBgm;
private:
	// private method
	void Init() {}
	void Destroy() {}
	int Process( float dt ) override;
}; // class XWndPlayMusic
