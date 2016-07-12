#ifndef __XSOUNDMNG_H__
#define __XSOUNDMNG_H__
#include "etc/global.h"
#include "etc/debug.h"
//#include "xString.h"
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
protected:
	virtual void PlayBGMusic( void ) {}
	virtual BOOL OpenBGMusic( const char* szFilename, BOOL repeat = TRUE) { 
		strcpy_s( m_szBGMFilename, szFilename ); 
		return TRUE; 
	}
public:
    XSoundMng();
    virtual ~XSoundMng();

    // Background Music
	void OpenPlayBGMusic( const char* cFilename, BOOL repeat = TRUE) { 
		if( OpenBGMusic( cFilename, repeat ) )
			PlayBGMusic();
	}
    virtual void StopBGMusic( void ) {}
    virtual void SetBGMVolume(float volume) { m_BGMVolume = m_BGMVolumeTemp = volume; }
    virtual void FadeOutBGM(float fLevel);
    float  GetBGMVolume( void ) { return m_BGMVolume; }
	BOOL IsCompleteFadeOut( void ) { return m_BGMVolumeTemp <= 0; }

	virtual BOOL IsBGMusic( void ) { return FALSE; }

	virtual void SetBGMMute( BOOL mute );
	virtual BOOL GetBGMMute( void ) { return m_bMuteBGM; }
	void PushBGMusic( void );
	void PopBGMusic( void );
	BOOL SaveConfig( void );
	BOOL LoadConfig( void );

    // Effect sound
	GET_SET_ACCESSOR( float, distListener );
	virtual void SetSoundVolume( float volume ) { m_SoundVolume = volume; }
	virtual float GetSoundVolume( void ) { return m_SoundVolume; }
	void SetSoundMute( BOOL mute );
	BOOL GetSoundMute() { return m_bMuteSound; }
	BOOL OpenPlaySound( int nFileID, BOOL bRepeat=FALSE ) { 
		if( OpenSound( nFileID ) == FALSE ) {
			XLOG( "SOUND ID:%d load failed", nFileID );
			return FALSE;
		}
		PlaySound( nFileID, bRepeat );
		return TRUE;
	}
    virtual BOOL OpenSound( int FileID ) { return TRUE; }
	virtual void CloseSound( int FileID ) {}
	virtual void PlaySound( int FileID, BOOL bRepeat=FALSE ) {}
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
