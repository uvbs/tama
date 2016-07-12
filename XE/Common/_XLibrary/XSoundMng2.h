#pragma once
#include "etc/global.h"
#include "etc/debug.h"
#include "etc/Timer.h"
#include <vector>
//

class XSoundImpl;
/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/04/19 16:16
*****************************************************************/
class ISoundMng
{
public:
	ISoundMng() { Init(); }
	virtual ~ISoundMng() { Destroy(); }
	//cKey를 플레이시키고 idStream를 얻는다.
	ID PlaySound( const std::string& strKey );
	// cKey를 로딩만 한다. idSound를 리턴한다.
	ID PreOpenSound( const std::string& strKey );
	void StopSoundAll( const std::string& strKey );
	void StopSound( ID idStream );
	void PauseSound( ID idStream );
	void SetVolumeSound( ID idStream );
private:
	static std::shared_ptr<ISoundMng> s_spInstance;
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

////////////////////////////////////////////////////////////////

/****************************************************************
* @brief BGM관리자
* @author xuzhu
* @date	2016/04/19 16:32
*****************************************************************/
class XBgmMng : public ISoundMng
{
public:
	static std::shared_ptr<XBgmMng>& sGet();
	static void sDestroyInstance();
public:
	XBgmMng();
	virtual ~XBgmMng() { Destroy(); }
	//
private:
	static std::shared_ptr<XBgmMng> s_spInstance;
	void Init() {}
	void Destroy() {}
}; // class XBgm

std::shared_ptr<XBgmMng> XBgmMng::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XBgmMng>& XBgmMng::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XBgmMng>( new XBgmMng );
	return s_spInstance;
}
void XBgmMng::sDestroyInstance() {
	s_spInstance.reset();
}
////////////////////////////////////////////////////////////////
XBgmMng::XBgmMng()
{
	XBREAK( s_spInstance != nullptr );
	Init();
}

void XBgmMng::Destroy()
{
}

/****************************************************************
* @brief 사운드 이펙트 관리자
* @author xuzhu
* @date	2016/04/19 16:32
*****************************************************************/
class XSoundEffectMng : public ISoundMng
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
