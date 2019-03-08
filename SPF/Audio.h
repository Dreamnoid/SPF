#pragma once

void InitAudio()
{
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) 
	{
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		exit(2);
	}

	Mix_AllocateChannels(16);
}

DLLExport ResourceIndex LoadSound(unsigned char* buffer, int length)
{
	Mix_Chunk* sample = Mix_LoadWAV_RW(SDL_RWFromMem(buffer, length), 1);
	for (ResourceIndex i = 0; i < SOUNDS_COUNT; ++i)
	{
		if (!Data.Sounds[i].InUse)
		{
			Data.Sounds[i].InUse = 1;
			Data.Sounds[i].Sample = sample;
			return i;
		}
	}
	exit(1);
}

DLLExport int PlaySound(ResourceIndex sound, bool looping)
{
	Mix_Chunk* sample = Data.Sounds[sound].Sample;
	return Mix_PlayChannel(-1, sample, looping ? -1 : 0);
}


DLLExport void StopChannel(int channel)
{
	Mix_HaltChannel(channel);
}

DLLExport void DeleteSound(ResourceIndex sound)
{
	Mix_FreeChunk(Data.Sounds[sound].Sample);
	Data.Sounds[sound].InUse = 0;
}

DLLExport float GetVolume()
{
	return Data.Volume;
}

DLLExport void SetVolume(float volume)
{
	Data.Volume = volume;
	Mix_Volume(-1, volume*MIX_MAX_VOLUME);
}

void QuitAudio()
{
	Mix_CloseAudio();
}

// TODO: The following is nice for music

// audio callback function
// here you have to copy the data of your audio buffer into the
// requesting audio buffer (stream)
// you should only copy as much as the requested length (len)
//void AudioCallback(void *userdata, Uint8 *stream, int len)
//{
//	Sound* sound = (Sound*)userdata;
//	if (sound->LengthRemaining == 0)
//		return;
//
//	len = (len > sound->LengthRemaining ? sound->LengthRemaining : len);
//	SDL_memset(stream, 0, len);
//	SDL_MixAudio(stream, sound->CurrentPosition, len, SDL_MIX_MAXVOLUME); // mix from one buffer into another
//
//	sound->CurrentPosition += len;
//	sound->LengthRemaining -= len;
//	if (sound->LengthRemaining <= 0)
//	{
//		SDL_CloseAudio();
//	}
//}

	// set the callback function
	//Sound* snd = &Data.Sounds[sound];
	//snd->Spec.callback = AudioCallback;
	//snd->Spec.userdata = snd;
	//// set our global static variables
	//snd->CurrentPosition = snd->Buffer; // copy sound buffer
	//snd->LengthRemaining = snd->Length; // copy file length

	///* Open the audio device */
	//if (SDL_OpenAudio(&snd->Spec, NULL) < 0)
	//{
	//	fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
	//	exit(-1);
	//}

	///* Start playing */
	//
