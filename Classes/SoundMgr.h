#ifndef _SOUNDMGR_H_
#define _SOUNDMGR_H_
#include "SimpleAudioEngine.h"
#include <unordered_map>
enum BackGroundMusic
{
	kBackGroundMusicMain
};
enum EffectMusic
{
	kEffectMusicButton,
	kEffectMusicBomb,
	kEffectStarErase,
};

class SoundMgr 
{
public:
	static SoundMgr *theMgr();
	void init();

	void playBackground();
	void playEffect(int effectType);
	void setMute(bool mute);
	bool isMute();
private:
	SoundMgr() : m_isMute(true){}
	~SoundMgr();
	void loadLastSetting();
	void saveCurSetting();
private:
	bool m_isMute;
	std::unordered_map<int, std::string> m_backgroundMusic;
	std::unordered_map<int, std::string> m_effectMusic;
};



#endif