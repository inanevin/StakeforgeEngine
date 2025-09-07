using StakeforgeEditor.Common;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Text.Json;
using System.Windows;
using System.Windows.Data;
using System.Windows.Input;

namespace StakeforgeEditor.Panels
{

	internal class ConsoleViewModel : PanelViewModel
	{
		private string _searchText = string.Empty;
		private const int MaxItems = 50000;
		private readonly object _lock = new();

		public ICollectionView FilteredLogs { get; }

		public string SearchText
		{
			get => _searchText;
			set
			{
				if (_searchText == value) return;
				_searchText = value;
				OnPropertyChanged();
				// If you want debounce, call DebouncedRefresh() instead
				FilteredLogs.Refresh();
			}
		}

		public ObservableCollection<Common.LogEntry> Logs { get; } = new();

		private int _levelMask = (1 << (int)LogLevel.Max) - 1;
		public int LevelMask
		{
			get => _levelMask;
			set
			{
				if (_levelMask == value) return;
				_levelMask = value;
				OnPropertyChanged();
				FilteredLogs.Refresh();
			}
		}

		public bool IsLevelEnabled(Common.LogLevel level) => (LevelMask & (1 << (byte)level)) != 0;

		public ICommand ToggleLevelCommand { get; }

		public ConsoleViewModel()
		{
			Title = "Console";
			Type = Common.PanelType.Console;

			FilteredLogs = CollectionViewSource.GetDefaultView(Logs);
			FilteredLogs.Filter = FilterPredicate;

			ToggleLevelCommand = new Commands.RelayCommand(lvl =>
			{
				LevelMask ^= 1 << (byte)lvl;

			});
		}

		private bool FilterPredicate(object obj)
		{
			if (obj is not Common.LogEntry e) return false;
			if (!IsLevelEnabled(e.Level)) return false;

			if (string.IsNullOrWhiteSpace(SearchText)) return true;
			var q = SearchText.Trim();

			return e.Message?.IndexOf(q, StringComparison.OrdinalIgnoreCase) >= 0
				|| e.Category?.IndexOf(q, StringComparison.OrdinalIgnoreCase) >= 0
				|| e.Level.ToString().IndexOf(q, StringComparison.OrdinalIgnoreCase) >= 0
				|| e.ThreadId.ToString(CultureInfo.InvariantCulture).Contains(q)
				|| e.Timestamp.ToString("HH:mm:ss.fff", CultureInfo.InvariantCulture)
					.IndexOf(q, StringComparison.OrdinalIgnoreCase) >= 0;
		}

		public void AddLog(Common.LogEntry entry)
		{
			if (Application.Current == null)
				return;

			// Ensure we're on UI thread if called from engine threads
			if (Application.Current.Dispatcher.CheckAccess())
			{
				Add(entry);
			}
			else
			{
				Application.Current.Dispatcher.BeginInvoke(() => Add(entry));
			}

			void Add(Common.LogEntry e)
			{
				Logs.Add(e);
				if (Logs.Count > MaxItems)
					Logs.RemoveAt(0);
				OnPropertyChanged();
			}
		}

		public static void Loge(string msg)
		{
			Panels.Instance.ConsoleViewModel.AddLog(new Common.LogEntry(DateTime.Now, LogLevel.Error, msg, null, 0));
		}
		public static void Logw(string msg)
		{
			Panels.Instance.ConsoleViewModel.AddLog(new Common.LogEntry(DateTime.Now, LogLevel.Warn, msg, null, 0));
		}
		public static void Logt(string msg)
		{
			Panels.Instance.ConsoleViewModel.AddLog(new Common.LogEntry(DateTime.Now, LogLevel.Trace, msg, null, 0));
		}
		public static void Logp(string msg)
		{
			Panels.Instance.ConsoleViewModel.AddLog(new Common.LogEntry(DateTime.Now, LogLevel.Progress, msg, null, 0));
		}

		public override void WriteJson(Utf8JsonWriter w)
		{
			w.WriteNumber("filter", _levelMask);
		}

		public override void ReadJson(ref Utf8JsonReader r)
		{
			while (r.Read() && r.TokenType != JsonTokenType.EndObject)
			{
				if (r.TokenType != JsonTokenType.PropertyName) continue;

				string propertyName = r.GetString() ?? "";
				if (!r.Read()) throw new JsonException();

				switch (propertyName)
				{
					case "filter":
						_levelMask = r.TokenType == JsonTokenType.Number ? r.GetInt32() : 0;
						break;

					// Add future fields here; otherwise:
					default:
						r.Skip(); // safely skip unknowns
						break;
				}
			}
		}
	}
}
