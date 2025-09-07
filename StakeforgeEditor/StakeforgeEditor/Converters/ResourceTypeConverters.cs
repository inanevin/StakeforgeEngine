using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Windows.Media;

namespace StakeforgeEditor.Converters
{
	public sealed class ResourceTypeToTagString : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			return value switch
			{
				Common.ResourceType.Texture => "T",
				Common.ResourceType.Material => "M",
				Common.ResourceType.World => "W",
				Common.ResourceType.Font => "F",
				Common.ResourceType.Audio => "A",
				_ => Brushes.Gray
			};
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) => Binding.DoNothing;
	}

	public sealed class ResourceTypeToTagBrush : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			return value switch
			{
				Common.ResourceType.Texture => System.Windows.Application.Current.Resources["brush_accent_primary"],
				Common.ResourceType.Material => System.Windows.Application.Current.Resources["brush_text_primary"],
				Common.ResourceType.World => System.Windows.Application.Current.Resources["brush_accent_warn"],
				Common.ResourceType.Font => System.Windows.Application.Current.Resources["brush_accent_error"],
				Common.ResourceType.Audio => System.Windows.Application.Current.Resources["brush_accent_green"],
				_ => Brushes.Gray
			};
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) => Binding.DoNothing;
	}
}
