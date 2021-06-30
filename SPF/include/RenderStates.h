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
			ResourceIndex Shader;
			ResourceIndex Texture1;
			ResourceIndex Texture2;
			ResourceIndex Texture3;
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