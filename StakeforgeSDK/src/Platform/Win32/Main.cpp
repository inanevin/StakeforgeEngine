/*
This file is a part of: Stakeforge Engine
https://github.com/inanevin/StakeforgeEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2025-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "SFG/Platform/Main.hpp"
#include "SFG/Data/String.hpp"
#include "SFG/IO/Log.hpp"
#include "SFG/Core/App.hpp"

namespace SFG
{
    SFGAppInit* g_sfgAppInit = nullptr;
    SFGAppShutdown* g_sfgAppShutdown = nullptr;
}

#define WINDOWS_MEAN_AND_LEAN
#include <Windows.h>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR pCmdLine, _In_ int nCmdShow)
{

#ifdef SFG_DEBUG
    bool consoleAllocated = false;
    if (AllocConsole() == FALSE)
    {
        consoleAllocated = true;
        SFG_ERR("Failed allocating console!");
    }
#endif

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

    SFG::String error = "";

    SFG::App* app = new SFG::App(error);
    
    if(SFG::g_sfgAppInit)
        SFG::g_sfgAppInit(app);

    if (!error.empty())
    {
        MessageBox(nullptr, error.c_str(), "Error", MB_OK | MB_ICONERROR);
        delete app;
        FreeConsole();
        return 0;
    }

    app->Tick();
    
    if(SFG::g_sfgAppShutdown)
        SFG::g_sfgAppShutdown(app);
    
    delete app;

#ifdef SFG_DEBUG
    if (consoleAllocated)
        FreeConsole();
#endif

    return 0;
}
