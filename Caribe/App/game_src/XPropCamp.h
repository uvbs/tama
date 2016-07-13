/********************************************************************
	@date:	2015/01/09 16:18
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XPropCamp.h
	@author:	xuzhu
	
	@brief:	캠페인 프로퍼티
*********************************************************************/
#pragma once

#include "XXMLDoc.h"
#include "XStruct.h"
//#include "XPropLegionH.h"

namespace XGAME {
struct xSquad;
struct xLegion;
}
/*
	ver 9
		별누적 추가
	ver 10
		별누적 수정
	ver 11
		스테이지 직렬화때 idStage저장
	ver 12
		maxTry삭제
	ver 13
		StageObjHero의 legionLevel저장
	ver 14
		StageObjHero에 시리얼라이즈 추가
	ver 15
		StageObjHero에 시리얼라이즈 추가
	ver 18
	  typeCamp추가
	ver 19
	  XStageObjHero2로 변경
	ver 20
	  캠페인 층개념 추가.
*/
#define VER_CAMP_SERIALIZE		20

#define XSPPropStage			StagePtr

XE_NAMESPACE_START( xCampaign )

enum xtType {
	xCT_NONE,
	xCT_NORMAL,
	xCT_HERO,
	xCT_HERO2,		// 리뉴얼된 영웅캠펜
};
XE_NAMESPACE_END; // xCampaign


/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/01/02 17:44
*****************************************************************/
class XPropCamp : public XXMLDoc
{
	static ID s_Global;
public:
	static ID sGetGlobalID() {
		return ++s_Global;
	}
	//////////////////////////////////////////////////////////////////////////
	class xStage {
	public:
// 		std::string m_strcNode;			// xml내 레이아웃 노드
		// DB에서 로딩서 스테이지배열에 변화가 생겼을때를 대비해 
		// 원래 스테이지가 현재 몇번째인덱스로 들어가야하는지 찾기위해 필요
		ID idProp = 0;		///< 스테이지 고유아이디
// 		int maxWin = 1;		///< 승리해야 하는 횟수(메달캠프에서 씀). 0은 ap가 허용하는한 계속 도전해서 승리할수 있음.(완전 클리어가 없음)
		int maxWin = 0;		///< 승리할수 있는 횟수. maxWin만큼 승리하면 더이상 캠페인전체 도전횟수가 있어도 도전할수 없다.  0은 ap가 허용하는한 계속 도전해서 승리할수 있음.(완전 클리어가 없음)
		int maxTry = 0;		///< 도전가능 횟수(0은 무한대로 도전으로 변경)(메달캠프에서 씀)
		int levelLimit = 0;	///< 제한레벨
		_tstring sidDropItem;	///< 드랍할 아이템이 있다면 아이템 식별자
		std::vector<XGAME::xReward> aryReward;	///< 스테이지 클리어시 보상
		float rateDrop = 0;		///< 드랍확률
		int idxStage = -1;		///< 몇번째 스테이지 인지
		std::shared_ptr<XGAME::xLegion> m_spxLegion;		///< 군단정보
// 		xStage() : legion("ids.dummy") {}		// 스테이지에서는 ids안씀. 장기적으로 XPropLegion것을 쓰도록 교체할것.
		xStage();		// 스테이지에서는 ids안씀. 장기적으로 XPropLegion것을 쓰도록 교체할것.
		~xStage() {
			idProp = 0;
		}
		int SerializeMin( XArchive& ar ) const;				// 이전이름: Serialize()
		int DeSerializeMin( XArchive& ar, int ver );	// 
		virtual int Serialize( XArchive& ar ) const;					// 이전이름: Serialize2() <- 풀버전
		virtual int DeSerialize( XArchive& ar, int ver );
	};
	//////////////////////////////////////////////////////////////////////////
	class xStageHero : public xStage {
	public:
		int m_idHero = 0;			// 이제 프로퍼티 로딩시 스테이지에 어떤 영웅이 배정될것인지 미리 지정한다. 이전방식이 너무나 난해해서 심플하게 바꿈.
// 		int m_lvLegion = 0;		// 스테이지의 군단레벨
		xStageHero() {}
		int Serialize( XArchive& ar ) const override;
		int DeSerialize( XArchive& ar, int ver ) override;
	};

