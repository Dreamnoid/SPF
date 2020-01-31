using System.Linq.Expressions;
using System.Text;

namespace SPFSharp
{
    public static partial class SPF
    {
        public class ShaderBuilder
        {
            public static class Variables
            {
                public const string Version = "#version 330 core";

                public static class VertexShader
                {
                    public const string InPosition = "in_Position";
                    public const string InUV = "in_UV";
                    public const string InColor = "in_Color";
                    public const string InOverlay = "in_Overlay";
                    public const string UniformMVP = "MVP";
                    public const string UniformCameraUp = "CameraUp";
                    public const string UniformCameraSide = "CameraSide";
                    public const string UniformFarPlane = "FarPlane";
                }

                public static class Shared
                {
                    public const string Distance = "share_Distance";
                    public const string UV = "share_UV";
                    public const string Color = "share_Color";
                    public const string Overlay = "share_Overlay";
                }

                public static class PixelShader
                {
                    public static readonly Float UniformFogIntensity = new Float("FogIntensity");
                    public static readonly Float UniformFogColor = new Float("FogColor");
                    public static readonly Float UniformOverlay = new Float("Overlay");
                }
            }

            private readonly StringBuilder _pixelShaderBuilder = new StringBuilder();

            public enum Texture
            {
                Texture0
            }

            public interface IVariable
            {
                string Name { get; }

                string Type { get; }
            }

            public struct Vec2 : IVariable
            {
                public string Name { get; }

                public string Type => "vec2";

                public Vec2(string name)
                {
                    Name = name;
                }
            }

            public struct Vec3 : IVariable
            {
                public string Name { get; }

                public string Type => "vec3";

                public Vec3(string name)
                {
                    Name = name;
                }
            }

            public struct Vec4 : IVariable
            {
                public string Name { get; }

                public string Type => "vec4";

                public Vec4(string name)
                {
                    Name = name;
                }
            }

            public struct Float : IVariable, IExpression<Float>
            {
                public string Name { get; }

                public string Type => "float";

                public Float(string name)
                {
                    Name = name;
                }

                public string Write() => Name;
            }

            public interface IExpression<TVariable> where TVariable : IVariable
            {
                string Write();
            }

            public static readonly Vec4 OutputColor = new Vec4("out_Color");
            public static readonly Float One = new Float("1.0");
            public static readonly Float Zero = new Float("0.0");

            public ShaderBuilder Declaration<TVariable>(TVariable variable, IExpression<TVariable> expression) where TVariable : IVariable
            {
                _pixelShaderBuilder.Append(variable.Type).Append(" ");
                return Assignation(variable, expression);
            }

            public ShaderBuilder Assignation<TVariable>(TVariable variable, IExpression<TVariable> expression) where TVariable : IVariable
            {
                _pixelShaderBuilder.Append(variable.Name).Append(" = ").Append(expression.Write()).AppendLine(";");
                return this;
            }

            public struct Add<TVariable> : IExpression<TVariable> where TVariable : IVariable
            {
                private readonly IExpression<TVariable> _a, _b;

                public Add(IExpression<TVariable> a, IExpression<TVariable> b)
                {
                    _a = a;
                    _b = b;
                }

                public string Write() => $"({_a.Write()} + {_b.Write()})";
            }

            public struct Multiply<TVariable> : IExpression<TVariable> where TVariable : IVariable
            {
                private readonly IExpression<TVariable> _a, _b;

                public Multiply(IExpression<TVariable> a, IExpression<TVariable> b)
                {
                    _a = a;
                    _b = b;
                }

                public string Write() => $"({_a.Write()} * {_b.Write()})";
            }

            public struct TextureSample : IExpression<Vec4>
            {
                private readonly Texture _texture;
                private readonly IExpression<Vec2> _uv;

                public TextureSample(Texture texture, IExpression<Vec2> uv)
                {
                    _texture = texture;
                    _uv = uv;
                }

                public string Write() => $"texture2D({TextureToString(_texture)}, {_uv.Write()})";

                private string TextureToString(Texture texture)
                {
                    switch (texture)
                    {
                        case Texture.Texture0: return "Texture";
                        default: return "Texture";
                    }
                }
            }

            public ShaderBuilder AlphaTest(IExpression<Vec4> color)
            {
                _pixelShaderBuilder.AppendLine($"if ({color.Write()}.a <= 0) discard;");
                return this;
            }

