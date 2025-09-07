using StakeforgeEditor.Common;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StakeforgeEditor.Editor
{
	public static class EditorPaths
	{
		// %LOCALAPPDATA%\Stakeforge\Editor
		public static readonly string BaseDir =
			Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "Stakeforge", "Editor");

		public static readonly string SettingsPath = Path.Combine(BaseDir, "EditorSettings.stkforge");
		public static readonly string LayoutPath = Path.Combine(BaseDir, "EditorLayout.stkforge");

		public static void EnsureSettingsPath()
		{
			Directory.CreateDirectory(BaseDir);

			if(!File.Exists(SettingsPath))
			{
				EditorSettings empty = new EditorSettings();
				EditorSettings.Save(SettingsPath, empty);
			}

		}
		
		public static void EnsureLayoutPath()
		{
			if (!File.Exists(LayoutPath))
			{
				EditorLayout empty = new EditorLayout();
				EditorLayout.Save(LayoutPath, empty);
			}
		}
	}
}
