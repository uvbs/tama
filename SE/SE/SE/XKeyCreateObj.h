#pragma once
#include "XKeyBase.h"


//////////////////////////////////////////////////////////////////////////
class XKeyCreateObj;
class XKeyCreateObj : public XBaseKey
{
public:
	// 이 키의 타입들
	static xSpr::xtKey sGetType() {
		return xSpr::xKT_CREATEOBJ;
	}
	static xSpr::xtKeySub sGetChannelType() {
		return xSpr::xKTS_MAIN;
	}
	//static xSpr::xtLayer sGetLayerType() {
	//	return xSpr::xLT_OBJECT;
	//}
	static xSpr::xtLayerSub sGetLayerChnnelType() {
		return xSpr::xLTS_MAIN;
	}
private:	
	DWORD m_dwID;					// base가 될 고유 아이디
	ID m_idSprObj;						// 이 키에서 생성한 SprObj의 아이디. 이전과 다른 SprObj가 생성됐을경우 처리하기 위한건데 현재는 사용하지 않음
	TCHAR m_szSprName[ 128 ];		// 스프라이트 파일 이름
	int m_nAction;						// 스프라이트를 읽은후 SetAction()을 할 번호
	xRPT_TYPE m_PlayType;				// 아직 지원안하는듯
//	XSPLayerObject m_spLayer;		// 이 키가 속해있는 레이어
	void Init() {
		SetSubType( xSpr::xKTS_MAIN );
		m_dwID = 0;
		memset( m_szSprName, 0, sizeof( m_szSprName ) );
		m_nAction = 0;
		m_PlayType = xRPT_LOOP;
		m_idSprObj = 0;
	}
	void Destroy();
	// 외부호출 방지용
	XKeyCreateObj( const XKeyCreateObj& src ) {
		*this = src;
	}
protected:
	XKeyCreateObj() : XBaseKey() { Init(); }
public:
	XKeyCreateObj( XSPAction spAction, ID idLayer, ID idLocalInLayer ) 
		: XBaseKey( spAction, idLayer, idLocalInLayer ) { Init(); }
	XKeyCreateObj( XSPAction spAction, DWORD id, LPCTSTR szSprObj, XSPLayerObject spLayer, int nAction, xRPT_TYPE playType, float fFrame, float x, float y );
	virtual ~XKeyCreateObj() { Destroy(); }

// 	XKeyCreateObj( const XKeyCreateObj& src, XSPActionConst spActNew, ID idLayer, ID idLocalInLayer ) 
// 		: /*XKeyCreateObj( src ),*/ XBaseKey( src, spActNew, idLayer, idLocalInLayer ) {
// //		*this = src;
// 		// this만의 깊은복사 처리
// 	}
	XBaseKey* CreateCopy() override;
	GET_ACCESSOR( LPCTSTR, szSprName );
	GET_SET_ACCESSOR( int, nAction );
	GET_SET_ACCESSOR( xRPT_TYPE, PlayType );
	GET_ACCESSOR( DWORD, dwID );
	GET_ACCESSOR_CONST( ID, idSprObj );
	//GET_ACCESSOR( xSPLayerObject, spLayer );
	void Execute( XSPActObj spActObj, XSPBaseLayer spLayer, float fOverSec = 0 ) override;
	void Save( xSpr::xtLayer typeLayer, int nLayer, XResFile *pRes ) override;
	virtual void Load( XResFile *pRes, XSPAction spAction, int ver ) override;
	virtual BOOL EditDialog();
	virtual void ValueAssign( XBaseKey *pSrc ) {
		*this = *( dynamic_cast<XKeyCreateObj *>( pSrc ) );
	}
//	virtual XBaseKey *CopyDeep() override;
	virtual void GetToolTipSize( float *w, float *h ) override;
	virtual CString GetToolTipString( XSPBaseLayer spLayer ) override;
	virtual void CompositLuaCodes( CString *pstrLua );
	void InitRandom() override {}
private:
//	void SetspLayer( XSPBaseLayer& spLayer ) override;
	friend XBaseKey* XBaseKey::sCreate( int verSprDat, XSPAction spAction, xSpr::xtKey type, xSpr::xtKeySub subType, ID idLayer, ID idLocalInLayer );
	XSprObj* GetpSprObjChild( XSPActObj spActObj, XSPBaseLayer spLayer );
}; // XKeyCreateObj
// namespace KE {
// 	typedef enum xSpr::xtKey	{
// 		NONE = 0,
// 		CREATE_OBJ,			// 오브젝트 생성 이벤트
// 		HIT,					// 타격 이벤트
// 		CREATE_SFX,			// 이펙트 생성
// 		ETC = 999			// 기타 다용도 이벤트
// 	};
// 	typedef struct {
// 		union {
// 			struct {
// 				BYTE b[4];
// 			};
// 			struct {
// 				WORD w[2];
// 			};
// 			DWORD dwParam;
// 			int nParam;
// 			float fParam;
// 		};
// 	} KEY_PARAM;
// }
