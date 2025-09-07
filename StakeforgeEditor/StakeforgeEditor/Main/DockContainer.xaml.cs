using StakeforgeEditor.Common;
using StakeforgeEditor.Panels;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
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

	public partial class DockContainer : UserControl, IJsonStreamable
	{
		private DockArea? _targetArea = null;
		private DockContainer? _container0 = null;
		private DockContainer? _container1 = null;
		private Common.Direction _splitDirection = Common.Direction.None;
		private double _minSize = 0;

		public Common.Direction SplitDirection { get => _splitDirection; }
		public DockContainer? Container0 { get => _container0; }
		public DockContainer? Container1 { get => _container1; }
		public DockArea? TargetArea { get => _targetArea; }
		public double StoredRatio0 { get; set; }
		public double StoredRatio1 { get; set; }
		public DockContainer()
		{
			InitializeComponent();

			_minSize = (double)Application.Current.Resources["size_min_dock_area"];
			RootGrid.ColumnDefinitions[0].Width = new GridLength(1, GridUnitType.Star);
			RootGrid.RowDefinitions[0].Height = new GridLength(1, GridUnitType.Star);
		}


		public void Kill()
		{
			DockContainer? parent = Common.ObjectUtil.FindParent<DockContainer>(this);

			if (parent == null)
			{
				SubWindow? sub = Common.ObjectUtil.FindParent<SubWindow>(this);
				Debug.Assert(sub != null);
				sub.CloseWindow();
				return;
			}

			RootGrid.Children.Clear();
			parent.UninitContainer(this);
		}

		public bool CanKill()
		{
			DockContainer? parent = Common.ObjectUtil.FindParent<DockContainer>(this);

			if (parent == null)
			{
				MainWindow? main = Common.ObjectUtil.FindParent<MainWindow>(this);
				return main == null;
			}

			return true;
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
				_splitDirection = Common.Direction.None;
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
			_splitDirection = Common.Direction.None;


		}

		private void Split(Common.Direction direction)
		{
			Debug.Assert(_container0 == null && _container1 == null);
			Debug.Assert(direction != Common.Direction.None && direction != Common.Direction.Center);
			_splitDirection = direction;

			_container0 = new DockContainer();
			_container1 = new DockContainer();

			GridSplitter splitter = new GridSplitter
			{
				Background = Application.Current.FindResource("brush_background1") as SolidColorBrush,
				HorizontalAlignment = HorizontalAlignment.Stretch,
				VerticalAlignment = VerticalAlignment.Stretch,
				ShowsPreview = false
			};
			splitter.DragDelta += OnSplitterDragDelta;

			if (direction == Common.Direction.Right || direction == Common.Direction.Left)
			{
				RootGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(5, GridUnitType.Pixel) });
				RootGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });

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
				RootGrid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(1, GridUnitType.Star) });


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
		public void SplitArea(Common.Direction direction, PanelViewModel panel)
		{
			Debug.Assert(_container0 == null && _container1 == null && _targetArea != null);
			Split(direction);
			Debug.Assert(_container0 != null && _container1 != null);

			// This will be added to new split.
			DockArea newArea = new DockArea();
			DockAreaViewModel newAreaVM = new DockAreaViewModel();
			newArea.DataContext = newAreaVM;
			newAreaVM.AddPanel(panel);

			DockArea existingArea = _targetArea;
			RootGrid.Children.Remove(_targetArea);
			_targetArea = null;

			if (direction == Common.Direction.Left)
			{
				_container0.InitArea(newArea);
				_container1.InitArea(existingArea);
			}
			else if (direction == Common.Direction.Right)
			{
				_container0.InitArea(existingArea);
				_container1.InitArea(newArea);
			}
			else if (direction == Common.Direction.Bottom)
			{
				_container0.InitArea(existingArea);
				_container1.InitArea(newArea);
			}
			else if (direction == Common.Direction.Top)
			{
				_container0.InitArea(newArea);
				_container1.InitArea(existingArea);
			}
		}

		private void OnSplitterDragDelta(object sender, DragDeltaEventArgs e)
		{
			return;
			GridSplitter? splitter = sender as GridSplitter;
			if (splitter == null) return;

			Debug.Assert(_container0 != null && _container1 != null);

			if (_splitDirection == Common.Direction.Left || _splitDirection == Common.Direction.Right)
			{
				ColumnDefinition col0 = RootGrid.ColumnDefinitions[0];
				ColumnDefinition col2 = RootGrid.ColumnDefinitions[2];

				double minSize0 = _container0.GetMinimumSize();
				double minSize1 = _container1.GetMinimumSize();

				double newWidth0 = col0.ActualWidth + e.HorizontalChange;
				double newWidth1 = col2.ActualWidth - e.HorizontalChange;

				if (newWidth0 < minSize0)
				{
					col0.Width = new GridLength(minSize0);
					col2.Width = new GridLength(col0.ActualWidth + col2.ActualWidth - minSize0);
					e.Handled = true; 
					return;
				}

				if (newWidth1 < minSize1)
				{
					col2.Width = new GridLength(minSize1);
					col0.Width = new GridLength(col0.ActualWidth + col2.ActualWidth - minSize1);
					e.Handled = true; 
					return;
				}
			}
			else 
			{
				RowDefinition row0 = RootGrid.RowDefinitions[0];
				RowDefinition row2 = RootGrid.RowDefinitions[2];

				double minSize0 = _container0.GetMinimumSize();
				double minSize1 = _container1.GetMinimumSize();

				double newHeight0 = row0.ActualHeight + e.VerticalChange;
				double newHeight1 = row2.ActualHeight - e.VerticalChange;

				if (newHeight0 < minSize0)
				{
					row0.Height = new GridLength(minSize0);
					row2.Height = new GridLength(row0.ActualHeight + row2.ActualHeight - minSize0);
					e.Handled = true;
					return;
				}

				if (newHeight1 < minSize1)
				{
					row2.Height = new GridLength(minSize1);
					row0.Height = new GridLength(row0.ActualHeight + row2.ActualHeight - minSize1);
					e.Handled = true;
					return;
				}
			}
		}

		public double GetMinimumSize()
		{
			if (_targetArea != null)
			{
				return _minSize;
			}
			else if (_container0 != null && _container1 != null)
			{
				// Container mode: Recursively get the minimum size of children and add splitter size.
				double splitterSize = 5;
				if (SplitDirection == Common.Direction.Left || SplitDirection == Common.Direction.Right)
				{
					return _container0.GetMinimumSize() + splitterSize + _container1.GetMinimumSize();
				}
				else
				{
					return _container0.GetMinimumSize() + splitterSize + _container1.GetMinimumSize();
				}
			}
			return 0;
		}

		public void WriteJson(Utf8JsonWriter w)
		{
			w.WriteString("split_direction", _splitDirection.ToString());

			if (_splitDirection == Common.Direction.None)
			{
				Debug.Assert(_targetArea != null);
				DockAreaViewModel? vm = _targetArea.DataContext as DockAreaViewModel;
				Debug.Assert(vm != null);

				w.WriteStartArray("panels");

				foreach (PanelViewModel pw in vm.Panels)
				{
					w.WriteStartObject();
					w.WriteString("panel_type", pw.Type.ToString());
					pw.WriteJson(w);
					w.WriteEndObject();
				}

				w.WriteEndArray();
			}
			else
			{

				GridSplitter? splitter = RootGrid.Children[1] as GridSplitter;
				Debug.Assert(splitter != null);

				Debug.Assert(_container0 != null && _container1 != null);

				if (_splitDirection == Common.Direction.Left || _splitDirection == Common.Direction.Right)
					w.WriteNumber("ratio0", RootGrid.ColumnDefinitions[0].Width.Value);
				else if (_splitDirection == Common.Direction.Top || _splitDirection == Common.Direction.Bottom)
					w.WriteNumber("ratio1", RootGrid.RowDefinitions[0].Height.Value);

				Debug.Assert(_container0 != null && _container1 != null);

				w.WriteStartObject("container0");
				_container0.WriteJson(w);
				w.WriteEndObject();

				w.WriteStartObject("container1");
				_container1.WriteJson(w);
				w.WriteEndObject();
			}

		}

		public void ReadJson(ref Utf8JsonReader r)
		{
			while (r.Read() && r.TokenType != JsonTokenType.EndObject)
			{
				if (r.TokenType == JsonTokenType.PropertyName)
				{
					string propertyName = r.GetString() ?? "";
					r.Read(); // Advance to the value

					switch (propertyName)
					{
						case "split_direction":
							// Set the split direction based on the string value
							string val = r.GetString() ?? "None";
							Enum.TryParse(val, out _splitDirection);

							if (_splitDirection != Common.Direction.None)
								Split(_splitDirection);
							break;

						case "panels":
							{
								var area = new DockArea();
								var vm = new DockAreaViewModel();
								area.DataContext = vm;

								if (r.TokenType != JsonTokenType.StartArray)
									throw new JsonException("Expected StartArray for 'panels'.");

								while (r.Read() && r.TokenType != JsonTokenType.EndArray)
								{
									if (r.TokenType != JsonTokenType.StartObject)
										throw new JsonException("Expected StartObject for panel item.");

									if (!r.Read() || r.TokenType != JsonTokenType.PropertyName || r.GetString() != "panel_type")
										throw new JsonException("First field in a panel object must be 'panel_type'.");

									if (!r.Read()) throw new JsonException("Missing value for 'panel_type'.");

									string typeStr = r.GetString() ?? nameof(PanelType.Resources);
									PanelType type = PanelType.Resources;
									Enum.TryParse(typeStr, out type);

									PanelViewModel pvm = Panels.Panels.Instance.GetPanelByType(type);

									pvm.ReadJson(ref r);
									vm.AddPanel(pvm);
								}

								if (_targetArea != null)
								{
									RootGrid.Children.Remove(_targetArea);
									_targetArea = null;
								}

								InitArea(area);
								break;
							}

						case "ratio0":
							double ratio0 = r.GetDouble();
							RootGrid.ColumnDefinitions[0].Width = new GridLength(ratio0, GridUnitType.Pixel);
							break;

						case "ratio1":
							double ratio1 = r.GetDouble();
							RootGrid.RowDefinitions[0].Height = new GridLength(ratio1, GridUnitType.Pixel);
							break;

						case "container0":
							if (r.TokenType == JsonTokenType.StartObject)
							{
								Debug.Assert(_container0 != null);
								_container0.ReadJson(ref r);
							}
							break;
						case "container1":
							if (r.TokenType == JsonTokenType.StartObject)
							{
								Debug.Assert(_container1 != null);
								_container1.ReadJson(ref r);
							}
							break;
					}
				}
			}
		}
	}
}
