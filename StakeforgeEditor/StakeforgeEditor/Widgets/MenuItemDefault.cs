using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace StakeforgeEditor.Widgets
{
	class MenuItemDefault : MenuItem
	{
		public static readonly DependencyProperty HeaderIconProperty = DependencyProperty.Register("HeaderIcon", typeof(string), typeof(MenuItemDefault), new PropertyMetadata(string.Empty));
		public static readonly DependencyProperty HeaderIconColorProperty = DependencyProperty.Register("HeaderIconColor", typeof(SolidColorBrush), typeof(MenuItemDefault), new PropertyMetadata(null));

		public string HeaderIcon
		{
			get { return (string)GetValue(HeaderIconProperty); }
			set { SetValue(HeaderIconProperty, value); }
		}

		public SolidColorBrush HeaderIconColor
		{
			get { return (SolidColorBrush)GetValue(HeaderIconColorProperty); }
			set { SetValue(HeaderIconColorProperty, value); }
		}
	}
}
