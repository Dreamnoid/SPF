#pragma once
#include <Core.h>

namespace SPF
{
	enum class BlendMode : int
	{
		Alpha,
		Additive
	};

	enum class Comparison : int
	{
		Always,
		Never,
		Less,
		LessOrEqual,
		Greater,
		GreaterOrEqual,
		Equal,
		NotEqual
	};

	enum class StencilAction : int
	{
		Keep,
		Replace,
		Increase,
	};

	enum class BackfaceCulling
	{
		Disabled,
		CW,
		CCW
	};

	namespace RenderState
	{
		struct Camera
		{
			Matrix ViewProjectionMatrix;
			float NearPlane;
			float FarPlane;
			Vector3 Up;
			Vector3 Side;
		};

		struct Material
		{
			ResourceIndex Shader = InvalidResource;
			ResourceIndex Texture1 = InvalidResource;
			ResourceIndex Texture2 = InvalidResource;
			ResourceIndex Texture3 = InvalidResource;
			ResourceIndex Texture4 = InvalidResource;
			ResourceIndex Texture5 = InvalidResource;
			ResourceIndex Texture6 = InvalidResource;
			ResourceIndex Texture7 = InvalidResource;
			ResourceIndex Texture8 = InvalidResource;

			ResourceIndex GetTexture(int i) const
			{
				switch (i)
				{
				case 0: return Texture1;
				case 1: return Texture2;
				case 2: return Texture3;
				case 3: return Texture4;
				case 4: return Texture5;
				case 5: return Texture6;
				case 6: return Texture7;
				case 7: return Texture8;
				default: return InvalidResource;
				}
			}
		};

		struct Fog
		{
			float Intensity;
			RGB Color;
		};

		struct UserData
		{
			float Animation;
			Vector4 UserData;
			Matrix UserMatrix;
		};

		struct ModelData
		{
			Matrix WorldMatrix;
			RGBA Overlay;
		};

		struct Rasterization
		{
			BlendMode Blending;
			bool Wireframe;
			BackfaceCulling BackfaceCulling;
			float LineWidth = 1.f;
		};

		struct Buffers
		{
			bool ColorWrite;
			bool DepthWrite;
			Comparison DepthTest;
		};

		struct Stencil
		{
			bool Write;
			Comparison Test;
			float Reference;
			StencilAction StencilFail;
			StencilAction DepthFail;
			StencilAction DepthPass;
		};

		struct States
		{
			Camera Camera;
			Material Material;
			Fog Fog;
			UserData UserData;
			Rasterization Rasterization;
			Buffers Buffers;
			Stencil Stencil;
			ModelData ModelData;
		};
	}
}