using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Interop;

namespace StakeforgeEditor.Common
{
	internal class WinAPI
	{

		public const int GWL_EXSTYLE = -20;
		public const int WS_EX_TRANSPARENT = 0x00000020;
		public const int WS_EX_TOOLWINDOW = 0x00000080;
		public const uint WS_POPUP = 0x80000000;
		public const uint WS_THICKFRAME = 0x00040000;
		public const uint WS_CAPTION = 0x00C00000;
		public const uint WS_MAXIMIZEBOX = 0x00010000;
		public const uint WS_MINIMIZEBOX = 0x00020000;
		public const uint WS_CLIPCHILDREN = 0x02000000;
		public const int WM_NCHITTEST = 0x0084;
		public const int WM_NCACTIVATE = 0x0086;
		public const int WM_NCLBUTTONDBLCLK = 0x00A3;
		public const int HTCLIENT = 1;
		public const int HTCAPTION = 2;
		public const int HTLEFT = 10;
		public const int HTRIGHT = 11;
		public const int HTTOP = 12;
		public const int HTTOPLEFT = 13;
		public const int HTTOPRIGHT = 14;
		public const int HTBOTTOM = 15;
		public const int HTBOTTOMLEFT = 16;
		public const int HTBOTTOMRIGHT = 17;
		public const uint SWP_NOMOVE = 0x0002;
		public const uint SWP_NOSIZE = 0x0001;
		public const uint SWP_NOZORDER = 0x0004;
		public const uint SWP_FRAMECHANGED = 0x0020;

		[DllImport("user32.dll")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool GetCursorPos(out POINT lpPoint);

		[DllImport("user32.dll")]
		public static extern bool ScreenToClient(IntPtr hWnd, ref POINT lpPoint);

		[StructLayout(LayoutKind.Sequential)]
		public struct POINT
		{
			public int X;
			public int Y;
		}

		// Native methods
		[DllImport("user32.dll")]
		public static extern IntPtr GetWindowLong(IntPtr hWnd, int nIndex);

		[DllImport("user32.dll")]
		public static extern IntPtr SetWindowLong(IntPtr hWnd, int nIndex, IntPtr dwNewLong);

		[DllImport("user32.dll", SetLastError = true)]
		public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter,
			int X, int Y, int cx, int cy, uint uFlags);


		public static void PixelToDip(UIElement element, out Point point)
		{
			var source = PresentationSource.FromVisual(element);
			if (source != null)
			{
				var m = source.CompositionTarget.TransformFromDevice;
				point = m.Transform(point);
			}
		}
	}
}
