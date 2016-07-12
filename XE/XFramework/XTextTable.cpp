
#include "StdAfx.h"
#include "XFramework/XTextTable.h"
#include "etc/Token.h"
#include "XResObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XTextTable *TEXT_TBL = nullptr;

#if defined(_CLIENT) && defined(WIN32)
bool compID( XTextTable::xText *pText1, XTextTable::xText *pText2 )
{
	return pText1->idText < pText2->idText;
}
#endif // CLIENT && WIN32
//////////////////////////////////////////////////////////////////////////
void XTextTable::Destroy() 
{
#if defined(_CLIENT) && defined(WIN32)
 	XLIST4_DESTROY( m_listText );
 	XLIST4_DESTROY( m_listRemark );
#endif 
// 	for( auto itor = m_mapText.begin(); itor != m_mapText.end(); itor++ ) {
// 		TCHAR *pText = itor->second;
// 		SAFE_DELETE_ARRAY( pText );
// 	}
	m_mapText.clear();
	TEXT_TBL = nullptr;
}

LPCTSTR XTextTable::Find( ID idText )
{
	auto itor = m_mapText.find( idText );
	if( itor == m_mapText.end() )
		return nullptr;
	return (itor->second).c_str();
// 	LPCTSTR szText = (*itor).second;
// 	XBREAK( szText == nullptr );
// 	return szText;
}

/**
 @brief 
*/
BOOL XTextTable::Load( LPCTSTR _szRes )
{
	const _tstring strRes = _szRes;
//	TCHAR szBuff[0xffff];
	CToken token;
	if( token.LoadFile( strRes, XE::TXT_UTF16 ) == xFAIL ) {
#if defined(WIN32)|| defined(_VER_IOS)
		_tstring strPath = XE::_GetPathPackageRoot();
#else
		_tstring strPath;
#endif
		strPath += strRes;
		XERROR( "\"%s\" not found.", strPath.c_str() );
		return FALSE;
	}
#if defined(_CLIENT) && defined(WIN32)
	m_strRes = XE::GetFileName( strRes );
#endif
	CONSOLE("%s....token.LoadFile() success", strRes.c_str() );
	BOOL result = TRUE;
	ID idLast = 0;
	while(1) {
		DWORD id = (DWORD)token.GetNumber();		// id번호
		if( id == TOKEN_ERROR ) {
			XALERT( "text id %d read after error", idLast );
			result = FALSE;
			break;
		}
		if( id == TOKEN_EOF )
			break;
		if( Find( id ) ) {
			XALERT( "text id %d: duplication id", id );
		}
#if defined(_CLIENT) && defined(WIN32)
		// 아이디 앞에 주석이 있었으면.
		if( token.m_listRemarkPrev.size() > 0 ) {
			auto pRemark = new xRemark;
			pRemark->idText = id;
			pRemark->bPrev = TRUE;			// 아이디 앞 주석
//			XLIST_LOOP_REF( token.m_listRemarkPrev, _tstring, strRemark )
			for( auto& strRemark  : token.m_listRemarkPrev ) {
				pRemark->strText += _T("//");
				pRemark->strText += strRemark;
				pRemark->strText += _T("\r\n");	// 주석은 \n을 포함해서 저장한다.
			};
			m_listRemark.Add( pRemark );
		}
		//
		token.GetToken();		// text
		//
		if( token.m_listRemarkCurrLine.size() > 0 ) {
			auto pRemark = new xRemark;
			pRemark->idText = id;
			pRemark->bPrev = FALSE;			// 현재 줄 주석.
//			XLIST_LOOP_REF( token.m_listRemarkCurrLine, _tstring, strRemark )
			for( auto& strRemark : token.m_listRemarkCurrLine ) {
				pRemark->strText += _T("//");
				pRemark->strText += strRemark;
				pRemark->strText += _T( " " );	// 
			};
			pRemark->strText += _T( "\r\n" );	// 주석은 \n을 포함해서 저장한다.
			m_listRemark.Add( pRemark );
		}
#else
		token.GetToken();		// text
// not defined(_CLIENT) && defined(WIN32)
#endif 

		AddText( id, token.m_Token );

#ifdef _VER_ANDROID
//	    XLOGXN("%d %s", id, pText );
#endif
		idLast = id;
		if( id > m_maxID )
			m_maxID = id;
	}
#if defined(_CLIENT) && defined(WIN32)
	m_listText.sort( compID );

//	ID idFind = AddText( _T("창병") );
//	idFind = AddText( _T( "장병" ) );
#endif // 
    XLOGXN("text loading success.....%s. lastID:%d num=%d", strRes.c_str(), idLast, m_mapText.size() );
	return result;
}

