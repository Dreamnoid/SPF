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
					ID = Native.LoadSound(cbuffer.Pointer, cbuffer.Length);
				}
			}

			public void Dispose()
			{
				Native.DeleteSound(ID);
			}

			public int Play(bool looping = false)
			{
				return Native.PlaySound(ID, looping);
			}
		}

		public class Music : IDisposable
		{
			private readonly CBuffer _cbuffer; // The buffer needs to stay open so streaming will work
			public Int32 ID { get; private set; }

			public Music(byte[] buffer)
			{
				_cbuffer = new CBuffer(buffer);
				ID = Native.LoadMusic(_cbuffer.Pointer, _cbuffer.Length);
			}

			public void Dispose()
			{
				Native.DeleteMusic(ID);
				_cbuffer.Dispose();
			}
		}

		public static class Audio
		{
			public static void StopChannel(int channel)
			{
				Native.StopChannel(channel);
			}

			public static void PlayMusic(Music music)
			{
				Native.PlayMusic(music.ID);
			}

			public static void StopMusic()
			{
				Native.StopMusic();
			}

			public static bool IsMusicPlaying()
			{
				return Native.IsMusicPlaying();
			}

			public static float GetSoundVolume()
			{
				return Native.GetSoundVolume();
			}

			public static float GetMusicVolume()
			{
				return Native.GetMusicVolume();
			}

			public static void SetVolume(float volume)
			{
				SetVolume(volume, volume);
			}

			public static void SetVolume(float soundVolume, float musicVolume)
			{
				Native.SetVolume(soundVolume, musicVolume);
			}
		}
	}
}
