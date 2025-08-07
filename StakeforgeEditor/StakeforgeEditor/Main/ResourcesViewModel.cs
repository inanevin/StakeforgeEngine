using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StakeforgeEditor.Main
{
	public class ResourcesViewModel : PanelViewModel
	{
		public object? SelectedObject { get; set; }

		public ResourcesViewModel() { Title = "Resources"; }

	}
}
