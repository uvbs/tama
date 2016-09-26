#pragma once
//#include "XFramework/Game/XEWndWorldImage.h"

class XBattleField;
class XECompCamp;
class XEWndWorldImage;
class XEWndWorld;;
class XEWorld;
class XEWorldCamera;
class XWndBatchRender;
class XSprObj;
#define WND_WORLD	XWndBattleField::sGet()

////////////////////////////////////////////////////////////////
// class XWndBattleField : public XEWndWorldImage
class XWndBattleField : public XWnd
{
public:
	static XWndBattleField* sGet();
	static XEWndWorld* sGetObjLayer() {
		return sGet()->GetpObjLayer();
	}
  static void sDestroy() {
    SAFE_DELETE( s_pInstance );
  }
private:
	static XWndBattleField *s_pInstance;
//	XBattleField *m_prefBattleField;
#ifdef _CHEAT
	XE::VEC2 m_vwMouse;		// 마우스 위치 월드좌표
	XE::VEC2 m_vsMouse;		// 마우스 스크린 좌표
	std::shared_ptr<XSprObj> m_spsoMouse;			// 마우스 위치를 따라다니는 테스트용
#endif // _CHEAT
	XSPWorld m_spWorld;
	std::shared_ptr<XEWorldCamera> m_spCamera;		// 바인딩된 카메라
	XEWndWorldImage* m_pBgLayer = nullptr;
	XEWndWorld* m_pObjLayer = nullptr;
	XWndBatchRender* m_pUnitUILayer = nullptr;
	XSPSquad m_spTempSelect;		///< 임시선택
	XSPSquad m_spSelectSquad;		///< 선택된 아군 부대
	XSPSquad m_spTempSelectEnemy;	///< 적진영 임시선택
	XE::VEC2 m_vTouch;				///< 터치다운시 좌표
//	_tstring m_strBg;
	void Init() {
//		m_prefBattleField = NULL;
	}
	void Destroy();
	void Release() override {
		m_spTempSelect.reset();
		m_spSelectSquad.reset();
		m_spTempSelectEnemy.reset();
	}
public:
	XWndBattleField( XSPWorld spWorld );
	virtual ~XWndBattleField() { Destroy(); }
	///< 
//	GET_ACCESSOR( XBattleField*, prefBattleField );
	GET_SET_ACCESSOR( XSPSquad, spSelectSquad );
	GET_ACCESSOR( XSPSquad, spTempSelect );
	GET_ACCESSOR( XSPSquad, spTempSelectEnemy );
	GET_ACCESSOR( XEWndWorld*, pObjLayer );
	GET_ACCESSOR( XWndBatchRender*, pUnitUILayer );
	GET_SET_ACCESSOR( std::shared_ptr<XEWorldCamera>, spCamera );
	GET_SET_ACCESSOR2( XSPWorld, spWorld );
	ID GetidSelectSquad( void );
	/// 선택한 부대를 해제시킨다.
	void ClearSelectSquad( void ) {
		m_spSelectSquad.reset();
	}
	//
	virtual int Process( float dt );
	virtual void Draw( void );
//	virtual XEWorld* OnCreateWorld( const XE::VEC2& vwSize );
	virtual void DrawDebugInfo( float x, float y, XCOLOR col, XBaseFontDat *pFontDat );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	BOOL OnCreate() override;
	XSPSquad PickingSquad( float lx, float ly, BIT bitCamp );
	BOOL IsSelectedSquad( XSquadObj *pSquadObj );
	float GetscaleCamera() const;
	void SetScaleCamera( float scale );
// 	XSPWorldConst GetspWorld();
// 	XSPWorld GetspWorldMutable();
	std::shared_ptr<const XBattleField> GetspBattleField();
	std::shared_ptr<XBattleField> GetspBattleFieldMutable();
	void SetFocus( const XE::VEC2& vFocus );
	XE::VEC2 GetPosWorldToWindow( const XE::VEC3& vwPos, float *pOutScale = nullptr );
	XE::VEC2 GetPosWindowToWorld( const XE::VEC2& vlsPos );
	XE::VEC2 GetvwCamera() const;
	XEWndWorldImage* AddBgLayer( XSurface* psfcBg, XSPWorld spWorld );
	XEWndWorld* AddObjLayer( XSPWorld spWorld );
	XWndBatchRender* AddUnitUILayer( XSPWorld spWorld );
	void OnZoom( float scale, float lx, float ly );
#ifdef _CHEAT
	void OnReset( bool bReCreate );
#endif // _CHEAT
};
