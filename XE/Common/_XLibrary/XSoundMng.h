#pragma once
#include "etc/global.h"
#include "etc/debug.h"
#include "etc/Timer.h"
#include <vector>

#ifdef PlaySound
#undef PlaySound
#endif

#define xINVALID_STREAM		0

XE_NAMESPACE_START( xnSound )
//
struct xReplay {
	ID m_idSound = 0;
	bool m_bRepeat = false;
	std::function<void(ID)> m_funcCallback;
	xReplay( ID idSound, bool bRepeat )
		: m_idSound( idSound ), m_bRepeat( bRepeat ) {}
};
//
XE_NAMESPACE_END; // xnSound


class XSoundMng
{
protected:
public:
	static XSoundMng *s_pSoundMng;
private:
// 	char m_szBGMFilename[256];
// 	char m_szBGMBackup[256];
	std::string m_strBGM;
//	std::string m_strBGMBackup;
	std::queue<xnSound::xReplay*> m_qReplay;			// 플레이 실패한 사운드의 재 시도.
	xnSound::xReplay* m_pLastFailed = nullptr;		// 플레이 실패하여 마지막으로 생성된 재실행 포인터
	bool m_bMuteBGM = false;
	bool m_bMuteSound = false;
	bool m_bBGMLoop = false;
  float m_BGMVolumeLocal = 1.f;		// 0~1		// 현재 볼륨(페이드인아웃 등에 사용)
	float m_BGMMasterVolume = 1.f;	// 0~1
	float m_SoundMasterVolume = 1.f;	// 0~1
	float m_BGMMasterVolumeBackup = 1.f;
	float m_SoundMasterVolumeBackup = 1.f;	// 0~1
	float m_BGMVolPrev = 1.f;
	float m_SfxVolPrev = 1.f;

	//효과음 중복을 막는용도
	struct xSoundTimer {
	private:
		struct xInfo {
			ID idSound;
			CTimer timer;
			xInfo() {
				idSound = 0;
			}
		};
		std::vector<xInfo> aryTime;
	public:
		void AddSound(ID idSound);
		bool IsOver(ID idSound, float sec);	//마지막으로 재생하고 1초가 지났나
	};

