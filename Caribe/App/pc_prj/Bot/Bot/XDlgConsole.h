#pragma once
#include "Resource.h"

// XDlgConsole 대화 상자입니다.

class XDlgConsole : public CDialog
{
	DECLARE_DYNAMIC(XDlgConsole)
	CRITICAL_SECTION m_cs;

public:
	XDlgConsole(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~XDlgConsole();

	void MessageString( LPCTSTR szString );
	void Message( LPCTSTR sz, ... );
	void Message( const char *cFormat, ... );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CONSOLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strConsole;
	afx_msg void OnSize(UINT nType, int cx, int cy);
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
#if 0
// 이제 CONSOLE디파인은 Debug.h로 옮겨져서 XLIB->Console을 호출하게 된다.
//#define CONSOLE( F, ... )			::XConsole( (_T(F)), ##__VA_ARGS__ )	
//#define CONSOLE( F, ... )			{}
//#define CONSOLE_ALERT(F,...)		::XConsoleAlert( (_T(F)), ##__VA_ARGS__ )
//#define CCONSOLE(F,...)	\
//	if( GetDlgConsole() )		\
//		(GetDlgConsole()->Message((F), ##__VA_ARGS__))

/*
#define CONSOLE(F,...)	\
	if( GetDlgConsole() )		\
		(GetDlgConsole()->Message((_T(F)), ##__VA_ARGS__))

#define CONSOLE_ALERT(F,...)		{ GetDlgConsole()->Message((_T(F)), ##__VA_ARGS__);		\
										  XALERT( F, ##__VA_ARGS__ ); }
*/

#ifdef _DEBUG
//#define DBG_CONSOLE	CONSOLE
#else
//	#define DBG_CONSOLE(F,...)	{}
#endif

#endif 
