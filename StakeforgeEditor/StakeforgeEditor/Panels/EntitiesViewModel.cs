using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StakeforgeEditor.Panels
{
	public class EntitiesViewModel : PanelViewModel
	{
		public object? SelectedObject { get; set; }
		public EntitiesViewModel() { Title = "Hierarchy"; }

	}
}
