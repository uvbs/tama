#include "stdafx.h"
#include "XEServerMainFrm.h"
#include "XFramework/server/XServerMain.h"
//#include "XMain.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XEServerMainFrame* XEServerMainFrame::s_pMainFrm = NULL;

CString XEServerMainFrame::GetstrTitle() const
{
	CString strTitle = m_strTitle;
	strTitle += _T( " - " );
	strTitle += __DATE__;
	strTitle += _T( " " );
	strTitle += __TIME__;
	return strTitle;
}

void XEServerMainFrame::SetTitle( void )
{
	CString strTitle = GetstrTitle();		// virtual
	CWnd *pMainfrm = AfxGetMainWnd(); 
	pMainfrm->SetWindowText( (LPCTSTR)strTitle );  
}
