#pragma once
#ifdef _VER_ANDROID
#ifndef _XSOUND2

// 구버전(sound.txt를 안쓰는 버전)


#include <list>
#include <string>
#include "XSoundMng.h"
//
typedef struct {
	int FileID;
	unsigned int idSound;
	std::string strFile;
	int Use;
}ST_SOUND;
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
	virtual ~XSoundMngAndroid();
	// Background Music
	BOOL OpenBGMusic( const char* szFilename, BOOL repeat = TRUE) override;
	void PlayBGMusic() override;
	void StopBGMusic() override;
	void SetBGMVolume(float volume) override;
	void FadeOutBGM(float fLevel) override;
	BOOL IsBGMusic() override;
	// Effect sound
	BOOL OpenSound( int FileID ) override;
	void CloseSound( int FileID ) override;
	void PlaySound( int FileID, BOOL bRepeat=FALSE ) override;
	void StopSound( int FileID ) override;
	void RemoveAll() override;

};
//------------------------------------------------------------------------------------------------------------

#endif // not xsound2
#endif // _VER_ANDROID
