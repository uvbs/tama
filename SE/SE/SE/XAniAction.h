#pragma once

#include <stdio.h>
#include "xColor.h"
//#include "Sprite.h"
#include "ResMng.h"
#include <list>
#include <algorithm>					// find함수를 사용하기 위해

// 현재는 0.5로 고정해서 쓰지만 장차 서피스크기와 텍스쳐크기가 완전히 분리되면 이 값이 마음대로 바뀐다
#define XHRS(A)		((A) * 0.5f)		// 고해상도로 지정된 이미지일때) 서피스크기 = 이미지크기 * XHRS. 

class XBaseKey;
class XKeyImage;
class XKeyPos;
class XKeyRot;
class XKeyScale;
class XKeyEffect;
class XKeyCreateObj;
class XKeyDummy;
class XKeyEvent;
class XKeySound;
class XKeyMng;
class XSprDat;
class XSprObj;
//class XObjAct;
class XBaseUndo;
class CAnimationView;
class XSprite;
struct xLayerInfo;

// typedef list<LAYER_INFO *>			LayerInfo_List;
// typedef list<LAYER_INFO *>::iterator	LayerInfo_Itor;
class XAniAction : public std::enable_shared_from_this<XAniAction>
{
	XSprDat *m_pSprDat;				// parent
	DWORD m_ActID;						// 이 액션의고유번호(걷기번호 뛰기번호등이 들어간다)
	TCHAR m_szActName[128];
	ID m_idLayerGlobal = 1;
	XBaseKey_Itor m_itorKey;			// GetNextKey()에서 쓸 이터레이터
	XBaseKey_Itor m_itorRangeKey;	// GetRangeKey()에서 사용
	XBaseKey_List m_listKey;
// 	LayerInfo_Itor m_itorLayerInfo;
// 	LayerInfo_List m_listLayerInfo;		// 가지고 있는 레이어정보. 타입만 가지고 있고 실제 레이어는 XSprObj가 가지고 있다
//	XList4<XSPBaseLayer> m_listLayer;
//	std::vector<xLayerInfo> m_aryLayerInfoForSave;		// 세이브용 임시버퍼
	XKeyMng *m_pKeyMng;
	float m_RepeatMark;					// 도돌이표 ㅋㅋ
	float m_fMaxFrame;
	float m_fSpeed;								// 1/60초당 몇프레임이 넘어가는가? 이 SprDat의 디폴드 속도. 같은것이 XSprObj에도 들어갈것이다. 왜냐하면 게임내에서도 자유롭게 속도를 조절할수 있어야 하니까
	xRPT_TYPE m_PlayMode;				// 이건 파일에 저장되는거다. 이값으로 SprObj의 PlayType이 자동바뀐다
	XE::VEC2 m_vBoundBox[2];			// 이 액션의 바운딩박스. Lefttop, RightBottom
	XE::VEC2 m_vScale;		// 이 액션의 전체 스케일링
	XE::VEC3 m_vRotate;		// 이 액션의 전체 회전
//	std::vector<int> m_nMaxLayer;
	std::map<xSpr::xtLayer, int> m_mapMaxLayerNumber;	// 각 레이어 타입당 최대 레이어번호
	void Init()	{
		m_pSprDat = nullptr;
		memset( m_szActName, 0, sizeof(m_szActName) );
		m_fSpeed = 0.2f;
		m_fMaxFrame = 0;
		m_ActID = 0;
		m_RepeatMark = 0;
		m_PlayMode = xRPT_LOOP;
		m_pKeyMng = nullptr;
		m_vScale.Set( 1.f );
		m_itorKey = m_listKey.end();			// 그냥 널로 놔두고 != == 같은 비교하니 죽어서...
		m_itorRangeKey = m_listKey.end();
		InitBoundBox();
	}
	void Destroy();
	void DelKey( XSprObj *pSprObj, XBaseKey *pDelKey );		// private이므로 이걸 직접 쓰지말고 XSprObj::DelKey()를 쓸것
	void DelKey( ID idKey ) {	
		DelKey( nullptr, FindKey( idKey ) );	
	}
//	LAYER_INFO* DelLayerInfo( SPBaseLayer spLayer, BOOL bNodeOnly=FALSE );
//	void InsertLayerInfo( SPBaseLayer spDst, LAYER_INFO *pSrc );		// pSrc를 pDst의 앞에 삽입 
	void SetidAct( ID idAct ) {
		m_ActID = idAct;
	}
	void SetszActName( LPCTSTR szName ) {
		_tcscpy_s( m_szActName, szName );
	}
	void SetszActName( const _tstring& strName ) {
		SetszActName( strName.c_str() );
	}
//	void SetLayerInfoForSave( const std::vector<xLayerInfo>& aryLayerInfo );
	XAniAction( const XAniAction& src ) {
		*this = src;
	}
public:
//	// pSrc의 내용을 this에 deep copy한다.
	void RestoreValue( const XSPAction& spSrc );
//	XAniAction( const XAniAction& src, XSprDat* pSprDat );
	XAniAction( XSprDat *pSprDat, ID idAct ) {
		Init(); 
		m_pSprDat = pSprDat; 
		m_ActID = idAct; 
		Create(); 
	}
	XAniAction( XSprDat *pSprDat, ID idAct, LPCTSTR szActName ) {
		Init(); 
		m_pSprDat = pSprDat; 
		m_ActID = idAct; 
		_tcscpy_s( m_szActName, szActName ); 
		Create(); 
	}
	~XAniAction() { Destroy(); }

