using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StakeforgeEditor.Common
{
	public enum LogLevel { Trace, Info, Warn, Error, Fatal, Max }
	public sealed class LogEntry
	{
		public LogEntry(DateTime now, LogLevel level, string msg, string? cat, int threadID)
		{
			Timestamp = now;
			Level = level;
			Message = msg;
			Category = cat;
			ThreadId = threadID;
		}

		public DateTime Timestamp { get; init; } = DateTime.Now;
		public LogLevel Level { get; init; }
		public string Message { get; init; } = string.Empty;
		public string? Category { get; init; }  // optional
		public int ThreadId { get; init; }      // optional
	}
}
