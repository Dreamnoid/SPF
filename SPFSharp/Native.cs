using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;
using System.Threading.Tasks;

namespace SPFSharp
{
	internal static class Native
	{
		const string DLLName = "SPF.dll";

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_Open(string title, int w, int h);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_BeginLoop(out float dt);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_EndLoop();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_Close();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_LoadTexture(IntPtr buffer, int length);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DeleteTexture(Int32 texture);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetTextureWidth(Int32 texture);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetTextureHeight(Int32 texture);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_FillVerticalGradient(
			int x, int y, int w, int h,
			float r1, float g1, float b1, float a1,
			float r2, float g2, float b2, float a2);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_FillHorizontalGradient(
			int x, int y, int w, int h,
			float r1, float g1, float b1, float a1,
			float r2, float g2, float b2, float a2);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DrawTexture(Int32 tex,
			int x, int y, int w, int h,
			int srcx, int srcy, int srcw, int srch,
			bool flipx, bool flipy,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DrawTexturedQuad(Int32 tex,
			float Ax, float Ay, float Az,
			float Bx, float By, float Bz,
			float Cx, float Cy, float Cz,
			float Dx, float Dy, float Dz,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetTextureFiltering(Int32 texture, bool filtering);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsKeyDown(int key);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsKeyPressed(int key);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsKeyReleased(int key);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_CreateSurface(int w, int h, bool depth);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DeleteSurface(Int32 surface);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_ClearSurface(Int32 surface);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_GetSurfaceTexture(Int32 surface);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetBlending(int blendMode);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_LoadSound(IntPtr buffer, int length);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_PlaySound(Int32 sound, bool looping);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_StopChannel(int channel);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DeleteSound(Int32 sound);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_LoadMusic(IntPtr buffer, int length);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_PlayMusic(Int32 music);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_StopMusic();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DeleteMusic(Int32 music);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsMusicPlaying();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetFullscreen(bool fullscreen);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetWindowSize(int w, int h);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsButtonDown(int button);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsButtonPressed(int button);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsButtonReleased(int button);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern float SPF_GetSoundVolume();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern float SPF_GetMusicVolume();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetVolume(float soundVolume, float musicVolume);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsControllerConnected();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_LoadImage(IntPtr buffer, int length);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DeleteImage(Int32 image);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetImageWidth(Int32 image);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetImageHeight(Int32 image);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 SPF_GetImagePixel(Int32 image, int x, int y);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetWindowWidth();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetWindowHeight();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetMousePositionX();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetMousePositionY();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetMouseDeltaX();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetMouseDeltaY();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsMouseButtonDown(int button);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsMouseButtonPressed(int button);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsMouseButtonReleased(int button);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_LoadMesh([In] SPF.Vertex[] vertices, int count);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DeleteMesh(Int32 mesh);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_Begin(Int32 surface);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_BeginLookAtPerspective(Int32 surface,
			float cameraX, float cameraY, float cameraZ,
			float cameraTargetX, float cameraTargetY, float cameraTargetZ,
			float fov, float nearDist, float farDist, float fogIntensity);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DrawMesh(
			Int32 texture, Int32 mesh, 
			int first, int count,
			[MarshalAs(UnmanagedType.LPArray, SizeConst = 16)] float[] world,
			float overlayR, float overlayG, float overlayB, float overlayA);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DrawBillboard(Int32 tex,
			float x, float y, float z,
			float width, float height,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern float SPF_GetLeftThumbstickX();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern float SPF_GetLeftThumbstickY();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetRelativeMouseState(bool state);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_FatalError(string msg);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetWireframe(bool wireframeEnabled);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetBackfaceCulling(bool cullingEnabled);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetFogColor(float r, float g, float b);

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_StartTextInput();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_StopTextInput();

		[SuppressUnmanagedCodeSecurity]
		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr SPF_GetTextInput();
	}
}
