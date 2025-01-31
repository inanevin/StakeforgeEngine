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

#include "Editor.hpp"
#include "SFG/System/Plugin.hpp"
#include "StakeforgeEditorAPI.hpp"

#ifdef SFG_PLATFORM_WINDOWS

extern "C" SFGEDITOR_API SFG::Plugin* CreatePlugin(const char* path, void* platformHandle, SFG::App* app)
{
	return new SFG::Editor(path, app, platformHandle);
}

extern "C" SFGEDITOR_API void DestroyPlugin(SFG::Plugin* plugin)
{
	SFG::Editor* editor = static_cast<SFG::Editor*>(plugin);
	delete editor;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,	 // handle to DLL module
					DWORD	  fdwReason, // reason for calling function
					LPVOID	  lpReserved)	 // reserved
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: {

		break;
	}
	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}

	// Successful. If this is FALSE, the process will be terminated eventually
	// https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-entry-point-function#entry-point-function-return-value
	return TRUE;
}

#else

extern "C" SFG::Plugin* CreatePlugin(const SFG::String& path, void* platformHandle, SFG::App* app)
{
	return new SFG::Editor(path, app, platformHandle);
}

extern "C" void DestroyPlugin(SFG::Plugin* plugin)
{
	SFG::Editor* editor = static_cast<SFG::Editor*>(plugin);
	delete editor;
}

#endif

namespace SFG
{
	void Editor::OnLoaded()
	{
	}

	void Editor::OnUnloaded()
	{
	}
} // namespace SFG