#pragma once
#ifdef _VER_ANDROID
#ifdef _XSOUND2
// 신버전. sound.txt를 쓰는 버전

#include <list>
#include <string>
#include "XSoundMng.h"
//
struct ST_SOUND {
	ID idSound;
	XList4<ID> m_listStreams;		// 이 사운드파일로 플레이중인 모든 스트림
	std::string strFile;
	void DelStream( ID idStream ) {
		m_listStreams.Del( idStream );
	}
};

XE_NAMESPACE_START( xnSound )
//
struct xStream {
	ID m_idSound = 0;			// 음원파일의 번호
	ID m_idStream = 0;		// 플레이중인 스트림 아이디
	float m_VolLocal = 1.f;			// 스트림의 볼륨(마스터 볼륨이 곱해지지 않은 값)
};

//
XE_NAMESPACE_END; // xnSound

//
//using namespace std;
typedef std::list<ST_SOUND>           Sound_List;
typedef std::list<ST_SOUND>::iterator Sound_Itor;
//
class XSoundMngAndroid : public XSoundMng
{
private:
	std::string m_strBGMFullpath;	// bgm fullpath
//	bool m_bBGMLoop;
	Sound_List  m_SoundList;
	std::map<ID, xnSound::xStream> m_mapStreams;		// idStream/xStream
public:
	//
	XSoundMngAndroid();
	~XSoundMngAndroid();
	bool IsPlayableBGM() const override {
		return true;
	}
	bool IsPlayableSound() const override {
		return true;
	}
	// Background Music
	bool OpenBGMusic( const char* szFilename, bool repeat = true) override;
	void PlayBGMusic() override;
	void StopBGMusic() override;
	void SetBGMVolumeLocal(float volLocal) override;
	void SetBGMVolumeFinal( float volFinal ) override;
	// Effect sound
	bool OpenSound( ID idSound ) override;
	void CloseSound( ID idSound ) override;
	ID PlaySound( ID idSound, bool bRepeat = false ) override;
	void StopSound( ID idSound ) override;
 	void SetSoundVolume( ID idStream, float volLocal ) override;
 	void SetSoundVolumeAllStream( ID idSound, float volLocal ) override;
	void SetSoundVolumeAllSound( float volMaster ) override;
	void RemoveAll() override;
	void StopAllSound() override;
private:
	xnSound::xStream* GetpStream( ID idStream );
	ID DelStream( ID idStream );
	ST_SOUND* GetpSound( ID idSound );
};
//------------------------------------------------------------------------------------------------------------

#endif // xsound2
#endif // _VER_ANDROID
