using System;

namespace SPFSharp
{
    public static partial class SPF
	{
		public class Sound : IDisposable
		{
			public Int32 ID { get; }
			public float Duration { get; }

			public Sound(byte[] buffer)
			{
				using (var cbuffer = new CBuffer(buffer))
				{
					ID = Native.Audio.SPF_LoadSound(cbuffer.Pointer, cbuffer.Length);
				}
				Duration = Native.Audio.SPF_GetSoundDuration(ID);
			}

			public void Dispose() => Native.Audio.SPF_DeleteSound(ID);

			public int Play(float volume = 1f, bool looping = false)
				=> Native.Audio.SPF_PlaySound(ID, volume, looping);
		}

		public class Music : IDisposable
		{
			private readonly CBuffer _cbuffer; // The buffer needs to stay open so streaming will work
			public Int32 ID { get; }

			public Music(byte[] buffer)
			{
				_cbuffer = new CBuffer(buffer);
				ID = Native.Audio.SPF_LoadMusic(_cbuffer.Pointer, _cbuffer.Length);
			}

			public void Dispose()
			{
				Native.Audio.SPF_DeleteMusic(ID);
				_cbuffer.Dispose();
			}
		}

		public static class Audio
		{
			public static void StopChannel(int channel) => Native.Audio.SPF_StopChannel(channel);

			public static void PlayMusic(Music music) => Native.Audio.SPF_PlayMusic(music.ID);

			public static void StopMusic() => Native.Audio.SPF_StopMusic();

			public static bool IsMusicPlaying() => Native.Audio.SPF_IsMusicPlaying();

			public static float GetSoundVolume() => Native.Audio.SPF_GetSoundVolume();

			public static float GetMusicVolume() => Native.Audio.SPF_GetMusicVolume();

			public static void SetVolume(float volume) => SetVolume(volume, volume);

			public static void SetVolume(float soundVolume, float musicVolume)
				=> Native.Audio.SPF_SetVolume(soundVolume, musicVolume);
		}
	}
}
