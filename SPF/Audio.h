#pragma once

void InitAudio()
{
	SDL_memset(&Data.SFXPlaybackSpecs, 0, sizeof(Data.SFXPlaybackSpecs));
	Data.SFXPlaybackSpecs.freq = 48000;
	Data.SFXPlaybackSpecs.format = AUDIO_S16;
	Data.SFXPlaybackSpecs.channels = 2;
	Data.SFXPlaybackSpecs.samples = 4096;
	Data.SFXDevice = SDL_OpenAudioDevice(NULL, 0, &Data.SFXPlaybackSpecs, NULL, 0);
	SDL_PauseAudioDevice(Data.SFXDevice, 0);
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

DLLExport void PlaySound(ResourceIndex sound)
{
	SDL_QueueAudio(Data.SFXDevice, Data.Sounds[sound].Buffer, Data.Sounds[sound].Length);
}

DLLExport void DeleteSound(ResourceIndex sound)
{
	SDL_FreeWAV(Data.Sounds[sound].Buffer);
	Data.Sounds[sound].InUse = 0;
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
