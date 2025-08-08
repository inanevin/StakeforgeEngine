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
	/// Interaction logic for ConsoleView.xaml
	/// </summary>
	public partial class ConsoleView : UserControl
	{
		public ConsoleView()
		{
			InitializeComponent();
			DataContext = Panels.Instance.ConsoleViewModel;
		}

		private void Filter_Click(object sender, RoutedEventArgs e)
		{
			Button? btn = sender as Button;
			Debug.Assert(btn != null);
			btn.ContextMenu.PlacementTarget = btn;
			btn.ContextMenu.Placement = System.Windows.Controls.Primitives.PlacementMode.Bottom;
			btn.ContextMenu.IsOpen = true;
		}
    }
}
