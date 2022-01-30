using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Numerics;
using System.Text;

namespace SPFSharp
{
    public static partial class SPF
    {
		public abstract class ShaderBuilder
		{
			public const string Version = "#version 330 core";

			public interface IVariable
			{
				string Name { get; }
				string Type { get; }
				string Write();
			}

			public class Vec2 : IVariable
			{
				public string Name { get; }

				public string Type => "vec2";

				public Vec2(string name)
				{
					Name = name;
				}

				public Vec2(Float x, Float y)
				{
					Name = $"vec2({x.Write()},{y.Write()})";
				}

				public Vec2(Float f)
				{
					Name = $"vec2({f.Write()},{f.Write()})";
				}

				public string Write() => Name;

				public Float X => new Float($"{Write()}.x");
				public Float Y => new Float($"{Write()}.y");

				public Vec3 ToVec3(Float z) => new Vec3($"vec3({Write()}, {z.Write()})");

				public static Vec2 operator *(Vec2 a, Vec2 b) => new Vec2($"({a.Write()} * {b.Write()})");
				public static Vec2 operator -(Vec2 a, Vec2 b) => new Vec2($"({a.Write()} - {b.Write()})");
				public static Vec2 operator +(Vec2 a, Vec2 b) => new Vec2($"({a.Write()} + {b.Write()})");
				public static Vec2 operator /(Vec2 a, Vec2 b) => new Vec2($"({a.Write()} / {b.Write()})");

				public static Vec2 operator *(Vec2 a, Float b) => new Vec2($"({a.Write()} * {b.Write()})");
				public static Vec2 operator /(Vec2 a, Float b) => new Vec2($"({a.Write()} / {b.Write()})");

				public Vec2 ReverseV() => new Vec2($"vec2({Write()}.x, 1f - {Write()}.y)");
			}

			public class Vec3 : IVariable
			{
				public string Name { get; }

				public string Type => "vec3";

				public Vec3() => Name = "vec3()";

				public Vec3(string name) => Name = name;

				public Vec3(float x, float y, float z)
					=> Name = $"vec3({x.ToString(CultureInfo.InvariantCulture)},{y.ToString(CultureInfo.InvariantCulture)},{z.ToString(CultureInfo.InvariantCulture)})";

				public Vec3(Float f) => Name = $"vec3({f.Write()},{f.Write()},{f.Write()})";

				public Vec3(Float x, Float y, Float z) => Name = $"vec3({x.Write()},{y.Write()},{z.Write()})";

				public string Write() => Name;

				public Float X => new Float($"{Write()}.x");
				public Float Y => new Float($"{Write()}.y");
				public Float Z => new Float($"{Write()}.z");

				public Vec2 XY => new Vec2($"{Write()}.xy");

				public Vec4 ToVec4(Float w) => new Vec4($"vec4({Write()}, {w.Write()})");

				public static Vec3 operator *(Vec3 a, Vec3 b) => new Vec3($"({a.Write()} * {b.Write()})");
				public static Vec3 operator -(Vec3 a, Vec3 b) => new Vec3($"({a.Write()} - {b.Write()})");
				public static Vec3 operator +(Vec3 a, Vec3 b) => new Vec3($"({a.Write()} + {b.Write()})");
				public static Vec3 operator /(Vec3 a, Vec3 b) => new Vec3($"({a.Write()} / {b.Write()})");

				public static Vec3 operator *(Vec3 a, Float b) => new Vec3($"({a.Write()} * {b.Write()})");
				public static Vec3 operator /(Vec3 a, Float b) => new Vec3($"({a.Write()} / {b.Write()})");

				public Vec3 Normalize() => new Vec3($"normalize({Write()})");

				public Float Length() => new Float($"length({Write()})");
			}

			public class Vec4 : IVariable
			{
				public string Name { get; }

				public string Type => "vec4";

				public Vec4(string name)
				{
					Name = name;
				}

