using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace StakeforgeEditor.Panels
{
	public class WorldViewModel : PanelViewModel
	{
		public WorldViewModel() { 
			Title = "World";
			Type = Common.PanelType.World;
		}

		public override void WriteJson(Utf8JsonWriter w)
		{

		}

		public override void ReadJson(ref Utf8JsonReader r)
		{
			while (r.Read() && r.TokenType != JsonTokenType.EndObject)
			{
				if (r.TokenType != JsonTokenType.PropertyName) continue;

				string propertyName = r.GetString() ?? "";
				if (!r.Read()) throw new JsonException();

				switch (propertyName)
				{

					default:
						r.Skip();
						break;
				}
			}
		}
	}
}
