#pragma once
#include "Select.h"
#include "key.h"
#include "Global.h"

class XBaseKey;
class XLua;
class XSprite;
class XSprDat;
class XUndoKey;
class XAniAction;
struct xLayerInfo;

typedef map<ID, XBaseKey*>		XMap_Key;
typedef map<ID, XBaseKey*>::iterator	XMapItor_Key;
//////////////////////////////////////////////////////////////////////////
class XKeyMng
{
	XMap_Key	m_mapKey;
public:
	XKeyMng() {}
	~XKeyMng() {}

	XBaseKey* GetKey( ID idKey ) {
		if( idKey == 0 )	return nullptr;
		XBaseKey *pKey = m_mapKey[ idKey ];
		return pKey;
	}
	void ClearAll() {
		m_mapKey.clear();
	}
	void Add( XBaseKey *pKey );
	void Clear( XBaseKey *pKey );
};


//////////////////////////////////////////////////////////////////////////
/**
 @brief m_pAction같은 동적인 값이 들어있는 베이스 클래스
*/
class XBaseKeyDynamic : public XBaseElem
{
public:
private:
	ID m_idLocalInLayer = 0;			// 레이어내에서 고유한 아이디(레이어 카피를 해도 값을 바꿀필요가 없음.)
private:
	void Init() {}
	void Destroy() {}
	// 이거말고 CreateCopy를 사용하라고 숨겨둠.
protected:
	XBaseKey *m_pShadowParent = nullptr;
	XSPAction m_spAction;			// 이키가 속해있는 액션
	XBaseKeyDynamic() { Init(); }
	void SetNewInstance( XSPActionConst spActNew, ID idLocalInLayer ) {
		XBaseElem::SetNewInstance();
		m_spAction = std::const_pointer_cast<XAniAction>( spActNew );
		m_idLocalInLayer = idLocalInLayer;
	}
	SET_ACCESSOR( ID, idLocalInLayer );
public:
//	XBaseKeyDynamic( const XBaseKeyDynamic& src, XSPActionConst spActNew, ID idLocalInLayer );
	XBaseKeyDynamic( XSPAction spAction, ID idLocalInLayer );
	virtual ~XBaseKeyDynamic() { Destroy(); }
	/**
	 @brief this의 카피본을 만든다. spAction은 새로 만들어진걸로 바꿔준다.
	*/
	BOOL IsShadow() { return (m_pShadowParent) ? TRUE : FALSE; }
	GET_SET_ACCESSOR( XBaseKey*, pShadowParent );
	GET_SET_ACCESSOR( XSPAction, spAction );
	GET_ACCESSOR_CONST( ID, idLocalInLayer );
friend XAniAction;
};
extern XKeyMng s_KeyMng;
//////////////////////////////////////////////////////////////////////////
/**
 @brief 순수하게 정적인 키값만 있는 클래스의 베이스
*/
class XBaseKey : public XBaseKeyDynamic
{
public:
	static xSpr::xtKey GetTypeFromLayer( xSpr::xtLayer typeLayer );
	// 레이어의 서브타입을 키서브타입으로 바꿔준다.
	static xSpr::xtKeySub sGetSubTypeFromLayer( xSpr::xtLayerSub subTypeLayer );
	static XBaseKey* sCreate( int verSprDat, XSPAction spAction, xSpr::xtKey type, xSpr::xtKeySub subType, ID idLayer, ID idLocalInLayer = 0 );
	static XBaseKey* sCreateRead( XResFile *pRes, XSPAction spAction, int verSprDat, const std::vector<xLayerInfo>& aryLayerInfo );
	static xSpr::xtLayer sConvertOldTypeToNewType( BYTE byteData ) {
		switch( byteData ) {
		case 1:	return xSpr::xLT_IMAGE;
		case 2: return xSpr::xLT_OBJECT;
		case 3: return xSpr::xLT_SOUND;
		case 4: return xSpr::xLT_EVENT;
		case 5: return xSpr::xLT_DUMMY;
		default:
			XBREAKF(1, "알수없는 예전 레이어타입:type=%d", byteData );
			break;
		}
		return xSpr::xLT_NONE;
	}
public:
	LPCTSTR GetTypeString( xSpr::xtKey type = xSpr::xKT_NONE );
	LPCTSTR GetSubTypeString();
	BOOL IsCorrectLayer( xSpr::xtLayer layerType, xSpr::xtKey keyType = xSpr::xKT_NONE );
	BOOL IsCorrectLayer( XSPBaseLayer spLayer, xSpr::xtKey keyType = xSpr::xKT_NONE );
protected:
private:
	int m_nIdx = 0;		// 저장되기직전 값이 들어간다. 로딩시 키배열의 인덱스로 사용된다.
	xSpr::xtKey m_Type;
	xSpr::xtKeySub m_SubType;
	float m_fFrame;
	float m_scrwx, m_scrwy;			// FrameView에서 위치하게될 키들의 일종의월드좌표	DrawKey에서 갱신된다
	XLua *m_pLua;				// 각 키를 제어할수 있는 루아포인터. SE에선 편의상 키마다 루아를 따로 할당하는데 게임에선 최적화땜에 XSprObj에서 루아를 만들어서 모두 여기에 넣고 써야한다
	char *m_pcLua;					// 각 키마다 붙어있는 루아코드
//	XSPBaseLayer m_spLayer;		// 이 키가 속해있는 레이어
	ID m_idLayer = 0;			// 이 키가 속해있는 레이어 아이디
	void Init() {
		m_Type = xSpr::xKT_NONE; 
		m_SubType =xSpr::xKTS_MAIN;
		m_fFrame = 0; 
		m_pLua = nullptr;
		m_pcLua = nullptr;
		m_scrwx = m_scrwy = 0;
	
	}
	void Destroy();
protected:		// draw관련
//	SET_ACCESSOR( ID, idKey );
	void SetidKey( ID idKey ) { SetidObj( idKey ); }
public:
	virtual void ValueAssign( XBaseKey *pSrc ) {
		*this = *pSrc;
	}
private:
//	XBaseKey( const XBaseKey& src );
protected:
	XBaseKey() : XBaseKeyDynamic() { Init(); }
//	XBaseKey( const XBaseKey& src, XSPActionConst spActNew, ID idLayer, ID idLocalInLayer );
public:
	XBaseKey( XSPAction spAction, ID idLayer, ID idLocalInLayer );
	XBaseKey( XSPAction spAction, xSpr::xtKey type, xSpr::xtKeySub subType, float fFrame, XSPBaseLayer spLayer );
	XBaseKey( XSPAction spAction, float fFrame, ID idLayer, ID idLocalInLayer );
	virtual ~XBaseKey() { Destroy(); }
// 	// this의 값을 본뜬 새인스턴스를 만든다 액션이 카피된것이라면 소속 액션도 새로 설정한다. 내부의 고유아이디도 별도의 아이디로 만들어낸다.
// 	// 용어정리: copy->아이디까지 같음(말그대로복사), newInstance=>값만 같고 아이디는 다른 독립된 인스턴스.
	// 아이디와 포인터까지 같은 완전한 복사본을 생성한다.
	virtual XBaseKey* CreateCopy() = 0;
	// 키를 카피하고 새 인스턴스를 만들었을때 새로운 아이디등을 부여한다.
	void SetNewInstance( XSPActionConst spActNew, ID idLayer, ID idLocalInLayer ) {
		XBaseKeyDynamic::SetNewInstance( spActNew, idLocalInLayer );
		m_idLayer = idLayer;
	}
	// 같은액션 같은레이어에 복사본 키를 만든다.
	XBaseKey* NewInstanceBySameLayer();
// 	XBaseKey* CopyDeep( XSPActionConst spActNew ) override { 
// 		XBaseKeyDynamic::CopyDeep( spActNew );
// 	}
	GET_SET_ACCESSOR_CONST( int, nIdx );
	ID GetidKey() const { return GetidObj(); }
	GET_SET_ACCESSOR_CONST( float, fFrame );
	xSpr::xtKey GetType() const { return m_Type; }
	void SetType( xSpr::xtKey type ) { m_Type = type; }
	GET_SET_ACCESSOR_CONST( xSpr::xtKeySub, SubType );
	GET_SET_ACCESSOR_CONST( float, scrwx );
	GET_SET_ACCESSOR_CONST( float, scrwy );
	GET_SET_ACCESSOR_CONST( XLua*, pLua );
	GET_SET_ACCESSOR_CONST( char*, pcLua );
//	GET_ACCESSOR( XSPBaseLayer, spLayer );
	GET_SET_ACCESSOR_CONST( ID, idLayer );
// 	virtual void SetspLayer( XSPBaseLayer& spLayer ) = 0; {
// 		m_idLayer = spLayer->GetidLayer();
// 	}
//	ID GetidLayer() const;
// 	BOOL IsSameLayer( XSPBaseLayer spLayer );
//	BOOL IsSameLayer( xSpr::xtLayer type, int nLayer );
// 	inline bool IsSameLayer( ID idLayer ) const {
// 		return idLayer == m_idLayer;
// 	}
	// 비교대상키와 레이어도 같고 채널도 같으면 이 키는 같은 레이어의 같은 채널에 있다고 판단.
	inline bool IsSameLayerAndChannel( XBaseKey* pKeyOther ) const {
		return (m_idLayer == pKeyOther->m_idLayer 
					&& m_Type == pKeyOther->m_Type
					&& m_SubType == pKeyOther->m_SubType);
	}
	bool IsSameLayerOnly( XSPBaseLayer spLayer ) const;
	inline bool IsSameLayerOnly( XBaseKey* pKeyOther ) const {
		return m_idLayer == pKeyOther->GetidLayer();
	}
//	bool IsSameLayer( XSPBaseLayerConst spLayer ) const;		// 예전에 이함수를 채널까지 같은것과 레이어만 같은것을 구분하지 않고 써서 바꾸는게 좋음.
	XBaseKey *GetNextKeyConst() const;
	XBaseKey *GetNextKey();		// this의 키타입/서브타입과 같은종류의 키를 this가 속한 레이어에서 찾는다
	XBaseKey *GetPrevKey();
	BOOL IsInside( float x, float y, float fScale );
	BOOL IsInside( const XE::VEC2& vPos, float fScale ) { return IsInside( vPos.x, vPos.y, fScale ); }
	BOOL IsInside( const XE::VEC2& vStart, const XE::VEC2& vEnd, float fScale );
	void CopyLua( XBaseKey *pKeySrc ) {
		if( pKeySrc->GetpcLua() ) {
			int len = strlen( pKeySrc->GetpcLua() );
			m_pcLua = new char[ len+1 ];
			strcpy_s( m_pcLua, len+1, pKeySrc->GetpcLua() );		// lua code copy
		}
	}
	// virtual
//	BOOL IsSameLayer( XBaseKey *pKey, BOOL bCheckOpen=TRUE );
	bool IsSameLayerByCheckOpen( XBaseKey *pKey, bool bCheckOpen );
	virtual void Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec=0 );
	XE::VEC2 UpdateDrawPos( XSprObj *pSprObj, float speed, float left, float top, float wx, float fScale );
	void DrawKey( D3DCOLOR color, float fScale, bool bSelected = false );
	virtual BOOL IsUseSprite( XSprite *pSpr ) { return FALSE; }
//	virtual XBaseKey* CopyKey();
	virtual BOOL EditDialog() { return FALSE; }
	virtual void GetToolTipSize( float *w, float *h );
	virtual CString GetToolTipString( XSPBaseLayer spLayer );
	//
	CString GetToolTipStringTail();
	virtual void DrawToolTip( XSprDat *pSprDat, float x, float y ) {}
	virtual void CompositLuaCodes( CString *pstrLua );
	CString MakeTableName();
	void ReplaceFuncName( CString *pstrLua, TCHAR *cOld );
	virtual void OnAdded() { }
	virtual XE::VEC2 GetPos() { return XE::VEC2( 0, 0 ); }
	virtual void SetPos( const XE::VEC2& vPos ) {}		
	virtual void Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes );
	virtual void Load( XResFile *pRes, XSPAction spAction, int ver ) = 0;
	ID GenerateidKey() {
		return GenerateNewID();
	}
	virtual void InitRandom() = 0;
protected:
private:
}; // class XBaseKey
