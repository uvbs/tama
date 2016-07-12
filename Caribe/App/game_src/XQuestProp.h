/********************************************************************
	@date:	2015/01/02 17:43
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XQuestProp.h
	@author:	xuzhu
	
	@brief:	퀘스트 프로퍼티
*********************************************************************/
#pragma once

const int VER_PROP_QUEST = 1;

#include "XXMLDoc.h"
#include "XStruct.h"

/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/01/02 17:44
*****************************************************************/
class XQuestProp : public XXMLDoc
{
public:
	static XQuestProp* sGet();
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
//	static ID sReadItemIdsToId( XEXmlNode& node, const char *cKey, LPCTSTR szNodeName );
#ifdef WIN32
	static DWORD sGetIdsToId( const _tstring& strIds ) {
		std::string str = SZ2C( strIds.c_str() );
		return sGetIdsToId( str );
	}
#endif
	struct xWhen {
		ID idWhen;			///< 조건코드
		XGAME::xParam param;
		xWhen() : idWhen( 0 ) {}
		void Serialize( XArchive& ar ) const {
			ar << idWhen << param;
		}
		void DeSerialize( XArchive& ar, int ) {
			ar >> idWhen >> param;
		}
	};
	struct xCond2 {
		ID idDesc;		// 완료조건 텍스트
		ID idWhere;
		XGAME::xParam paramWhere;
		ID idWhat;
		XGAME::xParam paramWhat;
		XGAME::xParam paramWhat2;
		ID idHow;
		XGAME::xParam paramHow;
		int num;			///< 횟수나 개수
		XGAME::xtParam typeParamWhat;
		bool bDelItem = true;		///< 퀘 완료후 퀘템을 삭제할것인지.
		bool reserved_b;
		char reserved_b2;
		xCond2() : idWhere(0), idWhat(0), idHow(0), num(1), idDesc(0), typeParamWhat(XGAME::xPT_NONE) {		}
		int Serialize( XArchive& ar ) const {
			ar << idDesc;
			ar << num;
			XBREAK( idWhere > 255 );
			XBREAK( idWhat > 255 );
			XBREAK( idHow > 255 );
			ar << (BYTE)idWhere;
			ar << (BYTE)idWhat;
			ar << (BYTE)idHow;
			ar << (BYTE)xboolToByte(bDelItem);
			ar << paramWhere.dwParam;
			ar << paramWhat.dwParam;
			ar << paramWhat2.dwParam;
			ar << paramHow.dwParam;
			ar << (BYTE)typeParamWhat;
			ar << (BYTE)0;
			ar << (short)0;
			return 1;
		}
		int DeSerialize( XArchive& ar, int ver ) {
			ar >> idDesc;
			BYTE b0;
			ar >> num;
			ar >> b0;	idWhere = b0;
			ar >> b0;	idWhat = b0;
			ar >> b0;	idHow = b0;
			ar >> b0;
			ar >> paramWhere.dwParam;
			ar >> paramWhat.dwParam;
			ar >> paramWhat2.dwParam;
			ar >> paramHow.dwParam;
			ar >> b0;		typeParamWhat = (XGAME::xtParam)b0;
			ar >> b0 >> b0 >> b0;
			return 1;
		}
		void Serialize2( XArchive& ar ) const;
		void DeSerialize2( XArchive& ar, int );
	};
	// where내부엔 idWhere가 하나밖에 없어서 and조합을 할수 없다. where블럭을 여러개 써서 or만 할수 있다.
	struct xWhereAttr {
		ID idWhere = 0;
		XGAME::xParam param;
		void Serialize( XArchive& ar ) const {
			ar << idWhere;
			ar << param;
		}
		void DeSerialize( XArchive& ar, int ) {
			ar >> idWhere;
			ar >> param;
		}
	};
	struct xWhere {
		XVector<xWhereAttr> aryAttr;	// 스팟,타입,코드,지역등의 속성을 and조건으로 여러개 담을수 있도록.
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
	};
	struct xDrop {
		XVector<xWhere> aryWhere;	///< 어디서의 or조합
		XVector<ID> aryItems;		///< 아이템의 or목록. 이중에 하나가 떨어짐
		float prob = 1.f;					///< 드랍확률. 실수로 누락시킬수도 있으므로 디폴트값은 100%
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
	};
	//////////////////////////////////////////////////////////////////////////
	// 퀘스트 하나의 프로퍼티
	struct xProp {
		_tstring strIdentifier;
		_tstring strIcon;
		ID idProp = 0;
		ID idName = 0;		// 퀘 제목
		ID idDesc = 0;		// 퀘 설명
		XVector<xWhen*> aryWhen;	///< 언제(등장조건) 리스트
		XVector<xCond2*> aryConds;
		XVector<xDrop> aryDrops;	///< 이 퀘를 받으면 드롭되어야 하는 특정아이템
		XVector<XGAME::xReward*> aryReward;	///< 보상리스트
		XGAME::xtUnit typeUnit = XGAME::xUNIT_NONE;		// 특정 유닛들로만 군단이 구성되어야 한다면.
	private:
		float rateExp = 1.f;					///< 보상경험치에 곱해지는 조정치
	public:
		char repeat = 0;				///< 반복퀘(퀘를 종료하면 다시 받아진다.0이면 반복아님.-1이면 무한반복, 1이상은 회수)
		bool bDelQuestItem = true;				///< 퀘 종료후 퀘템 삭제 여부.
		//
		~xProp();
		bool IsTutorial() const {
			const ID idWhat = aryConds[ 0 ]->idWhat;
			if( idWhat == XGAME::xQC_EVENT_UI_ACTION ) 
				return true;
			return false;
		}
		int GetExpReward( int level ) const;
		friend class XQuestProp;
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
	}; // struct xProp {
private:
	static XQuestProp *s_pInstance;
private:
	std::unordered_map<ID, xProp*> m_mapQuests;
	std::vector<xProp*> m_aryQuests;
	std::unordered_map<_tstring, xProp*> m_mapQuestsStr;
	bool m_bError;
	void Init() {
		m_bError = false;
	}
	void Destroy();
public:
	XQuestProp();
	virtual ~XQuestProp() { Destroy(); }
	//
	GET_ACCESSOR( const std::vector<xProp*>&, aryQuests );
	//
	BOOL Load( LPCTSTR szXml );
	bool OnDidFinishLoad() override;
	bool IsExist( xProp *pProp );
	xProp* LoadQuest( XEXmlNode& nodeQuest );
	void LoadCond( XEXmlNode& nodeCondBlock, xCond2 *pCond, xProp *pQuestProp );
	void LoadWhen( XEXmlNode& nodeWhen, xWhen *pCond, xProp *pQuestProp );
	void LoadWhere( XEXmlNode& nodeWhere, xCond2 *pCond, xProp *pQuestProp );
	void LoadWhere2( XEXmlNode& nodeDrop, xWhere *pOutWhere, xProp *pQuestProp );
	void LoadWhat( XEXmlNode& nodeWhat, xCond2 *pCond, xProp *pQuestProp );
	void LoadHow( XEXmlNode& nodehow, xCond2 *pCond, xProp *pQuestProp );
//	void LoadRewards( XEXmlNode& nodeReward, XGAME::xReward *pCond, xProp *pQuestProp );
	void LoadDrops( XEXmlNode& nodeDrop, xDrop *pDrop, xProp *pQuestProp );
	void ParseEvent( XEXmlAttr& attr, ID *pOutEvent, XGAME::xParam *pOut, xProp *pQuestProp );
	//
	xProp* GetpProp( const _tstring& strIdentifier ) {
		auto itor = m_mapQuestsStr.find( strIdentifier );
		if( itor == m_mapQuestsStr.end() )
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
		auto itor = m_mapQuests.find( idProp );
		if( itor == m_mapQuests.end() )
			return nullptr;
		xProp *pProp = itor->second;
		XBREAK( pProp == nullptr );
		return pProp;
	}
//	xCond2* GetpCondInProp( xProp *pProp, ID idCond );
	void LoadLegion( XEXmlNode& nodeWhat, xProp *pQuestProp );
	bool IsValidQuest( const _tstring& ids );
	bool CheckValidQuest();
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
}; // class XQuestProp