	//////////////////////////////////////////////////////////////////////////
	// 캠페인 하나의 프로퍼티
	class xProp {
	public:
		xCampaign::xtType m_Type = xCampaign::xCT_NONE;			// 리뉴얼 영웅던전으로 추가.
		XVector<std::shared_ptr<XPropCamp::xStage>> aryStages;
		_tstring strIdentifier;
		_tstring m_strtXml;				// 추가 레이아웃이 있다면..
		std::string m_strcLayout;	// 추가 레이아웃 노드명
		ID idProp = 0;
		ID idName = 0;		// 캠페인 제목
		int reward = 0;	// 보상포인트같은게 있다면 사용.
		int numTry = 0;	// 모든스테이지 합쳐서 도전할수 있는 횟수.(스테이지 진입해서 이길수 있는 횟수)
		int secResetCycle = 0;	// 캠페인 리셋사이클. 0이면 리셋없음(1회성).
		int m_Debug = 0;
		int m_numStage = 0;
		bool bAutoReset = false;	// 시간되면 자동으로 리셋되는가.
		//
		xProp( xCampaign::xtType type ) : m_Type( type ) {}
		int DeSerializeMin( XPropCamp *pPropCamp, XArchive& ar, int ver );
		virtual int Serialize( XArchive& ar ) const;
		virtual int DeSerialize( XArchive& ar, int ver );
		std::shared_ptr<XPropCamp::xStage>
		GetspStageByidxStage( int idxStage ) {
			if( idxStage < 0 || idxStage >= (int)aryStages.size() )
				return std::shared_ptr<XPropCamp::xStage>();
			return aryStages[idxStage];
		}
		std::shared_ptr<XPropCamp::xStage> GetspStageByidStage( ID idStage );
		int GetnumStages() const {
			return aryStages.size();
		}
	};
	//////////////////////////////////////////////////////////////////////////
	class xPropHero : public xProp {
	public:
		int m_maxFloor = 0;
//		int m_maxStage = 0;		// 층당 최대 스테이지 수.
		xPropHero() : xProp( xCampaign::xCT_HERO2 ) {}
		int Serialize( XArchive& ar ) const override;
		int DeSerialize( XArchive& ar, int ver ) override;
	};
public:
	static XPropCamp* sGet();
	static void sDestroyInstance() {
		SAFE_DELETE( s_pInstance );
	}
	static DWORD sGetIdsToId( const char *cIdentifier, int len ) {
		XINT64 i64CheckSum = XE::GetCheckSum( (const BYTE*)( cIdentifier ), len );
		XBREAK( i64CheckSum > 0x7fffffff );
		return (DWORD)i64CheckSum;
	}
	static DWORD sGetIdsToId( const std::string& strIds ) {
		return sGetIdsToId( strIds.c_str(), strIds.length() );
	}
	static ID sGetHeroIds( XPropCamp *pThis, XEXmlNode& node, const char *cKey, xProp *pCampProp );
	static XGAME::xtUnit sGetUnitIds( XPropCamp *pThis, XEXmlNode& node, const char *cKey, xProp *pCampProp );
	static std::shared_ptr<xStage> sCreatePropStage( xCampaign::xtType typeCamp );
private:
	static XPropCamp *s_pInstance;
	static xProp* sCreatePropCamp( xCampaign::xtType typeCamp );
private:
	int m_Ver = 0;
	std::unordered_map<ID, xProp*> m_mapCamp;
	XVector<xProp*> m_aryCamp;
	std::map<_tstring, xProp*> m_mapCampStr;
	bool m_bError = false;
	void Init() {}
	void Destroy();
public:
	XPropCamp();
	virtual ~XPropCamp() { Destroy(); }
	//
	GET_ACCESSOR_CONST( const XVector<xProp*>&, aryCamp );
	//
	BOOL Load( LPCTSTR szXml );
	bool OnDidFinishLoad() override;
	bool IsExist( xProp *pProp );
	xProp* LoadCamp( XEXmlNode& nodeCamp );
	void LoadStage( XEXmlNode& nodeStage, std::shared_ptr<xStage>& spStage, xProp *pCampProp );
// 	void LoadLegion( XEXmlNode& nodeLegion, std::shared_ptr<xStage>& spStage, xProp *pCampProp );
// 	void LoadSquad( XEXmlNode& nodeSquad, XGAME::xSquad *pSquad, xProp *pCampProp );
	//
	xProp* GetpProp( const _tstring& strIdentifier ) {
		auto itor = m_mapCampStr.find( strIdentifier );
		if( itor == m_mapCampStr.end() )
			return nullptr;
		xProp *pProp = itor->second;
		XBREAK( pProp == nullptr );
		return pProp;
	}
	xProp* GetpProp( LPCTSTR szIdentifier ) {
		_tstring ids = szIdentifier;
		return GetpProp( ids );
	}
	xProp* GetpProp( ID idProp ) {
		if( idProp == 0 )
			return nullptr;
		auto itor = m_mapCamp.find( idProp );
		if( itor == m_mapCamp.end() )
			return nullptr;
		xProp *pProp = itor->second;
		XBREAK( pProp == nullptr );
		return pProp;
	}
	void AddStage( ID idStage, std::shared_ptr<xStage>& spStage );
// 	std::shared_ptr<xStage> GetpPropStage( ID idStage );
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
	void GetBaseAttr( XEXmlNode& nodeCamp, xProp* pOutProp );
	void LoadCampHero( XEXmlNode& nodeCamp, xPropHero* pOutProp );
	static int sGetlvLegionByidxHero( int idxStage, int maxStage );
}; // class XPropCamp

typedef std::shared_ptr<XPropCamp::xStage> StagePtr;
typedef std::shared_ptr<XPropCamp::xStageHero> XSPPropStageHero;


