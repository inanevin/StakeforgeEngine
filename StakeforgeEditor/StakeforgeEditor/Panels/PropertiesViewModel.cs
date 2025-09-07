using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace StakeforgeEditor.Panels
{
	public class PropertiesViewModel : PanelViewModel
	{
		public PropertiesViewModel()
		{
			Title = "Properties";
			Type = Common.PanelType.Properties;
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
