using System.Linq.Expressions;
using System.Text;

namespace SPFSharp
{
    public static partial class SPF
    {
        public class ShaderBuilder
        {
			public const string Version = "#version 330 core";

			public static class Attribute
			{
				public static readonly Vec3 Position = new Vec3("in_Position");
				public static readonly Vec4 UV = new Vec4("in_UV");
				public static readonly Vec4 Color = new Vec4("in_Color");
				public static readonly Vec4 Overlay = new Vec4("in_Overlay");
			}

			public static class Variables
            {
                public static class VertexShader
                {

                    public const string UniformMVP = "MVP";
                    public const string UniformCameraUp = "CameraUp";
                    public const string UniformCameraSide = "CameraSide";
                    public const string UniformFarPlane = "FarPlane";
                }
            }

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

                public string Write() => Name;
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

            public readonly Float One = new Float("1.0");
            public readonly Float Zero = new Float("0.0");

			protected readonly StringBuilder _shaderBuilder = new StringBuilder();

			public ShaderBuilder Declaration<TVariable>(TVariable variable, IExpression<TVariable> expression) where TVariable : IVariable
            {
                _shaderBuilder.Append(variable.Type).Append(" ");
                return Assignation(variable, expression);
            }

            public ShaderBuilder Assignation<TVariable>(TVariable variable, IExpression<TVariable> expression) where TVariable : IVariable
            {
                _shaderBuilder.Append(variable.Name).Append(" = ").Append(expression.Write()).AppendLine(";");
                return this;
            }

            public class Add<TVariable> : IExpression<TVariable> where TVariable : IVariable
            {
                private readonly IExpression<TVariable> _a, _b;

                public Add(IExpression<TVariable> a, IExpression<TVariable> b)
                {
                    _a = a;
                    _b = b;
                }

                public string Write() => $"({_a.Write()} + {_b.Write()})";
            }

            public class Multiply<TVariable> : IExpression<TVariable> where TVariable : IVariable
            {
                private readonly IExpression<TVariable> _a, _b;

                public Multiply(IExpression<TVariable> a, IExpression<TVariable> b)
                {
                    _a = a;
                    _b = b;
                }

                public string Write() => $"({_a.Write()} * {_b.Write()})";
            }

            public class Vec3ToVec4 : IExpression<Vec4>
            {
                private readonly IExpression<Vec3> _xyz;
                private readonly IExpression<Float> _w;

                public Vec3ToVec4(IExpression<Vec3> xyz, IExpression<Float> w)
                {
                    _xyz = xyz;
                    _w = w;
                }

                public string Write() => $"vec4({_xyz.Write()}.xyz, {_w.Write()})";
            }

            public class Vec4ToVec3 : IExpression<Vec3>
            {
                private readonly IExpression<Vec4> _xyzw;

                public Vec4ToVec3(IExpression<Vec4> xyzw)
                {
                    _xyzw = xyzw;
                }

                public string Write() => $"{_xyzw.Write()}.xyz";
            }

            public class Alpha : IExpression<Float>
            {
                private readonly IExpression<Vec4> _xyzw;

                public Alpha(IExpression<Vec4> xyzw)
                {
                    _xyzw = xyzw;
                }

                public string Write() => $"{_xyzw.Write()}.a";
            }

            public class Lerp<TVariable> : IExpression<TVariable> where TVariable : IVariable
            {
                private readonly IExpression<TVariable> _a, _b;
                private readonly IExpression<Float> _t;

                public Lerp(IExpression<TVariable> a, IExpression<TVariable> b, IExpression<Float> t)
                {
                    _a = a;
                    _b = b;
                    _t = t;
                }

                public string Write() => $"mix({_a.Write()}, {_b.Write()}, {_t.Write()})";
            }