				public Vec4(float x, float y, float z, float w)
				{
					Name = $"vec4({x.ToString(CultureInfo.InvariantCulture)},{y.ToString(CultureInfo.InvariantCulture)},{z.ToString(CultureInfo.InvariantCulture)},{w.ToString(CultureInfo.InvariantCulture)})";
				}

				public Vec4(Float x, Float y, Float z, Float w)
				{
					Name = $"vec4({x.Write()},{y.Write()},{z.Write()},{w.Write()})";
				}


				public string Write() => Name;

				public Float X => new Float($"{Write()}.x");
				public Float Y => new Float($"{Write()}.y");
				public Float Z => new Float($"{Write()}.z");
				public Float W => new Float($"{Write()}.w");

				public Vec2 XY => new Vec2($"{Write()}.xy");

				public Vec2 ZW => new Vec2($"{Write()}.zw");

				public Vec3 XYZ => new Vec3($"{Write()}.xyz");

				public Float R => new Float($"{Write()}.r");
				public Float G => new Float($"{Write()}.g");
				public Float B => new Float($"{Write()}.b");
				public Float A => new Float($"{Write()}.a");

				public static Vec4 operator *(Vec4 a, Vec4 b) => new Vec4($"({a.Write()} * {b.Write()})");
				public static Vec4 operator -(Vec4 a, Vec4 b) => new Vec4($"({a.Write()} - {b.Write()})");
				public static Vec4 operator +(Vec4 a, Vec4 b) => new Vec4($"({a.Write()} + {b.Write()})");
				public static Vec4 operator /(Vec4 a, Vec4 b) => new Vec4($"({a.Write()} / {b.Write()})");

				public static Vec4 operator *(Vec4 a, Float b) => new Vec4($"({a.Write()} * {b.Write()})");
				public static Vec4 operator /(Vec4 a, Float b) => new Vec4($"({a.Write()} / {b.Write()})");
			}

			public class Mat4 : IVariable
			{
				public string Name { get; }

				public string Type => "mat4";

				public Mat4(string name)
				{
					Name = name;
				}

				public Mat4(Vec4 a, Vec4 b, Vec4 c, Vec4 d)
				{
					Name = $"mat4({a.Write()},{b.Write()},{c.Write()},{d.Write()})";
				}


				public string Write() => Name;

				public static Vec4 operator *(Mat4 a, Vec4 b) => new Vec4($"({a.Write()} * {b.Write()})");

				public static Mat4 operator *(Mat4 a, Mat4 b) => new Mat4($"({a.Write()} * {b.Write()})");

				public Mat4 Inverse() => new Mat4($"inverse({Write()})");

				public Vec3 ExtractPosition(Vec2 texCoords, Float depth)
				{
					Vec4 clipSpaceLocation = new Vec4(texCoords.X * 2f - 1f, texCoords.Y * 2f - 1f, depth * 2f - 1f, 1f);
					Vec4 homogenousLocation = Inverse() * clipSpaceLocation;
					return homogenousLocation.XYZ / homogenousLocation.W;
				}
			}

			public class Float : IVariable
			{
				public string Name { get; }

				public string Type => "float";

				public Float(string name)
				{
					Name = name;
				}

				public Float() : this(0.0f) { }

				public Float(float value)
				{
					Name = (value == (int)value)
						? value.ToString("N1", CultureInfo.InvariantCulture) // Ensure it's written as a float and not an int
						: value.ToString(CultureInfo.InvariantCulture);
				}

				public string Write() => Name;

				// https://stackoverflow.com/questions/7777913/how-to-render-depth-linearly-in-modern-opengl-with-gl-fragcoord-z-in-fragment-sh
				public Float LinearizeDepth()
					=> LinearizeDepth(new Float("NearPlane"), new Float("FarPlane"), false);

				public Float LinearizeDepth(Float nearPlane, Float farPlane, bool normalize)
				{
					var ndc_depth = this * 2f - 1f;
					var depth = 2f * nearPlane * farPlane / (farPlane + nearPlane - ndc_depth * (farPlane - nearPlane));
					return normalize ? ((depth - nearPlane) / (farPlane - nearPlane)) : depth;
				}


