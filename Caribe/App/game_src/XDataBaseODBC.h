#pragma once

#include <sql.h>
#include <sqlext.h>



#define MAX_STRING_DSN		20
#define MAX_STRING_ID		20
#define MAX_STRING_PASSWORD 20

#define	MAX_COLUMN_COUNT    128
#define MAX_STR				512

class CDataBaseODBC 
{
public:
	CDataBaseODBC();
	~CDataBaseODBC();

	int		Initialize	();
	void	EndDB		();
	void	Disconnect  ();
	
	BOOL	ConnectDB( const TCHAR * szDSN = _T( "" ),					//sql db에 접속한다.
						const TCHAR * szUserName = _T( "" ),
						const TCHAR * szPassword = _T( "" ) );

	BOOL	ConnectDB2( const TCHAR * szDSN = _T( "" ),					//sql db에 접속한다.
						const TCHAR * szUserName = _T( "" ),
						const TCHAR * szPassword = _T( "" ) );

	SQLLEN	RowCount();
	int		ColumnCount();

	void	Clearhandle(SQLSMALLINT handletype, SQLHANDLE handle);
	void	ShowErrorInfo(SQLSMALLINT hType, SQLHANDLE handle, TCHAR *szErrorMsg, int cbErrorMsgMax, SQLINTEGER &pfNativeError, SQLSMALLINT &pcbErrorMsg );		// SQLRETURN rc, 
	void	ShowErrorInfo(SQLSMALLINT hType, TCHAR *szErrorMsg, int cbErrorMsgMax );																			// SQLRETURN rc, 

	SQLRETURN	AllocStmt();		
	void		FreeStmt();
	void		ReAllocStmt() 		{ FreeStmt(); AllocStmt();}

	SQLRETURN	Excute(const TCHAR * szSQL);		
	SQLRETURN	Fetch();

	SQLRETURN	BindColSmallint( SQLSMALLINT TargetType, SQLSMALLINT *nDest, SQLINTEGER nBuffLen, SQLLEN *rlength, SQLUSMALLINT nColNum = 0)
				{ return ::SQLBindCol( m_hstmt, nColNum, TargetType, (SQLPOINTER) nDest, nBuffLen, rlength); }
	//	SQLRETURN BindCol( int nColNum, SQLSMALLINT TargetType, SQLPOINTER Dest, SQLINTEGER nLen, SQLINTEGER  *StrLen_or_Idn);
	

	SQLRETURN	BindColint( SQLSMALLINT TargetType, int * nDest, SQLINTEGER nBuffLen, SQLLEN *rlength, SQLUSMALLINT nColNum = 0)
				{ return ::SQLBindCol( m_hstmt, nColNum, TargetType, (SQLPOINTER) nDest, nBuffLen, rlength); }
	SQLRETURN	BindStr( SQLSMALLINT TargetType, TCHAR* szpBuff, SQLINTEGER nBuffLen, SQLLEN *rlength, SQLUSMALLINT nColNum = 0)
				{ return ::SQLBindCol( m_hstmt, nColNum, TargetType, (SQLPOINTER) szpBuff, nBuffLen, rlength); }

	SQLRETURN	BindCol( SQLSMALLINT TargetType, void *nDest, int nBuffLen,  SQLLEN* plength, SQLUSMALLINT nColNum)
				{ return ::SQLBindCol( m_hstmt, nColNum, TargetType, (SQLPOINTER) nDest, (SQLINTEGER )nBuffLen, plength); }
	
	SQLRETURN	BindParameter( SQLSMALLINT fParamType, SQLSMALLINT fCType, SQLSMALLINT fSqlType, SQLULEN cbColDef, SQLSMALLINT ibScale, 
								SQLPOINTER rgbValue, SQLLEN cbValueMax, SQLLEN *pcbValue, SQLUSMALLINT nColNum)
				{ return ::SQLBindParameter( m_hstmt, nColNum, fParamType, fCType, fSqlType, cbColDef, ibScale, rgbValue, cbValueMax, pcbValue ); }

	SQLHSTMT    GethStmt(){return m_hstmt;}						//Handle statement
	void CloseCursor();


protected:
	SQLHENV     m_hEnv;
    SQLHDBC     m_hdbc;  
    
	SQLHSTMT    m_hstmt;						//Handle statement	
	BOOL		m_bIsConnect;
};
 
inline SQLRETURN CDataBaseODBC::AllocStmt()		
{
	SQLRETURN rc = ::SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );

	if (rc != SQL_SUCCESS )
	{
//		ShowErrorInfo(rc , SQL_HANDLE_STMT , m_hstmt);
	}

	return rc;
}

inline void CDataBaseODBC::FreeStmt()		
{ 
	if( m_hstmt != SQL_NULL_HANDLE ) ::SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
}

