using System.Configuration;
using System.Data;
using System.Windows;

namespace StakeforgeEditor
{
	/// <summary>
	/// Interaction logic for App.xaml
	/// </summary>
	public partial class App : Application
	{
		private StakeforgeEditor.Editor.Editor? _editor;
		protected override void OnStartup(StartupEventArgs e)
		{
			base.OnStartup(e);
			_editor = new StakeforgeEditor.Editor.Editor();
			
		}

		protected override void OnExit(ExitEventArgs e)
		{
			_editor?.Dispose(); // deterministic save
			base.OnExit(e);
		}
	}

}
