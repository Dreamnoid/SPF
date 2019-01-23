﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.IO.Compression;

namespace SPFSharp
{
	internal static class VirtualFileSystem
	{
		private static List<ZipArchive> _archives = new List<ZipArchive>();

		public static void AddArchive(string filename)
		{
			_archives.Add(new ZipArchive(File.OpenRead(filename), ZipArchiveMode.Read, false));
		}

		public static byte[] Read(string filename)
		{
			if (File.Exists(filename))
			{
				return File.ReadAllBytes(filename);
			}

			foreach (var archive in _archives)
			{
				var entry = archive.GetEntry(filename);
				if (entry != null)
				{
					using (var stream = entry.Open())
					{
						using (var ms = new MemoryStream())
						{
							stream.CopyTo(ms);
							return ms.ToArray();
						}
					}
				}
			}

			throw new InvalidOperationException("File not found on disk and archives: " + filename);
		}

		public static void CloseAll()
		{
			foreach (var archive in _archives)
			{
				archive.Dispose();
			}
			_archives.Clear();
		}
	}
}
