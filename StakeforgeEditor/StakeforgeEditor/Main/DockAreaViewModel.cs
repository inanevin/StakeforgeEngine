using StakeforgeEditor.Commands;
using StakeforgeEditor.Panels;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Threading;

namespace StakeforgeEditor.Main
{

	public class DockAreaViewModel : INotifyPropertyChanged
	{
		public ICommand RemovePanelCommand { get; }
		public ICommand AddPanelCommand { get; }
		public ObservableCollection<PanelViewModel> Panels { get; } = [];

		private PanelViewModel? _selectedPanel;
		public PanelViewModel? SelectedPanel
		{
			get => _selectedPanel;
			set
			{
				if (_selectedPanel != value)
				{
					if (_selectedPanel != null)
						_selectedPanel.IsSelected = false;

					_selectedPanel = value;

					if (_selectedPanel != null)
						_selectedPanel.IsSelected = true;

					OnPropertyChanged();
				}
			}
		}



		public DockAreaViewModel()
		{
			RemovePanelCommand = new RelayCommand(RemovePanel);
			AddPanelCommand = new RelayCommand(AddPanelFromMenu);
			_selectedPanel = null;
			SelectedPanel = null;
		}

		public event PropertyChangedEventHandler? PropertyChanged;
		protected void OnPropertyChanged([CallerMemberName] string propertyName = "")
		{
			PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
		}



		public void AddPanelFromMenu(object parameter)
		{
			Common.PanelType panelType = parameter.ToString() switch
			{
				"Resources" => Common.PanelType.Resources,
				"Entities" => Common.PanelType.Entities,
				"World" => Common.PanelType.World,
				"Console" => Common.PanelType.Console,
				"Properties" => Common.PanelType.Properties,
				_ => Common.PanelType.Resources,
			};

			switch (panelType)
			{
				case Common.PanelType.Resources:
					AddPanel(StakeforgeEditor.Panels.Panels.Instance.ResourcesViewModel);
					break;
				case Common.PanelType.Properties:
					break;
				case Common.PanelType.Entities:
					AddPanel(StakeforgeEditor.Panels.Panels.Instance.EntitiesViewModel);
					break;
				case Common.PanelType.World:
					break;
				case Common.PanelType.Console:
					AddPanel(StakeforgeEditor.Panels.Panels.Instance.ConsoleViewModel);
					break;
				default:
					break;
			}

		}

		public void RemovePanel(object parameter)
		{
			if (parameter is PanelViewModel panelToRemove)
			{
				if (Panels.Contains(panelToRemove))
				{
					int nextSelected = 0;

					if (_selectedPanel == panelToRemove)
					{
						int index = Panels.IndexOf(_selectedPanel);

						if (Panels.Count > 2)
							nextSelected = Math.Clamp(index + 1 , 0, Panels.Count-2);
					}
					Panels.Remove(panelToRemove);
					SelectedPanel = nextSelected < Panels.Count ? Panels[nextSelected] : null;
				}
			}
		}

		public void AddPanel(PanelViewModel panel)
		{
			Panels.Add(panel);
			SelectedPanel = panel;
		}

		public void MovePanel(PanelViewModel panelToMove, int newIndex)
		{
			if (panelToMove == null || newIndex < 0 || newIndex >= Panels.Count)
			{
				return;
			}

			int oldIndex = Panels.IndexOf(panelToMove);
			if (oldIndex == -1 || oldIndex == newIndex)
			{
				return;
			}

			// Store a reference to the selected panel
			PanelViewModel currentSelection = this.SelectedPanel!;
			Panels.RemoveAt(oldIndex);
			Panels.Insert(newIndex, panelToMove);
			SelectedPanel = panelToMove;

		}
	}
}