	// this로부터 카피본을 만들어낸다.
	XSPAction CreateCopy( XSprDat* pSprDat ) const;
	void CopyKeyByLayer( ID idLayerSrc, ID idLayerNew );
// 	XBaseKey* CreateNewInstanceKeyBySameLayer( ID idLocalinLayer, ID idLayer );
// 	XBaseKey* CreateNewInstanceKeyBySameLayer( XBaseKey* pKeySrc );
	XBaseKey* GetpKeyByidLocal( ID idLocalInLayer, ID idLayer );
//	ID GenerateidKeyLocal( ID idLayer, ID idLocalInLayer );
	ID GetidAct() const {
		return m_ActID;
	}
	ID getid() const {
		return m_ActID;
	}
	float GetSpeed() const { return m_fSpeed; }
	void SetSpeed( float speed ) { m_fSpeed = speed; }
	GET_SET_ACCESSOR_CONST( float, fMaxFrame );
	LPCTSTR GetszActName() const { return m_szActName; }
	_tstring GetstrActName() const {
		return _tstring( m_szActName );
	}
	GET_SET_ACCESSOR_CONST( float, RepeatMark );
	GET_SET_ACCESSOR_CONST( xRPT_TYPE, PlayMode );
	GET_ACCESSOR_CONST( XKeyMng*, pKeyMng );
	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vScale );
	GET_SET_ACCESSOR_CONST( const XE::VEC3&, vRotate );
//	GET_ACCESSOR( const XList4<XSPBaseLayer>&, listLayer );
	GET_ACCESSOR_CONST( XSprDat*, pSprDat );
