using StakeforgeEditor.Panels;
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
	public class ProjectSettings
	{
		public string Name { get; set; }
		public string Path { get; set; }

		public ProjectSettings()
		{
			Name = "None";
			Path = "";
		}

		[JsonIgnore]
		private static readonly JsonSerializerOptions JsonOpts = new()
		{
			WriteIndented = true,
			AllowTrailingCommas = true,
			ReadCommentHandling = JsonCommentHandling.Skip
		};

		public static void Save(ProjectSettings settings)
		{
			EditorPaths.EnsureDirectories();
			var json = JsonSerializer.Serialize(settings, JsonOpts);
			File.WriteAllText(settings.Path, json);
		}

		public static ProjectSettings Load( string path)
		{
			if (!File.Exists(path))
			{
				return new ProjectSettings();
			}

			try
			{
				var json = File.ReadAllText(path);
				var loaded = JsonSerializer.Deserialize<ProjectSettings>(json, JsonOpts);

				if (loaded != null)
					return loaded;
			}
			catch
			{
				ConsoleViewModel.Loge("Failed loading project! " + path);
			}

			return new ProjectSettings();
		}
	}
}
