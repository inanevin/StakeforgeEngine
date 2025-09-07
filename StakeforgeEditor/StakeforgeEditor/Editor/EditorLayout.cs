using StakeforgeEditor.Common;
using StakeforgeEditor.Main;
using StakeforgeEditor.Panels;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace StakeforgeEditor.Editor
{
	public sealed class EditorLayout
	{
		public DockContainer MainDockContainer { get; private set; }

		public EditorLayout()
		{
			MainDockContainer = new DockContainer();
		}

		public static EditorLayout Load(string path)
		{
			byte[] jsonBytes = File.ReadAllBytes(path);
			var reader = new Utf8JsonReader(jsonBytes);

			if (!reader.Read() || reader.TokenType != JsonTokenType.StartObject)
				throw new JsonException("Root must be an object.");

			EditorLayout layout = new EditorLayout();
			MainWindow? mw = App.Current.MainWindow as MainWindow;
			Debug.Assert(mw != null);
			mw.ReadJson(ref reader);
			return layout;
		}

		public static void Save(string path, EditorLayout layout)
		{
			using var fs = new FileStream(path, FileMode.Create, FileAccess.Write, FileShare.None);
			using var w = new Utf8JsonWriter(fs, new JsonWriterOptions { Indented = true });
			MainWindow? mw = App.Current.MainWindow as MainWindow;
			Debug.Assert(mw != null);
			mw.WriteJson(w);
			w.Flush();
		}



	}
}
