#pragma once
#include "Core.h"

namespace SPF
{
	namespace Audio
	{
		void Init();
		void Dispose();

		DLLExport ResourceIndex LoadSound(unsigned char* buffer, int length);
		DLLExport int PlaySound(ResourceIndex sound, bool looping);
		DLLExport void StopChannel(int channel);
		DLLExport void DeleteSound(ResourceIndex sound);

		DLLExport float GetSoundVolume();
		DLLExport float GetMusicVolume();

		DLLExport void SetVolume(float soundVolume, float musicVolume);

		DLLExport ResourceIndex LoadMusic(unsigned char* buffer, int length);
		DLLExport void DeleteMusic(ResourceIndex music);
		DLLExport void PlayMusic(ResourceIndex music);
		DLLExport void StopMusic();
		DLLExport bool IsMusicPlaying();
	}
}