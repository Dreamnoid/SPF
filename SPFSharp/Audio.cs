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

		public static class Audio
		{
			public static void StopChannel(int channel)
			{
				Native.StopChannel(channel);
			}

			public static float GetVolume()
			{
				return Native.GetVolume();
			}

			public static void SetVolume(float volume)
			{
				Native.SetVolume(volume);
			}
		}
	}
}
