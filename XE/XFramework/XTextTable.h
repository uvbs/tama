#pragma once
#ifdef __cplusplus
#include "etc/Token.h"
#include <unordered_map>
//using namespace std;

class XTextTable;

class XTextTable
{
	ID m_maxID;
	std::unordered_map<ID, _tstring> m_mapText;	
	std::unordered_map<ID, _tstring>::iterator m_Itor;
	_tstring m_strEmpty;
#if defined(_CLIENT) && defined(WIN32)
public:
	struct xText {
		ID idText;
		_tstring strText;
		xText() {
			idText = 0;
		}
		xText( ID _idText, const _tstring& _strText ) 
			: idText(_idText), strText(_strText) { }
		ID getid() const {
			return idText;
		}
	};
	const std::unordered_map<ID, _tstring>& GetmapText() {
		return m_mapText;
	}
private:
	// 주석 블럭. 토큰과 토큰사이의 주석.
	struct xRemark {
		BOOL bPrev;			///< 아이디의 앞에 주석인가 텍스트의 뒤에 주석인가.
		ID idText;			///< 어떤 번호앞에 주석인지.
		_tstring strText;	///< 주석텍스트.
		ID getid() const {
			return idText;
		}
		xRemark() {
			bPrev = TRUE;
			idText = 0;
		}
	};
	XList4<xRemark*> m_listRemark;	// 주석블럭 리스트
	XList4<xText*> m_listText;		///< 모든텍스트들의 리스트(툴모드)
	std::unordered_map<_tstring, ID> m_mapTextByStr;		///< 텍스트로 검색할수 있는 맵(툴모드)
	_tstring m_strRes;			// 로딩한 파일의 패스
	/**
	 @brief idText의 텍스트가 없어도 assert를 안내는 버전
	*/
	LPCTSTR GetTextNoAssert( ID idText ) {
		auto itor = m_mapText.find( idText );
		if( itor == m_mapText.end() )
			return _T("");
		return ((*itor).second).c_str();
	}
	/**
	 @brief 텍스트로 아이디를 찾는 버전
	*/
	ID GetIDByText( LPCTSTR szStr ) {
		auto itor = m_mapTextByStr.find( szStr );
		if( itor == m_mapTextByStr.end() )
			return 0;
		return ( *itor ).second;
	}
	GET_ACCESSOR_CONST( const XList4<xText*>&, listText );
#endif // _CLIENT && WIN32
	void Init() {
// 		XBREAK( TEXT_TBL != NULL );
// 		TEXT_TBL = this;
		m_maxID = 0;
	}
	void Destroy();
public:
	// 테이블에 보관할때 어떤 인코딩타입으로 보관할건지 지정
	XTextTable() {	Init(); }
	virtual ~XTextTable() { Destroy(); }
	// get set
	GET_ACCESSOR_CONST( ID, maxID );
	LPCTSTR GetText( ID id ) const;
	const _tstring& GetstrText( ID id ) const;
	//
	BOOL Load( LPCTSTR szFilename );
	inline bool Load( const _tstring& strFile ) {
		return Load( strFile.c_str() ) != FALSE;
	}
//	void GetNextClear( void ) {
//		m_Itor = m_mapText.begin();
//	}
	void AddText( ID idText, LPCTSTR szStr );
#if defined(_CLIENT) && defined(WIN32)
	ID AddTextAutoID( LPCTSTR szStr, ID idStart = 1, DWORD range = 10000 );
	ID GetEmptyID( ID idStart = 1, DWORD range = 10000 );
	BOOL Save( LPCTSTR szFile );
	bool Save() {
		return Save( m_strRes.c_str() ) != FALSE;
	}
	xRemark* GetRemarkByID( ID idText, XList4<xRemark*>::iterator& itor );
	bool DoMerge( XTextTable* pTblOther );
	int GetHangulTextToAry( XVector<xText>* pOutAry, ID idStart = 0 );
	bool IsIncludedHangul( LPCTSTR szText );
	bool GetOmissionTextToList( XTextTable* pTableTarget, XList4<xText>* pOutList );
	int FillEmptyTextFromSrc( XTextTable* pTblSrc, XList4<xText>* pOutList = nullptr );
#endif
private:
// 	TCHAR* CreateText( LPCTSTR szStr );
// 	TCHAR* GetNext( void ) {
// 		if( m_mapText.end() == m_Itor )
// 			return NULL;
// 		return ( *( m_Itor++ ) ).second;
// 	}
	LPCTSTR Find( ID idText );
	bool PreProcess( LPCTSTR szSrc, _tstring* pOutStr );
};
#endif

extern XTextTable *TEXT_TBL;
#define XTEXT(id)	((TEXT_TBL)? TEXT_TBL->GetText( id ) : _T(""))
