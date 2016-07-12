#pragma once
#ifdef _VER_ANDROID
#ifdef _XSOUND2
// 신버전. sound.txt를 쓰는 버전

#include <list>
#include <string>
#include "XSoundMng.h"
//
typedef struct {
	ID FileID;
	unsigned int idSound;
	std::string strFile;
	int Use;
	CTimer timerPlay;	// 플레이 시작한 시간
} ST_SOUND;

//
using namespace std;
typedef list<ST_SOUND>           Sound_List;
typedef list<ST_SOUND>::iterator Sound_Itor;
//
class XSoundMngAndroid : public XSoundMng
{
private:
	//	XOpenAL		*m_pOpenAL;
	//	XOALObj		*m_pBGMusic;
	std::string m_strBGM;	// bgm fullpath
	BOOL m_bBGMLoop;
	Sound_List  m_SoundList;
public:
	//
	XSoundMngAndroid();
	~XSoundMngAndroid();
	// Background Music
	BOOL OpenBGMusic( const char* szFilename, BOOL repeat = TRUE) override;
	void PlayBGMusic() override;
	void StopBGMusic() override;
	void SetBGMVolume(float volume) override;
	void FadeOutBGM(float fLevel) override;
	BOOL IsBGMusic() override;
	// Effect sound
	BOOL OpenSound( ID idSound ) override;
	void CloseSound( ID idSound ) override;
	void PlaySound( ID idSound, BOOL bRepeat=FALSE ) override;
	void StopSound( ID idSound ) override;
	void RemoveAll() override;
	void StopAllSound() override;
};
//------------------------------------------------------------------------------------------------------------

#endif // xsound2
#endif // _VER_ANDROID
