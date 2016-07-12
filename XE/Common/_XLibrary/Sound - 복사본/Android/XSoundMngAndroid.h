#pragma once



#include <list>
#include <string>

#include "XSoundMng.h"

//

typedef struct
{
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

    virtual BOOL OpenBGMusic( const char* szFilename, BOOL repeat = TRUE);

    virtual void PlayBGMusic( void );

    virtual void StopBGMusic( void );

    virtual void SetBGMVolume(float volume);

	virtual void FadeOutBGM(float fLevel);

	virtual BOOL IsBGMusic( void );

	

    // Effect sound

    virtual BOOL OpenSound( int FileID );

	virtual void CloseSound( int FileID );

	virtual void PlaySound( int FileID, BOOL bRepeat=FALSE );

	virtual void StopSound( int FileID );

	

	virtual void RemoveAll();

};








//------------------------------------------------------------------------------------------------------------

