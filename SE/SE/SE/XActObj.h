#pragma once
#include "xLayerInfo.h"

/****************************************************************
* @brief XAniAction의 동적객체
* @author xuzhu
* @date	2016/03/04 9:26
*****************************************************************/
class XSprObj;
class XSprite;
class XBaseKey;

XE_NAMESPACE_START( xSpr )
//
// 오브젝트레이어 내에서 사용되는 
// struct xLayerInObj {
// 	XSprObj
// };
//
XE_NAMESPACE_END; // xSpr


class XActObj : public std::enable_shared_from_this<XActObj>
{
	// 외부호출 방지용
	XActObj( const XActObj& src ) {
		*this = src;
	}
public:
//	XActObj( XSprObj* pSprObj );
	XActObj( XSprObj* pSprObj, XSPAction spAction );
	XActObj( XSprObj* pSprObj, XSPAction spAction, const std::vector<xLayerInfo>& aryLayerInfo );
	virtual ~XActObj() { Destroy(); }
	//
//	XSPActObj CreateCopy();
	XSPBaseLayer CreateAddCopyLayer( XSPBaseLayerConst spLayerSrc );
	// get/setter
	GET_ACCESSOR_CONST( XSPAction, spAction );
	GET_ACCESSOR_CONST( XSprObj*, pSprObj );
	GET_ACCESSOR_CONST( const XList4<XSPBaseLayer>&, listLayer );
	// public member
	XSPActObj CreateNewInstance() const;
	int GetNumLayers() const {
		return m_listLayer.size();
	}
	void MakeLayerInfo( std::vector<xLayerInfo>* pOutAry );
//	void SetLayerInfo( const XList4<xLayerInfo>& listLayerInfo );
	ID GetidAct() const;
	bool IsEmptyLayer( const XSPBaseLayer& spLayer );
	void DoDelEmptyLayer();
	void SortKey();
	void FrameMove( float dt, float fFrmCurr );
	float GetMaxFrame() const;
	void ClearLayer();
	void MoveFrame( float fFrmCurr );
	void Draw( const D3DXMATRIX &m );
	DWORD GetPixel( float cx, float cy
								, float mx, float my
								, const D3DXMATRIX &m
								, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb ) const;
	XSPBaseLayer GetLayerInPixel( float cx, float cy, float mx, float my, const D3DXMATRIX &m ) const;
	XSPBaseLayer GetLayerInPixel( const D3DXMATRIX& mCamera, const D3DXVECTOR2& mv ) const;
	void PreCalcBoundBox();
	XBaseKey_Itor GetKeyItorBegin() const;
	void SetSpeed( float speed );
	void DelLayer( XSPBaseLayer spLayerDel, BOOL bDelLayerOnly = FALSE );
	void UpdateLayersPos( const XE::VEC2& sizeKey );
	void SortLayer(const XE::VEC2& sizeKey);
//	XSPBaseLayer GetspLayer( ID idLayer ) const;
	XSPBaseLayer GetspLayerByidLayer( ID idLayer ) const;
	XSPBaseLayer GetspLayer( xSpr::xtLayer type, int nLayer ) const;
//	XSPBaseLayer GetspLayerBynLayer( int nLayer ) const;
		XSPBaseLayer GetspLayer( xSpr::xtLayer typeLayer, ID idLayer ) const;
	XSPBaseLayer GetspLayer( XBaseKey* pKey );
	template<typename T>
	std::shared_ptr<T> GetspLayerByidLayer( ID idLayer ) const;
	// 레이어 번호로 찾는 버전.
// 	template<typename T>
// 	std::shared_ptr<T> GetspLayerByNumber( int nLayer ) const;
	template<typename T>
	std::shared_ptr<T> GetspLayerByIndex( /*xSpr::xtLayer typeLayer, */int idx ) const;
	XBaseKey* AddKeySprAtLast( ID idLayer, XSprite *pSprite );
	int AddKeySprWithFrame( ID idLayer, float fFrame, XSprite *pSprite );
	int AddKeyPosWithFrame( ID idLayer, float fFrame, float x, float y );
	XBaseKey_Itor AddKey( XBaseKey* pCopyKey, float fFrame = -1.0f, BOOL bNewGenerateID = TRUE );
	void SendLayerEvent_OnSelected( int nEvent );
	XSPBaseLayer GetLastLayer();
	XSPBaseLayer GetFirstLayer();
	int GetNumLayerByType( xSpr::xtLayer typeLayer ) const;
// 	void SetpLastKey( ID idLayer, XBaseKey* pKey );
// 	void SetpSprObjCurr( ID idLayer, XSprObj* pSprObj );
// 	void SetPosToLayer( XSPLayerMove _spLayer, const XE::VEC2& vPos );
// 	void SetRotToLayer( XSPLayerMove _spLayer, const float dAng );
// 	void SetScaleToLayer( XSPLayerMove _spLayer, const XE::VEC2& vScale );
// 	void SetcnPosToLayer( XSPLayerMove _spLayer, const CHANNEL_POS& cnPos );
// 	void SetcnRotToLayer( XSPLayerMove _spLayer, const CHANNEL_ROT& cnRot );
// 	void SetcnScaleToLayer( XSPLayerMove _spLayer, const CHANNEL_SCALE& cnScale );
	XSprite* AddAniFrame( const XE::VEC2& sizeSurface
											, const XE::VEC2& vAdj 
											, const XE::VEC2& sizeMem 
											, XSprite* pSprite );
//	XBaseKey* FindPrevKey( xSpr::xtKey type, xSpr::xtKeySub subType, float fFrame, XSPBaseLayerConst spLayer );
	void AddLayer(XSPBaseLayer spNewLayer, BOOL bCalcPriority = TRUE );
	void AddLayer( ID idLayer, XSPBaseLayer spNewLayer );
public:
	template<typename T>
	std::shared_ptr<T> AddLayerAuto();
	XSPBaseLayer CreateAddLayer( xSpr::xtLayer type
														 , int nLayer = -1
														 , const XE::VEC2& vAdjAxis = XE::VEC2() ) ;
//	XSPBaseLayer CreateLayerFromOtherLayer(XSPBaseLayer spLayerSrc);
	void DrawLayerName( float left, float top, XSPBaseLayer spPushLayer );
	bool IsSameLayerByKeyWithOpenCheck( XBaseKey* pKey1, XBaseKey* pKey2 );
	bool SwapLayer( XSPBaseLayer spLayer1, XSPBaseLayer spLayer2, const XE::VEC2& sizeKey );
	float GetSpeed() const;
	void SetShowLayerExcept( XSPBaseLayer spExceptLayer );
	void SetShowLayerAll();
	void SetMaxIndexByLayerType();
	void SetMaxidLayer();
	void SetidAct( ID idAct );
	void SetszActName( LPCTSTR szName );
	void CreateAddAction( const _tstring& strActName );
	ID GenerateidKeyLocal( ID idLayer, ID idLocalInLayer );
	XBaseKey* CreateNewInstanceKeyBySameLayer( ID idLocalinLayerSrc, ID idLayerSrc );
	XBaseKey* CreateNewInstanceKeyBySameLayer( XBaseKey* pKeySrc );
	ID GetidLayer( xSpr::xtLayer typeLayer, int nLayer ) const;
	XBaseKey* GetKeyByKeyWithDir( XBaseKey* pKeyBase, const XE::POINT& vDir );
private:
	// private member
	XSPAction m_spAction;
	XSprObj* m_pSprObj = nullptr;
	XList4<XSPBaseLayer> m_listLayer;
private:
	// private method
	void Init() {}
	void Destroy();
	void MakeLayerObj( XSPAction spAction, const std::vector<xLayerInfo>& aryLayerInfo );
	bool SwapLayer( XSPBaseLayer spLayer1, XSPBaseLayer spLayer2 );
	XSPBaseLayer GetspLayerMutable( xSpr::xtLayer type, int nLayer ) const;
	XSPBaseLayer GetspLayerMutable( ID idLayer ) const;
	XSPBaseLayer GetspLayerMutable( xSpr::xtLayer typeLayer, ID idLayer ) const;
	template<typename T>
	std::shared_ptr<T> GetspLayerByTypeMutable( ID idLayer ) const;
//	void SyncLayerInfoToAction();
	XSPActObj GetThis() {
		return shared_from_this();
	}
friend XSprObj;
}; // class XActObj

#include "XActObj.inl"
