using StakeforgeEditor.Common;
using StakeforgeEditor.Panels;
using System;
using System.Collections.Concurrent;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace StakeforgeEditor.Editor
{
	enum EngineServerStatus
	{
		NotConnected,
		ConnectedNoPipe,
		ConnectedWPipe,
	}

	enum PipeMessageType : Byte
	{
		Log,
	}

	public sealed class ConnectionManager
	{
		private Process? _engineProcess;
		private const string PipeName = "stakeforge_engine";
		private NamedPipeClientStream? _pipeClient;
		private CancellationTokenSource? _cancellationTokenSource;

		public ConnectionManager()
		{
		}
		public async void StartEngineProcess()
		{

			if (string.IsNullOrWhiteSpace(Editor.Instance.Settings.EngineHostExePath) || !File.Exists(Editor.Instance.Settings.EngineHostExePath))
			{
				ConsoleViewModel.Loge("Failed launching engine server, server path doesn't exist: " + Editor.Instance.Settings.EngineHostExePath);
				return;
			}

			if (_engineProcess != null && !_engineProcess.HasExited)
			{
				// The process is already running.
				ConsoleViewModel.Loge("Server already running.");
				return;
			}

			string engineExeName = Path.GetFileNameWithoutExtension(Editor.Instance.Settings.EngineHostExePath);
			Process[] existingProcesses = Process.GetProcessesByName(engineExeName);
			foreach (Process pr in existingProcesses)
				pr.Kill();

			ConsoleViewModel.Logw("Starting server.");

			_engineProcess = new Process
			{
				StartInfo = new ProcessStartInfo
				{
					FileName = Editor.Instance.Settings.EngineHostExePath!,
					Arguments = $"--pipe stakeforge",
					UseShellExecute = false,
					CreateNoWindow = false,
					WorkingDirectory = Path.GetDirectoryName(Editor.Instance.Settings.EngineHostExePath!)!
				},
				EnableRaisingEvents = true
			};

			_engineProcess.EnableRaisingEvents = true;
			_engineProcess.Exited += (s, e) =>
			{
				Panels.Panels.Instance.TopBarViewModel.UpdateServerConnectionStatus(EngineServerStatus.NotConnected);
				_engineProcess = null;
				ConsoleViewModel.Logw("Engine process exited! ");
			};

			if (!_engineProcess.Start())
				throw new InvalidOperationException("Failed to start engine process.");

			try { _engineProcess.PriorityClass = ProcessPriorityClass.High; } catch { ConsoleViewModel.Logw("Failed setting server priority."); }

			Panels.Panels.Instance.TopBarViewModel.UpdateServerConnectionStatus(EngineServerStatus.ConnectedNoPipe);
			await ConnectToEnginePipe();
		}

		public void StopEngineProcess()
		{
			if (_engineProcess != null && !_engineProcess.HasExited)
				_engineProcess.Kill();
			ConsoleViewModel.Logp("Engine process killed!");
			Panels.Panels.Instance.TopBarViewModel.UpdateServerConnectionStatus(EngineServerStatus.NotConnected);
		}

		public void RestartEngineProcess()
		{
			StopEngineProcess();
			StartEngineProcess();
		}


		public async Task ConnectToEnginePipe()
		{
			_cancellationTokenSource = new CancellationTokenSource();
			CancellationToken token = _cancellationTokenSource.Token;

			// Give the engine a moment to start and create the pipe
			await Task.Delay(1000);

			_pipeClient = new NamedPipeClientStream(".", PipeName, PipeDirection.InOut, PipeOptions.Asynchronous);

			try
			{
				// Wait for the engine to connect. Timeout after 5 seconds.
				await _pipeClient.ConnectAsync(5000, token);
				ConsoleViewModel.Logp($"Connected to engine via named pipe '{PipeName}'.");
				Panels.Panels.Instance.TopBarViewModel.UpdateServerConnectionStatus(EngineServerStatus.ConnectedWPipe);

				// Start the asynchronous read loop
				_ = Task.Run(() => ReadPipeAsync(token));
			}
			catch (TimeoutException)
			{
				ConsoleViewModel.Loge("Failed to connect to engine pipe: Timeout.");
				DisconnectPipe();
			}
			catch (Exception ex)
			{
				ConsoleViewModel.Loge($"Failed to connect to engine pipe: {ex.Message}");
				DisconnectPipe();
			}
			
		}

		private async Task ReadPipeAsync(CancellationToken token)
		{
			if (_pipeClient == null || !_pipeClient.IsConnected) return;

			byte[] buffer = new byte[4096];
			StringBuilder messageBuilder = new StringBuilder();

			try
			{
				while (!token.IsCancellationRequested)
				{
					// Read data from the pipe
					int bytesRead = await _pipeClient.ReadAsync(buffer, 0, buffer.Length, token);

					if (bytesRead > 0)
					{
						int index = 0;
						PipeMessageType messageType = (PipeMessageType)buffer[0];
						index += sizeof(PipeMessageType);

						if (messageType == PipeMessageType.Log)
						{
							Common.LogLevel logLevel = (Common.LogLevel)buffer[index];
							index += sizeof(Common.LogLevel);

							string data = Encoding.UTF8.GetString(buffer, index, (int)bytesRead - index);

							messageBuilder.Append("[Stakeforge.exe] -> ");
							messageBuilder.Append(data);
							if (messageBuilder.ToString().Contains('\n'))
							{
								string[] lines = messageBuilder.ToString().Split('\n', StringSplitOptions.RemoveEmptyEntries);
								foreach (var line in lines)
								{
									OnEngineLog(logLevel, line);
								}
								messageBuilder.Clear();
							}
							else
							{
								OnEngineLog(logLevel, messageBuilder.ToString());
								messageBuilder.Clear();
							}
						}
					}
					else if (_pipeClient.IsConnected == false)
					{
						break;
					}
				}
			}
			catch (OperationCanceledException)
			{
				// The task was cancelled.
			}
			catch (IOException ex)
			{
				// This exception usually means the pipe was closed unexpectedly.
				ConsoleViewModel.Loge($"Pipe read error: {ex.Message}");
			}
			finally
			{
				DisconnectPipe();
			}
		}

		public void DisconnectPipe()
		{
			_cancellationTokenSource?.Cancel();
			if (_pipeClient != null)
			{
				if (_pipeClient.IsConnected)
				{
					_pipeClient.Flush();
					_pipeClient.Dispose();
				}
				_pipeClient = null;
			}

			Panels.Panels.Instance.TopBarViewModel.UpdateServerConnectionStatus(EngineServerStatus.ConnectedNoPipe);
		}


		public async Task SendDataToPipe(byte[] data)
		{
			if (_pipeClient == null || !_pipeClient.IsConnected)
			{
				ConsoleViewModel.Loge("Cannot send message: Pipe is not connected.");
				return;
			}

			try
			{
				await _pipeClient.WriteAsync(data, 0, data.Length);
				await _pipeClient.FlushAsync(); // Ensure the data is sent immediately
			}
			catch (IOException ex)
			{
				// This exception indicates a problem with the pipe, likely a disconnection
				DisconnectPipe();
			}
			catch (Exception ex)
			{
				ConsoleViewModel.Loge($"An unexpected error occurred while sending message: {ex.Message}");
			}
		}

		private void OnEngineLog(Common.LogLevel level, string msg)
		{
			if (string.IsNullOrEmpty(msg))
				return;

			Panels.Panels.Instance.ConsoleViewModel.AddLog(new Common.LogEntry(DateTime.Now, level, msg, null, 0));
		}

	}
}
