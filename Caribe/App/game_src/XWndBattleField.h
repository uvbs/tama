#pragma once
#include "XFramework/Game/XEWndWorldImage.h"

class XBattleField;
class XECompCamp;

#define WND_WORLD	XWndBattleField::sGet()

////////////////////////////////////////////////////////////////
class XWndBattleField : public XEWndWorldImage
{
public:
	static XWndBattleField* sGet();
  static void sDestroy() {
    SAFE_DELETE( s_pInstance );
  }
private:
	static XWndBattleField *s_pInstance;
	XBattleField *m_prefBattleField;
	XSPSquad m_spTempSelect;		///< 임시선택
	XSPSquad m_spSelectSquad;		///< 선택된 아군 부대
	XSPSquad m_spTempSelectEnemy;	///< 적진영 임시선택
	XE::VEC2 m_vTouch;				///< 터치다운시 좌표
	void Init() {
		m_prefBattleField = NULL;
	}
	void Destroy();
	void Release() override {
		m_spTempSelect.reset();
		m_spSelectSquad.reset();
		m_spTempSelectEnemy.reset();
	}
public:
	XWndBattleField();
	virtual ~XWndBattleField() { Destroy(); }
	///< 
	GET_ACCESSOR( XBattleField*, prefBattleField );
	GET_SET_ACCESSOR( XSPSquad, spSelectSquad );
	GET_ACCESSOR( XSPSquad, spTempSelect );
	GET_ACCESSOR( XSPSquad, spTempSelectEnemy );
	ID GetidSelectSquad( void );
	/// 선택한 부대를 해제시킨다.
	void ClearSelectSquad( void ) {
		m_spSelectSquad.reset();
	}
	//
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual XEWorld* OnCreateWorld( const XE::VEC2& vwSize );
	virtual void DrawDebugInfo( float x, float y, XCOLOR col, XBaseFontDat *pFontDat );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	BOOL OnCreate() override;
	XSPSquad PickingSquad( float lx, float ly, BIT bitCamp );
	BOOL IsSelectedSquad( XSquadObj *pSquadObj );
#ifdef _CHEAT
	void OnReset( bool bReCreate );
#endif // _CHEAT
};