				public static implicit operator Float(float value) => new Float(value);

				public static Float operator *(Float a, Float b) => new Float($"({a.Write()} * {b.Write()})");
				public static Float operator -(Float a, Float b) => new Float($"({a.Write()} - {b.Write()})");
				public static Float operator +(Float a, Float b) => new Float($"({a.Write()} + {b.Write()})");
				public static Float operator /(Float a, Float b) => new Float($"({a.Write()} / {b.Write()})");
				public static Float operator %(Float a, Float b) => new Float($"mod({a.Write()}, {b.Write()})");
				public static Float operator -(Float a) => new Float($"(-{a.Write()})");

				public Vec2 ToVec2() => new Vec2($"vec2({Write()}, {Write()})");
			}

			public class Sampler2D : IVariable
			{
				public string Name { get; }

				public string Type => "sampler2D";

				public Sampler2D(string name)
				{
					Name = name;
				}

				public string Write() => Name;

				public Vec4 Sample(Vec2 uv) => new Vec4($"texture2D({Write()}, {uv.Write()})");

				public Vec2 Size() => new Vec2($"vec2(textureSize({Write()}, 0))");
			}

			public class Function<TResult> where TResult : IVariable
			{
				public string Name { get; }

				public Function(string name)
				{
					Name = name;
				}

				public TResult Call(params IVariable[] parameters)
					=> (TResult)Activator.CreateInstance(typeof(TResult), Name + "(" + string.Join(", ", parameters.Select(p => p.Write())) + ")");
			}

			protected StringBuilder _mainBlock = new StringBuilder();
			protected readonly StringBuilder _globalDeclarations = new StringBuilder();

			public void Set<TVariable>(TVariable variable, TVariable expression) where TVariable : IVariable
			{
				_mainBlock.Append(variable.Name).Append(" = ").Append(expression.Write()).AppendLine(";");
			}

			public TVariable Declare<TVariable>(TVariable variable, TVariable expression) where TVariable : IVariable
			{
				_mainBlock.Append(variable.Type).Append(" ");
				Set(variable, expression);
				return variable;
			}

			public Float Declare(string name, Float value) => Declare(new Float(name), value);
			public Vec2 Declare(string name, Vec2 value) => Declare(new Vec2(name), value);
			public Vec3 Declare(string name, Vec3 value) => Declare(new Vec3(name), value);
			public Vec4 Declare(string name, Vec4 value) => Declare(new Vec4(name), value);
			public Mat4 Declare(string name, Mat4 value) => Declare(new Mat4(name), value);

			public Float Lerp(Float a, Float b, Float t) => new Float($"mix({a.Write()}, {b.Write()}, {t.Write()})");
			public Vec2 Lerp(Vec2 a, Vec2 b, Float t) => new Vec2($"mix({a.Write()}, {b.Write()}, {t.Write()})");
			public Vec3 Lerp(Vec3 a, Vec3 b, Float t) => new Vec3($"mix({a.Write()}, {b.Write()}, {t.Write()})");
			public Vec4 Lerp(Vec4 a, Vec4 b, Float t) => new Vec4($"mix({a.Write()}, {b.Write()}, {t.Write()})");

			public Float Dot(Vec3 a, Vec3 b) => new Float($"dot({a.Write()}, {b.Write()})");

			public Vec3 Reflect(Vec3 incident, Vec3 normal) => new Vec3($"reflect({incident.Write()}, {normal.Write()})");

			public Vec3 Cross(Vec3 a, Vec3 b) => new Vec3($"cross({a.Write()}, {b.Write()})");

			public Float Abs(Float v) => new Float($"abs({v.Write()})");

			public Vec3 Abs(Vec3 v) => new Vec3($"abs({v.Write()})");

			public Float Min(Float a, Float b) => new Float($"min({a.Write()}, {b.Write()})");
			public Float Max(Float a, Float b) => new Float($"max({a.Write()}, {b.Write()})");

