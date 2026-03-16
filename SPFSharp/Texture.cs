using System;

namespace SPFSharp
{
    public static partial class SPF
	{
		public class Texture : IResource
		{
			public Int32 ID { get; }
			public int Width { get; }
			public int Height { get; }

			internal Texture(Int32 id, int w, int h)
			{
				ID = id;
				Width = w;
				Height = h;
			}

			public static Texture Create(int w, int h)
			{
				int id = Native.Textures.SPF_CreateEmptyTexture((uint)w, (uint)h);
				return new Texture(id, w, h);
			}

			public static Texture CreateWhite()
			{
				int id = Native.Textures.SPF_CreateWhiteTexture();
				return new Texture(id, Native.Textures.SPF_GetTextureWidth(id), Native.Textures.SPF_GetTextureHeight(id));
			}

			public Texture(byte[] buffer)
			{
				using (var cbuffer = new CBuffer(buffer))
				{
					ID = Native.Textures.SPF_LoadTexture(cbuffer.Pointer, cbuffer.Length);
				}

				Width = Native.Textures.SPF_GetTextureWidth(ID);
				Height = Native.Textures.SPF_GetTextureHeight(ID);
			}

			public void Dispose() => Native.Textures.SPF_DeleteTexture(ID);

			public void SetFiltering(bool filtering) => Native.Textures.SPF_SetTextureFiltering(ID, filtering);
		}
	}
}
