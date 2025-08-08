using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace StakeforgeEditor.Widgets
{
	class InputFieldDefault : TextBox
	{
		// Register the dependency property
		public static readonly DependencyProperty PlaceholderTextProperty = DependencyProperty.Register("PlaceholderText", typeof(string), typeof(InputFieldDefault), new PropertyMetadata(string.Empty));
		public static readonly DependencyProperty PlaceholderIconProperty = DependencyProperty.Register("PlaceholderIcon", typeof(string), typeof(InputFieldDefault), new PropertyMetadata(string.Empty));

		// .NET property wrapper
		public string PlaceholderText
		{
			get { return (string)GetValue(PlaceholderTextProperty); }
			set { SetValue(PlaceholderTextProperty, value); }
		}

		public string PlaceholderIcon
		{
			get { return (string)GetValue(PlaceholderIconProperty); }
			set { SetValue(PlaceholderIconProperty, value); }
		}
	}

}
