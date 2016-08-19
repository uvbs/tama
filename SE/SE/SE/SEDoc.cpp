
// SEDoc.cpp : CSEDoc 클래스의 구현
//

#include "stdafx.h"
#include "SE.h"

#include "SEDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSEDoc

IMPLEMENT_DYNCREATE(CSEDoc, CDocument)

BEGIN_MESSAGE_MAP(CSEDoc, CDocument)
END_MESSAGE_MAP()


// CSEDoc 생성/소멸

CSEDoc::CSEDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CSEDoc::~CSEDoc()
{
}

BOOL CSEDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CSEDoc serialization

void CSEDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CSEDoc 진단

#ifdef _DEBUG
void CSEDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSEDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSEDoc 명령
