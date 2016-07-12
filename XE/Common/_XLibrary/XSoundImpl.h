#pragma once
#include "etc/global.h"
#include "etc/debug.h"
#include "etc/Timer.h"
#include <vector>
//
/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/04/19 16:16
*****************************************************************/
class XSoundMng2
{
public:
	static std::shared_ptr<XSoundMng2>& sGet();
	static void sDestroyInstance();
public:
	XSoundMng2();
	virtual ~XSoundMng2() { Destroy(); }
	//cKey를 플레이시키고 idStream를 얻는다.
	ID PlaySound( const std::string& strKey );
	// cKey를 로딩만 한다. idSound를 리턴한다.
	ID PreOpenSound( const std::string& strKey );
	void StopSoundAll( const std::string& strKey );
	void StopSound( ID idStream );
	void PauseSound( ID idStream );
	void SetVolumeSound( ID idStream );
private:
	static std::shared_ptr<XSoundMng2> s_spInstance;
	XSoundImpl* m_pImpl = nullptr;			// 플랫폼별 사운드 플레이 구현부.
	std::map< std::string, ID > m_mapDat;		// key:filename, value: idSound
	std::map< ID, ID > m_mapStream;					// key:idStream, value: idSound
	std::map< ID, std::string > m_mapStreamByKey;	// key:idStream, value: filename
	std::string m_strBGM;			// bgm file
	bool m_bMuteBGM = false;
	bool m_bMuteSound = false;
	float m_volBGM = 1.f;			// bgm volume(master)
	float m_volSound = 1.f;		// sound volume(master)
	float m_volMaster = 1.f;		// BGM/Sound통합 volume
private:
	void Init() {}
	void Destroy() {}
}; // class XSoundMng2

std::shared_ptr<XSoundMng2> XSoundMng2::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XSoundMng2>& XSoundMng2::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XSoundMng2>( new XSoundMng2 );
	return s_spInstance;
}
void XSoundMng2::sDestroyInstance() {
	s_spInstance.reset();
}
////////////////////////////////////////////////////////////////
XSoundMng2::XSoundMng2()
{
	XBREAK( s_spInstance != nullptr );
	Init();
}

void XSoundMng2::Destroy()
{
}


/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/04/19 16:32
*****************************************************************/
class XBgm : public XSoundMng2
{
public:
	static std::shared_ptr<XBgm>& sGet();
	static void sDestroyInstance();
public:
	XBgm();
	virtual ~XBgm() { Destroy(); }
	//
private:
	static std::shared_ptr<XBgm> s_spInstance;
	void Init() {}
	void Destroy() {}
}; // class XBgm

std::shared_ptr<XBgm> XBgm::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XBgm>& XBgm::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XBgm>( new XBgm );
	return s_spInstance;
}
void XBgm::sDestroyInstance() {
	s_spInstance.reset();
}
////////////////////////////////////////////////////////////////
XBgm::XBgm()
{
	XBREAK( s_spInstance != nullptr );
	Init();
}

void XBgm::Destroy()
{
}

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/04/19 16:32
*****************************************************************/
class XSoundEffectMng : public XSoundMng2
{
public:
	static std::shared_ptr<XSoundEffectMng>& sGet();
	static void sDestroyInstance();
public:
	XSoundEffectMng();
	virtual ~XSoundEffectMng() { Destroy(); }
	//
private:
	static std::shared_ptr<XSoundEffectMng> s_spInstance;
	void Init() {}
	void Destroy() {}
}; // class XSoundEffectMng

std::shared_ptr<XSoundEffectMng> XSoundEffectMng::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XSoundEffectMng>& XSoundEffectMng::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XSoundEffectMng>( new XSoundEffectMng );
	return s_spInstance;
}
void XSoundEffectMng::sDestroyInstance() {
	s_spInstance.reset();
}
////////////////////////////////////////////////////////////////
XSoundEffectMng::XSoundEffectMng()
{
	XBREAK( s_spInstance != nullptr );
	Init();
}

void XSoundEffectMng::Destroy()
{
}
