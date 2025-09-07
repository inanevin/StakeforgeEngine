// Copyright (c) 2025 Inan Evin

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include "game.hpp"
#include "io/log.hpp"
#include "memory/memory_tracer.hpp"
#include "platform/process.hpp"

/*
const std::string PIPE_NAME = "\\\\.\\pipe\\stakeforge_engine";
*/

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR pCmdLine, _In_ int nCmdShow)
{
	if (AllocConsole() == FALSE)
	{
		SFG_ERR("Failed allocating console!");
	}
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	SetProcessPriorityBoost(GetCurrentProcess(), FALSE);

	DWORD_PTR mask = 1;
	SetThreadAffinityMask(GetCurrentThread(), mask);

	DWORD dwError;
	if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
	{
		dwError = GetLastError();
		SFG_ERR("Failed setting process priority: {0}", dwError);
	}

	/*

		HANDLE pipe;

		pipe = CreateNamedPipeA(PIPE_NAME.c_str(),					   // Pipe name
								PIPE_ACCESS_DUPLEX,					   // Read/write access
								PIPE_TYPE_MESSAGE |					   // Message-based pipe
									PIPE_READMODE_MESSAGE | PIPE_WAIT, // Blocking mode
								1,									   // Max instances
								1024,								   // Out buffer size
								1024,								   // In buffer size
								0,									   // Default timeout
								NULL								   // Default security attributes
		);

		if (pipe == INVALID_HANDLE_VALUE)
		{
			DWORD dwError = GetLastError();
			SFG_ERR("Failed to create named pipe: {0}", dwError);
			return 1;
		}

		SFG_PROG("Waiting for a pipe connection...");
		if (!ConnectNamedPipe(pipe, NULL))
		{
			DWORD dwError = GetLastError();
			CloseHandle(pipe);
			return 1;
		}

		SFG_PROG("Pipe connected...");
		SFG::process::set_pipe_handle(pipe);

	*/

	PUSH_MEMORY_CATEGORY("General");

	{
		SFG::game_app app;

		try
		{
			app.init(SFG::vector2ui16(1920, 1080));
		}
		catch (std::exception e)
		{
			MessageBox(nullptr, e.what(), "Error", MB_OK | MB_ICONERROR);
			FreeConsole();
			return 0;
		}

		app.tick();
		app.uninit();
	}

	POP_MEMORY_CATEGORY();

	/*
		FlushFileBuffers(pipe);
		DisconnectNamedPipe(pipe);
		CloseHandle(pipe);
	*/

	FreeConsole();

	return 0;
}
