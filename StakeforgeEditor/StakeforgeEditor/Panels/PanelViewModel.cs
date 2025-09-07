using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace StakeforgeEditor.Panels
{
	public abstract class PanelViewModel : INotifyPropertyChanged
	{
		private bool _isSelected = false;
		public string Title { get; set; }
		public Common.PanelType Type { get; set; }

		public bool IsSelected
		{
			get => _isSelected;
			set
			{
				if (_isSelected != value)
				{
					_isSelected = value;
					OnPropertyChanged();
				}
			}
		}
		public event PropertyChangedEventHandler? PropertyChanged;

		public PanelViewModel() { Title = "Panel"; }

		protected void OnPropertyChanged([CallerMemberName] string propertyName = "")
		{
			PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
		}

		public abstract void WriteJson(Utf8JsonWriter w);
		public abstract void ReadJson(ref Utf8JsonReader r);
	}
}