//	GET_ACCESSOR_CONST( const XList4<xLayerInfo>&, listLayerInfo );
	GET_ACCESSOR_CONST( const XList4<XBaseKey*>&, listKey );
	const XE::VEC2 GetBoundBoxLT() const { 
		return m_vBoundBox[0] * m_vScale; 
	}
	const XE::VEC2 GetBoundBoxRB() const { 
		return m_vBoundBox[1] * m_vScale; 
	}
	void UpdateBoundBox( const XE::VEC2& v );
	void InitBoundBox() {
		m_vBoundBox[0].Set( 999999.f, 999999.f );
		m_vBoundBox[1].Set( -999999.f, -999999.f );
	} 
	// 바운딩박스를 가지고 있는가? FALSE라면 bb값을 사용해선 안됨.
	BOOL IsHaveBoundBox() const {
		if( m_vBoundBox[0].x > 9999.f || m_vBoundBox[0].y > 9999.f ||
			m_vBoundBox[1].x < -9999.f || m_vBoundBox[1].y < -9999.f )
			return FALSE;
		return TRUE;
	}
	void GetNextKeyClear() { m_itorKey = m_listKey.begin(); }
	XBaseKey* GetNextKey() {
		if( m_itorKey == m_listKey.end() )
			return nullptr;
		return (*m_itorKey++);
	}
	// GetNext는 현재포인터 리턴해주고 다음포인터로 이동하지만
	// GetPrev는 이전포인터로 이동한후에 그 포인터를 리턴해준댜. itor의 end()가 cdcd인 특성때문..
	XBaseKey* GetPrevKey() {		
		if( m_itorKey == m_listKey.begin() )
			return nullptr;
		m_itorKey--;
		return (*m_itorKey);
	}

	XBaseKey_cItor GetNextKeyFirst() const { 
		return m_listKey.cbegin(); 
	}
	XBaseKey* GetNextKey( XBaseKey_cItor* pItorKey ) const {
		if( (*pItorKey) == m_listKey.cend() )
			return nullptr;
		return ( *(*pItorKey)++ );
	}
	// GetNext는 현재포인터 리턴해주고 다음포인터로 이동하지만
	// GetPrev는 이전포인터로 이동한후에 그 포인터를 리턴해준댜. itor의 end()가 cdcd인 특성때문..
	XBaseKey* GetPrevKey( XBaseKey_cItor* pItorKey ) const {
		if( (*pItorKey) == m_listKey.cbegin() )
			return nullptr;
		(*pItorKey)--;
		return ( *(*pItorKey) );
	}

	void Create();
	void Save( XResFile *pRes, const std::vector<xLayerInfo>& aryLayerInfo );
	void Load( XSprDat *pSprDat, XResFile *pRes, std::vector<xLayerInfo> *pOutAryLayerInfo );
	void CompositLuaCodes( char *cDst, int bufflen );

	// action
//	void CopyActionDeep( XSPAction spSrc, BOOL bKeepIDKey=FALSE );
	XSPAction CreateNewInstance() const;
// 	XSPAction CopyDeep();
// 	SPAction CopyDeep( BOOL bKeepIDKey );
	// key
// 	XKeyPos *CreatePosKey( xSpr::xtKey type );
// 	XKeyRot *CreateRotKey( xSpr::xtKey type );
// 	XKeyScale *CreateScaleKey( xSpr::xtKey type );
// 	XKeyEffect *CreateEffectKey( xSpr::xtKey type );
	XBaseKey_Itor AddKey( XBaseKey *pNewKey, float fFrame = -1.0f, BOOL bNewGenerateID=TRUE );
	XKeyImage *AddKeyImage( float fFrame, xSPLayerImage spLayer, XSprite *pSprite );
	XKeyDummy *AddKeyDummy( float fFrame, XSPLayerDummy spLayer, BOOL bActive );
//	XKeyPos *AddKeyPos( float fFrame, xSpr::xtLayer type, int nLayer, float x, float y );
	XKeyPos *AddKeyPos( float fFrame, XSPLayerMove spLayer, float x, float y );
	XKeyPos *AddKeyPos( float fFrame, XSPLayerMove spLayer, const XE::VEC2& vPos ) {
		return AddKeyPos( fFrame, spLayer, vPos.x, vPos.y );
	}
	XKeyScale* AddKeyScale( float fFrame, XSPLayerMove spLayer, float sx, float sy );
	inline XKeyScale* AddKeyScale( float fFrame, XSPLayerMove spLayer, const XE::VEC2& vScale ) {
		return AddKeyScale( fFrame, spLayer, vScale.x, vScale.y );
	}
//	XKeyScale *AddKeyScale( float fFrame, xSpr::xtLayer type, int nLayer, float sx, float sy );
	XKeyRot *AddKeyRot( float fFrame, XSPLayerMove spLayer, float fAngleZ );
//	XKeyRot *AddKeyRot( float fFrame, xSpr::xtLayer type, int nLayer, float fAngleZ );
	XKeyEffect *AddKeyEffect( float fFrame, XSPLayerMove spLayer, BOOL bInterpolation, DWORD dwDrawFlag, xDM_TYPE drawMode, float fOpacity );
