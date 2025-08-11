using StakeforgeEditor.Panels;
using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;
using System.Windows.Media;
using static System.Net.Mime.MediaTypeNames;

namespace StakeforgeEditor.Converters
{
	public sealed class LogLevelToBrush : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			return value switch
			{
				Common.LogLevel.Trace => System.Windows.Application.Current.Resources["brush_accent_primary"],
				Common.LogLevel.Info => System.Windows.Application.Current.Resources["brush_text_primary"],
				Common.LogLevel.Warn => System.Windows.Application.Current.Resources["brush_accent_warn"],
				Common.LogLevel.Error => System.Windows.Application.Current.Resources["brush_accent_error"],
				Common.LogLevel.Progress => System.Windows.Application.Current.Resources["brush_accent_green"],
				_ => Brushes.Gray
			};
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) => Binding.DoNothing;
	}
}
