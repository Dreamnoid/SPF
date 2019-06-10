#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>
#include "Core.h"

class Audio
{
private:
	float mSoundVolume = 1.0f;
	float mMusicVolume = 1.0f;

	std::vector<Mix_Chunk*> mSounds;
	std::vector<Mix_Music*> mMusics;

public:
	Audio() : mSounds(100), mMusics(20) {}

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