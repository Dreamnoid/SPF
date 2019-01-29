#pragma once

void AudioCallback(void *userdata, Uint8 *stream, int len)
{
	SDL_memset(stream, Data.SFXPlaybackSpecs.silence, len);

	int playingChannels = 0;
	for (int i = 0; i < CHANNELS_COUNT; ++i)
	{
		if (Data.Channels[i].CurrentSound)
		{
			playingChannels++;
		}
	}
	
	for (int i = 0; i < CHANNELS_COUNT; ++i)
	{
		Channel* channel = &Data.Channels[i];
		if (channel->CurrentSound)
		{
			Uint8* start = channel->CurrentSound->Buffer + channel->Cursor;
			int lengthRemaining = channel->CurrentSound->Length - channel->Cursor;

			int playedLen = (len > lengthRemaining ? lengthRemaining : len); // TODO: if looping, Mix twice
			SDL_MixAudioFormat(stream, start, Data.SFXPlaybackSpecs.format, playedLen, (int)((Data.Volume / (float)playingChannels) * SDL_MIX_MAXVOLUME));

			channel->Cursor += playedLen;
			if (channel->Cursor >= channel->CurrentSound->Length)
			{
				if (!channel->Looping)
				{
					channel->CurrentSound = NULL;
				}
				channel->Cursor = 0;
			}
		}
	}
}

void InitAudio()
{
	SDL_memset(&Data.SFXPlaybackSpecs, 0, sizeof(Data.SFXPlaybackSpecs));
	Data.SFXPlaybackSpecs.freq = 48000;
	Data.SFXPlaybackSpecs.format = AUDIO_S16;
	Data.SFXPlaybackSpecs.channels = 2;
	Data.SFXPlaybackSpecs.samples = 4096;
	Data.SFXPlaybackSpecs.callback = AudioCallback;
	Data.SFXDevice = SDL_OpenAudioDevice(NULL, 0, &Data.SFXPlaybackSpecs, NULL, 0);
	SDL_PauseAudioDevice(Data.SFXDevice, 0);
	for (int i = 0; i < CHANNELS_COUNT; ++i)
	{
		Data.Channels[i].CurrentSound = NULL;
		Data.Channels[i].Cursor = 0;
	}
}

DLLExport ResourceIndex LoadSound(unsigned char* buffer, int length)
{
	SDL_AudioSpec spec;
	Uint32 wav_length;
	Uint8 *wav_buffer;
	SDL_LoadWAV_RW(SDL_RWFromMem(buffer, length), 1, &spec, &wav_buffer, &wav_length);

	// Convert the WAV file to our desired format
	SDL_AudioCVT cvt;
	SDL_BuildAudioCVT(&cvt, spec.format, spec.channels, spec.freq, Data.SFXPlaybackSpecs.format, Data.SFXPlaybackSpecs.channels, Data.SFXPlaybackSpecs.freq);
	if( cvt.needed )
	{
		cvt.len = wav_length;
		cvt.buf = (Uint8 *)SDL_malloc(cvt.len * cvt.len_mult);
		memcpy(cvt.buf, wav_buffer, wav_length);
		SDL_ConvertAudio(&cvt);
		SDL_FreeWAV(wav_buffer);
		wav_buffer = cvt.buf;
		wav_length = cvt.len_cvt;
	}
	
	for (ResourceIndex i = 0; i < SOUNDS_COUNT; ++i)
	{
		if (!Data.Sounds[i].InUse)
		{
			Data.Sounds[i].InUse = 1;
			Data.Sounds[i].Spec = spec;
			Data.Sounds[i].Buffer = wav_buffer;
			Data.Sounds[i].Length = wav_length;
			return i;
		}
	}
	exit(1);
}

DLLExport int PlaySound(ResourceIndex sound, bool looping)
{
	for (int i = 0; i < CHANNELS_COUNT; ++i)
	{
		if(!Data.Channels[i].CurrentSound)
		{
			Data.Channels[i].CurrentSound = &Data.Sounds[sound];
			Data.Channels[i].Cursor = 0;
			Data.Channels[i].Looping = looping;
			return i;
		}
	}
	return -1;
}


DLLExport void StopChannel(int channel)
{
	Data.Channels[channel].CurrentSound = NULL;
	Data.Channels[channel].Cursor = 0;
	Data.Channels[channel].Looping = 0;
}

DLLExport void DeleteSound(ResourceIndex sound)
{
	SDL_FreeWAV(Data.Sounds[sound].Buffer);
	Data.Sounds[sound].InUse = 0;
}

DLLExport float GetVolume()
{
	return Data.Volume;
}

DLLExport void SetVolume(float volume)
{
	Data.Volume = volume;
}

void QuitAudio()
{
	SDL_CloseAudioDevice(Data.SFXDevice);
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
