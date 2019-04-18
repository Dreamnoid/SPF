#include "Audio.h"
#include <cstdio>

Audio mAudio;

void Audio::Init()
{
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		exit(2);
	}

	for (ResourceIndex i = 0; i < SoundsCount; ++i)
	{
		mSounds[i] = nullptr;
	}

	Mix_AllocateChannels(16);
}

void Audio::Dispose()
{
	Mix_CloseAudio();
}

ResourceIndex Audio::LoadSound(unsigned char* buffer, int length)
{
	Mix_Chunk* sample = Mix_LoadWAV_RW(SDL_RWFromMem(buffer, length), 1);
	for (ResourceIndex i = 0; i < SoundsCount; ++i)
	{
		if (!mSounds[i])
		{
			mSounds[i] = sample;
			return i;
		}
	}
	FatalError("All sound slots are used");
}

int Audio::PlaySound(ResourceIndex sound, bool looping)
{
	Mix_Chunk* sample = mSounds[sound];
	return Mix_PlayChannel(-1, sample, looping ? -1 : 0);
}

void Audio::StopChannel(int channel)
{
	Mix_HaltChannel(channel);
}

void Audio::DeleteSound(ResourceIndex sound)
{
	Mix_FreeChunk(mSounds[sound]);
	mSounds[sound] = nullptr;
}

float Audio::GetVolume()
{
	return mVolume;
}

void Audio::SetVolume(float volume)
{
	mVolume = volume;
	Mix_Volume(-1, (int)(volume*MIX_MAX_VOLUME));
}

extern "C"
{
	DLLExport ResourceIndex LoadSound(unsigned char* buffer, int length)
	{
		return mAudio.LoadSound(buffer, length);
	}

	DLLExport int PlaySound(ResourceIndex sound, bool looping)
	{
		return mAudio.PlaySound(sound, looping);
	}

	DLLExport void StopChannel(int channel)
	{
		return mAudio.StopChannel(channel);
	}

	DLLExport void DeleteSound(ResourceIndex sound)
	{
		return mAudio.DeleteSound(sound);
	}

	DLLExport float GetVolume()
	{
		return mAudio.GetVolume();
	}

	DLLExport void SetVolume(float volume)
	{
		return mAudio.SetVolume(volume);
	}
}