            public struct Vec3ToVec4 : IExpression<Vec4>
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

            public struct Vec4ToVec3 : IExpression<Vec3>
            {
                private readonly IExpression<Vec4> _xyzw;

                public Vec4ToVec3(IExpression<Vec4> xyzw)
                {
                    _xyzw = xyzw;
                }

                public string Write() => $"{_xyzw.Write()}.xyz";
            }

            public struct Alpha : IExpression<Float>
            {
                private readonly IExpression<Vec4> _xyzw;

                public Alpha(IExpression<Vec4> xyzw)
                {
                    _xyzw = xyzw;
                }

                public string Write() => $"{_xyzw.Write()}.a";
            }

            public struct Lerp<TVariable> : IExpression<TVariable> where TVariable : IVariable
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

            public void Build(out string vertexShader, out string pixelShader)
            {
                var sb = new StringBuilder();
                sb.AppendLine(Variables.Version);
                sb.Append("layout (location = 0) in vec3").Append(Variables.VertexShader.InPosition).AppendLine(";");
                sb.Append("layout (location = 1) in vec4").Append(Variables.VertexShader.InUV).AppendLine(";");
                sb.Append("layout (location = 2) in vec4").Append(Variables.VertexShader.InColor).AppendLine(";");
                sb.Append("layout (location = 3) in vec4").Append(Variables.VertexShader.InOverlay).AppendLine(";");
                sb.AppendLine($"uniform mat4 {Variables.VertexShader.UniformMVP};");
                sb.AppendLine($"uniform vec3 {Variables.VertexShader.UniformCameraUp};");
                sb.AppendLine($"uniform vec3 {Variables.VertexShader.UniformCameraSide};");
                sb.AppendLine($"uniform float {Variables.VertexShader.UniformFarPlane};");
                sb.AppendLine($"out float {Variables.Shared.Distance};");
                sb.AppendLine($"out vec2 {Variables.Shared.UV};");
                sb.AppendLine($"out vec4 {Variables.Shared.Color};");
                sb.AppendLine($"out vec4 {Variables.Shared.Overlay};");
                sb.AppendLine("void main()");
                sb.AppendLine("{");
                sb.AppendLine($"vec3 actualPosition = {Variables.VertexShader.InPosition} + ({Variables.VertexShader.InUV}.z * {Variables.VertexShader.UniformCameraSide}) + ({Variables.VertexShader.InUV}.w * {Variables.VertexShader.UniformCameraUp});");
                sb.AppendLine($"gl_Position = {Variables.VertexShader.UniformMVP} * vec4(actualPosition,1.0);");
                sb.AppendLine($"{Variables.Shared.Distance} = min(gl_Position.z / {Variables.VertexShader.UniformFarPlane},1);");
                sb.AppendLine($"{Variables.Shared.UV} = {Variables.VertexShader.InUV}.xy;");
                sb.AppendLine($"{Variables.Shared.Color} = {Variables.VertexShader.InColor};");
                sb.AppendLine($"{Variables.Shared.Overlay} = {Variables.VertexShader.InOverlay};");
                sb.AppendLine("}");
                vertexShader = sb.ToString();

                sb.Clear();
                sb.AppendLine(Variables.Version);
                sb.AppendLine($"uniform sampler2D {Variables.PixelShader.UniformTexture};");
                sb.AppendLine($"uniform float {Variables.PixelShader.UniformFogIntensity};");
                sb.AppendLine($"uniform vec3 {Variables.PixelShader.UniformFogColor};");
                sb.AppendLine($"uniform vec4 {Variables.PixelShader.UniformOverlay};");
                sb.AppendLine($"in float {Variables.Shared.Distance};");
                sb.AppendLine($"in vec2 {Variables.Shared.UV};");
                sb.AppendLine($"in vec4 {Variables.Shared.Color};");
                sb.AppendLine($"in vec4 {Variables.Shared.Overlay};");
                sb.AppendLine($"out vec4 {Variables.PixelShader.OutColor};");
                sb.AppendLine("void main()");
                sb.AppendLine("{");
                sb.AppendLine(_pixelShaderBuilder.ToString());
                sb.AppendLine("}");
                pixelShader = sb.ToString();
            }
        }
    }
}