// TCHAR* XTextTable::CreateText( LPCTSTR szStr )
// {
// 	TCHAR szBuff[ 0x10000 ];
// 	int len = _tcslen( szStr );
// 	TCHAR *pText = nullptr;
// 	if( len > 0 )
// 	{
// 		XE::ReplaceReturnChar( szBuff, szStr );
// 		len = _tcslen( szBuff );
// 		pText = new TCHAR[ len + 1 ];		// string을 위한 메모리 할당
// 		_tcscpy_s( pText, len + 1, szBuff );
// 	}
// 	else
// 	{
// 		pText = new TCHAR[ 1 ];		// string을 위한 메모리 할당
// 		pText[ 0 ] = 0;
// 	}
// 	return pText;
// }

bool XTextTable::PreProcess( LPCTSTR szSrc, _tstring* pOutStr )
{
	TCHAR szBuff[ 0x10000 ];
	const int len = _tcslen( szSrc );
	if( len > 0 ) {
		if( XASSERT(len < 0xffff) ) {
			XE::ReplaceReturnChar( szBuff, szSrc );
			*pOutStr = szBuff;
			return true;
		}
	}
	return true;
}

void XTextTable::AddText( ID idText, LPCTSTR szStr )
{
	_tstring strNew;
	auto bOk = PreProcess( szStr, &strNew );
	if( XBREAK(!bOk) )
		return;
// 	TCHAR *pNewText = CreateText( szStr );
// 	if( XBREAK( pNewText == nullptr ) )
// 		return;
	m_mapText[ idText ] = strNew;
#if defined(_CLIENT) && defined(WIN32)
	xText *pStruct =  new xText;
	pStruct->idText = idText;
//	pStruct->szText = pNewText;
	pStruct->strText = strNew;
	m_listText.Add( pStruct );
	m_mapTextByStr[ strNew ] = idText;
#endif // 
}

LPCTSTR XTextTable::GetText( ID id ) const 
{
	if( id == 0 )
		return _T("");
	auto itor = m_mapText.find( id );
	if( itor != m_mapText.end() ) {
		return (itor->second).c_str();
	}
	XBREAKF(1, "XTEXT(%d) is null", id );
	return _T("");
}

const _tstring& XTextTable::GetstrText( ID id ) const
{
	if( id == 0 )
		return m_strEmpty;
	auto itor = m_mapText.find( id );
	if( itor != m_mapText.end() ) {
		return (itor->second);
	}
	XBREAKF(1, "GetstrText(%d) is null", id );
	return m_strEmpty;
}


