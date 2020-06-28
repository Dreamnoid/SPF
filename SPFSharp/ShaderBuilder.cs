using System.Globalization;
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

				public string Write() => Name;

				public Float X => new Float($"{Write()}.x");
				public Float Y => new Float($"{Write()}.y");

				public static Vec2 operator *(Vec2 a, Vec2 b) => new Vec2($"({a.Write()} * {b.Write()})");
				public static Vec2 operator -(Vec2 a, Vec2 b) => new Vec2($"({a.Write()} - {b.Write()})");
				public static Vec2 operator +(Vec2 a, Vec2 b) => new Vec2($"({a.Write()} + {b.Write()})");
				public static Vec2 operator /(Vec2 a, Vec2 b) => new Vec2($"({a.Write()} / {b.Write()})");

				public static Vec2 operator *(Vec2 a, Float b) => new Vec2($"({a.Write()} * {b.Write()})");
				public static Vec2 operator /(Vec2 a, Float b) => new Vec2($"({a.Write()} / {b.Write()})");
			}

            public class Vec3 : IVariable
			{
                public string Name { get; }

                public string Type => "vec3";

                public Vec3(string name)
                {
                    Name = name;
                }

				public Vec3(float x, float y, float z)
				{
					Name = $"vec3({x.ToString(CultureInfo.InvariantCulture)},{y.ToString(CultureInfo.InvariantCulture)},{z.ToString(CultureInfo.InvariantCulture)})";
				}

				public Vec3(Float f)
				{
					Name = $"vec3({f.Write()},{f.Write()},{f.Write()})";
				}

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

				public Vec2 XY => new Vec2($"{Write()}.xy");

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

            public class Float : IVariable
            {
                public string Name { get; }

                public string Type => "float";

                public Float(string name)
                {
                    Name = name;
                }

				public Float(float value)
				{
					Name = value.ToString(CultureInfo.InvariantCulture);
				}

				public string Write() => Name;

				// https://stackoverflow.com/questions/7777913/how-to-render-depth-linearly-in-modern-opengl-with-gl-fragcoord-z-in-fragment-sh
				public Float LinearizeDepth()
					=> new Float($"((-2 * FarPlane * NearPlane / (FarPlane - NearPlane)) / ((-(FarPlane + NearPlane) / (FarPlane - NearPlane)) + (2.0 * ({Write()}) - 1.0)))");

				public static implicit operator Float(float value) => new Float(value);

				public static Float operator *(Float a, Float b) => new Float($"({a.Write()} * {b.Write()})");
				public static Float operator -(Float a, Float b) => new Float($"({a.Write()} - {b.Write()})");
				public static Float operator +(Float a, Float b) => new Float($"({a.Write()} + {b.Write()})");
				public static Float operator /(Float a, Float b) => new Float($"({a.Write()} / {b.Write()})");
				public static Float operator %(Float a, Float b) => new Float($"mod({a.Write()}, {b.Write()})");

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
			}

			protected readonly StringBuilder _shaderBuilder = new StringBuilder();

            public void Set<TVariable>(TVariable variable, TVariable expression) where TVariable : IVariable
            {
                _shaderBuilder.Append(variable.Name).Append(" = ").Append(expression.Write()).AppendLine(";");
            }

            public TVariable Declare<TVariable>(TVariable variable, TVariable expression) where TVariable : IVariable
            {
                _shaderBuilder.Append(variable.Type).Append(" ");
                Set(variable, expression);
				return variable;
            }

			public Float Declare(string name, Float value) => Declare(new Float(name), value);
			public Vec2 Declare(string name, Vec2 value) => Declare(new Vec2(name), value);
			public Vec3 Declare(string name, Vec3 value) => Declare(new Vec3(name), value);
			public Vec4 Declare(string name, Vec4 value) => Declare(new Vec4(name), value);

			public Float Lerp(Float a, Float b, Float t) => new Float($"mix({a.Write()}, {b.Write()}, {t.Write()})");
			public Vec2 Lerp(Vec2 a, Vec2 b, Float t) => new Vec2($"mix({a.Write()}, {b.Write()}, {t.Write()})");
			public Vec3 Lerp(Vec3 a, Vec3 b, Float t) => new Vec3($"mix({a.Write()}, {b.Write()}, {t.Write()})");
			public Vec4 Lerp(Vec4 a, Vec4 b, Float t) => new Vec4($"mix({a.Write()}, {b.Write()}, {t.Write()})");

			public Float Dot(Vec3 a, Vec3 b) => new Float($"dot({a.Write()}, {b.Write()})");

			public Float Abs(Float v) => new Float($"abs({v.Write()})");

			public Float Pow(Float v, Float power) => new Float($"pow({v.Write()}, {power.Write()})");

			public Float Saturate(Float value) => new Float($"clamp({value.Write()}, 0.0, 1.0)");
		}

		public class PixelShaderBuilder : ShaderBuilder
		{
			public readonly Float Distance = new Float("share_Distance");
			public readonly Vec2 UV = new Vec2("share_UV");
			public readonly Vec4 Color = new Vec4("share_Color");
			public readonly Vec4 Overlay = new Vec4("share_Overlay");
			public readonly Vec3 Position = new Vec3("share_Position");
			public readonly Float FogIntensity = new Float("FogIntensity");
			public readonly Vec3 FogColor = new Vec3("FogColor");
			public readonly Vec4 GlobalOverlay = new Vec4("Overlay");
			public readonly Float Animation = new Float("Animation");
			public readonly Vec4 OutputColor = new Vec4("out_Color");
			public readonly Sampler2D Texture = new Sampler2D("Texture");
			public readonly Sampler2D Texture1 = new Sampler2D("Texture1");
			public readonly Sampler2D Texture2 = new Sampler2D("Texture2");
			public readonly Float CurrentDepth = new Float("gl_FragCoord.z");
			public readonly Vec2 PixelCoordinates = new Vec2("gl_FragCoord.xy");
			public readonly Float NearPlane = new Float("NearPlane");
			public readonly Float FarPlane = new Float("FarPlane");

			public void AlphaTest(Vec4 color)
			{
				_shaderBuilder.AppendLine($"if ({color.Write()}.a <= 0) discard;");
			}

			public override string ToString()
			{
				var sb = new StringBuilder();
				sb.AppendLine(Version);
				sb.AppendLine($"uniform sampler2D {Texture.Name};");
				sb.AppendLine($"uniform sampler2D {Texture1.Name};");
				sb.AppendLine($"uniform sampler2D {Texture2.Name};");
				sb.AppendLine($"uniform float {FogIntensity.Name};");
				sb.AppendLine($"uniform vec3 {FogColor.Name};");
				sb.AppendLine($"uniform vec4 {GlobalOverlay.Name};");
				sb.AppendLine($"uniform float {Animation.Name};");
				sb.AppendLine($"uniform float {NearPlane.Name};");
				sb.AppendLine($"uniform float {FarPlane.Name};");
				sb.AppendLine($"in float {Distance.Name};");
				sb.AppendLine($"in vec2 {UV.Name};");
				sb.AppendLine($"in vec4 {Color.Name};");
				sb.AppendLine($"in vec4 {Overlay.Name};");
				sb.AppendLine($"in vec3 {Position.Name};");
				sb.AppendLine($"out vec4 {OutputColor.Name};");
				sb.AppendLine("void main()");
				sb.AppendLine("{");
				sb.Append(_shaderBuilder);
				sb.AppendLine("}");
				return sb.ToString();
			}
		}
	}
}
