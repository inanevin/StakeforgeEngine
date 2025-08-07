using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Animation;

namespace StakeforgeEditor.Common
{
	public enum Direction
	{
		None,
		Left,
		Right,
		Top,
		Bottom,
		Center
	}

	public enum PanelType
	{
		Resources,
		Entities,
		Properties,
		World,
		Console,
	}

	class ObjectUtil
	{
		public static T? FindParent<T>(DependencyObject child) where T : DependencyObject
		{
			DependencyObject? parent = VisualTreeHelper.GetParent(child);
			while (parent != null && parent is not T)
				parent = VisualTreeHelper.GetParent(parent);
			return parent as T;
		}
	}

	public static class StoryboardHelper
	{
		public static readonly DependencyProperty HoverStoryboardProperty =
			DependencyProperty.RegisterAttached(
				"HoverStoryboard",
				typeof(Storyboard),
				typeof(StoryboardHelper),
				new PropertyMetadata(null));

		public static void SetHoverStoryboard(UIElement element, Storyboard value)
		{
			element.SetValue(HoverStoryboardProperty, value);
		}

		public static Storyboard GetHoverStoryboard(UIElement element)
		{
			return (Storyboard)element.GetValue(HoverStoryboardProperty);
		}
	}
}
