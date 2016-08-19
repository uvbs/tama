#pragma once
// CConsoleView 폼 뷰입니다.

class CConsoleView : public CFormView
{
	DECLARE_DYNCREATE(CConsoleView)

protected:
	CConsoleView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CConsoleView();
public:
	void Message( LPCTSTR sz, ... );
	void Message( const char *cFormat, ... );
	void MessageString( LPCTSTR sz );
public:
	enum { IDD = IDD_CONSOLEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	static CConsoleView *CreateOne( CWnd *pParent );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strConsole;
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


CConsoleView *GetConsoleView();
inline void XConsole( LPCTSTR szFormat, ... ) {
	if( GetConsoleView() )
	{
		TCHAR buff[1024];
		va_list	vl;
		va_start(vl, szFormat);
		_vstprintf_s(buff, szFormat, vl);
		va_end(vl);
		GetConsoleView()->MessageString( buff );
	}
}
#define CONSOLE( F, ... )			::XConsole( (_T(F)), ##__VA_ARGS__ )

//#define CONSOLE(F,...)			\
//	if( GetConsoleView() )		\
//		GetConsoleView()->Message((_T(F)), ##__VA_ARGS__)
#define CCONSOLE(F,...)	\
	if( GetConsoleView() )		\
		(GetConsoleView()->Message((F), ##__VA_ARGS__))

#ifdef _DEBUG
#define DBG_CONSOLE	CONSOLE
#else
	#define DBG_CONSOLE(F,...)	{}
#endif
