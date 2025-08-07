using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;

namespace StakeforgeEditor.Converters
{
	internal class BooleanToVisibilityConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			if (value is bool b)
			{
				// If the value is true, return Visible
				return b ? Visibility.Visible : Visibility.Collapsed;
			}
			// Fallback for non-boolean values or null
			return Visibility.Collapsed;
		}
		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			// This converter is designed for one-way binding, so ConvertBack is not needed.
			// Throwing an exception is the standard way to handle this.
			throw new NotSupportedException();
		}
	}
}
