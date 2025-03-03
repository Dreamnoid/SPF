#include <vector>
#include <array>
#include <cstdio>
#include <string>
#include <Audio.h>
#include <SDL.h>

#include "stb_vorbis.h"

namespace SPF
{
	constexpr int Frequency = 44100;
	constexpr int Channels = 2; // Stereo

	struct SoundChunk
	{
		SDL_AudioSpec Specs;
		uint8_t* Samples = nullptr;
		uint32_t Length = 0;
		uint32_t SamplesCount = 0;
		stb_vorbis* VorbisStream = nullptr;
	};

	struct AudioChannel
	{
		float Volume = 1.0f;
		const SoundChunk* Sound = nullptr;
		int Position = 0;
		bool Looping = false;
	};

	struct
	{
		float SoundVolume = 1.0f;

		SDL_AudioDeviceID DeviceID = 0;
		std::vector<SoundChunk*> Sounds;
		std::vector<SoundChunk*> Musics;
		std::array<AudioChannel, 16> Channels;
		AudioChannel MusicChannel;
		float MusicSamplesBuffer[4096 * 2];
	} AudioData;

	namespace Audio
	{
		void Resample(const SoundChunk* sound, int& position, float& sampleL, float& sampleR)
		{
			const bool stereo = sound->Specs.channels == 2;

			if (SDL_AUDIO_ISFLOAT(sound->Specs.format))
			{
				sampleL = ((float*)sound->Samples)[position++];
				sampleR = stereo ? ((float*)sound->Samples)[position++] : sampleL;
				return;
			}

			const int format = SDL_AUDIO_BITSIZE(sound->Specs.format);
			switch (format)
			{
			case 16:
			{
				const int16_t sample = ((int16_t*)sound->Samples)[position++];
				sampleL = ((float)sample) / INT16_MAX;
				sampleR = sampleL;

				if (stereo)
				{
					const int16_t sample2 = ((int16_t*)sound->Samples)[position++];
					sampleR = ((float)sample2) / INT16_MAX;
				}
			}
			break;

			case 32:
			{
				const int32_t sample = ((int32_t*)sound->Samples)[position++];
				sampleL = ((float)sample) / INT32_MAX;
				sampleR = sampleL;

				if (stereo)
				{
					const int32_t sample2 = ((int32_t*)sound->Samples)[position++];
					sampleR = ((float)sample2) / INT32_MAX;
				}
			}
			break;
			}
		}

		void AudioCallback(void* userdata, Uint8* stream, int requestedLength)
		{
			int16_t* samples = (int16_t*)(stream);
			const int requestedSamplesCount = requestedLength / sizeof(int16_t) / Channels;

			int musicSamplesRead = 0;
			int musicRightChannelOffset = 1;
			const SoundChunk* musicSound = AudioData.MusicChannel.Sound; // Capture current value to avoid threading issues
			if (musicSound != nullptr)
			{
				musicSamplesRead = stb_vorbis_get_samples_float_interleaved(musicSound->VorbisStream, Channels, AudioData.MusicSamplesBuffer, requestedSamplesCount * Channels);
				musicRightChannelOffset = musicSound->Specs.channels > 1 ? 1 : 0;
			}

			for (int i = 0; i < requestedSamplesCount; ++i)
			{
				float mixedSampleL = 0;
				float mixedSampleR = 0;

				for (int c = 0; c < AudioData.Channels.size(); ++c)
				{
					AudioChannel& channel = AudioData.Channels[c];
					const SoundChunk* sound = channel.Sound; // Capture current value to avoid threading issues
					if (sound == nullptr)
						continue;

					float sampleL, sampleR;
					Resample(sound, channel.Position, sampleL, sampleR);

					mixedSampleL += sampleL * channel.Volume;
					mixedSampleR += sampleR * channel.Volume;

					if (channel.Position >= sound->SamplesCount)
					{
						if (!channel.Looping)
						{
							channel.Sound = nullptr;
						}
						channel.Position = 0;
					}
				}

				if (musicSound != nullptr)
				{
					if (i < musicSamplesRead)
					{
						const AudioChannel& channel = AudioData.MusicChannel;
						mixedSampleL += AudioData.MusicSamplesBuffer[i * Channels + 0] * channel.Volume;
						mixedSampleR += AudioData.MusicSamplesBuffer[i * Channels + musicRightChannelOffset] * channel.Volume;
					}
					else
					{
						stb_vorbis_seek_start(musicSound->VorbisStream);
					}
				}

				mixedSampleL = std::min(std::max(mixedSampleL, -1.f), 1.f);
				mixedSampleR = std::min(std::max(mixedSampleR, -1.f), 1.f);

				samples[(i * Channels) + 0] = mixedSampleL * INT16_MAX;
				samples[(i * Channels) + 1] = mixedSampleR * INT16_MAX;
			}
		}

