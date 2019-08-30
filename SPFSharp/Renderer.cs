﻿using System.Numerics;

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
				Vector3 cameraPosition, Vector3 cameraTarget,
				float fov, float nearDist, float farDist)
			{
				BeginLookAtPerspective(null, cameraPosition, cameraTarget, fov, nearDist, farDist);
			}

			public static void BeginLookAtPerspective(
				Surface surface, Vector3 cameraPosition, Vector3 cameraTarget,
				float fov, float nearDist, float farDist, float fogIntensity = 1.0f)
			{
				Native.SPF_BeginLookAtPerspective((surface != null) ? surface.ID : -1,
					cameraPosition.X, cameraPosition.Y, cameraPosition.Z,
					cameraTarget.X, cameraTarget.Y, cameraTarget.Z,
					fov, nearDist, farDist, fogIntensity);
			}

			public static void SetBlending(BlendMode blendMode)
			{
				Native.SPF_SetBlending((int)blendMode);
			}

			public static void FillRectangle(int x, int y, int w, int h, Vector4 color)
			{
				Native.SPF_FillRectangle(
					x, y, w, h,
					color.X, color.Y, color.Z, color.W);
			}

			public static void FillVerticalGradient(
				int x, int y, int w, int h, 
				Vector4 color1, Vector4 color2)
			{
				Native.SPF_FillVerticalGradient(
					x, y, w, h,
					color1.X, color1.Y, color1.Z, color1.W,
					color2.X, color2.Y, color2.Z, color2.W);
			}

			public static void FillHorizontalGradient(
				int x, int y, int w, int h,
				Vector4 color1, Vector4 color2)
			{
				Native.SPF_FillHorizontalGradient(
					x, y, w, h,
					color1.X, color1.Y, color1.Z, color1.W,
					color2.X, color2.Y, color2.Z, color2.W);
			}

			public static void DrawRectangle(int x, int y, int w, int h, Vector4 color)
			{
				Native.SPF_FillRectangle(x, y, w, 1, color.X, color.Y, color.Z, color.W);
				Native.SPF_FillRectangle(x, y + h - 1, w, 1, color.X, color.Y, color.Z, color.W);
				Native.SPF_FillRectangle(x, y + 1, 1, h - 2, color.X, color.Y, color.Z, color.W);
				Native.SPF_FillRectangle(x + w - 1, y + 1, 1, h - 2, color.X, color.Y, color.Z, color.W);
			}

			public static void DrawTexture(Texture tex, int x, int y)
			{
				Native.SPF_DrawTexture(tex.ID, 
					x, y, tex.Width, tex.Height,
					0, 0, tex.Width, tex.Height, 
					false, false, 
					1f, 1f, 1f, 1f,
					0f, 0f, 0f, 0f);
			}

			public static void DrawTexture(Texture tex,
				int x, int y, int w, int h,
				int clipx, int clipy, int clipw, int cliph,
				bool flipx, bool flipy,
				Vector4 color)
			{
				Native.SPF_DrawTexture(tex.ID,
					x, y, w, h,
					clipx, clipy, clipw, cliph, 
					flipx, flipy,
					color.X, color.Y, color.Z, color.W,
					0f, 0f, 0f, 0f);
			}

			public static void DrawTexture(Texture tex,
				int x, int y, int w, int h,
				int clipx, int clipy, int clipw, int cliph,
				bool flipx, bool flipy,
				Vector4 color,
				Vector4 overlay)
			{
				Native.SPF_DrawTexture(tex.ID,
					x, y, w, h, 
					clipx, clipy, clipw, cliph, 
					flipx, flipy,
					color.X, color.Y, color.Z, color.W,
					overlay.X, overlay.Y, overlay.Z, overlay.W);
			}

			public static void DrawTexturedQuad(Texture tex,
				Vector2 a,
				Vector2 b,
				Vector2 c,
				Vector2 d,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				Vector4 color,
				Vector4 overlay)
			{
				Native.SPF_DrawTexturedQuad(tex.ID,
					a.X, a.Y, 0f, 
					b.X, b.Y, 0f, 
					c.X, c.Y, 0f,
					d.X, d.Y, 0f,
					srcx, srcy, srcw, srch, 
					flipX, flipY,
					color.X, color.Y, color.Z, color.W,
					overlay.X, overlay.Y, overlay.Z, overlay.W);
			}

			public static void DrawTexturedQuad(Texture tex,
				Vector3 a,
				Vector3 b,
				Vector3 c,
				Vector3 d,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				Vector4 color,
				Vector4 overlay)
			{
				Native.SPF_DrawTexturedQuad(tex.ID, 
					a.X, a.Y, a.Z, 
					b.X, b.Y, b.Z, 
					c.X, c.Y, c.Z, 
					d.X, d.Y, d.Z, 
					srcx, srcy, srcw, srch,
					flipX, flipY,
					color.X, color.Y, color.Z, color.W,
					overlay.X, overlay.Y, overlay.Z, overlay.W);
			}

			public static readonly float[] IdentityMatrix = new float[]
			{
				1f, 0f, 0f, 0f,
				0f, 1f, 0f, 0f,
				0f, 0f, 1f, 0f,
				0f, 0f, 0f, 1f
			};

			public static void DrawMesh(Texture texture, Mesh mesh)
			{
				Native.SPF_DrawMesh(texture.ID, mesh.ID, 0, mesh.VerticesCount, IdentityMatrix, 0f, 0f, 0f, 0f);
			}

			public static void DrawMesh(Texture texture, Mesh mesh, float[] world)
			{
				Native.SPF_DrawMesh((texture != null) ? texture.ID : -1, mesh.ID, 0, mesh.VerticesCount, world, 0f, 0f, 0f, 0f);
			}

			public static void DrawMesh(
				Texture texture, Mesh mesh, 
				int first, int count, 
				float[] world, 
				Vector4 overlay)
			{
				Native.SPF_DrawMesh((texture != null) ? texture.ID : -1, mesh.ID, first, count, world, overlay.X, overlay.Y, overlay.Z, overlay.W);
			}

			public static void DrawMesh(Mesh mesh, float[] world)
			{
				Native.SPF_DrawMesh(-1, mesh.ID, 0, mesh.VerticesCount, world, 0f, 0f, 0f, 0f);
			}

			public static void DrawBillboard(Texture tex,
				Vector3 position, 
				float radius,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				Vector4 color,
				Vector4 overlay)
			{
				Native.SPF_DrawBillboard(tex.ID,
					position.X, position.Y, position.Z,
					radius, radius,
					srcx, srcy, srcw, srch,
					flipX, flipY,
					color.X, color.Y, color.Z, color.W,
					overlay.X, overlay.Y, overlay.Z, overlay.W);
			}

			public static void DrawBillboard(Texture tex,
				Vector3 position, 
				float width, float height,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				Vector4 color,
				Vector4 overlay)
			{
				Native.SPF_DrawBillboard(tex.ID,
					position.X, position.Y, position.Z,
					width, height,
					srcx, srcy, srcw, srch,
					flipX, flipY,
					color.X, color.Y, color.Z, color.W,
					overlay.X, overlay.Y, overlay.Z, overlay.W);
			}

			public static void SetWireframe(bool wireframeEnabled)
			{
				Native.SPF_SetWireframe(wireframeEnabled);
			}

			public static void SetBackfaceCulling(bool cullingEnabled)
			{
				Native.SPF_SetBackfaceCulling(cullingEnabled);
			}

			public static void SetFogColor(Vector3 color)
			{
				Native.SPF_SetFogColor(color.X, color.Y, color.Z);
			}
		}
	}
}
