// XDlgConvert.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "XDlgConvert.h"
#include "XList.h"
#include "SprDat.h"

// XDlgConvert 대화 상자입니다.

IMPLEMENT_DYNAMIC(XDlgConvert, CDialog)

XDlgConvert::XDlgConvert(CWnd* pParent /*=nullptr*/)
	: CDialog(XDlgConvert::IDD, pParent)
	, m_strConsole(_T(""))
{

}

XDlgConvert::~XDlgConvert()
{
}

void XDlgConvert::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
	DDX_Text(pDX, IDC_EDIT1, m_strConsole);
}


BEGIN_MESSAGE_MAP(XDlgConvert, CDialog)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(ID_GO, &XDlgConvert::OnBnClickedGo)
END_MESSAGE_MAP()



void XDlgConvert::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	TCHAR szFilename[256];
	int i = 0;
	list<CString> listFiles;
	while(1)
	{
		memset( szFilename, 0, sizeof(szFilename) );
		DragQueryFile( hDropInfo, i, szFilename, 256 );
		if( szFilename[0] == 0 )
			break;
		m_listFiles.push_back( CString(szFilename) );
		listFiles.push_back( CString(szFilename) );
		i ++;
	}
	//
	LIST_LOOP( listFiles, CString, itor, pStr )
	{
		LV_ITEM lvi;

		lvi.mask = LVIF_TEXT;
		lvi.iItem = m_nNumList;

		lvi.iSubItem = 0;
		CString strNum;
		strNum.Format( _T("%d"), m_nNumList );
		lvi.pszText = (LPTSTR)((LPCTSTR)strNum);
		m_ctrlList.InsertItem( &lvi );

		lvi.iSubItem = 1;
		lvi.pszText = (LPTSTR)((LPCTSTR)pStr);
		m_ctrlList.SetItem( &lvi );
		m_nNumList ++;
	} END_LOOP;
	
	CDialog::OnDropFiles(hDropInfo);
}

BOOL XDlgConvert::OnInitDialog()
{
	CDialog::OnInitDialog();

	DragAcceptFiles( TRUE );

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	TCHAR *szLable[2] = 
	{
		_T("번호"), _T("파일명")
	};
	int nColWidth[2] = { 50, 500 };
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	int		i;
	for( i = 0; i < 2; i ++ )
	{
		lvc.iSubItem = i;
		lvc.pszText = szLable[i];
		lvc.cx = nColWidth[i];
		lvc.fmt = LVCFMT_LEFT;
		m_ctrlList.InsertColumn( i, &lvc );
	}
	m_nNumList = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void XDlgConvert::OnBnClickedGo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	XSprDat *pSprDat = nullptr;
	LIST_LOOP( m_listFiles, CString, itor, str )
	{
		Message( _T("%s......"), str );		// 풀패스
		pSprDat = new XSprDat;			// SprDat객체 생성
		std::vector<xLayerInfoByAction> aryLayerInfo;
		pSprDat->Load( str, &aryLayerInfo, TRUE );		// 파일 로딩
		pSprDat->ConvertLowReso();		// 스프라이트 데이타 컨버트
		pSprDat->Save( str, aryLayerInfo );
		SAFE_DELETE( pSprDat );
		Message( _T("finished\r\n") );
	} END_LOOP;
}

void	XDlgConvert::Message( LPCTSTR sz, ... )
{
	TCHAR buff[4096];
	va_list	vl;

    va_start(vl, sz);
    _vstprintf_s(buff, sz, vl);
    va_end(vl);

	CString s(buff);

	UpdateData(TRUE);		// 컨트롤에서 변수
	s.Format( _T("%s"), buff );
	m_strConsole += s;
	if( m_strConsole.GetLength() > 0xffff )
		m_strConsole = _T("");		// clear
	UpdateData( FALSE );	// 변수에서 컨트롤
	::OutputDebugString( m_strConsole );		// TRACE로 한글출력이 안되서 이걸로 바꿈

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pEdit->SetSel( m_strConsole.GetLength(), m_strConsole.GetLength() );
//	XResFile *pRes = _tfopen( _T("aaa.txt"), _T("ab") );
//	_tcscpy_s( buff, s );
//	pRes->Write( buff, _tcslen(buff), sizeof(TCHAR), pRes );
//	pRes->Close();
}

