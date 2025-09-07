using Microsoft.Win32;
using StakeforgeEditor.Common;
using StakeforgeEditor.Engine;
using StakeforgeEditor.Main;
using StakeforgeEditor.Panels;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace StakeforgeEditor.Editor
{
	public class Editor : IDisposable
	{
		private bool _disposed;
		private static Editor? _instance;
		public EditorLayout Layout { get; private set; }
		public EditorSettings Settings { get; private set; }
		public ProjectSettings Project { get; private set; }
		public ConnectionManager Connection { get; private set; }

		public static Editor Instance { get { return _instance!; } }


		public Editor()
		{
			_instance = this;

			EditorPaths.EnsureSettingsPath();
			Settings = EditorSettings.Load(EditorPaths.SettingsPath);
		}

		public void Dispose()
		{
			if (_disposed) return;
			_disposed = true;

			GC.SuppressFinalize(this);
		}

		public void PostInit()
		{
			EditorPaths.EnsureLayoutPath();
			Layout = EditorLayout.Load(EditorPaths.LayoutPath);

			Project = new ProjectSettings();

			EnsureEngineHostPath();
			EnsureProject();

			Connection = new ConnectionManager();
			Connection.StartEngineProcess();
		}

		public void Shutdown()
		{
			Connection.StopEngineProcess();
			SaveSettings();
			SaveLayout();
			SaveProject();
			Application.Current.MainWindow.Close();
		}

		public void SelectEngineServerPath()
		{
			Settings.EngineHostExePath = "";
			EnsureEngineHostPath();
		}

		public void SaveSettings()
		{
			EditorSettings.Save(EditorPaths.SettingsPath, Settings);
		}

		public void SaveLayout()
		{
			EditorLayout.Save(EditorPaths.LayoutPath, Layout);
		}

		public void CreateProject()
		{
			var dlg = new SaveFileDialog
			{
				Title = "Save New Project",
				Filter = "Stakeforge Project (*.stkproject)|*.stkproject",
			};
			var result = dlg.ShowDialog() ?? false;
			if (!result)
			{
				return;
			}

			ProjectSettings settings = new ProjectSettings();
			settings.Path = dlg.FileName;
			settings.Name = dlg.SafeFileName.Substring(0, dlg.SafeFileName.IndexOf('.'));
			ProjectSettings.Save(settings);
			ConsoleViewModel.Logt("Created new project: " + settings.Path);

			LoadProject(dlg.FileName);
		}

		public void OpenProject()
		{
			var dlg = new OpenFileDialog
			{
				Title = "Select Stakeforge Host Executable",
				Filter = "Stakeforge Project (*.stkproject)|*.stkproject",
				CheckFileExists = true
			};

			var result = dlg.ShowDialog() ?? false;
			if (!result)
			{
				return;
			}

			LoadProject(dlg.FileName);
		}

		public void LoadProject(string path)
		{
			if (!File.Exists(path))
			{
				ConsoleViewModel.Loge("Failed loading project as file does not exists! " + path);
				return;
			}

			// Cleanup current.
			Project = ProjectSettings.Load(path);
			Settings.LastProjectPath = Project.Path;
			string? folder = System.IO.Path.GetDirectoryName(Project.Path);
			Debug.Assert(folder != null);
			Settings.LastProjectFolder = folder;
			SaveSettings();

			ConsoleViewModel.Logp("Loaded project: " + path);
			Panels.Panels.Instance.TopBarViewModel.ProjectName = Project.Path;
		}

		public void SaveProject()
		{
			if (string.IsNullOrEmpty(Project.Path))
				return;

			ProjectSettings.Save(Project);
			ConsoleViewModel.Logt("Saving project: " + Project.Path);
		}

		private void EnsureEngineHostPath()
		{
			if (string.IsNullOrWhiteSpace(Settings.EngineHostExePath) || !File.Exists(Settings.EngineHostExePath))
			{
				var dlg = new OpenFileDialog
				{
					Title = "Select Stakeforge Host Executable",
					Filter = "Executable (*.exe)|*.exe",
					CheckFileExists = true
				};

				var result = dlg.ShowDialog() ?? false;
				if (!result)
				{
					return;
				}

				Settings.EngineHostExePath = dlg.FileName;
				SaveSettings();
			}

			Panels.Panels.Instance.TopBarViewModel.ServerPath = Settings.EngineHostExePath;
		}

		private void EnsureProject()
		{
			if (File.Exists(Settings.LastProjectPath))
			{
				LoadProject(Settings.LastProjectPath);
			}
		}


	}
}
