#pragma once
#include "Select.h"

class XSprite;
class XSprObj;
class XBaseElem;
class XBaseUndo : public XBaseElem
{
private:
	CString m_strText;
	void Init() {
		m_pSprObj = NULL;
	}
	void Destroy() {}
protected:
	XSprObj *m_pSprObj;		// 이 언두행동이 어느SprObj에 속하는가.(아직은 필요없지만 다중SprObj를 읽어서 뭔가를 할때를 대비)
public:
	XBaseUndo( XSprObj *pSprObj, LPCTSTR szText=_T("") ) { Init(); m_pSprObj = pSprObj; m_strText = szText; }
	virtual ~XBaseUndo() { Destroy(); }
	GET_ACCESSOR(XSprObj*, pSprObj);

	virtual BOOL DoUndo() = 0;
	virtual BOOL DoRedo() = 0;
	virtual CString GetText() { return m_strText; }
	void SetText( LPCTSTR szText ) { m_strText = szText; }
};

class XUndoKey : public XBaseUndo
{
//	ACTIONID m_idAction;		// m_pKey의 m_pAction이 깨졌을수 있기때문에 id를 보관하고 있다가 이걸로 찾는게 안전하다
	void Init() {}
	void Destroy();
public:
	XUndoKey( XSprObj *pSprObj, XSPAction spAction, XBaseKey* pUndoKey, LPCTSTR szText=_T("") );
	virtual ~XUndoKey() { Destroy(); }

	GET_ACCESSOR( XBaseKey*, pKey );
	virtual BOOL DoUndo() = 0; //{return TRUE;}		// 언두행동
	virtual BOOL DoRedo() = 0; //{return TRUE;}		// 리두행동
	virtual CString GetText();
//	XBaseKey* FindIDKey( XBaseKey *pKey = NULL ) ;
//	XSPAction GetKeyAction( XBaseKey *pKey = NULL ) ;
protected:
	XBaseKey* m_pKey = nullptr;	// 키 카피본
	XSPAction m_spAction;		// 언두하려는 키가 어디액션꺼였는지를 알아야 복구가 된다.
protected:
	XBaseKey* CreateCopy( XBaseKey* pKeySrc );
};
// 키가 생성될때의 언두행동(키삭제)과 리두행동(키재생성)
class XUndoCreateKey : public XUndoKey
{
	void Init() {	
		if( XBaseUndo::GetText().IsEmpty() )
			SetText( _T("키 생성") );
	}
	void Destroy() { 	}
public:
	XUndoCreateKey( XSprObj *pSprObj, XSPAction spAction, XBaseKey *pCreateKey, LPCTSTR szText=_T("") ) 
		: XUndoKey( pSprObj, spAction, pCreateKey, szText ) {	Init();	}
	virtual ~XUndoCreateKey() { Destroy(); }

	virtual BOOL DoUndo() override;		// 언두행동(키삭제)
	virtual BOOL DoRedo() override;		// 리두행동(키재생성)
};

// 키가 삭제될때의 언두행동(키되살림)과 리두행동(키재삭제)
class XUndoDelKey : public XUndoKey
{
	void Init() {
		if( XBaseUndo::GetText().IsEmpty() )
			SetText( _T("키 삭제") );
	}
	void Destroy() { 
	}
public:
	XUndoDelKey( XSprObj *pSprObj, XSPAction spAction, XBaseKey *pDelKey, LPCTSTR szText=_T("") ) 
		: XUndoKey( pSprObj, spAction, pDelKey, szText ) { 	Init();	}
	virtual ~XUndoDelKey() { Destroy(); }

	virtual BOOL DoUndo() override;		// 언두행동(키되살림)
	virtual BOOL DoRedo() override;		// 리두행동(키재삭제)
};


// 키값이 바뀔때 언두행동(백업받은값 회복)과 리두행동(
class XUndoKeyChange : public XUndoKey
{
	XBaseKey* m_pKeyAfter;		// 값이 바뀌고 난후 키복사본(리두에 쓰인다)
	XSPActObj m_spActObj;
	void Init() {
		m_pKeyAfter = NULL;
		if( XBaseUndo::GetText().IsEmpty() )
			SetText( _T("키 값바꿈") );
	}
	void Destroy();
public:
	XUndoKeyChange(XSprObj *pSprObj, XSPActObj spActObj, XBaseKey *pKey, LPCTSTR szText = _T(""));

	virtual BOOL DoUndo() override;		// 언두행동(Restore)
	virtual BOOL DoRedo() override;		// 리두행동(Save)
	virtual CString GetText();
	void SetKeyAfter( XBaseKey *pKeyAfter );
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                      XUndoLayer  
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
class XUndoLayer : public XBaseUndo
{
//	LAYER_INFO m_LayerInfo;	// 레이어 인포 카피본
	void Init() {}
	void Destroy() {
//		SAFE_DELETE( m_pLayer );
	}
protected:
	XSPBaseLayer m_spLayer;		// 레이어 카피본
public:
	XUndoLayer( XSprObj *pSprObj, XSPBaseLayer spUndoLayer, LPCTSTR szText=_T("") ) : XBaseUndo( pSprObj, szText ) { 
		Init(); 
//		m_spLayer = spUndoLayer->CopyDeep();			// 언두할 레이어의 복사본을 만든다
		m_spLayer = spUndoLayer;
//		m_spLayer->SetidSerial( spUndoLayer->GetidSerial() );							// 언두할 레이어랑 같은레이어기때문에 아이디를 맞춰준다
// 		LAYER_INFO *pLayerInfoSrc = m_pSprObj->GetspAction()->FindLayerInfo( pUndoLayer );
// 		m_LayerInfo = *pLayerInfoSrc;
	}
	virtual ~XUndoLayer() { Destroy(); }

