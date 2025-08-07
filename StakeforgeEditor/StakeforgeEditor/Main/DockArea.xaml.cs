using StakeforgeEditor.Common;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace StakeforgeEditor.Main
{
	/// <summary>
	/// Interaction logic for DockArea.xaml
	/// </summary>
	public partial class DockArea : UserControl
	{
		private Point _dragStartPoint;
		private TabItem? _draggedTabItem;
		private PanelViewModel? _draggedPanelViewModel;
		private double _minSize;
		private List<Border> _previews = new List<Border>();
		private List<Storyboard> _previewAnimations = new List<Storyboard>();
		private List<Storyboard> _previewLoopingAnimations = new List<Storyboard>();
		private Common.Direction _previewHitTest = Common.Direction.None;

		public DockArea()
		{
			InitializeComponent();
			_minSize = (double)Application.Current.Resources["size_min_dock_area"];
			_previews.Add(PreviewLeft);
			_previews.Add(PreviewRight);
			_previews.Add(PreviewBottom);
			_previews.Add(PreviewTop);
			_previews.Add(PreviewCenter);

			PropertyPath px = new PropertyPath("RenderTransform.(ScaleTransform.ScaleX)");
			PropertyPath py = new PropertyPath("RenderTransform.(ScaleTransform.ScaleY)");

			ElasticEase ease = new ElasticEase();
			ease.Oscillations = 1;
			ease.Springiness = 2;

			foreach (Border border in _previews)
			{
				DoubleAnimation x = new DoubleAnimation(0.5, 0.9, new Duration(TimeSpan.FromSeconds(0.45)));
				DoubleAnimation y = new DoubleAnimation(0.5, 0.9, new Duration(TimeSpan.FromSeconds(0.45)));
				x.EasingFunction = ease;
				y.EasingFunction = ease;

				Storyboard.SetTarget(x, border);
				Storyboard.SetTarget(y, border);
				Storyboard.SetTargetProperty(x, px);
				Storyboard.SetTargetProperty(y, py);

				Storyboard storyboard = new Storyboard { };
				storyboard.Children.Add(x);
				storyboard.Children.Add(y);
				_previewAnimations.Add(storyboard);

				var loopX = new DoubleAnimation(1.0, 1.1, new Duration(TimeSpan.FromSeconds(0.075)));
				Storyboard.SetTarget(loopX, border);
				Storyboard.SetTargetProperty(loopX, px);

				var loopY = new DoubleAnimation(1.0, 1.1, new Duration(TimeSpan.FromSeconds(0.15)));
				Storyboard.SetTarget(loopY, border);
				Storyboard.SetTargetProperty(loopY, py);

				Storyboard loopingStoryboard = new Storyboard { RepeatBehavior = RepeatBehavior.Forever, AutoReverse = true };
				loopingStoryboard.Children.Add(loopX);
				loopingStoryboard.Children.Add(loopY);
				_previewLoopingAnimations.Add(loopingStoryboard);
			}

		}

		private void RemovePanel(PanelViewModel model)
		{
			DockAreaViewModel? viewModel = this.DataContext as DockAreaViewModel;
			Debug.Assert(viewModel != null);

			viewModel.RemovePanel(model);
			if (viewModel.Panels.Count == 0)
			{
				DockContainer? container = Common.ObjectUtil.FindParent<DockContainer>(this);
				Debug.Assert(container != null);
				container.Kill();
			}
		}

		private void TabItem_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			DockContainer? container = Common.ObjectUtil.FindParent<DockContainer>(this);
			Debug.Assert(container != null);

			DockAreaViewModel? viewModel = this.DataContext as DockAreaViewModel;
			Debug.Assert(viewModel != null);
			if (viewModel.Panels.Count == 1 && !container.CanKill())
				return;

			_dragStartPoint = e.GetPosition(this);
			_draggedTabItem = sender as TabItem;

			if (_draggedTabItem != null)
			{
				_draggedPanelViewModel = _draggedTabItem.DataContext as PanelViewModel;
			}
		}

		private void TabItem_PreviewMouseMove(object sender, MouseEventArgs e)
		{
			if (e.LeftButton != MouseButtonState.Pressed)
				return;

			if (_draggedTabItem == null || _draggedPanelViewModel == null)
				return;

			DockContainer? container = Common.ObjectUtil.FindParent<DockContainer>(this);
			Debug.Assert(container != null);

			DockAreaViewModel? viewModel = this.DataContext as DockAreaViewModel;
			Debug.Assert(viewModel != null);

			if (viewModel.Panels.Count == 1 && !container.CanKill())
				return;

			Point mousePos = e.GetPosition(this);
			Vector diff = _dragStartPoint - mousePos;

			if (Math.Abs(diff.X) > SystemParameters.MinimumHorizontalDragDistance || Math.Abs(diff.Y) > SystemParameters.MinimumVerticalDragDistance)
			{
				DragDropHandler.Instance.BeginDragPanel(this, _draggedTabItem, _draggedPanelViewModel);
				_draggedTabItem = null;
				_draggedPanelViewModel = null;
			}
		}

		private void TabItem_Drop(object sender, DragEventArgs e)
		{
			if (DragDropHandler.Instance.CurrentDragDrop != DragDropType.Panel)
				return;

			if (DragDropHandler.Instance.CurrentPanelData.tabItem == e.Source)
				return;

			TabItem? sourceItem = e.Source as TabItem;
			Debug.Assert(sourceItem != null);

			PanelViewModel? panelViewModel = sourceItem.DataContext as PanelViewModel;
			Debug.Assert(panelViewModel != null);

			PanelViewModel? target = DragDropHandler.Instance.CurrentPanelData.panelViewModel;
			Debug.Assert(target != null);

			DockAreaViewModel? viewModel = this.DataContext as DockAreaViewModel;
			Debug.Assert(viewModel != null);

			int targetIndex = viewModel.Panels.IndexOf(panelViewModel);
			viewModel.MovePanel(target, targetIndex);

			e.Handled = true;
		}


		private void Content_DragEnter(object sender, DragEventArgs e)
		{
			if (DragDropHandler.Instance.CurrentDragDrop != DragDropType.Panel)
				return;

			DockAreaViewModel? viewModel = this.DataContext as DockAreaViewModel;
			Debug.Assert(viewModel != null);

			if (DragDropHandler.Instance.CurrentPanelData.senderDockArea == this && viewModel.Panels.Count == 1)
				return;

			SetPreviewVisibility(true);
			ChangePreviewState(Common.Direction.None);
			AnimatePreviews();
		}


		private void Content_DragLeave(object sender, DragEventArgs e)
		{
			if (DragDropHandler.Instance.CurrentDragDrop != DragDropType.Panel)
				return;

			SetPreviewVisibility(false);
			ChangePreviewState(Common.Direction.None);
		}

		public void Content_DragOver(object sender, DragEventArgs e)
		{
			Point position = e.GetPosition(ContentGrid);

			VisualTreeHelper.HitTest(ContentGrid,
									null,
									new HitTestResultCallback(ContentHitTestCallback),
									new PointHitTestParameters(position)
			);

		}

		private HitTestResultBehavior ContentHitTestCallback(HitTestResult result)
		{
			if (result.VisualHit == null)
				return HitTestResultBehavior.Continue;


			UIElement? element = result.VisualHit as UIElement;

			if (element == null)
			{
				ChangePreviewState(Common.Direction.None);
				return HitTestResultBehavior.Stop;
			}

			Border? hitBorder = null;
			if (!_previews.Contains(element))
			{
				foreach (Border bd in _previews)
				{
					if (bd.IsAncestorOf(element))
					{
						hitBorder = bd;
						break;
					}
				}
			}
			else
			{
				hitBorder = element as Border;
			}

			if (hitBorder == null)
			{
				ChangePreviewState(Common.Direction.None);
				return HitTestResultBehavior.Continue;
			}

			Common.Direction state = hitBorder.Tag.ToString() switch
			{
				"Top" => Common.Direction.Top,
				"Bottom" => Common.Direction.Bottom,
				"Left" => Common.Direction.Left,
				"Right" => Common.Direction.Right,
				"Center" => Common.Direction.Center,
				_ => Common.Direction.None
			};

			ChangePreviewState(state);
			return HitTestResultBehavior.Stop;
		}

		public void Content_Drop(object sender, DragEventArgs e)
		{
			if (DragDropHandler.Instance.CurrentDragDrop != DragDropType.Panel)
				return;

			DockAreaViewModel? viewModel = this.DataContext as DockAreaViewModel;
			Debug.Assert(viewModel != null);

			switch (_previewHitTest)
			{
				case Common.Direction.Center:
					{
						DragDropHandler.Instance.CurrentPanelData.senderDockArea!.RemovePanel(DragDropHandler.Instance.CurrentPanelData.panelViewModel!);
						viewModel.AddPanel(DragDropHandler.Instance.CurrentPanelData.panelViewModel!);
						e.Handled = true;
						e.Effects = DragDropEffects.Move;
						break;
					}
				case Common.Direction.None:
					e.Effects = DragDropEffects.None;
					break;
				default:
					{
						DragDropHandler.Instance.CurrentPanelData.senderDockArea!.RemovePanel(DragDropHandler.Instance.CurrentPanelData.panelViewModel!);
						DockContainer? parentContainer = Common.ObjectUtil.FindParent<DockContainer>(this);
						Debug.Assert(parentContainer != null);
						parentContainer.SplitArea(_previewHitTest, DragDropHandler.Instance.CurrentPanelData.panelViewModel!);
						e.Handled = true;
						e.Effects = DragDropEffects.Move;
						break;
					}
			}

			SetPreviewVisibility(false);
			ChangePreviewState(Common.Direction.None);

		}



		private void AnimatePreviews()
		{
			foreach (Storyboard sb in _previewAnimations)
			{
				sb.Stop();
				sb.Begin();
			}
		}

		private void AnimatePreviewsLooping(Common.Direction direction)
		{
			foreach (Storyboard sb in _previewLoopingAnimations)
				sb.Stop();

			foreach (Border bd in _previews)
			{
				ScaleTransform? sc = bd.RenderTransform as ScaleTransform;
				Debug.Assert(sc != null);
				sc.ScaleX = 0.9f;
				sc.ScaleY = 0.9f;
			}

			if (direction == Common.Direction.None)
				return;

			switch (direction)
			{
				case Direction.Left:
					_previewLoopingAnimations[0].Begin();
					break;
				case Direction.Right:
					_previewLoopingAnimations[1].Begin();
					break;
				case Direction.Bottom:
					_previewLoopingAnimations[2].Begin();
					break;
				case Direction.Top:
					_previewLoopingAnimations[3].Begin();
					break;
				case Direction.Center:
					_previewLoopingAnimations[4].Begin();
					break;
				default:
					break;
			}
		}

		private void SetPreviewVisibility(bool isVisible)
		{
			PreviewGrid.Visibility = isVisible ? Visibility.Visible : Visibility.Hidden;
		}

		private void SetPreviewRectVisibility(Common.Direction direction)
		{
			PreviewRectLeft.Visibility = Visibility.Hidden;
			PreviewRectRight.Visibility = Visibility.Hidden;
			PreviewRectTop.Visibility = Visibility.Hidden;
			PreviewRectBottom.Visibility = Visibility.Hidden;
			PreviewRectCenter.Visibility = Visibility.Hidden;

			if (direction == Common.Direction.None)
				return;

			DockAreaViewModel? viewModel = this.DataContext as DockAreaViewModel;
			Debug.Assert(viewModel != null);

			if (direction == Common.Direction.Left || direction == Common.Direction.Right)
			{
				if (ActualWidth < _minSize + 25)
					return;
			}
			else if (direction == Common.Direction.Bottom || direction == Common.Direction.Top)
			{
				if (ActualHeight < _minSize + 25)
					return;
			}

			switch (direction)
			{
				case Direction.Left:
					PreviewRectLeft.Visibility = Visibility.Visible;
					break;
				case Direction.Right:
					PreviewRectRight.Visibility = Visibility.Visible;
					break;
				case Direction.Top:
					PreviewRectTop.Visibility = Visibility.Visible;
					break;
				case Direction.Bottom:
					PreviewRectBottom.Visibility = Visibility.Visible;
					break;
				case Direction.Center:
					PreviewRectCenter.Visibility = Visibility.Visible;
					break;
				default:
					break;
			}
		}

		private void ChangePreviewState(Common.Direction dir)
		{
			if (_previewHitTest == dir)
				return;

			_previewHitTest = dir;
			SetPreviewRectVisibility(dir);
			AnimatePreviewsLooping(dir);
		}

		private void Context_Click(object sender, RoutedEventArgs e)
		{
			Button? btn = sender as Button;
			Debug.Assert(btn != null);
			btn.ContextMenu.PlacementTarget = btn;
			btn.ContextMenu.Placement = System.Windows.Controls.Primitives.PlacementMode.Bottom;
			btn.ContextMenu.IsOpen = true;
		}
    }


}
