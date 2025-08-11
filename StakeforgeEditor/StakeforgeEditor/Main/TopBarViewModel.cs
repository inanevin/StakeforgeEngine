using StakeforgeEditor.Editor;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace StakeforgeEditor.Main
{
	class TopBarViewModel : INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler? PropertyChanged;

		private string _projectName = "NoProject";
		private SolidColorBrush _connectionBrush = new SolidColorBrush(new Color());
		private string _connectionStatus = "";
		private string _connectionIcon = "";
		private string _serverPath = "";

		public string ServerPath
		{
			get => _serverPath;
			set
			{
				if (_serverPath == value) return;
				_serverPath = value;
				OnPropertyChanged();
			}
		}
		public SolidColorBrush ConnectionBrush
		{
			get => _connectionBrush;
			set
			{
				if (_connectionBrush == value) return;
				_connectionBrush = value;
				OnPropertyChanged();
			}
		}

		public string ProjectName
		{
			get => _projectName;
			set
			{
				if (_projectName == value)
					return;

				_projectName = value;
				OnPropertyChanged();
			}
		}

		public string ConnectionIcon
		{
			get => _connectionIcon;
			set
			{
				if (_connectionIcon == value) return;
				_connectionIcon = value;
				OnPropertyChanged();
			}
		}


		public string ConnectionStatus
		{
			get => _connectionStatus;
			set
			{
				if (_connectionStatus == value) return;
				_connectionStatus = value;
				OnPropertyChanged();
			}
		}

		public TopBarViewModel()
		{
			UpdateServerConnectionStatus(EngineServerStatus.NotConnected);
		}

		protected void OnPropertyChanged([CallerMemberName] string propertyName = "")
		{
			PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
		}

		public void UpdateServerConnectionStatus(EngineServerStatus status)
		{
			if (Application.Current == null)
				return;

			SolidColorBrush? brush = null;
			string? icon = null;

			switch (status)
			{
				case EngineServerStatus.NotConnected:
					ConnectionStatus = "Engine Server: Not Connected";
					brush = Application.Current.Resources["brush_accent_error"] as SolidColorBrush;
					icon = Application.Current.Resources["str_icon_plug_error"] as string;
					break;
				case EngineServerStatus.ConnectedNoPipe:
					ConnectionStatus = "Engine Server: Connected + No Pipe";
					brush = Application.Current.Resources["brush_accent_warn"] as SolidColorBrush;
					icon = Application.Current.Resources["str_icon_plug_warn"] as string;

					break;
				case EngineServerStatus.ConnectedWPipe:
					ConnectionStatus = "Engine Server: Connected + Piped";
					brush = Application.Current.Resources["brush_accent_green"] as SolidColorBrush;
					icon = Application.Current.Resources["str_icon_plug_check"] as string;
					break;
			}
			Debug.Assert(brush != null);
			Debug.Assert(icon != null);
			ConnectionBrush = brush;
			ConnectionIcon = icon;
		}


	}
}
