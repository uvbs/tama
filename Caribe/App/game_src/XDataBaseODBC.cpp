#include "StdAfx.h"
#include "XDataBaseODBC.h"

CDataBaseODBC::CDataBaseODBC()
{		
	m_hEnv				= SQL_NULL_HANDLE;
	m_hdbc				= SQL_NULL_HANDLE;
	m_hstmt				= SQL_NULL_HANDLE;
	m_bIsConnect		= FALSE;
	
	SQLRETURN rc = ::SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv );
	rc = ::SQLSetEnvAttr( m_hEnv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, SQL_IS_INTEGER );
}


CDataBaseODBC::~CDataBaseODBC()
{
	EndDB();
}

void CDataBaseODBC::Clearhandle(SQLSMALLINT handletype, SQLHANDLE handle)
{
	if( handle != SQL_NULL_HANDLE ) 
		::SQLFreeHandle(handletype, handle);
}

SQLRETURN CDataBaseODBC::Fetch()
{
	SQLRETURN	rc = ::SQLFetch( m_hstmt );

	return rc;
}


SQLRETURN	CDataBaseODBC::Excute(const TCHAR * szSQL)
{
	SQLRETURN	rc;
		
	rc = ::SQLExecDirect(m_hstmt, (SQLTCHAR *)szSQL, SQL_NTS);

	return rc;
}

 
int CDataBaseODBC::Initialize()
{
	SQLRETURN   rc;

	rc = SQLAllocEnv (&m_hEnv);

	return rc;
}

    
//DB Connect
BOOL CDataBaseODBC::ConnectDB( const TCHAR * szDSN, const TCHAR * szUserName, const TCHAR * szPassword )
{   	
	SQLRETURN rc;

	rc = ::SQLAllocHandle( SQL_HANDLE_DBC, m_hEnv, &m_hdbc );

	if( !(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) )
	{
//	ShowErrorInfo(rc , SQL_HANDLE_DBC , m_hdbc );
		return FALSE;
	}
	// SQL_HANDLE_DBC 할당 못받을 경우 에러 처리
	
	rc = ::SQLConnect( m_hdbc, (SQLTCHAR *) szDSN, SQL_NTS, (SQLTCHAR *) szUserName, SQL_NTS, (SQLTCHAR *) szPassword, SQL_NTS );
	
	if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
	{
		m_bIsConnect = TRUE;
	}
	else
	{
		// 에러이면 
//		ShowErrorInfo(rc , SQL_HANDLE_DBC, m_hdbc);
		return FALSE;
	}

    return TRUE;
}

    
//DB Connect
BOOL CDataBaseODBC::ConnectDB2( const TCHAR * szDSN, const TCHAR * szUserName, const TCHAR * szPassword )
{   	
	//SQLRETURN rc;

	//rc = ::SQLAllocHandle( SQL_HANDLE_DBC, m_hEnv, &m_hdbc );

	//if ( rc != SQL_SUCCESS)
	//{
	//	return FALSE;
	//}
	//
	//TCHAR			szOutConn[1024];
	//SQLSMALLINT		OutConnStrLen = 0;
	//rc = ::SQLDriverConnect( m_hdbc, NULL, 
	//	(SQLTCHAR*) TEXT("DRIVER={SQL Server Native Client 10.0};SERVER=192.168.0.18;Database=hellas;UID=hellas;PWD=links)415;Trusted_Connection=no"),
	//				SQL_NTS,
	//				szOutConn,
	//				1024, 
	//				&OutConnStrLen,
	//				SQL_DRIVER_NOPROMPT );//SQL_DRIVER_PROMPT );//SQL_DRIVER_COMPLETE );// );     

	//
	//if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
	//{
	//	m_bIsConnect = TRUE;
	//}
	//else
	//{
	//	// 에러이면 
	//	TCHAR err[1024];
	//	ShowErrorInfo( SQL_HANDLE_DBC, err, 1024);
	//	return FALSE;
	//}

    return TRUE;
}
 
void CDataBaseODBC::Disconnect()
{
    SQLRETURN   rc;
 
    rc = SQLDisconnect (m_hdbc);               // disconnect from database  
	rc = SQLFreeConnect (m_hdbc);              // free connection handle    
}

void CDataBaseODBC::EndDB()
{
	SQLRETURN   rc;

	Disconnect();

	rc = SQLFreeEnv (m_hEnv);                  // free environment handle
//    if (rc != SQL_SUCCESS ) ShowErrorInfo( rc, SQL_HANDLE_ENV, m_hEnv);
}

void CDataBaseODBC::ShowErrorInfo( SQLSMALLINT hType, TCHAR *szErrorMsg, int cbErrorMsgMax )
{
	SQLINTEGER		pfNativeError;
	SQLSMALLINT		pcbErrorMsg;
	SQLHANDLE		handle;

	switch( hType )
	{
	case SQL_HANDLE_ENV:	handle = m_hEnv;		break;
	case SQL_HANDLE_DBC:	handle = m_hdbc;		break;
	default:				handle = m_hstmt;		break;	
	}
	
	ShowErrorInfo( hType, handle, szErrorMsg, cbErrorMsgMax, pfNativeError, pcbErrorMsg );
}

void CDataBaseODBC::ShowErrorInfo( SQLSMALLINT hType, SQLHANDLE handle, TCHAR *szErrorMsg, int cbErrorMsgMax, SQLINTEGER &pfNativeError, SQLSMALLINT &pcbErrorMsg )
{
	SQLRETURN		retcode;
	SQLSMALLINT		iRecord = 1;
	SQLTCHAR		szSqlState[MAX_STR];

	retcode = SQLGetDiagRecW (hType, m_hstmt,
							  iRecord, 
							  szSqlState, &pfNativeError,
							  (SQLTCHAR*)szErrorMsg, (SQLSMALLINT)cbErrorMsgMax,
							  &pcbErrorMsg);
}

SQLLEN CDataBaseODBC::RowCount()	//* 레코드 개수를 구함 * /
{
	SQLLEN  nRow = 0;
	
	SQLRETURN  rc = ::SQLRowCount(m_hstmt, &nRow);

	if ( rc == SQL_SUCCESS)
	{
		return nRow;
	}
	else 
	{
		return -1;
	}
}

int CDataBaseODBC::ColumnCount()	//* 레코드 개수를 구함 * /
{
	SQLSMALLINT nCol = 0;	
	
	SQLRETURN  rc = ::SQLNumResultCols(m_hstmt, &nCol);

	if ( rc == SQL_SUCCESS )
	{
		return nCol;
	}
	else
	{
		return -1;
	}
}

void CDataBaseODBC::CloseCursor()
{
	::SQLCloseCursor( m_hstmt );
	::SQLFreeStmt( m_hstmt, SQL_UNBIND );
}
