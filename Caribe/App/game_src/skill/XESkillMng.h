#pragma once
#include "SkillDef.h"
#include "XList.h"
#include "XXMLDoc.h"

const int VER_PROP_SKILL = 1;

class XTextTableUTF8;
class XConstant;
XE_NAMESPACE_START( XSKILL )
////////////////////////////////////////////////////////////////
struct EFFECT;
class XSkillDat;
class XESkillMng;
class XESkillMng
{
public:
	// 스킬매니저를 상속받아 써야 하는 구조상 싱글톤 생성해서 쓰는 방식은 못쓴다. 델리게이트 방식으로 바꿔야함.
 	static XESkillMng* sGet() {
		return s_pInstance;
 	}
private:
 	static XESkillMng *s_pInstance;		///< 싱글톤 인스턴스
// 	static XAutoPtr s_pDestroyer;		///< 프로그램 종료될때 자동 파괴될수 있게 사용
	//
	XList4<XSkillDat*> m_listSkillDat;
	_tstring m_strProp;		// 프로퍼티(xml)파일명
	XTextTableUTF8 *m_pTextTable;
	ID m_idKey;							///< 매니저가 생성될때마다 고유의 키값을 갖는다. 다른 객체에서 이 키값을 갖고 있다가 매니저가 재 로딩되었을 경우 키를 비교하여 다르면 포인터를 다시 읽어야 한다.
	void Init() {
		s_pInstance = this;
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
	DWORD ParsingConstantAndNumber( LPCTSTR szParam );
#endif // WIN32
	DWORD ParsingConstantAndNumber( const char *cParam );
	virtual xCOND_PARAM ParsingCond( TiXmlAttribute *pAttr, 
									const char *cAttrName,
									const char *cVal,
									XSkillDat* pSkillDat,
									EFFECT *pEffect ) {
		return xCOND_PARAM();
	}
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
	inline XSkillDat* FindByIdentifier( const _tstring& strIdentifier ) {
		return FindByIdentifier( strIdentifier.c_str() );
	}
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
	virtual void AdjustEffectParam( XSkillDat* pDat, EFFECT *pEffect ) {}
	/**
	 @brief 테이블형태로 된 스트링을 읽어 어레이에 담는다.
	*/
	template<typename T, int N>
	int ReadTableAry( LPCTSTR szAttrName, 
						LPCTSTR idsSkill, 
						XArrayLinearN<T,N> *pOutAry, 
						LPCTSTR szStr, 
						xtValType valType ) {
		CToken token;
		token.LoadString( szStr );
		int idx = 0;
		// pOutAry.size() != 0 경우도 있다. 상위블럭에서 값을 입력한 경우 하위에 상속되기때문에. 그러므로 그런경우는 클리어 시키고 다시 읽는다.
		pOutAry->Clear();
		pOutAry->Add( 0.f );	// index0은 쓰지 않음.
		int level = 0;
		float numFirst = 0;
		while( 1 ) {
			if( token.IsEof() )
				break;
			numFirst = token.GetNumberF();
			if( numFirst == TOKEN_ERROR )
				break;
//			level = token.GetNumber();
			if( token.IsEof() )
				break;

			level = (int)numFirst;
			if( level != idx + 1 ) {
				XALERT( "skill %s(%s):레벨번호가 순차적이지 않습니다.level:%d",
													idsSkill,
													szAttrName,
													level );
				return 0;
			}
			token.GetToken();	// :
			float ability = token.GetNumberF();
			if( token.IsError() ) {
				XALERT( "skill %s(%s):잘못된 숫자입니다..level:%d", 
														idsSkill,
														szAttrName,
														level );
				return FALSE;
			}
			if( valType == xPERCENT ) {
				pOutAry->Add( ability / 100.f );
			}
			else
				pOutAry->Add( ability );
			++idx;
			// 레벨 10을 넘어가도 더이상 읽지 않는다.
			if( idx >= XGAME::MAX_SKILL_LEVEL )
				break;
		}
		// 만약 스트링이 테이블형태가 아니고 숫자하나일경우 0번인덱스에 값을 넣는다.
		if( pOutAry->size() == 1 ) {
			if( valType == xPERCENT )
				( *pOutAry )[ 0 ] = numFirst / 100.f;
			else
				( *pOutAry )[ 0 ] = numFirst;
		}
		return pOutAry->size();
	}
	int ReadTableAry2( LPCTSTR szAttrName, LPCTSTR idsSkill, XVector<float>* pOutAry, LPCTSTR szStr, xtValType valType );
	
	void LoadCond( TiXmlElement *pElemRoot, EFFECT *pEffect );
	virtual bool IsInverseParam( int invokeParam ) const { return false; }
	virtual bool IsRateParam( int idxParam ) const { return false; }
	void Serialize( XArchive& ar ) const;
	void DeSerialize( XArchive& ar, int );
}; // class XESkillMng


#define XSKTEXTU8(N)	XU8TEXT(GetpTextTable(), N )
#define XSKTEXT(N)	U82SZ(XU8TEXT(GetpTextTable(), N ))
#define XSAME(UTF8, IDTEXT)	XE::IsSame( UTF8, XSKTEXTU8(IDTEXT) )


XE_NAMESPACE_END

