#include <vector>
#include <cstdio>
#include <Audio.h>
#include <SDL.h>
#include <SDL_mixer.h>

namespace SPF
{
	struct
	{
		float SoundVolume = 1.0f;
		float MusicVolume = 1.0f;

		std::vector<Mix_Chunk*> Sounds;
		std::vector<Mix_Music*> Musics;
	} AudioData;

	namespace Audio
	{
		void Init()
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

		void Dispose()
		{
			Mix_CloseAudio();
			Mix_Quit();
		}

		ResourceIndex LoadSound(unsigned char* buffer, int length)
		{
			Mix_Chunk* sample = Mix_LoadWAV_RW(SDL_RWFromMem(buffer, length), SDL_TRUE);
			for (ResourceIndex i = 0; i < AudioData.Sounds.size(); ++i)
			{
				if (!AudioData.Sounds[i])
				{
					AudioData.Sounds[i] = sample;
					return i;
				}
			}
			AudioData.Sounds.push_back(sample);
			return AudioData.Sounds.size() - 1;
		}

		int PlaySound(ResourceIndex sound, float volume, bool looping)
		{
			Mix_Chunk* sample = AudioData.Sounds[sound];
			int channel = Mix_PlayChannel(-1, sample, looping ? -1 : 0);
			Mix_Volume(channel, (int)(AudioData.SoundVolume * volume * MIX_MAX_VOLUME));
			return channel;
		}

		void StopChannel(int channel)
		{
			Mix_HaltChannel(channel);
		}

		void DeleteSound(ResourceIndex sound)
		{
			Mix_FreeChunk(AudioData.Sounds[sound]);
			AudioData.Sounds[sound] = nullptr;
		}

		float GetSoundVolume()
		{
			return AudioData.SoundVolume;
		}

		float GetMusicVolume()
		{
			return AudioData.MusicVolume;
		}

		void SetVolume(float soundVolume, float musicVolume)
		{
			AudioData.SoundVolume = soundVolume;
			AudioData.MusicVolume = musicVolume;
			Mix_Volume(-1, (int)(soundVolume*MIX_MAX_VOLUME));
			Mix_VolumeMusic((int)(musicVolume*MIX_MAX_VOLUME));
		}

		ResourceIndex LoadMusic(unsigned char* buffer, int length)
		{
			Mix_Music* music = Mix_LoadMUS_RW(SDL_RWFromMem(buffer, length), SDL_TRUE);
			if (!music)
			{
				FatalError(Mix_GetError());
			}
			for (ResourceIndex i = 0; i < AudioData.Musics.size(); ++i)
			{
				if (!AudioData.Musics[i])
				{
					AudioData.Musics[i] = music;
					return i;
				}
			}
			AudioData.Musics.push_back(music);
			return AudioData.Musics.size() - 1;
		}

		void DeleteMusic(ResourceIndex music)
		{
			if (AudioData.Musics[music])
			{
				Mix_FreeMusic(AudioData.Musics[music]);
				AudioData.Musics[music] = nullptr;
			}
		}

		void PlayMusic(ResourceIndex music)
		{
			if (AudioData.Musics[music])
			{
				if (Mix_PlayMusic(AudioData.Musics[music], -1) == -1)
				{
					FatalError(Mix_GetError());
				}
			}
		}

		void StopMusic()
		{
			Mix_HaltMusic();
		}

		bool IsMusicPlaying()
		{
			return (Mix_PlayingMusic() != 0);
		}
	}
}

extern "C"
{
	DLLExport int SPF_LoadSound(unsigned char* buffer, int length)
	{
		return SPF::Audio::LoadSound(buffer, length);
	}

	DLLExport int SPF_PlaySound(int sound, float volume, bool looping)
	{
		return SPF::Audio::PlaySound(sound, volume, looping);
	}

	DLLExport void SPF_StopChannel(int channel)
	{
		SPF::Audio::StopChannel(channel);
	}

	DLLExport void SPF_DeleteSound(int sound)
	{
		SPF::Audio::DeleteSound(sound);
	}

	DLLExport float SPF_GetSoundVolume()
	{
		return SPF::Audio::GetSoundVolume();
	}

	DLLExport float SPF_GetMusicVolume()
	{
		return SPF::Audio::GetMusicVolume();
	}

	DLLExport void SPF_SetVolume(float soundVolume, float musicVolume)
	{
		return SPF::Audio::SetVolume(soundVolume, musicVolume);
	}

	DLLExport int SPF_LoadMusic(unsigned char* buffer, int length)
	{
		return SPF::Audio::LoadMusic(buffer, length);
	}

	DLLExport void SPF_DeleteMusic(int music)
	{
		SPF::Audio::DeleteMusic(music);
	}

	DLLExport void SPF_PlayMusic(int music)
	{
		SPF::Audio::PlayMusic(music);
	}

	DLLExport void SPF_StopMusic()
	{
		SPF::Audio::StopMusic();
	}

	DLLExport int SPF_IsMusicPlaying()
	{
		return SPF::Audio::IsMusicPlaying() ? 1 : 0;
	}
}