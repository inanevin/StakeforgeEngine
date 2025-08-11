using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace StakeforgeEditor.Editor
{
	public sealed class EditorSettings
	{
		public string? EngineHostExePath { get; set; }
		public string? LastProjectPath { get; set; }

		[JsonIgnore]
		private static readonly JsonSerializerOptions JsonOpts = new()
		{
			WriteIndented = true,
			AllowTrailingCommas = true,
			ReadCommentHandling = JsonCommentHandling.Skip
		};

		public static EditorSettings LoadOrCreate()
		{
			EditorPaths.EnsureDirectories();

			if (!File.Exists(EditorPaths.SettingsPath))
			{
				var defaults = new EditorSettings();
				Save(defaults);
				return defaults;
			}

			try
			{
				var json = File.ReadAllText(EditorPaths.SettingsPath);
				var loaded = JsonSerializer.Deserialize<EditorSettings>(json, JsonOpts);
				return loaded ?? new EditorSettings();
			}
			catch
			{
				// Corrupt? Back it up and recreate.
				try
				{
					var backup = EditorPaths.SettingsPath + ".bak";
					File.Copy(EditorPaths.SettingsPath, backup, overwrite: true);
				}
				catch { /* best-effort */ }

				var defaults = new EditorSettings();
				Save(defaults);
				return defaults;
			}
		}

		public static void Save(EditorSettings settings)
		{
			EditorPaths.EnsureDirectories();
			var json = JsonSerializer.Serialize(settings, JsonOpts);
			File.WriteAllText(EditorPaths.SettingsPath, json);
		}
	}
}
