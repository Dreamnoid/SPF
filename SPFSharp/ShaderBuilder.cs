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
                    public const string UniformTexture = "Texture";
                    public const string UniformFogIntensity = "FogIntensity";
                    public const string UniformFogColor = "FogColor";
                    public const string UniformOverlay = "Overlay";
                    public const string OutColor = "out_Color";
                }
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
                sb.AppendLine($"vec4 texColor = texture2D({Variables.PixelShader.UniformTexture}, {Variables.Shared.UV}) * {Variables.Shared.Color};");
                sb.AppendLine($"if (texColor.a <= 0) discard;");
                sb.AppendLine($"{Variables.PixelShader.OutColor} = mix(texColor, vec4({Variables.Shared.Overlay}.xyz, texColor.a), {Variables.Shared.Overlay}.a);");
                sb.AppendLine($"{Variables.PixelShader.OutColor} = mix({Variables.PixelShader.OutColor}, vec4({Variables.PixelShader.UniformFogColor},texColor.a), {Variables.PixelShader.UniformFogIntensity} * {Variables.Shared.Distance});");
                sb.AppendLine($"{Variables.PixelShader.OutColor} = mix({Variables.PixelShader.OutColor}, vec4({Variables.PixelShader.UniformOverlay}.xyz, 1.0), {Variables.PixelShader.UniformOverlay}.a);");
                sb.AppendLine("}");
                pixelShader = sb.ToString();
            }
        }
    }
}
