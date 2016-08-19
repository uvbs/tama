#ifndef __XRARE_SOUNDMNG_H__
#define __XRARE_SOUNDMNG_H__

#include "Rare.h"
#include "XSoundMng.h"
//
typedef struct 
{
    int FileID;
    RARESOUND Sound;
    int Use;
	
}ST_SOUND;

//
using namespace std;

typedef list<ST_SOUND*>           XSound_List;
typedef list<ST_SOUND*>::iterator XSound_Itor;


//
class XRareSoundMng : public XSoundMng
{
public:
	static XRareSoundMng *s_pRareSoundMng;
private:
	void Init() {
	}
	void Destroy() {}
protected:
    RARECONTEXT	m_Context;
    RARESOUND		m_BGMusic;	

    XSound_List  m_listSound;

public:
    XRareSoundMng();
    virtual ~XRareSoundMng();


    // Background Music
	// virtual
	virtual void RemoveAll();
    virtual BOOL OpenBGMusic( const char* szFilename, BOOL repeat = TRUE);
    virtual void SetBGMVolume(float volume);
    virtual void PlayBGMusic();
    virtual void StopBGMusic();
    virtual void FadeOutBGM(float Level);

//	virtual BOOL IsBGMusic(RARESOUND sound);
	virtual BOOL IsBGMusic( void );

    // Effect sound
    virtual BOOL OpenSound( int FileID );
	virtual void CloseSound( int FileID );
	virtual void PlaySound( int FileID );

};


#endif // __XRARE_SOUNDMNG_H__

//------------------------------------------------------------------------------------------------------------
