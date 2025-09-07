using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.NetworkInformation;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Shapes;

namespace StakeforgeEditor.Editor
{
	public sealed class EditorSettings
	{
		public string EngineHostExePath { get; set; }
		public string LastProjectPath { get; set; }
		public string LastProjectFolder { get; set; }

		public EditorSettings()
		{
			EngineHostExePath = "";
			LastProjectPath = "";
			LastProjectFolder = "";
		}

		[JsonIgnore]
		private static readonly JsonSerializerOptions JsonOpts = new()
		{
			WriteIndented = true,
			AllowTrailingCommas = true,
			ReadCommentHandling = JsonCommentHandling.Skip
		};

		public static EditorSettings Load(string path)
		{
			string json = File.ReadAllText(path);
			EditorSettings? settings = JsonSerializer.Deserialize<EditorSettings>(json, JsonOpts);
			Debug.Assert(settings != null);
			return settings;
		}

		public static void Save(string path, EditorSettings settings)
		{
			string json = JsonSerializer.Serialize<EditorSettings>(settings, JsonOpts);
			File.WriteAllText(path, json);
		}
	}
}
