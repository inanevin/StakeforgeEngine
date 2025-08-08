using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace StakeforgeEditor.Common
{
	public static class AutoScrollBehavior
	{
		public static readonly DependencyProperty IsEnabledProperty =
			DependencyProperty.RegisterAttached(
				"IsEnabled",
				typeof(bool),
				typeof(AutoScrollBehavior),
				new PropertyMetadata(false, OnChanged));

		public static bool GetIsEnabled(DependencyObject d) => (bool)d.GetValue(IsEnabledProperty);
		public static void SetIsEnabled(DependencyObject d, bool v) => d.SetValue(IsEnabledProperty, v);

		private static void OnChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
		{
			if (d is ListView list && e.NewValue is true)
			{
				list.Loaded += (_, __) =>
				{
					var sv = FindDescendant<ScrollViewer>(list);
					if (sv == null) return;

					bool userPinnedBottom = true;
					sv.ScrollChanged += (_, args) =>
					{
						// If user moves up, stop autoscrolling until they return to bottom
						if (args.ExtentHeightChange == 0)
							userPinnedBottom = sv.VerticalOffset >= sv.ScrollableHeight - 1;
					};

					if (list.ItemsSource is INotifyCollectionChanged incc)
					{
						incc.CollectionChanged += (_, args2) =>
						{
							if (args2.Action is NotifyCollectionChangedAction.Add && userPinnedBottom)
								sv.ScrollToEnd();
						};
					}
				};
			}
		}

		private static T? FindDescendant<T>(DependencyObject root) where T : DependencyObject
		{
			var count = VisualTreeHelper.GetChildrenCount(root);
			for (int i = 0; i < count; i++)
			{
				var child = VisualTreeHelper.GetChild(root, i);
				if (child is T t) return t;
				var deeper = FindDescendant<T>(child);
				if (deeper != null) return deeper;
			}
			return null;
		}

	}
}
