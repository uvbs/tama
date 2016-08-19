#pragma once
#include "Resource.h"
#include "alert.h"

// XDlgConsole ��ȭ �����Դϴ�.

class XDlgConsole : public CDialog
{
	DECLARE_DYNAMIC(XDlgConsole)

public:
	XDlgConsole(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~XDlgConsole();

	void MessageString( LPCTSTR szString );
	void Message( LPCTSTR sz, ... );
	void Message( const char *cFormat, ... );

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CONSOLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strConsole;
};

XDlgConsole *GetDlgConsole( void );

inline void XConsole( LPCTSTR szFormat, ... ) {
	if( GetDlgConsole() )
	{
		TCHAR buff[1024];
		va_list	vl;
		va_start(vl, szFormat);
		_vstprintf_s(buff, szFormat, vl);
		va_end(vl);
		GetDlgConsole()->MessageString( buff );
	}
}
inline void XConsoleAlert( LPCTSTR szFormat, ... ) {
	if( GetDlgConsole() )
	{
		TCHAR buff[1024];
		va_list	vl;
		va_start(vl, szFormat);
		_vstprintf_s(buff, szFormat, vl);
		va_end(vl);
		GetDlgConsole()->MessageString( buff );
		XAlertString( MB_OK, buff );
	}
}
#define CONSOLE( F, ... )			::XConsole( (_T(F)), ##__VA_ARGS__ )
#define CONSOLE_ALERT(F,...)		::XConsoleAlert( (_T(F)), ##__VA_ARGS__ )
#define CCONSOLE(F,...)	\
	if( GetDlgConsole() )		\
		(GetDlgConsole()->Message((F), ##__VA_ARGS__))
/*
#define CONSOLE(F,...)	\
	if( GetDlgConsole() )		\
		(GetDlgConsole()->Message((_T(F)), ##__VA_ARGS__))

#define CONSOLE_ALERT(F,...)		{ GetDlgConsole()->Message((_T(F)), ##__VA_ARGS__);		\
										  XALERT( F, ##__VA_ARGS__ ); }
*/
#ifdef _DEBUG
#define DBG_CONSOLE	CONSOLE
#else
	#define DBG_CONSOLE(F,...)	{}
#endif