// 	{	
// 		return AddKeyEffect( fFrame, spLayer->GetType(), spLayer->GetnLayer(), bInterpolation, dwDrawFlag, drawMode, fOpacity );	
// 	}
//	XKeyEffect *AddKeyEffect( float fFrame, xSpr::xtLayer type, int nLayer, BOOL bInterpolation, DWORD dwDrawFlag, xDM_TYPE drawMode, float fOpacity );
//	XKeyCreateObj *AddKeyCreateObj( XBaseKey& keyBase, XKeyCreateObj& key );		// 키를 카피할때는 이걸 써야한다. 그래야 루아코드까지 넘어간다
	XKeyCreateObj *AddKeyCreateObj( DWORD id, LPCTSTR szSprDat, ID idAct, xRPT_TYPE playType, XSPLayerObject spLayer, float fFrame, float x, float y );
// 	XKeyCreateObj *AddKeyCreateObj( DWORD id, LPCTSTR szSprObj, ID idAct, xRPT_TYPE playType, xSpr::xtLayer type, int nLayer, float fFrame, float x, float y );
	XKeyEvent *AddKeyEventCreateObj( float fFrame, XSPLayerEvent spLayer, WORD id, float x, float y );
//	XKeyEvent *AddKeyEvent( float fFrame, xSpr::xtLayer type, int nLayer, xSpr::xTYPE Event, float x, float y );
	XKeyEvent *AddKeyEvent( float fFrame, XSPLayerEvent spLayer, xSpr::xtEventKey Event, float x, float y );
	XKeySound *AddKeySound( float fFrame, XSPLayerSound spLayer, ID idSound ); // { return AddKeySound( fFrame, spLayer->GetType(), spLayer->GetnLayer(), idSound ); }
//	XKeySound *AddKeySound( float fFrame, xSpr::xtLayer type, int nLayer, ID idSound );
	void JumpKey( XSprObj *pSprObj, XBaseKey_Itor &itorKeyCurr, float fFrame ) const;		// fFrame위치로 점프만 한다. 중간에 키는 실행하지 않는다
//	void ExecuteKey( XSprObj *pSprObj, XBaseKey_Itor &itorKeyCurr, float fFrame );
	void FrameMove( float dt, float fFrmCurr );
	void MoveFrame( XSprDat *pSprDat, float fFrmCurr );
//	void Draw( int nView, int x, int y );
	void DrawPathLayer( SPR::xtLINE lineType, CAnimationView *pView, XLayerMove *spLayer, XCOLOR col );
//	XBaseKey_Itor FindKey2( SPBaseLayer spLayer, float fFrame, T **ppKey );
// 	template<class T>
// 	void FindKey( SPBaseLayer spLayer, float fFrame, T **ppKey );
	XBaseKey *FindKey( ID idKey ) const;
	XBaseKey *FindKey( XSPBaseLayer spLayer
										 , xSpr::xtKey typeFilter
										 , xSpr::xtKeySub subType
										 , float fFrame=-1.0f ) const;
	template<typename T>
	T* FindKeyByidLayer( ID idLayer, float fFrame ) const;
	template<typename T>
	T* FindKeyByidLayerNear( ID idLayer, float frame ) const;
// 	XKeyPos* FindKeyPos( XSPLayerMoveConst spLayer, float fFrame );
// 	XKeyRot* FindKeyRot( XSPLayerMoveConst spLayer, float fFrame );
// 	XKeyScale* FindKeyScale( XSPLayerMoveConst spLayer, float fFrame );
// 	XKeyEffect* FindKeyEffect( XSPLayerMoveConst spLayer, float fFrame );
// 	XKeyDummy* FindKeyDummy( XSPLayerMoveConst spLayer, float fFrame );
// 	XKeyEvent* FindKeyEvent( XSPLayerMoveConst spLayer, float fFrame );
// 	XKeySound* FindKeySound( XSPLayerMoveConst spLayer, float fFrame );
	XBaseKey* FindPrevKey( xSpr::xtKey type, xSpr::xtKeySub subType, float fFrame, XSPBaseLayer spLayer );
	XBaseKey* FindPrevKey( XBaseKey* pKeyBase );
	XBaseKey* FindNextKey( XBaseKey* pKeyBase );
	void SortKeyForXActObj( const std::vector<xLayerInfo>& aryLayerInfo );
//	void SortKeyForXActObj();
	void ScaleKey( float fScale );
	void GetRangeKeyClear();
	XBaseKey *GetRangeKey( XSprObj *pSprObj, XBaseKey *pKey1, XBaseKey *pKey2 );
	XBaseKey *FindUseSprite( XSprite *pSpr );
	// layer info
