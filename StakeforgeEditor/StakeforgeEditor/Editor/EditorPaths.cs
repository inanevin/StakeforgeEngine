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

		public static void EnsureDirectories()
		{
			Directory.CreateDirectory(BaseDir);
		}
	}
}
