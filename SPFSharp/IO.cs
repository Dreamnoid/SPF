using System.IO;

namespace SPFSharp
{
    public static partial class SPF
	{
		public static class IO
		{
			public static void AddArchive(string filename)
			{
				VirtualFileSystem.AddArchive(filename);
			}

			public static bool TryAddArchive(string filename)
			{
				if (File.Exists(filename))
				{
					VirtualFileSystem.AddArchive(filename);
					return true;
				}
				return false;
			}

			public static void AddArchive(byte[] buffer)
			{
				VirtualFileSystem.AddArchive(buffer);
			}

			public static byte[] ReadFile(string filename)
			{
				return VirtualFileSystem.Read(filename);
			}

			public static bool Exists(string filename)
			{
				return VirtualFileSystem.Exists(filename);
			}

			public static string[] ListArchivedFiles()
			{
				return VirtualFileSystem.ListArchivedFiles();
			}
		}
	}
}