            //public void Build(out string vertexShader, out string pixelShader)
            //{
            //    var sb = new StringBuilder();
            //    sb.AppendLine(Version);
            //    sb.Append("layout (location = 0) in vec3").Append(Attribute.Position.Name).AppendLine(";");
            //    sb.Append("layout (location = 1) in vec4").Append(Attribute.UV.Name).AppendLine(";");
            //    sb.Append("layout (location = 2) in vec4").Append(Attribute.Color.Name).AppendLine(";");
            //    sb.Append("layout (location = 3) in vec4").Append(Attribute.Overlay.Name).AppendLine(";");
            //    sb.AppendLine($"uniform mat4 {Variables.VertexShader.UniformMVP};");
            //    sb.AppendLine($"uniform vec3 {Variables.VertexShader.UniformCameraUp};");
            //    sb.AppendLine($"uniform vec3 {Variables.VertexShader.UniformCameraSide};");
            //    sb.AppendLine($"uniform float {Variables.VertexShader.UniformFarPlane};");
            //    sb.AppendLine($"out float {Shared.Distance.Name};");
            //    sb.AppendLine($"out vec2 {Shared.UV.Name};");
            //    sb.AppendLine($"out vec4 {Shared.Color.Name};");
            //    sb.AppendLine($"out vec4 {Shared.Overlay.Name};");
            //    sb.AppendLine("void main()");
            //    sb.AppendLine("{");
            //    sb.AppendLine($"vec3 actualPosition = {Attribute.Position.Name} + ({Attribute.UV.Name}.z * {Variables.VertexShader.UniformCameraSide}) + ({Attribute.UV.Name}.w * {Variables.VertexShader.UniformCameraUp});");
            //    sb.AppendLine($"gl_Position = {Variables.VertexShader.UniformMVP} * vec4(actualPosition,1.0);");
            //    sb.AppendLine($"{Shared.Distance.Name} = min(gl_Position.z / {Variables.VertexShader.UniformFarPlane},1);");
            //    sb.AppendLine($"{Shared.UV.Name} = {Attribute.UV.Name}.xy;");
            //    sb.AppendLine($"{Shared.Color.Name} = {Attribute.Color.Name};");
            //    sb.AppendLine($"{Shared.Overlay.Name} = {Attribute.Overlay.Name};");
            //    sb.AppendLine("}");
            //    vertexShader = sb.ToString();

            //    sb.Clear();
            //    sb.AppendLine(Version);
            //    sb.AppendLine($"uniform sampler2D {new Texture().Name};");
            //    sb.AppendLine($"uniform float {Variables.PixelShader.UniformFogIntensity.Name};");
            //    sb.AppendLine($"uniform vec3 {Variables.PixelShader.UniformFogColor.Name};");
            //    sb.AppendLine($"uniform vec4 {Variables.PixelShader.UniformOverlay.Name};");
            //    sb.AppendLine($"in float {Shared.Distance.Name};");
            //    sb.AppendLine($"in vec2 {Shared.UV.Name};");
            //    sb.AppendLine($"in vec4 {Shared.Color.Name};");
            //    sb.AppendLine($"in vec4 {Shared.Overlay.Name};");
            //    sb.AppendLine($"out vec4 {OutputColor.Name};");
            //    sb.AppendLine("void main()");
            //    sb.AppendLine("{");
            //    sb.AppendLine(_pixelShaderBuilder.ToString());
            //    sb.AppendLine("}");
            //    pixelShader = sb.ToString();
            //}
        }

		public class PixelShaderBuilder : ShaderBuilder
		{
			public readonly Float Distance = new Float("share_Distance");
			public readonly Vec2 UV = new Vec2("share_UV");
			public readonly Vec4 Color = new Vec4("share_Color");
			public readonly Vec4 Overlay = new Vec4("share_Overlay");
			public readonly Float FogIntensity = new Float("FogIntensity");
			public readonly Float FogColor = new Float("FogColor");
			public readonly Vec4 GlobalOverlay = new Vec4("Overlay");
			public readonly Vec4 OutputColor = new Vec4("out_Color");
			public readonly Sampler2D Texture = new Sampler2D("Texture");

			public class TextureSample : IExpression<Vec4>
			{
				private readonly Sampler2D _texture;
				private readonly IExpression<Vec2> _uv;

				public TextureSample(Sampler2D texture, IExpression<Vec2> uv)
				{
					_texture = texture;
					_uv = uv;
				}

				public string Write() => $"texture2D({_texture.Write()}, {_uv.Write()})";
			}

			public TextureSample Sample(Sampler2D texture, IExpression<Vec2> uv) => new TextureSample(texture, uv);

			public ShaderBuilder AlphaTest(IExpression<Vec4> color)
			{
				_shaderBuilder.AppendLine($"if ({color.Write()}.a <= 0) discard;");
				return this;
			}

			public override string ToString()
			{
				var sb = new StringBuilder();
				sb.AppendLine(Version);
				sb.AppendLine($"uniform sampler2D {Texture.Name};");
				sb.AppendLine($"uniform float {FogIntensity.Name};");
				sb.AppendLine($"uniform vec3 {FogColor.Name};");
				sb.AppendLine($"uniform vec4 {GlobalOverlay.Name};");
				sb.AppendLine($"in float {Distance.Name};");
				sb.AppendLine($"in vec2 {UV.Name};");
				sb.AppendLine($"in vec4 {Color.Name};");
				sb.AppendLine($"in vec4 {Overlay.Name};");
				sb.AppendLine($"out vec4 {OutputColor.Name};");
				sb.AppendLine("void main()");
				sb.AppendLine("{");
				sb.Append(_shaderBuilder.ToString());
				sb.AppendLine("}");
				return sb.ToString();
			}
		}
	}
}
