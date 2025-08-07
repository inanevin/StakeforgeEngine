using StakeforgeEditor.Main;
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
	public class DockOverlayStateToVisibilityConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value is Common.Direction currentState &&
				parameter is string targetState &&
				Enum.TryParse(typeof(Common.Direction), targetState, out var targetEnum))
			{
				return currentState.Equals(targetEnum) ? Visibility.Visible : Visibility.Collapsed;
			}

			return Visibility.Collapsed;
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
			=> throw new NotImplementedException();
	}

}
