#pragma once

class XSprObj;
class XSprDat;
class XSprite;
class XBaseKey;
class XKeyEvent;
class XLayerImage;
class XLayerObject;
struct CHANNEL_EFFECT;
struct CHANNEL_POS;
struct CHANNEL_ROT;
struct CHANNEL_SCALE;
struct XEFFECT_PARAM;
enum xRPT_TYPE : int;
namespace xSpr {
	struct xEvent;
}

class XDelegateSprObj
{
private:
	void _Init() {}
	void _Destroy() { }
public:
	XDelegateSprObj() {
		_Init();
	}
	virtual ~XDelegateSprObj() {
		_Destroy();
	}
	// get/set
	// virtual
	//	virtual XSprObj* CreateSprObj( LPCTSTR szSpr, XDelegateSprObj* pParent=NULL ) { return new XSprObj( szSpr, pParent ); }
	//	virtual void OnEventCreateObj( float lx, float ly, WORD id, float fAngle, float fOverSec ) {}
	virtual void OnEventSprObj( XSprObj *pSprObj, XKeyEvent *pKey, float lx, float ly, ID idEvent, float fAngle, float fOverSec ) {}
	virtual void OnEventHit( const xSpr::xEvent& event ) {}
	virtual void OnProcessDummy( ID id, const XE::VEC2& vLocalT, float ang, const XE::VEC2& vScale, const CHANNEL_EFFECT& cnEffect, float dt, float fFrmCurr ) {}
	virtual void OnEventCreateSfx( XSprObj *pSprObj, XBaseKey *pKey, float lx, float ly, float scale, LPCTSTR szSpr, ID idAct, xRPT_TYPE typeLoop, float secLifeTime, BOOL bTraceParent, float fAngle, float fOverSec ) {}
	virtual void OnEventCreateSfx( const xSpr::xEvent& event ) {}
	virtual void OnEventEtc( const xSpr::xEvent& event ) {}
	virtual void OnFinishPlayAction( XSprObj *pSprObj, ID idAct, ID idDelegate ) {}
	// 델리게이트에서 XLayerImage::SetDrawInfoToSpr()로 pSpr을 세팅해주고 pSpr을 드로잉 해주시오
	virtual BOOL OnDelegateDrawImageLayerBefore( XSprObj *pSprObj, XSprite *pSprSrc, XLayerImage *pImageLayer, XEFFECT_PARAM *pEffectParam, float x, float y, const MATRIX &mParent ) {
		return FALSE;
	}
	virtual BOOL OnDelegateDrawImageLayerAfter( XSprObj *pSprObj, XSprite *pSprSrc, XLayerImage *pImageLayer, XEFFECT_PARAM *pEffectParam, const XE::VEC2& vDraw, const MATRIX &mParent ) {
		return FALSE;
	}
	// 오브젝트 생성키가 실행되기전 불려지는 델리게이트
	virtual BOOL OnDelegateCreateObj( XSprObj *pSprObj, XSprite *pSprSrc, XLayerObject *pImageLayer, XEFFECT_PARAM *pEffectParam, float x, float y, const MATRIX &mParent ) {
		return FALSE;
	}
	// 비동기 로딩으로 spr로딩과 SetAction까지 끝나면 호출된다.
	virtual void OnFinishAsyncLoad( const XSprDat* pSprDat ) {}
};