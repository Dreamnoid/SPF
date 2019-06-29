using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace SPFSharp
{
	internal static class Native
	{
		const string DLLName = "SPF.dll";

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_Open(string title, int w, int h);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_BeginLoop(out float dt);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_EndLoop();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_Close();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_LoadTexture(IntPtr buffer, int length);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DeleteTexture(Int32 texture);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetTextureWidth(Int32 texture);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetTextureHeight(Int32 texture);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_FillVerticalGradient(
			int x, int y, int w, int h,
			float r1, float g1, float b1, float a1,
			float r2, float g2, float b2, float a2);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_FillHorizontalGradient(
			int x, int y, int w, int h,
			float r1, float g1, float b1, float a1,
			float r2, float g2, float b2, float a2);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DrawTexture(Int32 tex,
			int x, int y, int w, int h,
			int srcx, int srcy, int srcw, int srch,
			bool flipx, bool flipy,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);

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

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetTextureFiltering(Int32 texture, bool filtering);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsKeyDown(int key);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsKeyPressed(int key);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsKeyReleased(int key);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_CreateSurface(int w, int h, bool depth);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DeleteSurface(Int32 surface);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_ClearSurface(Int32 surface);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_GetSurfaceTexture(Int32 surface);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetBlending(int blendMode);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_LoadSound(IntPtr buffer, int length);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_PlaySound(Int32 sound, bool looping);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_StopChannel(int channel);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DeleteSound(Int32 sound);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_LoadMusic(IntPtr buffer, int length);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_PlayMusic(Int32 music);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_StopMusic();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DeleteMusic(Int32 music);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsMusicPlaying();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetFullscreen(bool fullscreen);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetWindowSize(int w, int h);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsButtonDown(int button);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsButtonPressed(int button);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsButtonReleased(int button);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern float SPF_GetSoundVolume();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern float SPF_GetMusicVolume();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetVolume(float soundVolume, float musicVolume);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsControllerConnected();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_LoadImage(IntPtr buffer, int length);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DeleteImage(Int32 image);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetImageWidth(Int32 image);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetImageHeight(Int32 image);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 SPF_GetImagePixel(Int32 image, int x, int y);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetWindowWidth();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetWindowHeight();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetMousePositionX();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetMousePositionY();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetMouseDeltaX();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int SPF_GetMouseDeltaY();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsMouseButtonDown(int button);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsMouseButtonPressed(int button);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool SPF_IsMouseButtonReleased(int button);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 SPF_LoadMesh([In] SPF.Vertex[] vertices, int count);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DeleteMesh(Int32 mesh);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_Begin(Int32 surface);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_BeginLookAtPerspective(Int32 surface,
			float cameraX, float cameraY, float cameraZ,
			float cameraTargetX, float cameraTargetY, float cameraTargetZ,
			float fov, float nearDist, float farDist, float fogIntensity);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DrawMesh(Int32 texture, Int32 mesh);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_DrawBillboard(Int32 tex,
			float x, float y, float z, float radius,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern float SPF_GetLeftThumbstickX();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern float SPF_GetLeftThumbstickY();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_SetRelativeMouseState(bool state);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SPF_FatalError(string msg);
		
	}
}
