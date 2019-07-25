using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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

			public static byte[] ReadFile(string filename)
			{
				return VirtualFileSystem.Read(filename);
			}

			public static string[] ListArchivedFiles()
			{
				return VirtualFileSystem.ListArchivedFiles();
			}
		}
	}
}
