#include <Audio.h>
#include <cstdio>
#include <SDL.h>
#include <SDL_mixer.h>

namespace SPF
{
	Audio mAudio;

	void Audio::Init()
	{
		if (Mix_Init(MIX_INIT_OGG) == 0)
		{
			FatalError(Mix_GetError());
		}

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
		{
			FatalError(Mix_GetError());
		}

		Mix_AllocateChannels(16);
	}

	void Audio::Dispose()
	{
		Mix_CloseAudio();
		Mix_Quit();
	}

	ResourceIndex Audio::LoadSound(unsigned char* buffer, int length)
	{
		Mix_Chunk* sample = Mix_LoadWAV_RW(SDL_RWFromMem(buffer, length), SDL_TRUE);
		for (ResourceIndex i = 0; i < mSounds.size(); ++i)
		{
			if (!mSounds[i])
			{
				mSounds[i] = sample;
				return i;
			}
		}
		mSounds.push_back(sample);
		return mSounds.size() - 1;
	}

	int Audio::PlaySound(ResourceIndex sound, bool looping)
	{
		Mix_Chunk* sample = (Mix_Chunk*)mSounds[sound];
		return Mix_PlayChannel(-1, sample, looping ? -1 : 0);
	}

	void Audio::StopChannel(int channel)
	{
		Mix_HaltChannel(channel);
	}

	void Audio::DeleteSound(ResourceIndex sound)
	{
		Mix_FreeChunk((Mix_Chunk*)mSounds[sound]);
		mSounds[sound] = nullptr;
	}

	float Audio::GetSoundVolume()
	{
		return mSoundVolume;
	}

	float Audio::GetMusicVolume()
	{
		return mMusicVolume;
	}

	void Audio::SetVolume(float soundVolume, float musicVolume)
	{
		mSoundVolume = soundVolume;
		mMusicVolume = musicVolume;
		Mix_Volume(-1, (int)(soundVolume*MIX_MAX_VOLUME));
		Mix_VolumeMusic((int)(musicVolume*MIX_MAX_VOLUME));
	}

	ResourceIndex Audio::LoadMusic(unsigned char* buffer, int length)
	{
		Mix_Music* music = Mix_LoadMUS_RW(SDL_RWFromMem(buffer, length), SDL_TRUE);
		if (!music)
		{
			FatalError(Mix_GetError());
		}
		for (ResourceIndex i = 0; i < mMusics.size(); ++i)
		{
			if (!mMusics[i])
			{
				mMusics[i] = music;
				return i;
			}
		}
		mMusics.push_back(music);
		return mMusics.size() - 1;
	}

	void Audio::DeleteMusic(ResourceIndex music)
	{
		if (mMusics[music])
		{
			Mix_FreeMusic((Mix_Music*)mMusics[music]);
			mMusics[music] = nullptr;
		}
	}

	void Audio::PlayMusic(ResourceIndex music)
	{
		if (mMusics[music])
		{
			if (Mix_PlayMusic((Mix_Music*)mMusics[music], -1) == -1)
			{
				FatalError(Mix_GetError());
			}
		}
	}

	void Audio::StopMusic()
	{
		Mix_HaltMusic();
	}

	bool Audio::IsMusicPlaying()
	{
		return (Mix_PlayingMusic() != 0);
	}

}

extern "C"
{
	DLLExport SPF::ResourceIndex LoadSound(unsigned char* buffer, int length)
	{
		return SPF::mAudio.LoadSound(buffer, length);
	}

	DLLExport int PlaySound(SPF::ResourceIndex sound, bool looping)
	{
		return SPF::mAudio.PlaySound(sound, looping);
	}

	DLLExport void StopChannel(int channel)
	{
		SPF::mAudio.StopChannel(channel);
	}

	DLLExport void DeleteSound(SPF::ResourceIndex sound)
	{
		SPF::mAudio.DeleteSound(sound);
	}

	DLLExport float GetSoundVolume()
	{
		return SPF::mAudio.GetSoundVolume();
	}

	DLLExport float GetMusicVolume()
	{
		return SPF::mAudio.GetMusicVolume();
	}

	DLLExport void SetVolume(float soundVolume, float musicVolume)
	{
		return SPF::mAudio.SetVolume(soundVolume, musicVolume);
	}

	DLLExport SPF::ResourceIndex LoadMusic(unsigned char* buffer, int length)
	{
		return SPF::mAudio.LoadMusic(buffer, length);
	}

	DLLExport void DeleteMusic(SPF::ResourceIndex music)
	{
		SPF::mAudio.DeleteMusic(music);
	}

	DLLExport void PlayMusic(SPF::ResourceIndex music)
	{
		SPF::mAudio.PlayMusic(music);
	}

	DLLExport void StopMusic()
	{
		SPF::mAudio.StopMusic();
	}

	DLLExport int IsMusicPlaying()
	{
		return SPF::mAudio.IsMusicPlaying() ? 1 : 0;
	}
}
