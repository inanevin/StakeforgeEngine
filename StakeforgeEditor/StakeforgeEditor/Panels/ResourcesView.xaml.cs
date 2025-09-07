using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace StakeforgeEditor.Panels
{
	/// <summary>
	/// Interaction logic for ResourcesView.xaml
	/// </summary>
	public partial class ResourcesView : UserControl
	{
		public ResourcesView()
		{
			InitializeComponent();
			Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			ResourcesViewModel? vm = this.DataContext as ResourcesViewModel;
			Debug.Assert(vm != null);
			vm.Init();
		}

		private void Expander_MouseDown(object sender, MouseButtonEventArgs e)
		{
			var textBlock = sender as TextBlock;
			if (textBlock != null)
			{
				var treeViewItem = Common.ObjectUtil.FindParent<TreeViewItem>(textBlock);

				if (treeViewItem != null)
				{
					treeViewItem.IsExpanded = !treeViewItem.IsExpanded;
					e.Handled = true; 
				}
			}
		}
    }
}
