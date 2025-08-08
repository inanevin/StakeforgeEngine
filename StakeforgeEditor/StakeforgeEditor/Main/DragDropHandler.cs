using StakeforgeEditor.Common;
using StakeforgeEditor.Panels;
using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

namespace StakeforgeEditor.Main
{
	enum DragDropType
	{
		None,
		Panel,
	};

	struct DragDropPanelData
	{
		public DragDropPanelData() { }
		public TabItem? tabItem = null;
		public PanelViewModel? panelViewModel = null;
		public DockArea? senderDockArea = null;

		public void Reset()
		{
			tabItem = null;
			panelViewModel = null;
			senderDockArea = null;
		}
	}

	class DragDropHandler
	{
		private DragDropPreviewWindow _previewWindow = new DragDropPreviewWindow();
		private DragDropPanelData _panelData;
		private static DragDropHandler? _instance = null;
		private Point _cursorPos;
		private List<DockArea> _dockAreas = new List<DockArea>();
		public static DragDropHandler Instance
		{
			get
			{
				if (_instance == null)
					_instance = new DragDropHandler();
				return _instance;
			}
			private set { _instance = value; }
		}
		public DragDropType CurrentDragDrop { get; private set; }
		public DragDropPanelData CurrentPanelData { get => _panelData; set => _panelData = value; }

		public void RegisterDockArea(DockArea area)
		{
			_dockAreas.Add(area);
		}

		public void UnregisterDockArea(DockArea area)
		{
			_dockAreas.Remove(area);
		}
		public void BeginDragPanel(DockArea sender, TabItem tabItem, PanelViewModel viewModel)
		{

			foreach (DockArea da in _dockAreas)
				da.OnPanelDragDropStarted();

			CurrentDragDrop = DragDropType.Panel;
			_panelData.senderDockArea = sender;
			_panelData.tabItem = tabItem;
			_panelData.panelViewModel = viewModel;

			Rect bounds = VisualTreeHelper.GetDescendantBounds(tabItem);
			Rectangle rect = new Rectangle
			{
				Width = bounds.Width,
				Height = bounds.Height,
				Fill = new VisualBrush(tabItem),
				IsHitTestVisible = false,
				Opacity = 0.8
			};

			Common.WinAPI.POINT cursorPos;
			Common.WinAPI.GetCursorPos(out cursorPos);
			_cursorPos.X = cursorPos.X;
			_cursorPos.Y = cursorPos.Y;
			Common.WinAPI.PixelToDip(_previewWindow, out _cursorPos);

			_previewWindow.SetPreviewContent(rect, bounds.Width, bounds.Height, _cursorPos.X, _cursorPos.Y);

			DragDrop.AddGiveFeedbackHandler(tabItem, OnGiveFeedback);
			DragDrop.AddQueryContinueDragHandler(tabItem, OnQueryContinueDrag);

			DragDropEffects result = DragDrop.DoDragDrop(_panelData.tabItem, _panelData, DragDropEffects.Move);

			foreach (DockArea da in _dockAreas)
				da.OnPanelDragDropEnded();

			if (result == DragDropEffects.None)
			{
				DockAreaViewModel? dvm = CurrentPanelData.senderDockArea!.DataContext as DockAreaViewModel;
				CurrentPanelData.senderDockArea!.RemovePanel(CurrentPanelData.panelViewModel!);

				MainWindow? mw = Application.Current.MainWindow as MainWindow;
				Debug.Assert(mw != null);

				Common.WinAPI.GetCursorPos(out cursorPos);
				_cursorPos.X = cursorPos.X;
				_cursorPos.Y = cursorPos.Y;
				Common.WinAPI.PixelToDip(_previewWindow, out _cursorPos);
				SubWindow sw = mw.CreateSubWindow(sender.ActualWidth, sender.ActualHeight);
				sw.Show();
				sw.SetPosition(_cursorPos.X - 50, _cursorPos.Y - 50);

				DockArea dockArea = new DockArea();
				DockAreaViewModel vm = new DockAreaViewModel();
				dockArea.DataContext = vm;
				vm.AddPanel(CurrentPanelData.panelViewModel!);

				DockContainer container = new DockContainer();
				container.InitArea(dockArea);

				Grid.SetRow(container, 1);
				sw.RootGrid.Children.Add(container);
			}

			_previewWindow.HidePreview();
			CurrentDragDrop = DragDropType.None;
			CurrentPanelData.Reset();
		}

		private void OnGiveFeedback(object sender, GiveFeedbackEventArgs e)
		{
			e.Handled = true;
			e.UseDefaultCursors = false;
		}

		private void OnQueryContinueDrag(object sender, QueryContinueDragEventArgs e)
		{
			if (e.EscapePressed)
			{
				e.Action = DragAction.Cancel;
				e.Handled = true;
			}

			Common.WinAPI.POINT cursorPos;
			Common.WinAPI.GetCursorPos(out cursorPos);
			_cursorPos.X = cursorPos.X;
			_cursorPos.Y = cursorPos.Y;
			WinAPI.PixelToDip(_previewWindow, out _cursorPos);
			_previewWindow.SetPosition(_cursorPos.X, _cursorPos.Y);
		}

	}


}
