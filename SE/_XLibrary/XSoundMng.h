#ifndef __XSOUNDMNG_H__
#define __XSOUNDMNG_H__
#include "global.h"
#include "debug.h"
#include "xString.h"
//
class XSoundMng
{
public:
	static XSoundMng *s_pSoundMng;
protected:
	char        m_szBGMFilename[256];
	char        m_szBGMBackup[256];

	BOOL     m_bMuteBGM;
    float       m_BGMVolume;		// 0~1
	float       m_BGMVolumeTemp;
	float		m_SoundVolume;	// 0~1
	BOOL     m_bMuteSound;
	float		m_distListener;		// 리쓰너(플레이어)와 소리나는곳까지의 거리
public:
    XSoundMng();
    virtual ~XSoundMng();

    // Background Music
	virtual BOOL OpenBGMusic( const char* szFilename, BOOL repeat = TRUE) { strcpy_s( m_szBGMFilename, szFilename ); return TRUE; }
    virtual void PlayBGMusic() {}
    virtual void StopBGMusic() {}
    virtual void SetBGMVolume(float volume) { m_BGMVolume = m_BGMVolumeTemp = volume; }
    virtual void FadeOutBGM(float fLevel);
    float  GetBGMVolume() { return m_BGMVolume; }
	BOOL IsCompleteFadeOut() { return m_BGMVolumeTemp <= 0; }

	virtual BOOL IsBGMusic() { return FALSE; }

	virtual void SetBGMMute( BOOL mute );
	virtual BOOL GetBGMMute() { return m_bMuteBGM; }
	void PushBGMusic();
	void PopBGMusic();

    // Effect sound
	GET_SET_ACCESSOR( float, distListener );
	virtual void SetSoundVolume( float volume ) { m_SoundVolume = volume; }
	virtual float GetSoundVolume() { return m_SoundVolume; }
	void SetSoundMute( BOOL mute );
	BOOL GetSoundMute() { return m_bMuteSound; }
	BOOL OpenPlaySound( int nFileID ) { 
		if( OpenSound( nFileID ) == FALSE ) {
			XLOG( "SOUND ID:%d load failed", nFileID );
			return FALSE;
		}
		PlaySound( nFileID );
		return TRUE;
	}
    virtual BOOL OpenSound( int FileID ) { return TRUE; }
	virtual void CloseSound( int FileID ) {}
	virtual void PlaySound( int FileID ) {}
	virtual void StopSound( int FileID ) {}

	virtual void RemoveAll() {}
};

#define SOUNDMNG	XSoundMng::s_pSoundMng

//------------------------------------------------------------------------------------------------------------
// hello
BOOL SetVolumeConfig(BOOL bMuteBGM,  float BGMVolume,  BOOL bMuteSound  ,float SndVolume);		
BOOL GetVolumeConfig(BOOL &bMuteBGM, float &BGMVolume, BOOL &bMuteSound ,float &SndVolume);

#endif

//------------------------------------------------------------------------------------------------------------
