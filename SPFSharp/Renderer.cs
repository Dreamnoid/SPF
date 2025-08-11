using System.Numerics;

namespace SPFSharp
{
	public static partial class SPF
	{
		public enum BlendMode : int
		{
			None = 0,
			Alpha = 1,
			Additive = 2,
			Multiply = 3
		}

		public enum Culling : int
		{
			Disabled,
			CW,
			CCW
		}

		public enum Comparison : int
		{
			Always,
			Never,
			Less,
			LessOrEqual,
			Greater,
			GreaterOrEqual,
			Equal,
			NotEqual
		}

		public enum StencilAction : int
		{
			Keep,
			Replace,
			Increase,
		}

		public enum PrimitiveType : int
		{
			Triangle,
			Line
		}

		public static class Renderer
		{
			private static int Resolve(IResource resource) => (resource != null) ? resource.ID : -1;

			private static Surface _currentSurface = null;

			public static void Begin() => Begin(null);

			public static void Begin(Surface surface, bool clear = false)
			{
				_currentSurface = surface;
				Native.Renderer.SPF_Begin(Resolve(surface), clear);
			}

			public static void FillRectangle(int x, int y, int w, int h, Vector4 color)
			{
				SetPrimitiveType(PrimitiveType.Triangle);
				SetMaterial(null);

				PushVertex(new Vector3(x, y + h, 0), Vector2.Zero, color);
				PushVertex(new Vector3(x + w, y + h, 0), Vector2.Zero, color);
				PushVertex(new Vector3(x + w, y, 0), Vector2.Zero, color);

				PushVertex(new Vector3(x, y + h, 0), Vector2.Zero, color);
				PushVertex(new Vector3(x + w, y, 0), Vector2.Zero, color);
				PushVertex(new Vector3(x, y, 0), Vector2.Zero, color);
			}

			public static void FillVerticalGradient(
				int x, int y, int w, int h, 
				Vector4 color1, Vector4 color2)
			{
				SetPrimitiveType(PrimitiveType.Triangle);
				SetMaterial(null);

				PushVertex(new Vector3(x, y + h, 0), Vector2.Zero, color2);
				PushVertex(new Vector3(x + w, y + h, 0), Vector2.Zero, color2);
				PushVertex(new Vector3(x + w, y, 0), Vector2.Zero, color1);

				PushVertex(new Vector3(x, y + h, 0), Vector2.Zero, color2);
				PushVertex(new Vector3(x + w, y, 0), Vector2.Zero, color1);
				PushVertex(new Vector3(x, y, 0), Vector2.Zero, color1);
			}

			public static void FillHorizontalGradient(
				int x, int y, int w, int h,
				Vector4 color1, Vector4 color2)
			{
				SetPrimitiveType(PrimitiveType.Triangle);
				SetMaterial(null);

				PushVertex(new Vector3(x, y + h, 0), Vector2.Zero, color1);
				PushVertex(new Vector3(x + w, y + h, 0), Vector2.Zero, color2);
				PushVertex(new Vector3(x + w, y, 0), Vector2.Zero, color2);

				PushVertex(new Vector3(x, y + h, 0), Vector2.Zero, color1);
				PushVertex(new Vector3(x + w, y, 0), Vector2.Zero, color2);
				PushVertex(new Vector3(x, y, 0), Vector2.Zero, color1);
			}

			public static void DrawRectangle(int x, int y, int w, int h, Vector4 color, int lineWidth = 1)
			{
				FillRectangle(x, y, w, lineWidth, color);
				FillRectangle(x, y + h - lineWidth, w, lineWidth, color);
				FillRectangle(x, y + lineWidth, lineWidth, h - (lineWidth * 2), color);
				FillRectangle(x + w - lineWidth, y + lineWidth, lineWidth, h - (lineWidth * 2), color);
			}

			public static void DrawTexture(Texture tex, int x, int y)
				=> DrawTexture(tex,
					x, y, tex.Width, tex.Height,
					0, 0, tex.Width, tex.Height,
					false, false,
					Vector4.One, Vector4.Zero);

