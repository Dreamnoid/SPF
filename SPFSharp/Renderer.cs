using System.Numerics;

namespace SPFSharp
{
	public static partial class SPF
	{
		public enum BlendMode : int
		{
			Alpha = 0,
			Additive = 1
		}

		public enum Culling : int
		{
			Disabled,
			CW,
			CCW
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

            public static void BeginOrthographic(Surface surface,
                float minX, float maxX,
                float minY, float maxY,
                float minZ, float maxZ, float fogIntensity = 0f)
            {
                Native.SPF_BeginOrthographic((surface != null) ? surface.ID : -1,
                    minX, maxX,
                    minY, maxY,
                    minZ, maxZ, fogIntensity);
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
				DrawTexturedQuad(
					tex,
					new Vector3(a.X, a.Y, 0f),
					new Vector3(b.X, b.Y, 0f),
					new Vector3(c.X, c.Y, 0f),
					new Vector3(d.X, d.Y, 0f),
					srcx, srcy, srcw, srch,
					flipX, flipY,
					color, color, color, color,
					overlay);
			}

			public static void DrawTexturedQuad(Texture tex,
				Vector2 a,
				Vector2 b,
				Vector2 c,
				Vector2 d,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				Vector4 aColor, Vector4 bColor, Vector4 cColor, Vector4 dColor,
				Vector4 overlay)
			{
				DrawTexturedQuad(
					tex,
					new Vector3(a.X, a.Y, 0f),
					new Vector3(b.X, b.Y, 0f),
					new Vector3(c.X, c.Y, 0f),
					new Vector3(d.X, d.Y, 0f),
					srcx, srcy, srcw, srch,
					flipX, flipY,
					aColor, bColor, cColor, dColor,
					overlay);
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
				DrawTexturedQuad(
					tex,
					a, b, c, d,
					srcx, srcy, srcw, srch,
					flipX, flipY,
					color, color, color, color,
					overlay);
			}

			public static void DrawTexturedQuad(Texture tex,
				Vector3 a,
				Vector3 b,
				Vector3 c,
				Vector3 d,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				Vector4 aColor, Vector4 bColor, Vector4 cColor, Vector4 dColor,
				Vector4 overlay)
			{
				Native.SPF_DrawTexturedQuad((tex != null) ? tex.ID : 0,
					a.X, a.Y, a.Z,
					b.X, b.Y, b.Z,
					c.X, c.Y, c.Z,
					d.X, d.Y, d.Z,
					srcx, srcy, srcw, srch,
					flipX, flipY,
					aColor.X, aColor.Y, aColor.Z, aColor.W,
					bColor.X, bColor.Y, bColor.Z, bColor.W,
					cColor.X, cColor.Y, cColor.Z, cColor.W,
					dColor.X, dColor.Y, dColor.Z, dColor.W,
					overlay.X, overlay.Y, overlay.Z, overlay.W);
			}

			public static void DrawTexturedTriangle(Texture tex, Vertex a, Vertex b, Vertex c)
			{
				Native.SPF_DrawTexturedTriangle((tex != null) ? tex.ID : 0, a, b, c);
			}

			public static readonly float[] IdentityMatrix = new float[]
			{
				1f, 0f, 0f, 0f,
				0f, 1f, 0f, 0f,
				0f, 0f, 1f, 0f,
				0f, 0f, 0f, 1f
			};

			public static void DrawMesh(Texture texture, Mesh mesh) => DrawMesh(null, texture, null, null, mesh, 0, mesh.VerticesCount, IdentityMatrix, Vector4.Zero);

			public static void DrawMesh(Texture texture, Mesh mesh, float[] world) => DrawMesh(null, texture, null, null, mesh, 0, mesh.VerticesCount, world, Vector4.Zero);

			public static void DrawMesh(
				Texture texture,
				Mesh mesh, int first, int count,
				float[] world,
				Vector4 overlay) => DrawMesh(null, texture, null, null, mesh, first, count, world, overlay);

				public static void DrawMesh(
				Shader shader, Texture texture, Texture texture1, Texture texture2,
				Mesh mesh, int first, int count, 
				float[] world, 
				Vector4 overlay)
			{
				Native.SPF_DrawMesh(
					(shader != null) ? shader.ID : InvalidResource, 
					(texture != null) ? texture.ID : InvalidResource,
					(texture1 != null) ? texture1.ID : InvalidResource,
					(texture2 != null) ? texture2.ID : InvalidResource,
					mesh.ID, first, count, 
					world,
					overlay.X, overlay.Y, overlay.Z, overlay.W);
			}

			public static void DrawMesh(Mesh mesh, float[] world) => DrawMesh(null, null, null, null, mesh, 0, mesh.VerticesCount, world, Vector4.Zero);

			public static void DrawBillboard(Texture tex,
				Vector3 position, 
				float radius,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				Vector4 color,
				Vector4 overlay)
			{
				DrawBillboard(tex,
					position,
					radius, radius,
					srcx, srcy, srcw, srch,
					flipX, flipY,
					color,
					overlay);
			}

			public static void DrawBillboard(Texture tex,
				Vector3 position, 
				float width, float height,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				Vector4 color,
				Vector4 overlay)
			{
				Native.SPF_DrawBillboard((tex != null) ? tex.ID : 0,
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

			public static void SetBackfaceCulling(Culling culling)
			{
				Native.SPF_SetBackfaceCulling((int)culling);
			}

			public static void SetFogColor(Vector3 color)
			{
				Native.SPF_SetFogColor(color.X, color.Y, color.Z);
			}

			public static void SetAnimation(float animation) => Native.SPF_SetAnimation(animation);

			public static void SetUserData(Vector4 userData) => Native.SPF_SetUserData(userData.X, userData.Y, userData.Z, userData.W);

			public static Texture GetDefaultDepthTexture()
				=> new Texture(Native.SPF_GetSurfaceDepthTexture(Native.SPF_GetFinalSurface()), GetWindowWidth(), GetWindowHeight());
		}
	}
}
