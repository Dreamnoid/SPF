#pragma once

typedef struct
{
	Uint8* Buffer;
	Uint32 Length;
} Sound;

void LoadSound(const char* filename)
{
	SDL_AudioSpec spec;
	Uint32 wav_length;
	Uint8 *wav_buffer;
	SDL_LoadWAV(filename, &spec, &wav_buffer, &wav_length);
}

void PlaySound()
{

	// set the callback function
	wav_spec.callback = my_audio_callback;
	wav_spec.userdata = NULL;
	// set our global static variables
	audio_pos = wav_buffer; // copy sound buffer
	audio_len = wav_length; // copy file length

	/* Open the audio device */
	if (SDL_OpenAudio(&wav_spec, NULL) < 0)
	{
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		exit(-1);
	}

	/* Start playing */
	SDL_PauseAudio(0);

	// wait until we're don't playing
	while (audio_len > 0) 
	{
		SDL_Delay(100);
	}

	SDL_CloseAudio();
}

void DeleteSound()
{
	SDL_FreeWAV(wav_buffer);
}

// audio callback function
// here you have to copy the data of your audio buffer into the
// requesting audio buffer (stream)
// you should only copy as much as the requested length (len)
void AudioCallback(void *userdata, Uint8 *stream, int len) 
{
	if (audio_len == 0)
		return;

	len = (len > audio_len ? audio_len : len);
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another

	audio_pos += len;
	audio_len -= len;
}