#pragma once
#include <Core.h>

namespace SPF
{
	enum class BlendMode : int
	{
		Alpha,
		Additive,
		Multiply
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
			ResourceIndex Shader = UnsetResource;
			ResourceIndex Texture1 = UnsetResource;
			ResourceIndex Texture2 = UnsetResource;
			ResourceIndex Texture3 = UnsetResource;
			ResourceIndex Texture4 = UnsetResource;
			ResourceIndex Texture5 = UnsetResource;
			ResourceIndex Texture6 = UnsetResource;
			ResourceIndex Texture7 = UnsetResource;
			ResourceIndex Texture8 = UnsetResource;

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
			int Reference;
			StencilAction StencilFail;
			StencilAction DepthFail;
			StencilAction DepthPass;
		};

		struct States
		{
			Camera CurrentCamera;
			Material CurrentMaterial;
			Fog CurrentFog;
			UserData CurrentUserData;
			Rasterization CurrentRasterization;
			Buffers CurrentBuffers;
			Stencil CurrentStencil;
			ModelData CurrentModelData;
		};
	}
}