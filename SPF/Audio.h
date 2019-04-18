#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include "Core.h"

constexpr uint32_t SoundsCount = 100;

class Audio
{
private:
	float mVolume = 1.0f;
	Mix_Chunk* mSounds[SoundsCount];

public:
	void Init();
	void Dispose();

	ResourceIndex LoadSound(unsigned char* buffer, int length);
	int PlaySound(ResourceIndex sound, bool looping);
	void StopChannel(int channel);
	void DeleteSound(ResourceIndex sound);
	float GetVolume();
	void SetVolume(float volume);
};

extern Audio mAudio;