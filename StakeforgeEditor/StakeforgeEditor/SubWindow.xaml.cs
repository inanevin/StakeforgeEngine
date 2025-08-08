using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Xml.Linq;
using static System.Net.WebRequestMethods;

namespace StakeforgeEditor
{
	/// <summary>
	/// Interaction logic for SubWindow.xaml
	/// </summary>
	public partial class SubWindow : Window
	{
		private double _barHeight = 0;

		public SubWindow()
		{
			InitializeComponent();
			this.ShowInTaskbar = true;
			this.Visibility = Visibility.Visible;
			Loaded += OnLoaded;
			_barHeight = (double)Application.Current.Resources["height_item_base"];
		}

		public void SetPosition(double x, double y)
		{
			this.Left = x;
			this.Top = y;
		}

		public void SetSize(double width, double height)
		{
			this.Width = width;
			this.Height = height;
		}

		void OnClickMinimize(object sender, RoutedEventArgs e)
		{

		}

		void OnClickMaximize(object sender, RoutedEventArgs e)
		{

		}

		void OnClickClose(object sender, RoutedEventArgs e)
		{
			CloseWindow();
		}

		public void CloseWindow()
		{
			MainWindow? mw = Application.Current.MainWindow as MainWindow;
			Debug.Assert(mw != null);
			mw.RemoveSubwindow(this);
			this.Close();
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			var hwnd = new WindowInteropHelper(this).Handle;

			// Hook WndProc for resize + drag
			HwndSource.FromHwnd(hwnd).AddHook(WndProc);
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

						Point relativePos = Minimize.TransformToAncestor(this)
						  .Transform(new Point(0, 0));

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
						else if (mouse.Y <= _barHeight && mouse.X <= relativePos.X)
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



	}
}
