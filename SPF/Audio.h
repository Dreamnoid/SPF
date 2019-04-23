#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include "Core.h"

constexpr uint32_t SoundsCount = 100;
constexpr uint32_t MusicsCount = 20;

class Audio
{
private:
	float mSoundVolume = 1.0f;
	float mMusicVolume = 1.0f;

	Mix_Chunk* mSounds[SoundsCount];
	Mix_Music* mMusics[MusicsCount];

public:
	void Init();
	void Dispose();

	ResourceIndex LoadSound(unsigned char* buffer, int length);
	int PlaySound(ResourceIndex sound, bool looping);
	void StopChannel(int channel);
	void DeleteSound(ResourceIndex sound);

	float GetSoundVolume();
	float GetMusicVolume();

	void SetVolume(float soundVolume, float musicVolume);

	ResourceIndex LoadMusic(unsigned char* buffer, int length);
	void DeleteMusic(ResourceIndex music);
	void PlayMusic(ResourceIndex music);
	void StopMusic();
	bool IsMusicPlaying();
};

extern Audio mAudio;