	//효과음 중복재생 제한
	xSoundTimer m_SoundTimer; // = xSoundTimer();
private:
	void Init()	{	}
public:
	XSoundMng();
	virtual ~XSoundMng();
	///< 
	/// 전체 볼륨을 조절한다. 게임옵션등에서 조절하는 볼륨은 이것이다.
	GET_ACCESSOR_CONST( float, BGMMasterVolume );
	GET_ACCESSOR_CONST( float, SoundMasterVolume );
	void PushBGMVol( float vol );
	void PushSfxVol( float vol );
	void PopBGMVol();
	void PopSfxVol();
	void SetBGMMasterVolume( float vol ) {
		m_BGMMasterVolume = vol;
		SetBGMVolumeFinal( m_BGMVolumeLocal * vol );
	}
	// 일단은 이런게 있으면 안된다. 환경음의 경우 각자 볼륨이 다른 상태에서 플레이되고 있는데
	// 일괄적으로 마스터볼륨으로 다 바꿔버리면 곤란
	// 각 스트림이 현재 플레이되고 있는 볼륨값(마스터볼륨이 곱해지지 않은)을 알고 있어야 하며, 
	// 그 볼륨값에 새 마스터볼륨값을 곱해서 다시 셋 해야한다.
	void SetSoundMasterVolume( float vol ) {
		m_SoundMasterVolume = vol;
		SetSoundVolumeAllSound( vol );
	}
	GET_BOOL_ACCESSOR( bMuteBGM );
	GET_BOOL_ACCESSOR( bMuteSound );
	void SetbMuteBGM( bool bMute );
	void SetbMuteSound( bool bMute );
	void TogglebMuteBGM() {
		SetbMuteBGM( !m_bMuteBGM );
	}
	void TogglebMuteSound() {
		SetbMuteSound( !m_bMuteSound );
	}
	GET_ACCESSOR_CONST( float, BGMVolumeLocal );
	GET_SET_ACCESSOR_CONST( bool, bBGMLoop );
	GET_ACCESSOR_CONST( const std::string&, strBGM );
    // Background Music
	void OpenPlayBGMusic( const char* cFilename, bool repeat = true) { 
		if( OpenBGMusic( cFilename, repeat ) )
			PlayBGMusic();
	}
	virtual bool IsPlayableBGM() const = 0;
	virtual bool IsPlayableSound() const = 0;
	virtual bool OpenBGMusic( const char* szFileTitle, bool repeat = true);
	virtual void PlayBGMusic() {
		XLOGXN( "playbgm: %s", m_strBGM.c_str() );
	}
	virtual void StopBGMusic() {
		m_strBGM.clear();
	}
	/// 로컬 볼륨을 조절한다. 페이드인아웃등에 쓰이는 일시적 볼륨조절이다.
	/**
		@brief 
		@param volume 0 ~ 1
	*/
	virtual void SetBGMVolumeLocal( float volLocal ) {
		m_BGMVolumeLocal = volLocal;
	}
	virtual void SetBGMVolumeFinal( float volFinal ) {
	}
	bool IsBGMusic() const { 
		return !m_strBGM.empty(); 
	}

//  	void SetBGMMute( bool mute );
    // Effect sound
	virtual void SetSoundVolume( ID idStream, float volume ) { 
	}
	virtual void SetSoundVolumeAllStream( ID idSound, float volume ) {
	}
	/// 플레이되고 있는 모든 사운드의 스트림의 볼륨을 조절
	virtual void SetSoundVolumeAllSound( float volume ) {	}
	virtual float GetSoundVolume() { return m_SoundMasterVolume; }
// 	void SetSoundMute(bool mute) { m_bMuteSound = mute; }
// 	bool GetSoundMute() { return m_bMuteSound; }
	//
	ID OpenPlaySound( ID idSound, bool bRepeat=false );
	//재생한지 1초도 안지났으면 다시 재생하지 않음 / repeat 지원안함
// 	ID OpenPlaySoundOneSec(ID idSound) {
// 		if (m_SoundTimer.IsOver(idSound, 1.f))
// 			return OpenPlaySound(idSound);
// 		return 0;
// 	}
	/// sec시간 이내에 다시 요청이 오면 플레이 하지 않는 버전.
	ID OpenPlaySoundBySec( ID idSound, float sec );
	const char* GetFullPath( ID idSound );
  virtual bool OpenSound( ID idSound ) { return true; }
	virtual void CloseSound( ID idSound ) {}
	virtual ID PlaySound( ID idSound, bool bRepeat=false ) { return 0; }
	virtual void StopSound( ID idStream ) {}

	virtual void RemoveAll() {}
	virtual void StopAllSound() = 0;
	void SetCallback( xnSound::xReplay* pReplay, std::function<void( ID )> func );
	xnSound::xReplay* GetpLastReplay() {
		auto pTemp = m_pLastFailed;
		m_pLastFailed = nullptr;		// 이 함수는 1회용임.
		return pTemp;
	}
	void Process( float dt );
protected:
	xnSound::xReplay* AddReplayToQ( ID idSound, bool bRepeat );
	const xnSound::xReplay* FrontPoppReplayQ() {
		if( m_qReplay.size() == 0 )
			return nullptr;
		auto pReplay = m_qReplay.front();
		m_qReplay.pop();
		return pReplay;
	}
};

#define SOUNDMNG	XSoundMng::s_pSoundMng

//------------------------------------------------------------------------------------------------------------
// hello
// bool SetVolumeConfig(bool bMuteBGM,  float BGMVolume,  bool bMuteSound  ,float SndVolume);		
// bool GetVolumeConfig(bool &bMuteBGM, float &BGMVolume, bool &bMuteSound ,float &SndVolume);


//------------------------------------------------------------------------------------------------------------