		void Init()
		{
			SDL_AudioSpec desiredSpec;
			SDL_zero(desiredSpec);
			desiredSpec.freq = Frequency;
			desiredSpec.format = AUDIO_S16SYS;
			desiredSpec.channels = Channels;
			desiredSpec.samples = 1024;
			desiredSpec.callback = AudioCallback;
			desiredSpec.userdata = nullptr;

			SDL_AudioSpec obtainedSpec;

			AudioData.DeviceID = SDL_OpenAudioDevice(NULL, SDL_FALSE, &desiredSpec, &obtainedSpec, 0);
			if (AudioData.DeviceID < 2)
			{
				FatalError(SDL_GetError());
			}

			for (int i = 0; i < AudioData.Channels.size(); ++i)
			{
				AudioData.Channels[i].Sound = nullptr;
			}

			SDL_PauseAudioDevice(AudioData.DeviceID, SDL_FALSE);
		}

		void Dispose()
		{
			SDL_CloseAudioDevice(AudioData.DeviceID);
		}

		ResourceIndex LoadSound(unsigned char* buffer, int length)
		{
			SoundChunk* sample = new SoundChunk();
			SDL_LoadWAV_RW(SDL_RWFromMem(buffer, length), SDL_TRUE, &sample->Specs, &sample->Samples, &sample->Length);

			sample->SamplesCount = sample->Length / (SDL_AUDIO_BITSIZE(sample->Specs.format) / 8);

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
			if (sound >= AudioData.Sounds.size())
				return -1;

			SoundChunk* sample = AudioData.Sounds[sound];
			int channel = -1;
			for (int i = 0; i < AudioData.Channels.size(); ++i)
			{
				AudioChannel& channelData = AudioData.Channels[i];
				if (channelData.Sound == nullptr)
				{
					channel = i;
					channelData.Position = 0;
					channelData.Volume = AudioData.SoundVolume * volume;
					channelData.Looping = looping;
					channelData.Sound = sample;
					break;
				}
			}
			return channel;
		}

		void StopChannel(int channel)
		{
			if (channel >= 0 && channel < AudioData.Channels.size())
			{
				AudioData.Channels[channel].Sound = nullptr;
			}
		}

		void DeleteSound(ResourceIndex sound)
		{
			SDL_FreeWAV(AudioData.Sounds[sound]->Samples);
			delete AudioData.Sounds[sound];
			AudioData.Sounds[sound] = nullptr;
		}

		float GetSoundDuration(ResourceIndex sound)
		{
			SoundChunk* sample = AudioData.Sounds[sound];
			return sample->Length / (float)((Frequency * sample->Specs.channels * Channels));
		}

		float GetSoundVolume()
		{
			return AudioData.SoundVolume;
		}

		float GetMusicVolume()
		{
			return AudioData.MusicChannel.Volume;
		}

		void SetVolume(float soundVolume, float musicVolume)
		{
			AudioData.SoundVolume = soundVolume;
			AudioData.MusicChannel.Volume = musicVolume;
		}

		ResourceIndex LoadMusic(unsigned char* buffer, int length)
		{
			int error = 0;
			stb_vorbis* vorbis = stb_vorbis_open_memory(buffer, length, &error, nullptr);
			if (!vorbis)
			{
				FatalError(std::to_string(error).c_str());
			}

			stb_vorbis_info info = stb_vorbis_get_info(vorbis);
			
			SoundChunk* music = new SoundChunk();
			music->VorbisStream = vorbis;
			music->Specs.freq = info.sample_rate;
			music->Specs.channels = info.channels;
			music->Specs.format = AUDIO_S16;

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
				stb_vorbis_close(AudioData.Musics[music]->VorbisStream);
				free(AudioData.Musics[music]->Samples);
				delete AudioData.Musics[music];
				AudioData.Musics[music] = nullptr;
			}
		}

		void PlayMusic(ResourceIndex music)
		{
			if (AudioData.Musics[music])
			{
				AudioData.MusicChannel.Sound = AudioData.Musics[music];
				AudioData.MusicChannel.Position = 0;
			}
		}

		void StopMusic()
		{
			AudioData.MusicChannel.Sound = nullptr;
		}

		bool IsMusicPlaying()
		{
			return AudioData.MusicChannel.Sound != nullptr;
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

	DLLExport float SPF_GetSoundDuration(int sound)
	{
		return SPF::Audio::GetSoundDuration(sound);
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