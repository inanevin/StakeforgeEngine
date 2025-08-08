using StakeforgeEditor.Common;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace StakeforgeEditor.Converters
{
	public sealed class LogMaskToIconConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value is not int mask || parameter is not LogLevel level) return "";
			bool on = (mask & (1 << (int)level)) != 0;
			string huh = on ? (string)Application.Current.Resources["str_icon_circle"] ?? "" : "";
			return huh;
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) => Binding.DoNothing;
	}
}