	GET_ACCESSOR( XSPBaseLayer, spLayer );
//	GET_ACCESSOR( LAYER_INFO&, LayerInfo );
	virtual BOOL DoUndo() {return TRUE;}		// 언두행동
	virtual BOOL DoRedo() {return TRUE;}		// 리두행동
	virtual CString GetText();
	XSPBaseLayer FindIDLayer( XSPBaseLayer spLayer = NULL ) ;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// 레이어가 생성될때의 언두행동(레이어삭제)과 리두행동(레이어재생성)
class XUndoCreateLayer : public XUndoLayer
{
	void Init() {	
		if( XBaseUndo::GetText().IsEmpty() )
			SetText( _T("레이어 생성") );
	}
	void Destroy() { 	}
public:
	XUndoCreateLayer( XSprObj *pSprObj, XSPBaseLayer spCreateLayer, LPCTSTR szText=_T("") ) 
		: XUndoLayer( pSprObj, spCreateLayer, szText ) {	Init();	}
	virtual ~XUndoCreateLayer() { Destroy(); }

	virtual BOOL DoUndo() override;		// 언두행동(레이어삭제)
	virtual BOOL DoRedo() override;		// 리두행동(레이어재생성)
};

// 레이어가 삭제될때의 언두행동(레이어되살림)과 리두행동(레이어재삭제)
class XUndoDelLayer : public XUndoLayer
{
	void Init() {
		if( XBaseUndo::GetText().IsEmpty() )
			SetText( _T("레이어 삭제") );
	}
	void Destroy() { 
	}
public:
	XUndoDelLayer( XSprObj *pSprObj, XSPBaseLayer spDelLayer, LPCTSTR szText=_T("") ) 
		: XUndoLayer( pSprObj, spDelLayer, szText ) { Init();	}
	virtual ~XUndoDelLayer() { Destroy(); }

	virtual BOOL DoUndo() override;		// 언두행동(레이어되살림)
	virtual BOOL DoRedo() override;		// 리두행동(레이어재삭제)
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                      XUndoObjAct
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
class XUndoAction : public XBaseUndo
{
	void Init() {}
	void Destroy() {}
protected:
	XSPAction m_spAction;
 	ID m_idAct;		// Undo할때 위 포인터가 널이 되므로 idAct는 따로 빼놔야 한다.
public:
	XUndoAction( XSprObj *pSprObj, XSPAction spUndoAction, LPCTSTR szText=_T("") );
	virtual ~XUndoAction() { Destroy(); }
	GET_ACCESSOR( XSPAction, spAction );
	GET_ACCESSOR( ID, idAct );
};

// 액션값 전체를 복구해주는 언두
class XUndoActionChange : public XUndoAction
{
	XSPAction m_spActionAfter;
	void Init() {
		if( XBaseUndo::GetText().IsEmpty() )
			SetText( _T("액션 값바꿈") );
	}
	void Destroy() {
// 		SAFE_DELETE( m_pActionAfter );
// 		SAFE_DELETE( m_pAction );
		m_spAction = nullptr;
	}
public:
	XUndoActionChange( XSprObj *pSprObj, XSPAction spUndoAction, LPCTSTR szText=_T("") );
	virtual ~XUndoActionChange() { Destroy(); }

	GET_ACCESSOR( XSPAction, spActionAfter );
	void SetActionAfter( XSPAction spActionAfter );
	virtual BOOL DoUndo() override;		// 언두행동
	virtual BOOL DoRedo() override;		// 리두행동
};
// class XUndoObjAct : public XUndoAction
// {
// 	void Init() {
// 		m_pObjAct = NULL;
// 		m_idAct = 0;
// 	}
// 	void Destroy();
// protected:
// 	XObjAct *m_pObjAct;			// 원본액션을 삭제하면 실제론 삭제하진 않고 포인터만 이리로 옮겨온다. 언두하면 다시 되돌려줌
// 	DWORD m_idAct;				// Undo할때 위 포인터가 널이 되므로 idAct는 따로 빼놔야 한다.
// public:
// // 	XUndoObjAct( XSprObj *pSprObj, XObjAct *pUndoObjAct, LPCTSTR szText=_T("") );
// 	XUndoObjAct( XSprObj *pSprObj, SPAction spActUndo, LPCTSTR szText = _T( "" ) );
// 	virtual ~XUndoObjAct() { Destroy(); }
// 
// 	GET_ACCESSOR( XObjAct*, pObjAct );
// 	GET_ACCESSOR( DWORD, idAct );
// 	virtual BOOL DoUndo() {return TRUE;}		// 언두행동
// 	virtual BOOL DoRedo() {return TRUE;}		// 리두행동
// //	virtual CString GetText() { 
// //		return XBaseUndo::GetText() + _T("(") + GetspAction()->GetszActName() + _T(')'); 
// //	}
// };
///////////////////////////////////////////////////////////////////////////////////////////////////////
// 액션이 삭제될때의 언두행동(액션되살림)과 리두행동(액션재삭제)
class XUndoDelAction : public XUndoAction
{
	void Init() {
		if( XBaseUndo::GetText().IsEmpty() )
			SetText( _T("액션 삭제") );
	}
	void Destroy() {}
public:
// 	XUndoDelAction( XSprObj *pSprObj, SPAction spActDel, LPCTSTR szText=_T("") ) : XUndoObjAct( pSprObj, pDelObjAct, szText ) { 	Init();	}
	XUndoDelAction( XSprObj *pSprObj, XSPAction spActDel, LPCTSTR szText = _T( "" ) ) 
		: XUndoAction( pSprObj, spActDel, szText ) { 
		Init(); 
	}
	virtual ~XUndoDelAction() { Destroy(); }

