using StakeforgeEditor.Editor;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace StakeforgeEditor.Common
{
	public interface IJsonStreamable
	{
		[JsonIgnore]
		private static readonly JsonSerializerOptions JsonOpts = new()
		{
			WriteIndented = true,
			AllowTrailingCommas = true,
			ReadCommentHandling = JsonCommentHandling.Skip
		};

		/*
		public void Load(string path)
		{
			byte[] jsonBytes = File.ReadAllBytes(path);
			var reader = new Utf8JsonReader(jsonBytes);

			if (!reader.Read() || reader.TokenType != JsonTokenType.StartObject)
				throw new JsonException("Root must be an object.");

			ReadJson(ref reader);
		}

		public void Save(string path)
		{
			using var fs = new FileStream(path, FileMode.Create, FileAccess.Write, FileShare.None);
			using var w = new Utf8JsonWriter(fs, new JsonWriterOptions { Indented = true });
			WriteJson(w);
			w.Flush();
		}*/

		void WriteJson(Utf8JsonWriter w);
		void ReadJson(ref Utf8JsonReader r);
	}
}