#if defined(_CLIENT) && defined(WIN32)
/**
 @brief 새 아이디를 부여해서 szStr텍스트를 추가한다.
 중복되는것이 있다면 그 아이디의 텍스트를 바꾼다.
 @param idStart 원하는 번호대가 있다면 그 시작번호를 입력한다.
*/
ID XTextTable::AddTextAutoID( LPCTSTR szStr, ID idStart, DWORD range/* = 10000*/ )
{
	const _tstring strNew = szStr;
	// 우선 중복되는 텍스트가 있는지 찾는다.
	auto itorByText = m_mapTextByStr.find( strNew );
	if( itorByText != m_mapTextByStr.end() ) {
		// 이미 같은 텍스트가 있음.
		const ID idExist = itorByText->second;
		XBREAK( idExist == 0 );
		auto itor = m_mapText.find( idExist );
		if( XASSERT(itor != m_mapText.end()) ) {
			_tstring strProcessed;
			bool bOk = PreProcess( strNew.c_str(), &strProcessed );
			if( XASSERT(bOk) ) {
				itor->second = strProcessed;		// 해당 아이디의 텍스트를 교체
				m_mapTextByStr.erase( itorByText );		// 기존거 삭제하고 
				m_mapTextByStr[ strProcessed ] = idExist;		// 다시 키를 잡아줌.
			}
		}
		return idExist;
	} else {
		// 중복되는 텍스트가 없음. 새 아이디 부여받아서 추가.
		ID idEmpty = GetEmptyID( idStart, range );
		if( XASSERT(idEmpty > 0) ) {
			// 새 아이디로 추가
			AddText( idEmpty, strNew.c_str() );
			// 소트
			m_listText.sort( compID );
			return idEmpty;
		}
	}
	return 0;
}
// ID XTextTable::AddTextAutoID( LPCTSTR szStr, ID idStart, DWORD range/* = 10000*/ )
// {
// 	// 우선 중복되는 텍스트가 있는지 찾는다.
// 	ID idExist = GetIDByText( szStr );
// 	if( idExist > 0 ) {
// 		LPCTSTR pText = GetText( idExist );
// 		if( XE::IsSame( szStr, pText ) == FALSE )
// 		{
// 			m_listText.DelByID( idExist );		// 리스트에서 삭제
// // 			TCHAR *pNewText = CreateText( pText );
// // 			XBREAK( pNewText == nullptr );
// 
// 			auto bOk = PreProcess( pText )
// 			AddText( idExist, pNewText );
// // 			m_listText.Add( pNewText );
// // 			m_mapTextByStr[ _tstring(pNewText) ] = idExist;	// 어차피 저장은 m_listText로할거기땜에 맵에 중복된 데이타가 있어도 상관없음.
// // 			m_mapText[ idExist ] = pNewText;
// 			SAFE_DELETE_ARRAY( pText );
// 			m_listText.sort( compID );
// 		}
// 		return idExist;
// 	} else
// 	// 중복된 텍스트가 없으면 새 아이디 부여받아서 추가.
// 	{
// 		ID idEmpty = GetEmptyID( idStart, range );
// 		if( XASSERT(idEmpty > 0) )
// 		{
// 			AddText( idEmpty, szStr );
// 			m_listText.sort( compID );
// 			return idEmpty;
// 		}
// 	}
// 	return 0;
// }

ID XTextTable::GetEmptyID( ID idStart, DWORD range )
{
	ID idNum = idStart;
	do {
		LPCTSTR szText = GetTextNoAssert( idNum );
		if( XE::IsEmpty(szText) )
			return idNum;

	} while( ++idNum < idStart + range );		// 가급적 이보다 큰 텍스트 아이디는 쓰지 말것.
	return 0;
}

static void WriteString( XResFile& resFile, LPCTSTR szStr )
{
	int size = (_tcslen(szStr) ) * sizeof(TCHAR);	// 바이너리로 저장하므로 스트링뒤에 null은 저장하면 안된다.
	resFile.Write( (void*)szStr, size );
}

BOOL XTextTable::Save( LPCTSTR szFile )
{
	XResFile resFile;
	LPCTSTR szFull = XE::MakePackageFullPath( DIR_PROP, szFile );
//	LPCTSTR szFull = XE::MakePackageFullPath( DIR_PROP, _T("text_ko.txt") );
	resFile.Open( szFull, XResFile::xWRITE );
	if( resFile.GetLastError() == XE::ERR_READONLY ||
		resFile.GetLastError() == XE::ERR_PERMISSION_DENIED )
	{
		XALERT( "파일을 check out하십시오.\n%s", XE::GetFileName( szFull ) );
		return FALSE;
	}
	TCHAR bom = 0xFEFF;
	resFile.Write( &bom, 2 );
	auto itor = m_listRemark.begin();
	for( auto pText : m_listText ) {
		// 현재 아이디 앞에 붙어야할 주석이 있는가.
		xRemark *pRemark = GetRemarkByID( pText->idText, itor );
		if( pRemark && pRemark->bPrev ) {
			LPCTSTR szStr = XE::Format(_T("\r\n%s"), pRemark->strText.c_str());
			WriteString( resFile, szStr );	// <<- 이함수는 이런용도가 아닌데?
		}
		// id와 텍스트를 쓴다. 리턴은 붙이지 않는다.
		{
			LPCTSTR szStr = XE::Format( _T("%d\t\"%s\""), pText->idText, pText->strText.c_str() );
			WriteString( resFile, szStr );
		}
		// 현재줄 주석이 있으면 쓴다.
		if( pRemark && pRemark->bPrev == FALSE ) {
			LPCTSTR szStr = XE::Format( _T("\t%s"), pRemark->strText.c_str() );
			WriteString( resFile, szStr );
		} else {
			LPCTSTR tc = _T("\r\n");
			resFile.Write( (void*)tc, sizeof(TCHAR) * 2 );
		}

	};
	CONSOLE("save text");
	return TRUE;	
}