			public static void DrawTexture(Texture tex,
				int x, int y, int w, int h,
				int clipx, int clipy, int clipw, int cliph,
				bool flipx, bool flipy,
				Vector4 color) 
				=> DrawTexture(tex,
					x, y, w, h, 
					clipx, clipy, clipw, cliph,
					flipx, flipy,
					color, Vector4.Zero);

			public static void DrawTexture(Texture tex,
				int x, int y, int w, int h,
				int clipx, int clipy, int clipw, int cliph,
				bool flipx, bool flipy,
				Vector4 color,
				Vector4 overlay)
			{
				DrawTexturedQuad(tex, 
					new Vector2(x, y), 
					new Vector2(x + w, y), 
					new Vector2(x + w, y + h), 
					new Vector2(x, y + h), 
					clipx, clipy, clipw, cliph, 
					flipx, flipy, 
					color, overlay);
			}

			public static void DrawTexturedQuad(Texture tex,
				in Vector2 a,
				in Vector2 b,
				in Vector2 c,
				in Vector2 d,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				in Vector4 color,
				in Vector4 overlay)
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
				in Vector2 a,
				in Vector2 b,
				in Vector2 c,
				in Vector2 d,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				in Vector4 aColor, in Vector4 bColor, in Vector4 cColor, in Vector4 dColor,
				in Vector4 overlay)
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
				in Vector3 a,
				in Vector3 b,
				in Vector3 c,
				in Vector3 d,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				in Vector4 color,
				in Vector4 overlay)
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
				in Vector3 a,
				in Vector3 b,
				in Vector3 c,
				in Vector3 d,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				in Vector4 aColor, in Vector4 bColor, in Vector4 cColor, in Vector4 dColor,
				in Vector4 overlay)
			{
				SetPrimitiveType(PrimitiveType.Triangle);

				var uv1 = Vector2.Zero;
				var uv2 = Vector2.One;
				if (tex != null)
				{
					tex.DetermineUV(srcx, srcy, srcw, srch, flipX, flipY, out uv1, out uv2);
					SetMaterial(null, tex);
				}
				else
				{
					SetMaterial(null, null);
				}
				PushVertex(d, Vector3.Zero, new Vector2(uv1.X, uv2.Y), Vector2.Zero, dColor, overlay);
				PushVertex(c, Vector3.Zero, new Vector2(uv2.X, uv2.Y), Vector2.Zero, cColor, overlay);
				PushVertex(b, Vector3.Zero, new Vector2(uv2.X, uv1.Y), Vector2.Zero, bColor, overlay);

				PushVertex(d, Vector3.Zero, new Vector2(uv1.X, uv2.Y), Vector2.Zero, dColor, overlay);
				PushVertex(b, Vector3.Zero, new Vector2(uv2.X, uv1.Y), Vector2.Zero, bColor, overlay);
				PushVertex(a, Vector3.Zero, new Vector2(uv1.X, uv1.Y), Vector2.Zero, aColor, overlay);
			}

			public static void DrawTexturedTriangle(Texture tex, in Vertex a, in Vertex b, in Vertex c)
			{
				SetPrimitiveType(PrimitiveType.Triangle);
				SetMaterial(null, tex);
				PushVertex(a);
				PushVertex(b);
				PushVertex(c);
			}

			public static void FillSurface() => FillSurface(Vector4.One);

			public static void FillSurface(in Vector4 color)
			{
				SetMaterial(null);
				DrawFullScreenQuad(color);
			}

			public static void DrawFullScreenQuad() => DrawFullScreenQuad(Vector4.One);

			public static void DrawFullScreenQuad(in Vector4 color)
			{
				SetPrimitiveType(PrimitiveType.Triangle);
				var width = _currentSurface?.Width ?? GetWindowWidth();
				var height = _currentSurface?.Height ?? GetWindowHeight();

				PushVertex(new Vector3(0, height, 0), new Vector2(0, 0), color);
				PushVertex(new Vector3(width, height, 0), new Vector2(1, 0), color);
				PushVertex(new Vector3(width, 0, 0), new Vector2(1, 1), color);

				PushVertex(new Vector3(0, height, 0), new Vector2(0, 0), color);
				PushVertex(new Vector3(width, 0, 0), new Vector2(1, 1), color);
				PushVertex(new Vector3(0, 0, 0), new Vector2(0, 1), color);
			}

			public static readonly float[] IdentityMatrix = new float[]
			{
				1f, 0f, 0f, 0f,
				0f, 1f, 0f, 0f,
				0f, 0f, 1f, 0f,
				0f, 0f, 0f, 1f
			};

			public static void DrawMesh(Mesh mesh, int first, int count, float[] world, in Vector4 overlay)
			{
				SetPrimitiveType(PrimitiveType.Triangle);
				Native.Renderer.SPF_DrawMesh(
					mesh.ID, first, count,
					world,
					overlay.X, overlay.Y, overlay.Z, overlay.W);
			}

			public static void DrawMesh(Mesh mesh, float[] world) => DrawMesh(mesh, 0, mesh.VerticesCount, world, Vector4.Zero);

			public static void DrawMesh(Mesh mesh, float[] world, in Vector4 overlay) => DrawMesh(mesh, 0, mesh.VerticesCount, world, overlay);

			public static void DrawLine(in Vector3 from, in Vector3 to, in Vector4 color, float width = 1f)
			{
				SetMaterial(null);
				SetRasterization(lineWidth: width);
				SetPrimitiveType(PrimitiveType.Line);
				PushVertex(from, Vector2.Zero, color);
				PushVertex(to, Vector2.Zero, color);
			}

			public static void DrawBillboard(
				Texture tex,
				in Vector3 position, 
				float radius,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				in Vector4 color,
				in Vector4 overlay)
			{
				DrawBillboard(
					tex,
					position,
					new Vector2(radius, radius),
					srcx, srcy, srcw, srch,
					flipX, flipY,
					color,
					overlay);
			}

			public static void DrawBillboard(
				Texture tex,
				in Vector3 position,
				in Vector2 size,
				int srcx, int srcy, int srcw, int srch,
				bool flipX, bool flipY,
				in Vector4 color,
				in Vector4 overlay)
			{
				SetPrimitiveType(PrimitiveType.Triangle);
				tex.DetermineUV(srcx, srcy, srcw, srch, flipX, flipY, out var uv1, out var uv2);
				float halfWidth = size.X * 0.5f;
				PushVertex(position, Vector3.Zero, new Vector2(uv1.X, uv2.Y), new Vector2(-halfWidth, 0.0f), color, overlay);
				PushVertex(position, Vector3.Zero, new Vector2(uv2.X, uv2.Y), new Vector2(+halfWidth, 0.0f), color, overlay);
				PushVertex(position, Vector3.Zero, new Vector2(uv2.X, uv1.Y), new Vector2(+halfWidth, size.Y), color, overlay);

				PushVertex(position, Vector3.Zero, new Vector2(uv1.X, uv2.Y), new Vector2(-halfWidth, 0.0f), color, overlay);
				PushVertex(position, Vector3.Zero, new Vector2(uv2.X, uv1.Y), new Vector2(+halfWidth, size.Y), color, overlay);
				PushVertex(position, Vector3.Zero, new Vector2(uv1.X, uv1.Y), new Vector2(-halfWidth, size.Y), color, overlay);
			}

			public static void SetCamera(float[] viewMatrix, float[] projectionMatrix, float nearPlane, float farPlane, in Vector3 cameraUp, in Vector3 cameraSide)
				=> Native.Renderer.SPF_SetCamera(viewMatrix, projectionMatrix, nearPlane, farPlane, cameraUp.X, cameraUp.Y, cameraUp.Z, cameraSide.X, cameraSide.Y, cameraSide.Z);

			public static void SetMaterial(
				Shader shader,
				Texture texture1 = null,
				Texture texture2 = null, 
				Texture texture3 = null,
				Texture texture4 = null, 
				Texture texture5 = null, 
				Texture texture6 = null, 
				Texture texture7 = null,
				Texture texture8 = null)
				=> Native.Renderer.SPF_SetMaterial(
					Resolve(shader),
					Resolve(texture1),
					Resolve(texture2),
					Resolve(texture3),
					Resolve(texture4),
					Resolve(texture5),
					Resolve(texture6),
					Resolve(texture7),
					Resolve(texture8));

			public static void SetRasterization(
				BlendMode blendMode = BlendMode.Alpha,
				Culling backfaceCulling = Culling.Disabled,
				bool wireframeEnabled = false, float lineWidth = 1f)
				=> Native.Renderer.SPF_SetRasterization((int)blendMode, wireframeEnabled, (int)backfaceCulling, lineWidth);

			public static void SetFog(float intensity, in Vector3 color)
				=> Native.Renderer.SPF_SetFog(intensity, color.X, color.Y, color.Z);

			public static void SetBuffers(bool writeColor = true, bool writeDepth = true, Comparison testDepth = Comparison.Less)
				=> Native.Renderer.SPF_SetBuffers(writeColor, writeDepth, (int)testDepth);

			public static void SetBuffersFor2D() => SetBuffers(true, false, Comparison.Always);

			public static void SetStencil(
				bool writeStencil = false,
				Comparison testStencil = Comparison.Always, 
				int stencilRef = 1,
				StencilAction fail = StencilAction.Keep, 
				StencilAction zfail = StencilAction.Keep, 
				StencilAction zpass = StencilAction.Keep)
				=> Native.Renderer.SPF_SetStencil(writeStencil, (int)testStencil, stencilRef, (int)fail, (int)zfail, (int)zpass);

			public static void SetUserData(float animation, in Vector4 userData, float[] userMatrix)
				=> Native.Renderer.SPF_SetUserData(animation, userData.X, userData.Y, userData.Z, userData.W, userMatrix);

			public static Texture GetDefaultDepthTexture()
				=> new Texture(Native.Surfaces.SPF_GetSurfaceDepthTexture(Native.Renderer.SPF_GetFinalSurface()), GetWindowWidth(), GetWindowHeight(), true);

			public static void SetPrimitiveType(PrimitiveType type)
				=> Native.Renderer.SPF_SetPrimitiveType((int)type);

			public static void PushVertex(in Vertex v)
				=> Native.Renderer.SPF_PushVertex(
					v.X, v.Y, v.Z, 
					v.NormalX, v.NormalY, v.NormalZ, 
					v.U, v.V, v.BU, v.BV,
					v.R, v.G, v.B, v.A, 
					v.OverlayR, v.OverlayG, v.OverlayB, v.OverlayA);

			public static void PushVertex(in Vector3 position, in Vector3 normal, in Vector2 uv, in Vector2 buv, in Vector4 color, in Vector4 overlay)
				=> Native.Renderer.SPF_PushVertex(
					position.X, position.Y, position.Z, 
					normal.X, normal.Y, normal.Z, 
					uv.X, uv.Y, buv.X, buv.Y,
					color.X, color.Y, color.Z, color.W,
					overlay.X, overlay.Y, overlay.Z, overlay.W);

			public static void PushVertex(in Vector3 position, in Vector3 normal, in Vector2 uv, in Vector4 color, in Vector4 overlay)
				=> PushVertex(position, normal, uv, Vector2.Zero, color, overlay);

			public static void PushVertex(in Vector3 position, in Vector3 normal, in Vector2 uv, in Vector4 color)
				=> PushVertex(position, normal, uv, Vector2.Zero, color, Vector4.Zero);

			public static void PushVertex(in Vector3 position, in Vector2 uv, in Vector4 color)
				=> PushVertex(position, Vector3.Zero, uv, Vector2.Zero, color, Vector4.Zero);

			public static void PushVertex(in Vector3 position, in Vector2 uv)
				=> PushVertex(position, Vector3.Zero, uv, Vector2.Zero, Vector4.One, Vector4.Zero);
		}
	}
}
