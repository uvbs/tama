#pragma once
#ifdef _VER_IOS

#include <list>
#include "XSoundMng.h"
#include "XOpenAL.h"
//
typedef struct {
	int FileID;
	XOALObj *Sound;
	int Use;
} ST_SOUND;
//
using namespace std;
typedef list<ST_SOUND*>           Sound_List;
typedef list<ST_SOUND*>::iterator Sound_Itor;
//
class XSoundMngOpenAL : public XSoundMng
{
private:
	XOpenAL		*m_pOpenAL;
	XOALObj		*m_pBGMusic;
	Sound_List  m_SoundList;
public:
	XSoundMngOpenAL();
	virtual ~XSoundMngOpenAL();
	// Background Music
	BOOL OpenBGMusic( const char* szFilename, BOOL repeat = TRUE ) override;
	void PlayBGMusic() override;
	void StopBGMusic() override;
	void SetBGMVolume( float volume ) override;
	void FadeOutBGM( float fLevel ) override;
	BOOL IsBGMusic() override;
	// Effect sound
	BOOL OpenSound( ID FileID ) override;
	void CloseSound( ID FileID ) override;
	void PlaySound( ID FileID, BOOL bRepeat = FALSE ) override;
	void StopSound( ID FileID ) override;
	void RemoveAll() override;
};
#endif // _VER_IOS




//------------------------------------------------------------------------------------------------------------

