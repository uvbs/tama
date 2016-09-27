#pragma once
//#include "XGameWnd.h"
#include "_Wnd2/XWndImage.h"
#include "_Wnd2/XWndCtrls.h"
#include "XPropItem.h"
#pragma warning( disable : 4250 )	//  warning C4250: 'XWndButtonHexNumber' : 우위에 따라 'XWndButton::XWndButton::Process'을(를) 상속합니다.

class XWndImage;
class XWndTextString;

//struct XGlobalConst::cashItem;

class XWndShopElem : public XWndImage
{
	XWndImage *m_pIcon;
	XWndTextString *m_pName;
	XWndTextString *m_pPrice;
//	XGlobalConst::cashItem* m_pCashItem;	// 포인터엔 p를 붙일것.
	XPropItem::xPROP *m_pEtcItemProp;
	_tstring m_idsProduct;
	void Init()	{
		m_pIcon = nullptr;
		m_pName = nullptr;
		m_pPrice = nullptr;
//		m_pCashItem = nullptr;
		m_pEtcItemProp = 0;
	}
	void Destroy(){}
public:
	//아이템 정보를 받아와야함 : 아이콘, 이름, 가격 / 프랍을 통채로 받아오면 될듯
	XWndShopElem( const _tstring& idsProduct );
	XWndShopElem( XPropItem::xPROP* pEtcItemProp );
	virtual ~XWndShopElem(){ Destroy(); }

	GET_ACCESSOR_CONST( const _tstring&, idsProduct );
	GET_ACCESSOR_CONST( XPropItem::xPROP*, pEtcItemProp );
// 	const _tstring& GetidsProduct() const {
// 		return m_pCashItem->m_productID;
// 	}
};


