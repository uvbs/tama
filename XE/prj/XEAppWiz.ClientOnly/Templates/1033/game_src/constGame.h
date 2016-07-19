#pragma once
#include "defineGame.h"

#define BUTT01_U				_T("butt01_u.png")
#define BUTT01_D				_T("butt01_d.png") 
#define BUTT01_OFF			_T("butt01_off.png") 
#define BUTT01				BUTT01_U, BUTT01_D, BUTT01_OFF

namespace XGAME {
	enum xtScene { xSC_NONE,
					xSC_START,
					xSC_TITLE,
					xSC_LOBBY,
					xSC_OPTION,
					xSC_ENDING,
					xSC_INGAME,
				};
};

////////////////////////////////////////////////////////////////
// 글로벌 상수모음.
class XGlobalConst;
class XGameCommon;
extern XGlobalConst *XGC;
#include "XGlobal.h"
class XGlobalConst : public XGlobalVal
{
public:
private:
	XGameCommon *m_pGameCommon;
public:
	static void sCreateSingleton( XGameCommon *pGameCommon );
	static void sDestroySingleton( void );
	//
	int m_SampleInt;
	float m_SampleFloat;
	float m_SampleLuaFloat;
	std::string m_strSampleLua;
	//
private:
	void Init() {
		m_pGameCommon = NULL;
	}
	void Destroy() {
	}
public:
	XGlobalConst( XGameCommon *pGameCommon, LPCTSTR szXml ) { 
		// 이 클래스는 싱글톤으로만 사용할 수 있다.
		XBREAK( XGC != NULL );
		XGC = this;
		Init(); 
		m_pGameCommon = pGameCommon;
		// m_pGameCommon->GetpLua()로 부터 상수값들을 읽어들임.
		Load( XE::MakePath( DIR_SCRIPTW, szXml ) );
		LoadConst( szXml );
	}
	virtual ~XGlobalConst() { 
		Destroy(); 
		XGC = NULL;
	}
	//
	void LoadConst( LPCTSTR szXml );

};


