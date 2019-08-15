using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SPFSharp
{
	public static partial class SPF
	{
		public enum BlendMode : int
		{
			Alpha = 0,
			Additive = 1
		}

		public static class Renderer
		{
			public static void Begin()
			{
				Begin(null);
			}

			public static void Begin(Surface surface)
			{
				Native.SPF_Begin((surface != null) ? surface.ID : -1);
			}

			public static void BeginLookAtPerspective(
				float cameraX, float cameraY, float cameraZ,
				float cameraTargetX, float cameraTargetY, float cameraTargetZ,
				float fov, float nearDist, float farDist)
			{
				BeginLookAtPerspective(null, 
					cameraX, cameraY, cameraZ,
					cameraTargetX, cameraTargetY, cameraTargetZ,
					fov, nearDist, farDist);
			}

			public static void BeginLookAtPerspective(Surface surface,
				float cameraX, float cameraY, float cameraZ,
				float cameraTargetX, float cameraTargetY, float cameraTargetZ,
				float fov, float nearDist, float farDist, float fogIntensity = 1.0f)
			{
				Native.SPF_BeginLookAtPerspective((surface != null) ? surface.ID : -1,
					cameraX, cameraY, cameraZ,
					cameraTargetX, cameraTargetY, cameraTargetZ,
					fov, nearDist, farDist, fogIntensity);
			}

			public static void SetBlending(BlendMode blendMode)
			{
				Native.SPF_SetBlending((int)blendMode);
			}

			public static void FillRectangle(int x, int y, int w, int h, float r, float g, float b, float a)
			{
				Native.SPF_FillRectangle(x, y, w, h, r, g, b, a);
			}

			public static void FillVerticalGradient(
				int x, int y, int w, int h, 
				float r1, float g1, float b1, float a1,
				float r2, float g2, float b2, float a2)
			{
				Native.SPF_FillVerticalGradient(x, y, w, h, r1, g1, b1, a1, r2, g2, b2, a2);
			}

			public static void FillHorizontalGradient(
				int x, int y, int w, int h,
				float r1, float g1, float b1, float a1,
				float r2, float g2, float b2, float a2)
			{
				Native.SPF_FillHorizontalGradient(x, y, w, h, r1, g1, b1, a1, r2, g2, b2, a2);
			}

			public static void DrawRectangle(int x, int y, int w, int h, float r, float g, float b, float a)
			{
				Native.SPF_FillRectangle(x, y, w, 1, r, g, b, a);
				Native.SPF_FillRectangle(x, y + h - 1, w, 1, r, g, b, a);
				Native.SPF_FillRectangle(x, y + 1, 1, h - 2, r, g, b, a);
				Native.SPF_FillRectangle(x + w - 1, y + 1, 1, h - 2, r, g, b, a);
			}

			public static void DrawTexture(Texture tex, int x, int y)
			{
				Native.SPF_DrawTexture(tex.ID, x, y, tex.Width, tex.Height, 0, 0, tex.Width, tex.Height, false, false, 1, 1, 1, 1, 0, 0, 0, 0);
			}

			public static void DrawTexture(Texture tex,
				int x, int y, int w, int h,
				int clipx, int clipy, int clipw, int cliph,
				bool flipx, bool flipy,
				float r, float g, float b, float a)
			{
				Native.SPF_DrawTexture(tex.ID, x, y, w, h, clipx, clipy, clipw, cliph, flipx, flipy, r, g, b, a, 0, 0, 0, 0);
			}

			public static void DrawTexture(Texture tex,
				int x, int y, int w, int h,
				int clipx, int clipy, int clipw, int cliph,
				bool flipx, bool flipy,
				float r, float g, float b, float a,
				float overlayR, float overlayG, float overlayB, float overlayA)
			{
				Native.SPF_DrawTexture(tex.ID, x, y, w, h, clipx, clipy, clipw, cliph, flipx, flipy, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
			}

			public static void DrawTexturedQuad(Texture tex,
				float Ax, float Ay, 
				float Bx, float By, 
				float Cx, float Cy, 
				float Dx, float Dy,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				float r, float g, float b, float a,
				float overlayR, float overlayG, float overlayB, float overlayA)
			{
				Native.SPF_DrawTexturedQuad(tex.ID, Ax, Ay, 0, Bx, By, 0, Cx, Cy, 0, Dx, Dy, 0, srcx, srcy, srcw, srch, flipX, flipY, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
			}

			public static void DrawTexturedQuad(Texture tex,
				float Ax, float Ay, float Az,
				float Bx, float By, float Bz,
				float Cx, float Cy,  float Cz,
				float Dx, float Dy, float Dz,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				float r, float g, float b, float a,
				float overlayR, float overlayG, float overlayB, float overlayA)
			{
				Native.SPF_DrawTexturedQuad(tex.ID, Ax, Ay, Az, Bx, By, Bz, Cx, Cy, Cz, Dx, Dy, Dz, srcx, srcy, srcw, srch, flipX, flipY, r, g, b, a, overlayR, overlayG, overlayB, overlayA);
			}

			private static readonly float[] _identityMatrix = new float[]
			{
				1f, 0f, 0f, 0f,
				0f, 1f, 0f, 0f,
				0f, 0f, 1f, 0f,
				0f, 0f, 0f, 1f
			};

			public static void DrawMesh(Texture texture, Mesh mesh)
			{
				Native.SPF_DrawMesh(texture.ID, mesh.ID, 0, mesh.VerticesCount, _identityMatrix);
			}

			public static void DrawMesh(Texture texture, Mesh mesh, float[] world)
			{
				Native.SPF_DrawMesh((texture != null) ? texture.ID : -1, mesh.ID, 0, mesh.VerticesCount, world);
			}

			public static void DrawMesh(Texture texture, Mesh mesh, int first, int count, float[] world)
			{
				Native.SPF_DrawMesh((texture != null) ? texture.ID : -1, mesh.ID, first, count, world);
			}

			public static void DrawMesh(Mesh mesh, float[] world)
			{
				Native.SPF_DrawMesh(-1, mesh.ID, 0, mesh.VerticesCount, world);
			}

			public static void DrawBillboard(Texture tex,
				float x, float y, float z, float radius,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				float r, float g, float b, float a,
				float overlayR, float overlayG, float overlayB, float overlayA)
			{
				Native.SPF_DrawBillboard(tex.ID,
					x, y, z, 
					radius, radius,
					srcx, srcy, srcw, srch,
					flipX, flipY,
					r, g, b, a,
					overlayR, overlayG, overlayB, overlayA);
			}

			public static void DrawBillboard(Texture tex,
				float x, float y, float z, 
				float width, float height,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				float r, float g, float b, float a,
				float overlayR, float overlayG, float overlayB, float overlayA)
			{
				Native.SPF_DrawBillboard(tex.ID,
					x, y, z,
					width, height,
					srcx, srcy, srcw, srch,
					flipX, flipY,
					r, g, b, a,
					overlayR, overlayG, overlayB, overlayA);
			}

			public static void SetWireframe(bool wireframeEnabled)
			{
				Native.SPF_SetWireframe(wireframeEnabled);
			}

			public static void SetBackfaceCulling(bool cullingEnabled)
			{
				Native.SPF_SetBackfaceCulling(cullingEnabled);
			}

			public static void SetFogColor(float r, float g, float b)
			{
				Native.SPF_SetFogColor(r, g, b);
			}
		}
	}
}