			public Float Pow(Float v, Float power) => new Float($"pow({v.Write()}, {power.Write()})");

			public Float Saturate(Float value) => new Float($"clamp({value.Write()}, 0.0, 1.0)");

			public Float Posterize(Float value, Float cutout) => new Float($"(({value.Write()} >= {cutout.Write()}) ? 1.0 : 0.0)");

			public Float GreaterThan(Float a, Float b) => new Float($"(({a.Write()} > {b.Write()}) ? 1.0 : 0.0)");

			public Float GreaterThanEqual(Float a, Float b) => new Float($"(({a.Write()} >= {b.Write()}) ? 1.0 : 0.0)");

			public Float LesserThan(Float a, Float b) => new Float($"(({a.Write()} < {b.Write()}) ? 1.0 : 0.0)");

			public Float LesserThanEqual(Float a, Float b) => new Float($"(({a.Write()} <= {b.Write()}) ? 1.0 : 0.0)");

			public Mat4 OrthographicProjection(Float left, Float right, Float bottom, Float top, Float near, Float far)
			{
				var m11 = 2f / (right - left);
				var m22 = 2f / (top - bottom);
				var m33 = 1f / (near - far);
				var m41 = (left + right) / (left - right);
				var m42 = (top + bottom) / (bottom - top);
				var m43 = near / (near - far);
				var m44 = 1f;

				return new Mat4(
					new Vec4(m11, 0, 0, 0),
					new Vec4(0, m22, 0, 0),
					new Vec4(0, 0, m33, 0),
					new Vec4(m41, m42, m43, m44));
			}

			public Mat4 LookAt(Vec3 cameraPosition, Vec3 cameraTarget, Vec3 cameraUp)
			{
				var vector = (cameraPosition - cameraTarget).Normalize();
				var vector2 = Cross(cameraUp, vector).Normalize();
				var vector3 = Cross(vector, vector2);

				var m41 = -Dot(vector2, cameraPosition);
				var m42 = -Dot(vector3, cameraPosition);
				var m43 = -Dot(vector, cameraPosition);

				return new Mat4(
					new Vec4(vector2.X, vector3.X, vector.X, 0),
					new Vec4(vector2.Y, vector3.Y, vector.Y, 0),
					new Vec4(vector2.Z, vector3.Z, vector.Z, 0),
					new Vec4(m41, m42, m43, 1));
			}

			public void For(int iterations, string iteratorName, Action<Float> block)
			{
				_mainBlock.AppendLine($"for(int {iteratorName} = 0; {iteratorName} < {iterations}; ++{iteratorName})");
				_mainBlock.AppendLine("{");
				block?.Invoke(new Float(iteratorName));
				_mainBlock.AppendLine("}");
			}

			public Function<TResult> DeclareFunction<TResult>(string name, IReadOnlyList<IVariable> parameters, Func<TResult> block) where TResult : IVariable, new()
			{
				_globalDeclarations.Append(new TResult().Type).Append(' ').Append(name).Append('(');
				_globalDeclarations.Append(string.Join(", ", parameters.Select(p => $"{p.Type} {p.Name}")));
				_globalDeclarations.AppendLine(")");
				_globalDeclarations.AppendLine("{");
				var mainBlock = _mainBlock;
				_mainBlock = _globalDeclarations;
				var result = block.Invoke();
				_mainBlock.Append("return ").Append(result.Write()).Append(';').AppendLine();
				_mainBlock = mainBlock;
				_globalDeclarations.AppendLine("}");
				return new Function<TResult>(name);
			}
		}

