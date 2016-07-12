#pragma once
#include "SkillDef.h"
#include "XList.h"
#include "XXMLDoc.h"

class XTextTableUTF8;
class XConstant;
NAMESPACE_XSKILL_START
////////////////////////////////////////////////////////////////

class XSkillDat;
class XESkillMng
{
public:
	// 스킬매니저를 상속받아 써야 하는 구조상 이런식의 싱글톤은 못쓴다. 델리게이트 방식으로 바꿔야함.
// 	static XSkillMng* sGet() {
// 		if( s_pInstance == NULL )
// 			s_pInstance = new XSkillMng
// 	}
private:
// 	static XSkillMng *s_pInstance;		///< 싱글톤 인스턴스
// 	static XAutoPtr s_pDestroyer;		///< 프로그램 종료될때 자동 파괴될수 있게 사용
	//
	std::list<XSkillDat*> m_listSkillDat;
	_tstring m_strProp;		// 프로퍼티(xml)파일명
	XTextTableUTF8 *m_pTextTable;
	ID m_idKey;							///< 매니저가 생성될때마다 고유의 키값을 갖는다. 다른 객체에서 이 키값을 갖고 있다가 매니저가 재 로딩되었을 경우 키를 비교하여 다르면 포인터를 다시 읽어야 한다.
	void Init() {
		m_pTextTable = NULL;
		m_idKey = XE::GenerateID();
	}
	void Destroy();
protected:
	GET_ACCESSOR( XTextTableUTF8*, pTextTable );
public:
	XESkillMng();
	virtual ~XESkillMng() { Destroy(); }
	//
	GET_ACCESSOR( const _tstring&, strProp );
	LPCTSTR GetText( ID idText );
	GET_ACCESSOR( ID, idKey );
	//
	BOOL Load( LPCTSTR resXml );
	XSkillDat* LoadSkill( TiXmlElement *pRoot, XSkillDat *pSkillDat, EFFECT *pParentEffect );
	int LoadEffect( TiXmlElement *pElemEff, XSkillDat *pSkillDat, EFFECT *pEffect );
	BOOL ParsingAttr( TiXmlAttribute *pAttr, 
					const char *cAttrName,
					const char *cParam,
					XSkillDat* pSkillDat,
					EFFECT *pEffect );
	BOOL ParsingEffect( TiXmlAttribute *pAttr,
						const char *cAttrName,
						const char *cParam,
						XSkillDat* pSkillDat,
						EFFECT *pEffect );
	DWORD ParsingParam( const char *cParam );
#ifdef WIN32
	DWORD ParsingParam( LPCTSTR szParam );
#endif
	xtValType GetValType( TCHAR tcChar );
	void AddConstant( void );
	// cParam의 내용을 파싱해서 리스트에 넣어준다.
	// cParam의 내용은 콤마로 구분되어있다.
	// ex) "전사,마법사"
	template<typename T>
	BOOL ParsingParamList( const char *cParam, XList<T>& listOut ) {
		CToken token;
		if( token.LoadString( U82SZ( cParam ) ) == xFAIL ) {
			XLOG( "%s: load string error.", U82SZ( cParam ) );
			return FALSE;
		}
		while(token.GetToken()) {
			if( token.m_Token[0] != ',' ) {
				listOut.Add( (T)ParsingParam( token.m_Token ) );
			}
		}
		return TRUE;
	}
	//
	XSkillDat* Add( XSkillDat *pSkillDat );
	XSkillDat* FindByID( ID idDat );
	XSkillDat* FindByName( ID idName );
	XSkillDat* FindByIdentifier( LPCTSTR szIdentifier );
	XSkillDat* FindByRandomActive( void );
	XSkillDat* FindByRandomPassive( void ) ;
	//
	virtual XSkillDat* CreateSkillDat( void );
	virtual BOOL CustomParsingAttr( TiXmlAttribute *pAttr, 
									const char *cAttrName,
									const char *cParam,
									XSkillDat* pSkillDat,
									EFFECT *pEffect ) { return FALSE; }
	virtual BOOL CustomParsingEffect( TiXmlAttribute *pAttr,
									const char *cAttrName,
									const char *cParam,
									XSkillDat* pSkillDat,
									EFFECT *pEffect ) { return FALSE; }
	virtual void OnAddConstant( XConstant *pConstant ) {}	
};

#define XSKTEXTU8(N)	XU8TEXT(GetpTextTable(), N )
#define XSKTEXT(N)	U82SZ(XU8TEXT(GetpTextTable(), N ))
#define XSAME(UTF8, IDTEXT)	XE::IsSame( UTF8, XSKTEXTU8(IDTEXT) )


NAMESPACE_XSKILL_END

