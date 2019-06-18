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
		public static extern void Open(string title, int w, int h);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool BeginLoop(out float dt);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void EndLoop();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void Close();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 LoadTexture(IntPtr buffer, int length);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void DeleteTexture(Int32 texture);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int GetTextureWidth(Int32 texture);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int GetTextureHeight(Int32 texture);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void DrawTexture(Int32 tex,
			int x, int y, int w, int h,
			int srcx, int srcy, int srcw, int srch,
			bool flipx, bool flipy,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void DrawTexturedQuad(Int32 tex,
			float Ax, float Ay, float Az,
			float Bx, float By, float Bz,
			float Cx, float Cy, float Cz,
			float Dx, float Dy, float Dz,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SetTextureFiltering(Int32 texture, bool filtering);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool IsKeyDown(int key);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool IsKeyPressed(int key);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool IsKeyReleased(int key);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 CreateSurface(int w, int h, bool depth);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void DeleteSurface(Int32 surface);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void ClearSurface(Int32 surface);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 GetSurfaceTexture(Int32 surface);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SetBlending(int blendMode);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 LoadSound(IntPtr buffer, int length);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int PlaySound(Int32 sound, bool looping);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void StopChannel(int channel);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void DeleteSound(Int32 sound);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 LoadMusic(IntPtr buffer, int length);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void PlayMusic(Int32 music);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void StopMusic();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void DeleteMusic(Int32 music);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool IsMusicPlaying();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SetFullscreen(bool fullscreen);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SetWindowSize(int w, int h);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool IsButtonDown(int button);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool IsButtonPressed(int button);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool IsButtonReleased(int button);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern float GetSoundVolume();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern float GetMusicVolume();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SetVolume(float soundVolume, float musicVolume);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool IsControllerConnected();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 LoadImage(IntPtr buffer, int length);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void DeleteImage(Int32 image);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int GetImageWidth(Int32 image);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int GetImageHeight(Int32 image);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 GetImagePixel(Int32 image, int x, int y);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int GetWindowWidth();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int GetWindowHeight();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int GetMousePositionX();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int GetMousePositionY();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int GetMouseDeltaX();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern int GetMouseDeltaY();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool IsMouseButtonDown(int button);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool IsMouseButtonPressed(int button);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern bool IsMouseButtonReleased(int button);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern Int32 LoadMesh([In] SPF.Vertex[] vertices, int count);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void DeleteMesh(Int32 mesh);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void Begin(Int32 surface);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void BeginLookAtPerspective(Int32 surface,
			float cameraX, float cameraY, float cameraZ,
			float cameraTargetX, float cameraTargetY, float cameraTargetZ,
			float fov, float nearDist, float farDist, float fogIntensity);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void DrawMesh(Int32 texture, Int32 mesh);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void DrawBillboard(Int32 tex,
			float x, float y, float z, float radius,
			int srcx, int srcy, int srcw, int srch,
			bool flipX, bool flipY,
			float r, float g, float b, float a,
			float overlayR, float overlayG, float overlayB, float overlayA);

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern float GetLeftThumbstickX();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern float GetLeftThumbstickY();

		[DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
		public static extern void SetRelativeMouseState(bool state);
	}
}
