#ifndef __XSOUNDMNGOPENAL_H__
#define __XSOUNDMNGOPENAL_H__


#include <list>
#include "XSoundMng.h"
#include "XOpenAL.h"
#include "Ogg\\XOgg.h"
//
typedef struct 
{
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
//	XOALObj		*m_pBGMusic;
	XOgg			*m_pOgg;
	XOggObj		*m_pBGMusic;

    Sound_List  m_SoundList;
public:
    XSoundMngOpenAL();
    virtual ~XSoundMngOpenAL();

    // Background Music
    virtual BOOL OpenBGMusic( const char* szFilename, BOOL repeat = TRUE);
    virtual void PlayBGMusic();
    virtual void StopBGMusic();
    virtual void SetBGMVolume(float volume);
	virtual void FadeOutBGM(float fLevel);
	virtual BOOL IsBGMusic();

    // Effect sound
    virtual BOOL OpenSound( int FileID );
	virtual void CloseSound( int FileID );
	virtual void PlaySound( int FileID );
	virtual void StopSound( int FileID );

	virtual void RemoveAll();
};

#endif
//------------------------------------------------------------------------------------------------------------

