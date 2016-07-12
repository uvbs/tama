/********************************************************************
	@date:	2014/09/16 20:26
	@file: 	D:\xuzhu_work\Project\iPhone\XE\XFramework\Game\XEComponents.h
	@author:	xuzhu
	
	@brief:	게임객체에 쓸수 있는 깨알같은 콤포넌트들 모음
*********************************************************************/
#pragma once

class XEComponent;
class XDelegateCompWave;
/****************************************************************
* @brief
* @author xuzhu
* @date	2016/02/03 12:14
*****************************************************************/
class XEComponentMng
{
public:
	XEComponentMng() { Init(); }
	virtual ~XEComponentMng() { Destroy(); }
	// get/setter
	GET_SET_BOOL_ACCESSOR( bActive );
	// public member
	XEComponent* AddComponent( XEComponent *pComp );
	XEComponent* FindComponentByFunc( const std::string& strFunc );
	XEComponent* FindComponentByIds( const std::string& strIdentifier );
	XEComponent* AddComponentWave( const std::string& strIdentifier
																, XDelegateCompWave *pDelegate = nullptr
																, float secDelay = 1.f
																, float secFade = 1.f
																, float minValue = 0.f
																, float maxValue = 1.f );
	int Process( float dt );
private:
	// private member
	XVector<XEComponent*> m_aryComponent;
	bool m_bActive = true;
private:
	// private method
	void Init() {}
	void Destroy();
}; // class XEComponentMng
