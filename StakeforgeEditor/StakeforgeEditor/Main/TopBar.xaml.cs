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

namespace StakeforgeEditor.Main
{
	/// <summary>
	/// Interaction logic for top_bar.xaml
	/// </summary>
	public partial class TopBar : UserControl
	{
	
		public TopBar()
		{
			InitializeComponent();
			this.DataContext = Panels.Panels.Instance.TopBarViewModel;
		}

		void OnClickMinimize(object sender, RoutedEventArgs e)
		{

		}

		void OnClickMaximize(object sender, RoutedEventArgs e)
		{

		}

		void OnClickClose(object sender, RoutedEventArgs e)
		{
			Editor.Editor.Instance.Shutdown();
		}

		private void MenuItem_SetEngineServer(object sender, RoutedEventArgs e)
		{
			Editor.Editor.Instance.SelectEngineServerPath();
		}

		private void OnClickContext(object sender, RoutedEventArgs e)
		{
			Button? btn = sender as Button;
			Debug.Assert(btn != null);
			btn.ContextMenu.PlacementTarget = btn;
			btn.ContextMenu.Placement = System.Windows.Controls.Primitives.PlacementMode.Bottom;
			btn.ContextMenu.IsOpen = true;
		}

		private void MenuItem_NewProject(object sender, RoutedEventArgs e)
		{
			Editor.Editor.Instance.CreateProject();

		}

		private void MenuItem_OpenProject(object sender, RoutedEventArgs e)
		{
			Editor.Editor.Instance.OpenProject();

		}

		private void MenuItem_SaveProject(object sender, RoutedEventArgs e)
		{
			Editor.Editor.Instance.SaveProject();
		}

		private void MenuItem_RestartEngineServer(object sender, RoutedEventArgs e)
		{
			Editor.Editor.Instance.Connection.RestartEngineProcess();
		}

		private void MenuItem_KillEngineServer(object sender, RoutedEventArgs e)
		{
			Editor.Editor.Instance.Connection.StopEngineProcess();
		}
	}
}