// 창고 리스트 아이템
// class XWndStoragyItemElem : public XWnd
// {
// public:
// 	static XWndStoragyItemElem* sUpdateCtrl( XWnd* pParent, const XE::VEC2& vPos, XHero* pHero, const std::string& ids );
// 	static XWndStoragyItemElem* sUpdateCtrlToList( XWndList *pWndList, XHero *pHero );
// private:
// 	XGAME::xReward m_Reward;
// // 	ID m_idItem = 0;
// 	ID m_snItem = 0;			// 무기상같은경우는 idItem만으로 쓰기때문에 0일수가 있다.
// 	ID m_snHero = 0;			// 인벤에 있는 영웅의 경우
// 	XPropItem::xPROP *m_pProp = nullptr;	// 아이템 프랍
// 	XBaseItem *m_pItem = nullptr;			// 아이템 정보
// 	bool m_bEquip = false;		// 장착표시
// 	bool m_bSelected = false;		// 선택중?
// 	bool m_bShowName = true;	// 이름을 보여줄건지
// 	bool m_bShowUnit = false;		// 영웅일 경우 장착한 유닛을 표시할건지.
// 	bool m_bNotUse = false;			// 빨간색으로 덮힘.
// // 	int m_Num = 0;				// 개수가 있을경우
// 	// UI
// 	XSurface *m_psfcBgGradation = nullptr;
// 	XSurface *m_pBG;			// 아이템 슬롯 bg
// 	XSurface *m_pItemImg;		// 아이템 이미지
// 	XE::VEC2 m_vScaleItemImg;	// m_pItemImg만 따로 스케일링을 줄때.
// 	XArrayLinearN<XSurface*, XGAME::xGD_MAX> m_aryStar;		// 별
// 	XSurface *m_psfcSelected;		// 선택중
// //	XSurface *m_pNumImg;		// 갯수 bg
// 	XWndTextString *m_pNum;		// 갯수
// 	XWndButton *m_lockButt;		// 잠금 버튼
// //	XSurface *m_slotLock;		// 슬롯 잠금 이미지
// 	XSurface *m_pSoulStone = nullptr;
// 	XSurface *m_psfcNameBg = nullptr;
// 	CTimer m_timerGlow;		// 선택외곽선용
// 	bool m_bGlowAnimation = false;
// 	void Init() {
// 		m_pProp = nullptr;
// 		m_pItem = nullptr;
// 		m_pBG = nullptr;
// 		m_pItemImg = nullptr;
// 		m_psfcSelected = nullptr;
// //		m_pNumImg = nullptr;
// 		m_pNum = 0;
// 		m_lockButt = nullptr;
// //		m_slotLock = nullptr;
// 		m_vScaleItemImg.Set( 1.f );
// 	}
// 
// 	void Destory();
// 
// public:
// 	XWndStoragyItemElem( const XE::VEC2& vPos );
// 	XWndStoragyItemElem( const XE::VEC2& vPos, const XGAME::xReward& reward );
// 	XWndStoragyItemElem( const XGAME::xReward& reward )
// 		: XWndStoragyItemElem( XE::VEC2(0), reward ) {}
// 	XWndStoragyItemElem( const XE::VEC2& vPos, XHero *pHero );
// 	XWndStoragyItemElem(XBaseItem* pItem);
// 	XWndStoragyItemElem(ID idItem);
// 	XWndStoragyItemElem( const XE::VEC2& vPos, const _tstring& idsItem );
// 	template<typename T1, typename T2>
// 	XWndStoragyItemElem( T1 x, T2 y, ID idItem) 
// 	: XWndStoragyItemElem( idItem ) {
// 		SetPosLocal( x, y );
// 	}
// 	XWndStoragyItemElem( const XE::VEC2& vPos, ID idItem )
// 		: XWndStoragyItemElem( idItem ) {
// 		SetPosLocal( vPos );
// 	}
// 	XWndStoragyItemElem( LPCTSTR szImg, int num );
// 	virtual ~XWndStoragyItemElem(){ Destory(); }
// 
// 	BOOL OnCreate() override;
// 
// 	XBaseItem* GetpItem(void) { return m_pItem; }		// 아이템 정보
// 	XPropItem::xPROP *getpProp() { return m_pProp;  }	// 아이템 프랍
// 	int GetnumStars() { return m_pProp->grade; }		// 별 갯수
// 	XSurface* GetItemImg() { return m_pItemImg; }		// 아이템 이미지
// 	GET_SET_ACCESSOR(bool, bEquip);
// 	ID GetidItem() {
// 		return m_Reward.GetidItem();
// 	}
// 	GET_SET_ACCESSOR_CONST( bool, bGlowAnimation );
// 	GET_SET_ACCESSOR( const XE::VEC2&, vScaleItemImg );
// 	GET_SET_BOOL_ACCESSOR( bSelected );
// 	GET_SET_ACCESSOR( bool, bShowUnit );
// 	GET_SET_ACCESSOR( bool, bShowName );
// 	GET_SET_ACCESSOR_CONST( bool, bNotUse );
// 	void SetNum( int num );
// 	GET_ACCESSOR( ID, snItem );
// 	const char* GetIdsName() {
// 		return "img.bg.name";
// 	}
// 	void SetItemImg( LPCTSTR szImg );
// 	void SetHero( ID idProp );
// 	void SetHero( XHero* pHero );
// 	void SetLockButt();
// 	XWndButton* GetLockButt() { return m_lockButt; }
// 	void SetSlotLock();
// 	bool IsLock();
// 	void Draw() override;
// //	virtual void DrawDrag(const XE::VEC2& vMouse);
// //	void SetUnitFace(void);
// 	void Update() override;
// 	void SetEventItemTooltip();
// 	void ClearHero();
// };

// 슬라이더
class XWndSliderOption : public XWndSlider
{
	XSurface *m_psfcSlider;
	void Init() {
		m_psfcSlider = NULL;
	}
	void Destroy();
public:
	XWndSliderOption(float x, float y, float w, float h, float min, float max, float curr);
	virtual ~XWndSliderOption() { Destroy(); }
	//
	virtual void Draw(void);

	SET_ACCESSOR(float, Curr);
};







#pragma warning ( default : 4250 )