		public class PixelShaderBuilder : ShaderBuilder
		{
			public readonly Vec2 UV = new Vec2("share_UV");
			public readonly Vec4 Color = new Vec4("share_Color");
			public readonly Vec4 Overlay = new Vec4("share_Overlay");
			public readonly Vec3 Position = new Vec3("share_Position");
            public readonly Vec3 Normal = new Vec3("share_Normal");
			public readonly Mat4 World = new Mat4("WorldMatrix");
			public readonly Mat4 ViewProjection = new Mat4("ViewProjectionMatrix");
			public readonly Vec3 CameraUp = new Vec3("CameraUp");
			public readonly Vec3 CameraSide = new Vec3("CameraSide");
			public readonly Vec2 ViewportSize = new Vec2("ViewportSize");
			public readonly Float FogIntensity = new Float("FogIntensity");
			public readonly Vec3 FogColor = new Vec3("FogColor");
			public readonly Vec4 GlobalOverlay = new Vec4("Overlay");
			public readonly Float Animation = new Float("Animation");
			public readonly Vec4 UserData = new Vec4("UserData");
			public readonly Mat4 UserMatrix = new Mat4("UserMatrix");
			public readonly Vec4 OutputColor = new Vec4("out_Color");
			public readonly Sampler2D Texture = new Sampler2D("Texture1");
			public readonly Sampler2D Texture1 = new Sampler2D("Texture2");
			public readonly Sampler2D Texture2 = new Sampler2D("Texture3");
			public readonly Sampler2D Texture3 = new Sampler2D("Texture4");
			public readonly Sampler2D Texture4 = new Sampler2D("Texture5");
			public readonly Sampler2D Texture5 = new Sampler2D("Texture6");
			public readonly Sampler2D Texture6 = new Sampler2D("Texture7");
			public readonly Sampler2D Texture7 = new Sampler2D("Texture8");
			public readonly Float CurrentDepth = new Float("gl_FragCoord.z");
			public readonly Vec2 PixelCoordinates = new Vec2("gl_FragCoord.xy");
			public readonly Float NearPlane = new Float("NearPlane");
			public readonly Float FarPlane = new Float("FarPlane");
			public readonly Float Distance = new Float("(smoothstep(NearPlane, FarPlane, gl_FragCoord.z / gl_FragCoord.w))");

			public Vec2 NormalizedCoordinates => PixelCoordinates / ViewportSize;

			public void AlphaTest(Vec4 color)
			{
				_mainBlock.AppendLine($"if ({color.Write()}.a <= 0) discard;");
			}

			public void DiscardIfLTE(Float a, Float b)
			{
				_mainBlock.AppendLine($"if ({a.Write()} <= {b.Write()}) discard;");
			}

			public Vec3 ApplyKernel(Vec3 color, float offset, float[] kernel)
			{
				var offsets = CreateOffsets(offset);
				for (int i = 0; i < kernel.Length; ++i)
				{
					Set(color, color + Texture.Sample(UV + new Vec2(offsets[i].X, offsets[i].Y)).XYZ * kernel[i]);
				}

				return color;
			}

			public Vec4 ApplyKernel(Vec4 color, float offset, float[] kernel)
			{
				var offsets = CreateOffsets(offset);
				for (int i = 0; i < kernel.Length; ++i)
				{
					Set(color, color + Texture.Sample(UV + new Vec2(offsets[i].X, offsets[i].Y)) * kernel[i]);
				}

				return color;
			}

			private static Vector2[] CreateOffsets(float offset) => new Vector2[]
			{
				new Vector2(-offset, offset), // top-left
				new Vector2(0.0f, offset), // top-center
				new Vector2(offset, offset), // top-right
				new Vector2(-offset, 0.0f),   // center-left
				new Vector2(0.0f, 0.0f),   // center-center
				new Vector2(offset, 0.0f),   // center-right
				new Vector2(-offset, -offset), // bottom-left
				new Vector2(0.0f, -offset), // bottom-center
				new Vector2(offset, -offset)  // bottom-right    
			};

			private static readonly float[] _blurKernel = new float[]
			{
				1.0f / 16f, 2.0f / 16f, 1.0f / 16f,
				2.0f / 16f, 4.0f / 16f, 2.0f / 16f,
				1.0f / 16f, 2.0f / 16f, 1.0f / 16f
			};