//	void AddLayerInfo( const xLayerInfo& layerInfo );
// 	int GetNumLayerInfo() { 
// 		return m_listLayerInfo.size(); 
// 	}
// 	LAYER_INFO *AddLayerInfo( xSpr::xtLayer type, int nLayer, int idLayer );
// 	void AddLayerInfo( LAYER_INFO *pLayerInfo )	{
// 		m_listLayerInfo.push_back( pLayerInfo );
// 	}
 	BOOL DelLayerLinkKey( XSPBaseLayer spLayer );
// 	BOOL IsEmptyLayer( XSPBaseLayer spLayer );
// 	int GetNumLayerInfo() const {
// 		return m_listLayerInfo.size();
// 	}
	const xLayerInfo* FindpLayerInfo( const std::vector<xLayerInfo>& aryLayerInfo, ID idLayer );
	const xLayerInfo* FindpLayerInfoByType( const std::vector<xLayerInfo>& aryLayerInfo, xSpr::xtLayer bitLayerType, int nLayer );
//	const xLayerInfo* FindpLayerInfo( ID idLayer );
// 	XSPBaseLayer FindLayer( XSPBaseLayer spLayer );
// 	XSPBaseLayer FindLayer( xSpr::xtLayer type, int nLayer ) {
// 		return GetspLayer( type, nLayer );
// 	}
// // 	SPBaseLayer FindLayer( ID idSerial ) {
// // 		return GetspLayer( idSerial );
// // 	}
// 	XSPBaseLayer FindLayer( ID idLayer ) {
// 		return GetspLayer( idLayer );
// 	}
// 	XSPBaseLayer GetspLayer( xSpr::xtLayer type, int nLayer );
// //	SPBaseLayer GetspLayer( ID idSerial );
// 	XSPBaseLayer GetspLayer( ID idLayer );
// 	XSPLayerMove GetspLayerMove( xSpr::xtLayer type, int nLayer );
// 	xSPLayerImage GetspLayerImage( int nLayer );
// 	XSPLayerSound GetspLayerSound( int nLayer );
// 	XSPLayerEvent GetspLayerEvent( int nLayer );
// 	void ( const xLayerInfo& layerInfo );
// // 	void AddLayer( XSPBaseLayer spNewLayer, BOOL bCalcPriority = TRUE );
// // 	void AddLayer( ID idLayer, XSPBaseLayer spNewLayer );
// 	xSPLayerImage AddImgLayer();
// 	XSPLayerObject AddObjLayer();
// 	XSPLayerSound AddSndLayer();
// 	XSPLayerEvent AddEventLayer();
// 	XSPLayerDummy AddDummyLayer();
// 	void UpdateLayersPos( float hKey );
// 	XSPBaseLayer CreateAddLayer( xSpr::xtLayer type, 
// 							int nLayer = -1, 
// 							const XE::VEC2& vAdjAxis = XE::VEC2() );
	void SortLayer();
	void SendLayerEvent_OnSelected( int nEvent );
// 	XSPBaseLayer GetLastLayer();
// 	XSPBaseLayer GetFirstLayer();
	void FrameMove( float dt, float fFrmCurr, XSprObj *pSprObj );
	void MoveFrame( float fFrmCurr );
	void Draw( const D3DXMATRIX &m );
//	DWORD GetPixel( float cx, float cy, float mx, float my, const D3DXMATRIX &m, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb );
// 	XSPBaseLayer GetLayerInPixel( float cx, float cy, float mx, float my, const D3DXMATRIX &m );
// 	XSPBaseLayer GetLayerInPixel( const D3DXMATRIX& mCamera, const D3DXVECTOR2& mv );
//	void DrawLayerName( float left, float top, XSPBaseLayer spPushLayer );
//	XSPBaseLayer CheckPushLayer( float lx, float ly, float h = 0, xSpr::xtLayerSub *pOutSubType = nullptr );
// 	inline XSPBaseLayer CheckPushLayer( const XE::VEC2& vLocal, float h = 0, xSpr::xtLayerSub *pOutSubType = nullptr ) {
// 		return CheckPushLayer( vLocal.x, vLocal.y, h, pOutSubType );
// 	}
	int GetKeysToAryByLayer( XSPBaseLayer spLayer, std::vector<XBaseKey*> *pOut ) const;