XTextTable::xRemark* XTextTable::GetRemarkByID( ID idText, XList4<xRemark*>::iterator& itor )
{
	// itor위치부터 검색
	while( itor != m_listRemark.end() ) {
//		xRemark **ppRemark = m_listRemark.GetCurrent( &itor );
		auto pRemark = (*itor);
		if( pRemark->idText == idText ) {
			++itor;
			return pRemark;
		} else
			break;
	}
	return nullptr;
}

/**
 @brief pTblOther의 텍스트를 this로 합친다.
*/
bool XTextTable::DoMerge( XTextTable* const pTblOther )
{
	auto& listOther = pTblOther->GetlistText();
	for( auto pTextOther : listOther ) {
		const ID idTextOther = pTextOther->idText;
		auto itor = m_mapText.find( idTextOther );
		if( itor != m_mapText.end() ) {
			// 같은번호의 텍스트가 0이미 있음
			const auto& strExist = itor->second;
			// 기존거 삭제
			auto itorByText = m_mapTextByStr.find( strExist );
// 			if( XASSERT(itorByText != m_mapTextByStr.end()) ) {
			if( itorByText != m_mapTextByStr.end() ) {	// 이게 없는 경우도 있네...
				m_mapTextByStr.erase( itorByText );
			}
			m_mapTextByStr[ pTextOther->strText ] = pTextOther->idText;
			auto itorList = m_listText.GetItorByID( pTextOther->idText );
			if( XASSERT(itorList != m_listText.end()) ) {
				auto pText = (*itorList);
				pText->strText = pTextOther->strText;
			}
			// 새걸로 교체
			itor->second = pTextOther->strText;		
		} else {
			// 같은번호의 텍스트가 없음.
			AddText( pTextOther->idText, pTextOther->strText.c_str() );
		}
	}
	m_listText.sort( compID );
	return true;
}

bool XTextTable::GetOmissionTextToList( XTextTable* pTblTarget, XList4<xText>* pOutList )
{
	for( auto pText : m_listText ) {
		// 현재 텍스트가 타겟테이블에 있는지...
		auto szTextTarget = pTblTarget->Find( pText->idText );
		if( szTextTarget == nullptr ) {
			// 없으면 리스트에 추가
			pOutList->Add( *pText );
		}
	}
	return true;
}

/**
 @brief src엔 있으나 this엔 없는 텍스트를 src로부터 복사해 채워넣는다.
*/
int XTextTable::FillEmptyTextFromSrc( XTextTable* pTblSrc, XList4<xText>* pOutList )
{
	int num = 0;
	auto& listSrc = pTblSrc->GetlistText();
	for( auto pTextSrc : listSrc ) {
		LPCTSTR szText = Find( pTextSrc->idText );
		if( !szText ) {
			AddText( pTextSrc->idText, pTextSrc->strText.c_str() );
			++num;
			if( pOutList )
				pOutList->Add( *pTextSrc );
		}
	}
	m_listText.sort( compID );
	return num;
}

/**
 @brief szText에 한글이 섞였는지를 검사한다.
*/
bool XTextTable::IsIncludedHangul( LPCTSTR szText )
{
	const int len = _tcslen( szText );
	for( int i = 0; i < len; ++i ) {
		if( XE::IsHangul( szText[i] ) )
			return true;
	}
	return false;
}
/**
 @brief 한글이 섞인 텍스트를 골라서 pOutAry에 담는다.
 @param idStart 번호가 지정되면 그 번호 이상의 텍스트만 대상으로 한다.
*/
int XTextTable::GetHangulTextToAry( XVector<xText>* pOutAry, ID idStart )
{
	for( auto itor : m_mapText ) {
		xText text;
		text.idText = itor.first;
		text.strText = itor.second;
		if( text.idText > idStart ) {
			if( IsIncludedHangul( text.strText.c_str() ) ) {
				pOutAry->Add( text );
			}
		}
	}
	return pOutAry->size();
}

#endif // CLIENT && WIN32