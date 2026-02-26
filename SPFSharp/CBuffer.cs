using System;
using System.Runtime.InteropServices;

namespace SPFSharp
{
    internal class CBuffer : IDisposable
	{
		public IntPtr Pointer { get; private set; }
		public int Length { get; private set; }

		public CBuffer(byte[] buffer)
		{
			Pointer = Marshal.AllocHGlobal(buffer.Length);
			Length = buffer.Length;
			Marshal.Copy(buffer, 0, Pointer, buffer.Length);
		}

		public void Dispose()
		{
			Marshal.FreeHGlobal(Pointer);
		}
	}
}