// 	XSPBaseLayer CreateLayerFromOtherLayer( XSPBaseLayer spLayerSrc );
	void ReassignspActionByKey( XSPAction spAction );
//	void SetShowLayerExcept( XSPBaseLayer spLayer );
//	void SetShowLayerAll();
//	void SetLayerInfo( const XList4<xLayerInfo>& listLayerInfo );
	D3DXMATRIX GetLocalMatrix() const;
//	ID GetidLayer( xSpr::xtLayer typeLayer, int nLayer ) const;
// public
//////////////////////////////////////////////////////////////////////////
private:
//	XBaseKey* GetKeyFirst( xSpr::xtKeySub typeSub, XSPBaseLayer spLayer );
	template<typename T>
	T* GetKeyFirstByType( ID idLayer ) {
		for( auto pKey : m_listKey ) {
			if( pKey->GetSubType() == T::sGetChannelType() && pKey->GetidLayer() == idLayer )
				return SafeCast<T*>( pKey );
		}
		return nullptr;
	}
	int GetKeysToAryByChannel( xSpr::xtKeySub typeSub, XSPBaseLayer spLayer, XList4<XBaseKey*> *pOut );
	template<typename T>
	int GetKeysToAryByChannel( xSpr::xtKeySub typeSub,
														ID idLayer,
														XList4<T> *pOut );
// 	void DelLayer( XSPBaseLayer spLayer, BOOL bDelLayerOnly = FALSE );
// 	void ClearLayer();
// 	void InsertLayer( XSPBaseLayer spDst, XSPBaseLayer spSrc );
	int GenerateMaxLayer( xSpr::xtLayer type ) { 
		int num = 0;
		auto itor = m_mapMaxLayerNumber.find( type );
		if( itor != m_mapMaxLayerNumber.end() ) {
			++((*itor).second);
			num = (*itor).second;
		} else {
			m_mapMaxLayerNumber[type] = 1;
			num = 1;
		}
		return num;
//		return m_nMaxLayer[ (int)type ]++; 
	}
	
	void SetMaxLayerNumber( xSpr::xtLayer bitType, int num ) {
		auto itor = m_mapMaxLayerNumber.find( bitType );
		if( itor != m_mapMaxLayerNumber.end() ) {
			(*itor).second = num;
		} else {
			m_mapMaxLayerNumber[bitType] = num;
		}
	}
	XSPAction GetThis() {
		return shared_from_this();
	}
	void SetNewForAfterCopy();
	float GetFrameToSec( float fFrame ) const;
private:
	void UpdateLayersPos();
	int GetMaxLayer( xSpr::xtLayer type ) { 
		auto itor = m_mapMaxLayerNumber.find( type );
		if( itor != m_mapMaxLayerNumber.end() ) {
			return (*itor).second;
		} 
//		return m_nMaxLayer[ type ]; 
		return 0;
	}
	void CalcBoundBox( float dt, float fFrmCurr, XSprObj *pSprObj );
	ID GenerateIDLayer() {
		return m_idLayerGlobal++;
	}
	bool SwapLayer( XSPBaseLayer spLayer1, XSPBaseLayer spLayer2 );
	XBaseKey_Itor GetKeyItorBegin() {
		return m_listKey.begin();
	}
// 	void SetMaxIndexByLayerType();
// 	void SetMaxidLayer();
	void SetMaxIndexByLayerType( const std::vector<xLayerInfo>& aryLayerInfo );
	void SetMaxidLayer( const std::vector<xLayerInfo>& aryLayerInfo );
//	xLayerInfo* FindpLayerInfoMutable( ID idLayer );
	XBaseKey_Itor FindKeyItor( XBaseKey* pKeySrc );
//	void DoDelEmptyLayer();
//	void DelLayerInfo( ID idLayer );
	void SetidLocalKeyInLayerToKeys( XSPActObj spActObj );;
	void InitKeysRandom();
	// private
//////////////////////////////////////////////////////////////////////////
	friend class XSprObj;
	friend class XActObj;
}; // class XAniAction

typedef list<XAniAction*>				XAniAction_List;
typedef list<XAniAction*>::iterator	XAniAction_Itor;

#include "XAniAction.inl"
