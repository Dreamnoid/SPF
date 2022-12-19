namespace SPFSharp
{
    public static partial class SPF
    {
        public static class Clipboard
        {
            public static void SetText(string text) => Native.Window.SPF_SetClipboard(text);

            public static string GetText() => Native.Window.SPF_GetClipboard().MarshalToString();
        }
    }
}