	virtual BOOL DoUndo() override;		// 언두행동(액션되살림)
	virtual BOOL DoRedo() override;		// 리두행동(액션재삭제)
};
// class XUndoDelAction : public XUndoObjAct
// {
// 	void Init() {
// 		if( XBaseUndo::GetText().IsEmpty() )
// 			SetText( _T("액션 삭제") );
// 	}
// 	void Destroy() { 
// 	}
// public:
// // 	XUndoDelAction( XSprObj *pSprObj, SPAction spActDel, LPCTSTR szText=_T("") ) : XUndoObjAct( pSprObj, pDelObjAct, szText ) { 	Init();	}
// 	XUndoDelAction( XSprObj *pSprObj, SPAction spActDel, LPCTSTR szText = _T( "" ) ) 
// 		: XUndoObjAct( pSprObj, spActDel, szText ) { 
// 		Init(); 
// 	}
// 	virtual ~XUndoDelAction() { Destroy(); }
// 
// 	virtual BOOL DoUndo() override;		// 언두행동(액션되살림)
// 	virtual BOOL DoRedo() override;		// 리두행동(액션재삭제)
// };

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                      XUndoSprite
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
class XUndoSprite : public XBaseUndo
{
	void Init() {
		m_pSprite = m_pSpriteBackup = NULL;
	}
	void Destroy();
protected:
	XSprite *m_pSprite;
	XSprite *m_pSpriteBackup;			// m_pSprite는 널이 되도 이건 변하지 않는다
public:
	XUndoSprite( XSprObj *pSprObj, XSprite *pUndoSprite, LPCTSTR szText=_T("") ) : XBaseUndo( pSprObj, szText ) { 
		Init();
		m_pSprite = m_pSpriteBackup = pUndoSprite;		// 스프라이트 언두도 원본을 삭제하지 않는다
	}
	virtual ~XUndoSprite() { Destroy(); }

	GET_ACCESSOR( XSprite*, pSprite );

	virtual BOOL DoUndo() = 0	;	// 언두행동
	virtual BOOL DoRedo() = 0;		// 리두행동
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// 스프라이트이 삭제될때의 언두행동(스프라이트되살림)과 리두행동(스프라이트재삭제)
class XUndoDelSprite : public XUndoSprite
{
	void Init() {
		if( XBaseUndo::GetText().IsEmpty() )
			SetText( _T("스프라이트 삭제") );
	}
	void Destroy() { 	}
public:
	XUndoDelSprite( XSprObj *pSprObj, XSprite *pDelSprite, LPCTSTR szText=_T("") ) 
		: XUndoSprite( pSprObj, pDelSprite, szText ) { 	Init();	}
	virtual ~XUndoDelSprite() { Destroy(); }

	virtual BOOL DoUndo() override;		// 언두행동(스프라이트되살림)
	virtual BOOL DoRedo() override;		// 리두행동(스프라이트재삭제)
};

// 스프라이트값이 바뀔때 언두행동(백업받은값 회복)과 리두행동(
class XUndoSpriteChange : public XUndoSprite
{
	XSprite* m_pSpriteAfter;		// 값이 바뀌고 난후 스프라이트복사본(리두에 쓰인다)
	void Init() {
		m_pSpriteAfter = NULL;
		if( XBaseUndo::GetText().IsEmpty() )
			SetText( _T("스프라이트 값바꿈") );
	}
	void Destroy();
public:
	XUndoSpriteChange( XSprObj *pSprObj, XSprite *pUndoSprite, LPCTSTR szText=_T("") ); 
	virtual ~XUndoSpriteChange() { Destroy(); }

	virtual BOOL DoUndo() override;		// 언두행동(Restore)
	virtual BOOL DoRedo() override;		// 리두행동(Save)
	virtual CString GetText() { 
		return XBaseUndo::GetText() + _T("(") + _T(')'); 
	}

	void SetSpriteAfter( XSprite *pSpriteAfter );
};
