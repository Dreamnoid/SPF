using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace SPFSharp
{
	public static partial class SPF
	{
		public class Texture : IResource
		{
			public Int32 ID { get; }
			public int Width { get; }
			public int Height { get; }
			public bool Flipped { get; }

			internal Texture(Int32 id, int w, int h, bool flipped = false)
			{
				ID = id;
				Width = w;
				Height = h;
				Flipped = flipped;
			}

			public Texture(byte[] buffer)
			{
				using (var cbuffer = new CBuffer(buffer))
				{
					ID = Native.Textures.SPF_LoadTexture(cbuffer.Pointer, cbuffer.Length);
				}

				Width = Native.Textures.SPF_GetTextureWidth(ID);
				Height = Native.Textures.SPF_GetTextureHeight(ID);
				Flipped = Native.Textures.SPF_IsTextureFlipped(ID);
			}

			public void Dispose() => Native.Textures.SPF_DeleteTexture(ID);

			public void SetFiltering(bool filtering) => Native.Textures.SPF_SetTextureFiltering(ID, filtering);

			public void GenerateMipmaps() => Native.Textures.SPF_GenerateTextureMipmaps(ID);

			public void DetermineUV(int x, int y, int width, int height, bool flipX, bool flipY, out Vector2 uv1, out Vector2 uv2)
			{
				uv1.X = x / (float)Width;
				uv2.X = (x + width) / (float)Width;
				uv1.Y = y / (float)Height;
				uv2.Y = (y + height) / (float)Height;

				if (flipX)
				{
					float t = uv1.X;
					uv1.X = uv2.X;
					uv2.X = t;
				}

				if (Flipped)
				{
					uv1.Y = 1.0f - uv1.Y;
					uv2.Y = 1.0f - uv2.Y;
				}

				if (flipY)
				{
					float t = uv1.Y;
					uv1.Y = uv2.Y;
					uv2.Y = t;
				}
			}
		}
	}
}
