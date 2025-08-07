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
	public enum DockDirection
	{
		Top,
		Bottom,
		Left,
		Right
	}

	public partial class DockContainer : UserControl
	{
		private DockArea? _targetArea = null;
		private DockContainer? _container0 = null;
		private DockContainer? _container1 = null;
		private Common.Direction _splitDirection = Common.Direction.None;
		private double _minSize = 0;

		public DockContainer()
		{
			InitializeComponent();

			_minSize = (double)Application.Current.Resources["size_min_dock_area"];
			RootGrid.ColumnDefinitions[0].MinWidth = _minSize;
			RootGrid.RowDefinitions[0].MinHeight = _minSize;
		}
		public void Kill()
		{
			DockContainer? parent = Common.ObjectUtil.FindParent<DockContainer>(this);
			Debug.Assert(parent != null);
			RootGrid.Children.Clear();
			parent.UninitContainer(this);
		}

		public bool CanKill()
		{
			DockContainer? parent = Common.ObjectUtil.FindParent<DockContainer>(this);
			return parent != null;
		}

		public void UninitContainer(DockContainer container)
		{
			Debug.Assert(_container0 != null && _container1 != null);
			Debug.Assert(container == _container0 || container == _container1);

			RootGrid.Children.Clear();
			RootGrid.ColumnDefinitions.Clear();
			RootGrid.RowDefinitions.Clear();
			RootGrid.ColumnDefinitions.Add(new ColumnDefinition { MinWidth = _minSize, Width = new GridLength(1, GridUnitType.Star) });
			RootGrid.RowDefinitions.Add(new RowDefinition { MinHeight = _minSize, Height = new GridLength(1, GridUnitType.Star) });

			DockContainer safeContainer = container == _container0 ? _container1 : _container0;
			safeContainer.RootGrid.Children.Clear();

			if (safeContainer._targetArea != null)
			{
				_targetArea = safeContainer._targetArea;
				RootGrid.Children.Add(_targetArea);
				Grid.SetRow(_targetArea, 0);
				Grid.SetColumn(_targetArea, 0);
				Grid.SetRowSpan(_targetArea, 1);
				Grid.SetColumnSpan(_targetArea, 1);
				_container0 = null;
				_container1 = null;
			}
			else
			{
				_container0 = safeContainer._container0;
				_container1 = safeContainer._container1;
				_splitDirection = safeContainer._splitDirection;

				GridSplitter splitter = new GridSplitter
				{
					Background = Application.Current.FindResource("brush_background1") as SolidColorBrush,
					HorizontalAlignment = HorizontalAlignment.Stretch,
					VerticalAlignment = VerticalAlignment.Stretch,
					ShowsPreview = false
				};

				if (_splitDirection == Common.Direction.Left || _splitDirection == Common.Direction.Right)
				{
					RootGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(5, GridUnitType.Pixel) });
					RootGrid.ColumnDefinitions.Add(new ColumnDefinition { MinWidth = _minSize, Width = new GridLength(1, GridUnitType.Star) });

					Grid.SetColumn(_container0, 0);
					Grid.SetColumn(splitter, 1);
					Grid.SetColumn(_container1, 2);
					Grid.SetRowSpan(_container0, 1);
					Grid.SetRowSpan(splitter, 1);
					Grid.SetRowSpan(_container1, 1);

					RootGrid.Children.Add(_container0);
					RootGrid.Children.Add(splitter);
					RootGrid.Children.Add(_container1);
				}
				else
				{
					RootGrid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(5, GridUnitType.Pixel) });
					RootGrid.RowDefinitions.Add(new RowDefinition { MinHeight = _minSize, Height = new GridLength(1, GridUnitType.Star) });


					Grid.SetRow(_container0, 0);
					Grid.SetRow(splitter, 1);
					Grid.SetRow(_container1, 2);
					Grid.SetColumnSpan(_container0, 1);
					Grid.SetColumnSpan(splitter, 1);
					Grid.SetColumnSpan(_container1, 1);

					RootGrid.Children.Add(_container0);
					RootGrid.Children.Add(splitter);
					RootGrid.Children.Add(_container1);
				}
			}

		}

		public void InitArea(DockArea area)
		{
			Debug.Assert(_container0 == null && _container1 == null && _targetArea == null);
			_targetArea = area;

			Grid.SetRow(_targetArea, 0);
			Grid.SetColumn(_targetArea, 0);
			RootGrid.Children.Add(_targetArea);
		}

		public void SplitArea(Common.Direction direction, PanelViewModel panel)
		{
			Debug.Assert(_container0 == null && _container1 == null && _targetArea != null);
			Debug.Assert(direction != Common.Direction.None && direction != Common.Direction.Center);
			_splitDirection = direction;

			// This will be added to new split.
			DockArea newArea = new DockArea();
			DockAreaViewModel newAreaVM = new DockAreaViewModel();
			newArea.DataContext = newAreaVM;
			newAreaVM.AddPanel(panel);

			DockArea existingArea = _targetArea;
			RootGrid.Children.Remove(_targetArea);
			_targetArea = null;

			_container0 = new DockContainer();
			_container1 = new DockContainer();

			GridSplitter splitter = new GridSplitter
			{
				Background = Application.Current.FindResource("brush_background1") as SolidColorBrush,
				HorizontalAlignment = HorizontalAlignment.Stretch,
				VerticalAlignment = VerticalAlignment.Stretch,
				ShowsPreview = false
			};

			if (direction == Common.Direction.Right || direction == Common.Direction.Left)
			{
				RootGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(5, GridUnitType.Pixel) });
				RootGrid.ColumnDefinitions.Add(new ColumnDefinition { MinWidth = _minSize, Width = new GridLength(1, GridUnitType.Star) });

				Grid.SetColumn(_container0, 0);
				Grid.SetColumn(splitter, 1);
				Grid.SetColumn(_container1, 2);
				Grid.SetRowSpan(_container0, 1);
				Grid.SetRowSpan(splitter, 1);
				Grid.SetRowSpan(_container1, 1);

				RootGrid.Children.Add(_container0);
				RootGrid.Children.Add(splitter);
				RootGrid.Children.Add(_container1);

				if (direction == Common.Direction.Left)
				{
					_container0.InitArea(newArea);
					_container1.InitArea(existingArea);
				}
				else
				{
					_container0.InitArea(existingArea);
					_container1.InitArea(newArea);
				}

			}
			else
			{
				RootGrid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(5, GridUnitType.Pixel) });
				RootGrid.RowDefinitions.Add(new RowDefinition { MinHeight = _minSize, Height = new GridLength(1, GridUnitType.Star) });


				Grid.SetRow(_container0, 0);
				Grid.SetRow(splitter, 1);
				Grid.SetRow(_container1, 2);
				Grid.SetColumnSpan(_container0, 1);
				Grid.SetColumnSpan(splitter, 1);
				Grid.SetColumnSpan(_container1, 1);

				RootGrid.Children.Add(_container0);
				RootGrid.Children.Add(splitter);
				RootGrid.Children.Add(_container1);

				if (direction == Common.Direction.Bottom)
				{
					_container0.InitArea(existingArea);
					_container1.InitArea(newArea);
				}
				else
				{
					_container0.InitArea(newArea);
					_container1.InitArea(existingArea);
				}
			}
		}
	}
}
