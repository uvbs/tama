#pragma once
#include "XXMLDoc.h"

const int VER_PROP_CLOUD = 1;

class XArchive;
////////////////////////////////////////////////////////////////

#define LOOP_SPOTS( ELEM ) \
	for( auto ELEM : spots ) {{
#define LOOP_HEXA( ELEM ) \
		for( auto ELEM : idxs ) {{
			
/**
 구름 객체
*/
class XPropCloud : public XXMLDoc
{
public:
	enum { MAX_IMG=4 };
	struct xHexa {
		int idx;		///< 헥사인덱스
		int idxImg;		///< 그림 인덱스
		bool bOpened = false;		// 지역에 속해있으나 오픈되어있는(그리지 않는)헥사 구름.
		xHexa() {
			idx = 0;
			idxImg = xRandom(MAX_IMG);
		}
		BOOL operator == ( const xHexa& h ) const {
			return h.idx == idx;
		}
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
	};
	struct xCloud {
		_tstring strIdentifier;		// 지역식별자(없을수도 있음)
		ID idCloud;			// 구름 고유 아이디
		int lvArea;		// 지역 레벨(0이 될 수 없음)
		int lvOpenable2 = 0;	// 구름을 열수 있는 레벨(0이면 lvArea를 기준으로 자동으로 계산한다.) 0이면 레벨에 관계없이 돈만 있으면 열수 있다.
		int cost;				// 구름 열때 가격
		XE::VEC2 vAdjust;	// 가격표 표시할때 위치 보정
		ID idName = 0;				///< 지역이름
		_tstring idsPrecedeArea;	/// 선행 오픈지역. 지정이 되어있다면 선행지역이 먼저 오픈되어야 열린다.
		_tstring idsQuest;			/// 먼저 깨야하는 퀘스트가 있다면.
		_tstring idsItem;		// 지역을 여는데 필요한 아이템
		XGAME::xtUnit unitUnlock = XGAME::xUNIT_NONE;		// 이 지역을 열면 언락시킬수 있는 유닛(xUNIT_ANY는 잠겨있는거 아무거나)
#ifdef _xIN_TOOL
		XList4<ID> spots;	// 구름밑에 가려질 스팟리스트
		XList4<xHexa> idxs;	///< 헥사구름 인덱스.
#else
		XVector<ID> spots;
		XVector<xHexa> idxs;
#endif
		xCloud() {
			idCloud = 0;
			lvArea = 0;
			cost = 0;
		}
		ID getid() {
			return idCloud;
		}
		template<int N>
		int GetSpotsToAry( XArrayLinearN<ID,N>& ary ) {
			LOOP_SPOTS( idSpot ) {
				ary.Add( idSpot );
			} END_LOOP;
			return ary.size();
		}
		void GetSpotsToAry( XVector<ID> *pOut );
		void AddSpot( ID idNew );
		void ClearSpots( void );
		///< 이 구름지역에 idSpot이 있는지 검사.
		bool IsHaveSpot( ID _idSpot );
		template<int N>
		int GetIdxsToAry( XArrayLinearN<xHexa, N>& ary ) {
			LOOP_HEXA( hexa ) {
				ary.Add( hexa );
			} END_LOOP;
			return ary.size();
		}
		void AddIdx( int idxNew );
		void ClearIdxs( void );
		BOOL IsHaveIdx( int idxFind );
		/// idxFind헥사가 이 구름에 속해있고 뚫려있는가.
		bool IsHaveIdxOpened( int idxFind );
#ifdef _xIN_TOOL
		void DelIdx( int idxDel ) {
			xHexa hexa;
			hexa.idx = idxDel;
			idxs.Del( hexa );
		}
#endif
		int GetSizeIdxs() {
			return idxs.size();
		}
		bool IsOpenable( int lvAcc ) {
			if( lvOpenable2 > 0 ) {
				if( lvAcc < lvOpenable2 )
					return false;
			} else {
// 				if( lvAcc + 2 < lvArea )	// 레벨제한없음.
// 					return false;
			}
			return true;
		}
		// idxHexa를 찾아서 bOpened를 채운다.
		bool SetIdxOpened( int idxHexa, bool bOpened );
		void Serialize( XArchive& ar ) const;
		void DeSerialize( XArchive& ar, int );
	}; // struct xCloud
#ifdef _xIN_TOOL
public:
	static ID sGenerateID( void ) {
		return ++s_idGlobal;
	}
	static void sSetidGlobal( ID idGlobal ) {
		s_idGlobal = idGlobal;
	}
private:
	static ID s_idGlobal;
#endif // _xIN_TOOL
private:
	XList4<xCloud*> m_Clouds;	// 툴때문에 리스트로 함.
 	std::map<_tstring, xCloud*> m_mapCloudsIds;
	std::map<ID, xCloud*> m_mapCloudsID;
	//
	void Init() {
	}
	void Destroy();
public:
	XPropCloud() {
		Init();
	}
	XPropCloud( LPCTSTR szXml );
	virtual ~XPropCloud() { Destroy(); }
	//
	bool OnDidFinishLoad() override;
#ifdef _xIN_TOOL
	BOOL Save( LPCTSTR szXml );
#endif
	int GetPropToAry( XArrayLinearN<xCloud*,512> *pOutAry );
	int GetPropToAry( XArrayLinearN<ID,512> *pOutAry  );
	int GetPropToAry( XVector<xCloud*> *pOutAry );
	int GetPropToAry( XVector<ID> *pOutAry );
	void AddCloud( XPropCloud::xCloud *pProp );
	xCloud* GetpProp( ID idCloud );
	xCloud* GetpProp( const _tstring& strIdentifier );
	xCloud* GetpProp( LPCTSTR szIdentifier ) {
		_tstring ids = szIdentifier;
		return GetpProp( ids );
	}
	int GetIdxHexaFromWorld( float wx, float wy );
	inline int GetIdxHexaFromWorld( const XE::VEC2& vWorld ) {
		return GetIdxHexaFromWorld( vWorld.x, vWorld.y );
	}
	int GetAreaAryByRect( float wx, float wy, float ww, float wh, std::vector<ID> *pOutAry );
	int GetAreaAryByRect( const XE::xRECT& rect, std::vector<ID> *pOutAry ) {
		return GetAreaAryByRect( rect.vLT.x, rect.vLT.y, rect.GetWidth(), rect.GetHeight(), pOutAry );
	}
	XE::VEC2 GetPosFromIdxHexa( int idx );
	XE::VEC2 GetCenterFromIdxHexa( int idx ) {
		XE::VEC2 v = GetPosFromIdxHexa( idx );
		v += HEXA_CENTER_OFFSET;
		return v;
	}
	ID GetCloudByidxHexa( int idxHexa );
	xCloud* GetpPropByidxHexa( int idxHexa );
	void DestroyCloud( ID idCloud );
	void DoReArrangeCloudRandom( void );
	ID GetidAreaHaveSpot( ID idSpot );
	xCloud* GetpAreaHaveSpot( ID idSpot );
	XE::VEC2 GetPosByArea( ID idCloud );
	XE::VEC2 GetPosByArea( xCloud *pProp );
	void UpdateAreaIds( xCloud *pProp );
	xCloud* GetAreaByPrecedeArea( LPCTSTR idsArea );
	inline xCloud* GetAreaByPrecedeArea( const _tstring& idsArea ) {
		return GetAreaByPrecedeArea( idsArea.c_str() );
	}
	bool IsOpenedSpot( ID idSpot, ID idArea, const XE::VEC2& vwPos );
	inline bool IsOpenedSpot( ID idSpot, xCloud *pPropArea, const XE::VEC2& vwPos ) {
		if( XBREAK(pPropArea == nullptr) )
			return false;
		return IsOpenedSpot( idSpot, pPropArea->idCloud, vwPos );
	}
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
}; // propCloud

extern XPropCloud *PROP_CLOUD;
