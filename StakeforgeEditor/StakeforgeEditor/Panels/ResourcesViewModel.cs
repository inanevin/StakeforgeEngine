using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using System.Windows.Input;
using System.Windows.Shapes;

namespace StakeforgeEditor.Panels
{

	public class ResourcesViewModel : PanelViewModel
	{
		public ObservableCollection<ResourceNode> Roots { get; } = new();

		public ResourcesViewModel()
		{
			Type = Common.PanelType.Resources;
			Title = "Resources";

			
		}
		public void Init()
		{
			var project = Editor.Editor.Instance.Settings.LastProjectFolder;
			var resourcesPath = System.IO.Path.Combine(project, "Resources");
			if (Directory.Exists(resourcesPath))
			{
				Roots.Add(BuildFolderNode(resourcesPath));
				OnPropertyChanged();
			}
		}

		FolderNode BuildFolderNode(string path)
		{
			var node = new FolderNode { Name = System.IO.Path.GetFileName(path), FullPath = path };
			node.Children.Add(new FolderNode { Name = "AQQ", FullPath = "", });
			node.Children.Add(new FileNode { Name = "Test2", FullPath = "", ResourceType = Common.ResourceType.Texture });
			node.Children.Add(new FileNode { Name = "Test2", FullPath = "", });
			node.Children.Add(new FileNode { Name = "Test2", FullPath = "", });
			node.Children.Add(new FileNode { Name = "Test2", FullPath = "", });
			node.Children.Add(new FileNode { Name = "Test2", FullPath = "", });
			node.Children.Add(new FileNode { Name = "Test2", FullPath = "", });
			node.Children.Add(new FileNode { Name = "Test2", FullPath = "", });
			node.Children.Add(new FileNode { Name = "Test2", FullPath = "", });
			node.Children.Add(new FileNode { Name = "Test2", FullPath = "", });
			return node;
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
