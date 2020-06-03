using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace SPFSharp
{
	public static partial class SPF
	{
		public class Sound : IDisposable
		{
			public Int32 ID { get; private set; }

			public Sound(byte[] buffer)
			{
				using (var cbuffer = new CBuffer(buffer))
				{
					ID = Native.SPF_LoadSound(cbuffer.Pointer, cbuffer.Length);
				}
			}

			public void Dispose()
			{
				Native.SPF_DeleteSound(ID);
			}

			public int Play(float volume = 1f, bool looping = false)
			{
				return Native.SPF_PlaySound(ID, volume, looping);
			}
		}

		public class Music : IDisposable
		{
			private readonly CBuffer _cbuffer; // The buffer needs to stay open so streaming will work
			public Int32 ID { get; private set; }

			public Music(byte[] buffer)
			{
				_cbuffer = new CBuffer(buffer);
				ID = Native.SPF_LoadMusic(_cbuffer.Pointer, _cbuffer.Length);
			}

			public void Dispose()
			{
				Native.SPF_DeleteMusic(ID);
				_cbuffer.Dispose();
			}
		}

		public static class Audio
		{
			public static void StopChannel(int channel)
			{
				Native.SPF_StopChannel(channel);
			}

			public static void PlayMusic(Music music)
			{
				Native.SPF_PlayMusic(music.ID);
			}

			public static void StopMusic()
			{
				Native.SPF_StopMusic();
			}

			public static bool IsMusicPlaying()
			{
				return Native.SPF_IsMusicPlaying();
			}

			public static float GetSoundVolume()
			{
				return Native.SPF_GetSoundVolume();
			}

			public static float GetMusicVolume()
			{
				return Native.SPF_GetMusicVolume();
			}

			public static void SetVolume(float volume)
			{
				SetVolume(volume, volume);
			}

			public static void SetVolume(float soundVolume, float musicVolume)
			{
				Native.SPF_SetVolume(soundVolume, musicVolume);
			}
		}
	}
}
