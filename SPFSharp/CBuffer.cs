using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

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
