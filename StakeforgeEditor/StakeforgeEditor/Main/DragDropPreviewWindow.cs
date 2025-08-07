using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Media;

namespace StakeforgeEditor.Main
{
	public class DragDropPreviewWindow : Window
	{
		private Grid _contentRoot;

		public DragDropPreviewWindow()
		{
			// Window appearance
			this.WindowStyle = WindowStyle.None;
			this.AllowsTransparency = true;
			this.Background = Brushes.Transparent;
			this.IsHitTestVisible = false;
			this.ShowInTaskbar = false;
			this.Topmost = true;
			this.ResizeMode = ResizeMode.NoResize;
			this.Show();
			this.Visibility = Visibility.Hidden;

			_contentRoot = new Grid();
			this.Content = _contentRoot;
			_contentRoot.IsHitTestVisible = false;
		}

		public void SetPreviewContent(UIElement content, double width, double height, double x, double y)
		{
			_contentRoot.Children.Clear();
			_contentRoot.Children.Add(content);

			this.Width = width;
			this.Height = height;
			this.Left = x;
			this.Top = y;
			this.Visibility = Visibility.Visible;
		}

		public void SetPosition(double x, double y)
		{
			this.Left = x;
			this.Top = y;
		}

		public void HidePreview()
		{
			this.Hide();
		}

		protected override void OnSourceInitialized(EventArgs e)
		{
			base.OnSourceInitialized(e);

			IntPtr hwnd = new WindowInteropHelper(this).Handle;
			int extendedStyle = (int)Common.WinAPI.GetWindowLong(hwnd, Common.WinAPI.GWL_EXSTYLE);
			Common.WinAPI.SetWindowLong(hwnd, Common.WinAPI.GWL_EXSTYLE, extendedStyle | Common.WinAPI.WS_EX_TRANSPARENT | Common.WinAPI.WS_EX_TOOLWINDOW);
		}

	}
}