			public static float[] CreateBlurKernel(float dist)
				=> new float[]
				{
					1.0f / dist, 2.0f / dist, 1.0f / dist,
					2.0f / dist, 4.0f / dist, 2.0f / dist,
					1.0f / dist, 2.0f / dist, 1.0f / dist
				};

			public Vec3 Blur(Vec3 color, float offset) => ApplyKernel(color, offset, _blurKernel);

			public Vec4 Blur(Vec4 color, float offset) => ApplyKernel(color, offset, _blurKernel);

			public Vec4 Blur5(Sampler2D image, Vec2 uv, Vec2 resolution, Vec2 direction)
			{
				var color = new Vec4(0.0f, 0.0f, 0.0f, 0.0f);
				var off1 = new Vec2(1.3333333333333333f) * direction;
				color += image.Sample(uv) * 0.29411764705882354f;
				color += image.Sample(uv + (off1 / resolution)) * 0.35294117647058826f;
				color += image.Sample(uv - (off1 / resolution)) * 0.35294117647058826f;
				return color;
			}

			public Vec4 Blur9(Sampler2D image, Vec2 uv, Vec2 resolution, Vec2 direction)
			{
				var color = new Vec4(0.0f, 0.0f, 0.0f, 0.0f);
				var off1 = new Vec2(1.3846153846f) * direction;
				var off2 = new Vec2(3.2307692308f) * direction;
				color += image.Sample(uv) * 0.2270270270f;
				color += image.Sample(uv + (off1 / resolution)) * 0.3162162162f;
				color += image.Sample(uv - (off1 / resolution)) * 0.3162162162f;
				color += image.Sample(uv + (off2 / resolution)) * 0.0702702703f;
				color += image.Sample(uv - (off2 / resolution)) * 0.0702702703f;
				return color;
			}

			public override string ToString()
			{
				var sb = new StringBuilder();
				sb.AppendLine(Version);
				sb.AppendLine($"uniform sampler2D {Texture.Name};");
				sb.AppendLine($"uniform sampler2D {Texture1.Name};");
				sb.AppendLine($"uniform sampler2D {Texture2.Name};");
				sb.AppendLine($"uniform sampler2D {Texture3.Name};");
				sb.AppendLine($"uniform sampler2D {Texture4.Name};");
				sb.AppendLine($"uniform sampler2D {Texture5.Name};");
				sb.AppendLine($"uniform sampler2D {Texture6.Name};");
				sb.AppendLine($"uniform sampler2D {Texture7.Name};");
				sb.AppendLine($"uniform mat4 {World.Name};");
				sb.AppendLine($"uniform mat4 {ViewProjection.Name};");
				sb.AppendLine($"uniform vec3 {CameraUp.Name};");
				sb.AppendLine($"uniform vec3 {CameraSide.Name};");
				sb.AppendLine($"uniform vec2 {ViewportSize.Name};");
				sb.AppendLine($"uniform float {FogIntensity.Name};");
				sb.AppendLine($"uniform vec3 {FogColor.Name};");
				sb.AppendLine($"uniform vec4 {GlobalOverlay.Name};");
				sb.AppendLine($"uniform float {Animation.Name};");
				sb.AppendLine($"uniform vec4 {UserData.Name};");
				sb.AppendLine($"uniform mat4 {UserMatrix.Name};");
				sb.AppendLine($"uniform float {NearPlane.Name};");
				sb.AppendLine($"uniform float {FarPlane.Name};");
				sb.AppendLine($"in vec2 {UV.Name};");
				sb.AppendLine($"in vec4 {Color.Name};");
				sb.AppendLine($"in vec4 {Overlay.Name};");
				sb.AppendLine($"in vec3 {Position.Name};");
                sb.AppendLine($"in vec3 {Normal.Name};");
				sb.AppendLine($"out vec4 {OutputColor.Name};");
				sb.Append(_globalDeclarations);
				sb.AppendLine("void main()");
				sb.AppendLine("{");
				sb.Append(_mainBlock);
				sb.AppendLine("}");
				return sb.ToString();
			}
		}
	}
}
