using System;
using System.IO;
using System.Runtime.InteropServices;

namespace SPFSharp
{
	public static class Extensions
	{
		public static SPF.Vertex ReadSPFVertex(this BinaryReader br)
		{
			var v = new SPF.Vertex();
			v.X = br.ReadSingle();
			v.Y = br.ReadSingle();
			v.Z = br.ReadSingle();
			v.U = br.ReadSingle();
			v.V = br.ReadSingle();
			v.BU = br.ReadSingle();
			v.BV = br.ReadSingle();
			v.R = br.ReadSingle();
			v.G = br.ReadSingle();
			v.B = br.ReadSingle();
			v.A = br.ReadSingle();
			v.OverlayR = br.ReadSingle();
			v.OverlayG = br.ReadSingle();
			v.OverlayB = br.ReadSingle();
			v.OverlayA = br.ReadSingle();
			return v;
		}

		public static void Write(this BinaryWriter bw, SPF.Vertex v)
		{
			bw.Write(v.X);
			bw.Write(v.Y);
			bw.Write(v.Z);
			bw.Write(v.U);
			bw.Write(v.V);
			bw.Write(v.BU);
			bw.Write(v.BV);
			bw.Write(v.R);
			bw.Write(v.G);
			bw.Write(v.B);
			bw.Write(v.A);
			bw.Write(v.OverlayR);
			bw.Write(v.OverlayG);
			bw.Write(v.OverlayB);
			bw.Write(v.OverlayA);
		}

		internal static string MarshalToString(this IntPtr ptr) => ptr != IntPtr.Zero ? Marshal.PtrToStringAnsi(ptr) : null;
	}
}
