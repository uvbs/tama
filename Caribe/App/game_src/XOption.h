/********************************************************************
	@date:	2016/03/29 14:40
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XOption.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XFramework/XEOption.h"
/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/03/29 14:40
*****************************************************************/
class XOption : public XEOption
{
public:
	XOption();
	virtual ~XOption() { Destroy(); }
	// get/setter
	GET_SET_ACCESSOR_CONST( const std::string&, strcKeyCurrLang );
	GET_SET_BOOL_ACCESSOR( bSound );
	GET_SET_BOOL_ACCESSOR( bMusic );
	GET_SET_BOOL_ACCESSOR( bLow );
	GET_SET_ACCESSOR_CONST( float, volSound );
	GET_SET_ACCESSOR_CONST( float, volMusic );
	// public member
	bool Parse( const std::string& strcKey, CToken& token ) override;
	bool Write( std::string* pOut ) override;
private:
	// private member
	std::string m_strcKeyCurrLang;		// 현재 선택된 언어의 키
	bool m_bSound = true;
	bool m_bMusic = true;
	bool m_bLow = false;
	float m_volSound = 1.f;
	float m_volMusic = 0.5f;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XOption
