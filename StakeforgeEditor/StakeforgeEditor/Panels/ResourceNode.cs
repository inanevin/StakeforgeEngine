using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;

namespace StakeforgeEditor.Panels
{
	public abstract class ResourceNode : INotifyPropertyChanged
	{
		public string Name { get => _name; set { _name = value; OnPropertyChanged(); } }
		public string FullPath { get; init; } = "";
	
		protected string _name = "";
		public abstract bool IsFolder { get; }
		public virtual ObservableCollection<ResourceNode>? Children => null;

		public event PropertyChangedEventHandler? PropertyChanged;
		protected void OnPropertyChanged([CallerMemberName] string? m = null)
			=> PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(m));
	}

	public sealed class FolderNode : ResourceNode
	{
		public override bool IsFolder => true;
		public override ObservableCollection<ResourceNode> Children { get; } = new();
	}

	public sealed class FileNode : ResourceNode
	{
		public override bool IsFolder => false;
		public Common.ResourceType ResourceType { get => _type; set { _type = value; OnPropertyChanged(); } }

		private Common.ResourceType _type = Common.ResourceType.Audio;
	
	}
}
