
using StakeforgeEditor.Main;
using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.Json;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;


namespace StakeforgeEditor
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{

		private List<SubWindow> _subWindows = new List<SubWindow>();
		private DockContainer _mainDockContainer = new DockContainer();
		private double _loadedLeft = 0;
		private double _loadedTop = 0;

		public MainWindow()
		{
			InitializeComponent();
			Loaded += OnLoaded;

			Grid.SetRow(_mainDockContainer, 1);
			RootGrid.Children.Add(_mainDockContainer);

		}

		public SubWindow CreateSubWindow(double width, double height)
		{
			SubWindow sw = new SubWindow();
			sw.SetSize(width, height);
			_subWindows.Add(sw);
			return sw;
		}

		public void RemoveSubwindow(SubWindow sw)
		{
			_subWindows.Remove(sw);
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			var hwnd = new WindowInteropHelper(this).Handle;
			HwndSource.FromHwnd(hwnd).AddHook(WndProc);

			this.Left = _loadedLeft;
			this.Top = _loadedTop;
			Editor.Editor.Instance.PostInit();

		}

		private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
		{
			switch (msg)
			{

				case Common.WinAPI.WM_NCLBUTTONDBLCLK:
					{
						// This message arrives only if WM_NCHITTEST returned HTCAPTION for the double-click point
						WindowState = WindowState == WindowState.Normal
							? WindowState.Maximized
							: WindowState.Normal;

						handled = true;
						break;
					}
				case Common.WinAPI.WM_NCHITTEST:
					{
						Point mouse = GetMousePosition(lParam);

						const int edge = 8;
						double width = ActualWidth;
						double height = ActualHeight;

						Point relative = TopBar.LastColumn.TransformToAncestor(this).Transform(new Point(0, 0));
						Point relativeMin = TopBar.Minimize.TransformToAncestor(this).Transform(new Point(0, 0));

						if (mouse.Y <= edge)
						{
							if (mouse.X <= edge) { handled = true; return (IntPtr)Common.WinAPI.HTTOPLEFT; }
							else if (mouse.X >= width - edge) { handled = true; return (IntPtr)Common.WinAPI.HTTOPRIGHT; }
							else { handled = true; return (IntPtr)Common.WinAPI.HTTOP; }
						}
						else if (mouse.Y >= height - edge)
						{
							if (mouse.X <= edge) { handled = true; return (IntPtr)Common.WinAPI.HTBOTTOMLEFT; }
							else if (mouse.X >= width - edge) { handled = true; return (IntPtr)Common.WinAPI.HTBOTTOMRIGHT; }
							else { handled = true; return (IntPtr)Common.WinAPI.HTBOTTOM; }
						}
						else if (mouse.X <= edge)
						{
							handled = true;
							return (IntPtr)Common.WinAPI.HTLEFT;
						}
						else if (mouse.X >= width - edge)
						{
							handled = true;
							return (IntPtr)Common.WinAPI.HTRIGHT;
						}
						else if (mouse.Y <= TopBar.ActualHeight && mouse.X > relative.X && mouse.X < relativeMin.X)
						{
							handled = true;
							return (IntPtr)Common.WinAPI.HTCAPTION;
						}

						return (IntPtr)Common.WinAPI.HTCLIENT;
					}

			}

			return IntPtr.Zero;
		}
		private Point GetMousePosition(IntPtr lParam)
		{
			int x = (short)((lParam.ToInt64()) & 0xFFFF);
			int y = (short)((lParam.ToInt64() >> 16) & 0xFFFF);
			return PointFromScreen(new Point(x, y));
		}

		public void WriteJson(Utf8JsonWriter w)
		{
			w.WriteStartObject();

			w.WriteNumber("left", this.Left);
			w.WriteNumber("top", this.Top);
			w.WriteNumber("width", this.Width);
			w.WriteNumber("height", this.Height);

			if (_mainDockContainer.SplitDirection == Common.Direction.None && _mainDockContainer.TargetArea == null)
				InitDefault();
			else if (_mainDockContainer.SplitDirection != Common.Direction.None && _mainDockContainer.Container0 == null)
				InitDefault();

			w.WriteStartObject("container");
			_mainDockContainer.WriteJson(w);
			w.WriteEndObject();

			foreach (SubWindow sw in _subWindows)
			{
				w.WriteStartObject("subwindow");
				sw.WriteJson(w);
				w.WriteEndObject();
			}

			w.WriteEndObject();
		}

		public void ReadJson(ref Utf8JsonReader r)
		{
			while (r.Read() && r.TokenType != JsonTokenType.EndObject)
			{
				if (r.TokenType == JsonTokenType.PropertyName)
				{
					string propertyName = r.GetString() ?? "";
					r.Read();

					switch (propertyName)
					{
						case "left":
							{
								double val = r.GetDouble();
								this.Left = val;
								_loadedLeft = val;
								break;
							}
						case "top":
							{
								double val = r.GetDouble();
								this.Top = val;
								_loadedTop = val;
								break;
							}
						case "width":
							{
								double val = r.GetDouble();
								this.Width = val;
								break;
							}
						case "height":
							{
								double val = r.GetDouble();
								this.Height = val;
								break;
							}
						case "container":
							{
								_mainDockContainer.ReadJson(ref r);
								break;
							}
						case "subwindow":
							{
								SubWindow sw = CreateSubWindow(10, 10);
								sw.ReadJson(ref r);
								break;
							}
					}
				}
			}
		}

		private void InitDefault()
		{
			DockArea area0 = new DockArea();
			DockAreaViewModel area0VM = new DockAreaViewModel();
			area0.DataContext = area0VM;
			area0VM.AddPanel(Panels.Panels.Instance.ConsoleViewModel);
			area0VM.AddPanel(Panels.Panels.Instance.ResourcesViewModel);
			area0VM.AddPanel(Panels.Panels.Instance.EntitiesViewModel);
			area0VM.AddPanel(Panels.Panels.Instance.WorldViewModel);
			area0VM.AddPanel(Panels.Panels.Instance.PropertiesViewModel);

			_mainDockContainer.InitArea(area0);
		}
	}
}