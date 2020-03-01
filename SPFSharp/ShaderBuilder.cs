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
            }

            public class Vec2 : IVariable, IExpression<Vec2>
            {
                public string Name { get; }

                public string Type => "vec2";

                public Vec2(string name)
                {
                    Name = name;
                }

				public Vec2(float x, float y)
				{
					Name = $"vec2({x.ToString(CultureInfo.InvariantCulture)},{y.ToString(CultureInfo.InvariantCulture)})";
				}

				public string Write() => Name;
			}

            public class Vec3 : IVariable, IExpression<Vec3>
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

				public string Write() => Name;
			}

            public class Vec4 : IVariable, IExpression<Vec4>
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

				public string Write() => Name;
            }

            public class Float : IVariable, IExpression<Float>
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

                public static readonly Float One = new Float("1.0");
                public static readonly Float Zero = new Float("0.0");
            }

			public class Sampler2D : IVariable, IExpression<Sampler2D>
			{
				public string Name { get; }

				public string Type => "sampler2D";

				public Sampler2D(string name)
				{
					Name = name;
				}

				public string Write() => Name;
			}

			public interface IExpression<TVariable> where TVariable : IVariable
            {
                string Write();
            }

			protected readonly StringBuilder _shaderBuilder = new StringBuilder();

            public void Set<TVariable>(TVariable variable, IExpression<TVariable> expression) where TVariable : IVariable
            {
                _shaderBuilder.Append(variable.Name).Append(" = ").Append(expression.Write()).AppendLine(";");
            }

            public void Declare<TVariable>(TVariable variable, IExpression<TVariable> expression) where TVariable : IVariable
            {
                _shaderBuilder.Append(variable.Type).Append(" ");
                Set(variable, expression);
            }

            internal class GenericExpression<TVariable> : IExpression<TVariable> where TVariable : IVariable
            {
                private readonly string _code;

                public GenericExpression(string code)
                {
                    _code = code;
                }

                public string Write() => _code;
            }

            public IExpression<TVariable> Lerp<TVariable>(IExpression<TVariable> a, IExpression<TVariable> b, IExpression<Float> t)
                where TVariable : IVariable
                => new GenericExpression<TVariable>($"mix({a.Write()}, {b.Write()}, {t.Write()})");

            public IExpression<TVariable> Mult<TVariable>(IExpression<TVariable> a,IExpression<TVariable> b)
                where TVariable : IVariable
                => new GenericExpression<TVariable>($"({a.Write()} * {b.Write()})");

            public IExpression<TVariable> Add<TVariable>(IExpression<TVariable> a, IExpression<TVariable> b)
                where TVariable : IVariable
                => new GenericExpression<TVariable>($"({a.Write()} + {b.Write()})");

            public IExpression<TVariable> Sub<TVariable>(IExpression<TVariable> a, IExpression<TVariable> b)
                where TVariable : IVariable
                => new GenericExpression<TVariable>($"({a.Write()} - {b.Write()})");

            public IExpression<TVariable> Div<TVariable>(IExpression<TVariable> a, IExpression<TVariable> b)
                where TVariable : IVariable
                => new GenericExpression<TVariable>($"({a.Write()} / {b.Write()})");

            public IExpression<Vec4> ToVec4(IExpression<Vec3> xyz, IExpression<Float> w)
                => new GenericExpression<Vec4>($"vec4({xyz.Write()}, {w.Write()})");
		}

		public class PixelShaderBuilder : ShaderBuilder
		{
			public readonly Float Distance = new Float("share_Distance");
			public readonly Vec2 UV = new Vec2("share_UV");
			public readonly Vec4 Color = new Vec4("share_Color");
			public readonly Vec4 Overlay = new Vec4("share_Overlay");
			public readonly Float FogIntensity = new Float("FogIntensity");
			public readonly Vec3 FogColor = new Vec3("FogColor");
			public readonly Vec4 GlobalOverlay = new Vec4("Overlay");
			public readonly Float Animation = new Float("Animation");
			public readonly Vec4 OutputColor = new Vec4("out_Color");
			public readonly Sampler2D Texture = new Sampler2D("Texture");

			public void AlphaTest(IExpression<Vec4> color)
			{
				_shaderBuilder.AppendLine($"if ({color.Write()}.a <= 0) discard;");
			}

			public override string ToString()
			{
				var sb = new StringBuilder();
				sb.AppendLine(Version);
				sb.AppendLine($"uniform sampler2D {Texture.Name};");
				sb.AppendLine($"uniform float {FogIntensity.Name};");
				sb.AppendLine($"uniform vec3 {FogColor.Name};");
				sb.AppendLine($"uniform vec4 {GlobalOverlay.Name};");
				sb.AppendLine($"uniform float {Animation.Name};");
				sb.AppendLine($"in float {Distance.Name};");
				sb.AppendLine($"in vec2 {UV.Name};");
				sb.AppendLine($"in vec4 {Color.Name};");
				sb.AppendLine($"in vec4 {Overlay.Name};");
				sb.AppendLine($"out vec4 {OutputColor.Name};");
				sb.AppendLine("void main()");
				sb.AppendLine("{");
				sb.Append(_shaderBuilder);
				sb.AppendLine("}");
				return sb.ToString();
			}
		}

        public static ShaderBuilder.IExpression<ShaderBuilder.Vec3> XYZ(this ShaderBuilder.IExpression<ShaderBuilder.Vec4> vector)
            => new ShaderBuilder.GenericExpression<ShaderBuilder.Vec3>($"{vector.Write()}.xyz");

        public static ShaderBuilder.IExpression<ShaderBuilder.Float> A(this ShaderBuilder.IExpression<ShaderBuilder.Vec4> vector)
            => new ShaderBuilder.GenericExpression<ShaderBuilder.Float>($"{vector.Write()}.a");

        public static ShaderBuilder.IExpression<ShaderBuilder.Vec4> Sample(
            this ShaderBuilder.IExpression<ShaderBuilder.Sampler2D> sampler,
            ShaderBuilder.IExpression<ShaderBuilder.Vec2> uv)
            => new ShaderBuilder.GenericExpression<ShaderBuilder.Vec4>($"texture2D({sampler.Write()}, {uv.Write()})");

		public static ShaderBuilder.IExpression<ShaderBuilder.Vec2> ToVec2(this ShaderBuilder.IExpression<ShaderBuilder.Float> f)
			=> new ShaderBuilder.GenericExpression<ShaderBuilder.Vec2>($"vec2({f.Write()}, {f.Write()})");

		public static ShaderBuilder.IExpression<ShaderBuilder.Vec4> ToVec4(this ShaderBuilder.IExpression<ShaderBuilder.Vec3> xyz, ShaderBuilder.IExpression<ShaderBuilder.Float> w)
			=> new ShaderBuilder.GenericExpression<ShaderBuilder.Vec4>($"vec4({xyz.Write()}, {w.Write()})");
	}
}
