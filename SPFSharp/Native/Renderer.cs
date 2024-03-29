﻿using System;
using System.Runtime.InteropServices;
using System.Security;

namespace SPFSharp
{
	internal static partial class Native
	{
		public static class Renderer
		{
			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_Begin(int surface, bool clear);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SetCamera(
				[MarshalAs(UnmanagedType.LPArray, SizeConst = 16)] float[] viewMatrix,
				[MarshalAs(UnmanagedType.LPArray, SizeConst = 16)] float[] projectionMatrix,
				float nearPlane, float farPlane,
				float upX, float upY, float upZ,
				float sideX, float sideY, float sideZ);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SetMaterial(int shader, int texture1, int texture2, int texture3, int texture4, int texture5, int texture6, int texture7, int texture8);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SetFog(float intensity, float r, float g, float b);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SetUserData(
				float animation,
				float x, float y, float z, float w,
				[MarshalAs(UnmanagedType.LPArray, SizeConst = 16)] float[] userMatrix);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SetRasterization(int blending, bool wireframe, int backfaceCulling, float lineWidth);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SetBuffers(bool colorWrite, bool depthWrite, int depthTest);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SetStencil(bool write, int test, int reference, int stencilFail, int depthFail, int depthPass);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int SPF_GetFinalSurface();

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_DrawMesh(
				Int32 mesh, int first, int count,
				[MarshalAs(UnmanagedType.LPArray, SizeConst = 16)] float[] world,
				float overlayR, float overlayG, float overlayB, float overlayA);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_SetPrimitiveType(int type);

			[SuppressUnmanagedCodeSecurity, DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void SPF_PushVertex(
				float x, float y, float z,
				float normalX, float normalY, float normalZ,
				float u, float v, float bu, float bv,
				float r, float g, float b, float a,
				float overlayR, float overlayG, float overlayB, float overlayA);
		}
	}
}