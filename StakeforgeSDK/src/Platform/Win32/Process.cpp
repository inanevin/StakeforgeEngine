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

#include "SFG/Platform/Process.hpp"
#include "SFG/IO/Log.hpp"
#include "SFG/Core/App.hpp"
#include "SFG/System/Plugin.hpp"

namespace SFG
{
	void Process::PumpOSMessages()
	{
		MSG msg	   = {0};
		msg.wParam = 0;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	typedef Plugin*(__cdecl* CreatePluginFunc)(const char* path, App* app, void* handle);
	typedef void(__cdecl* DestroyPluginFunc)(Plugin*);

	Plugin* Process::LoadPlugin(const char* path, App* app)
	{
		HINSTANCE hinstLib;
		BOOL	  fFreeResult = FALSE;
		hinstLib			  = LoadLibrary(TEXT(path));
		Plugin* plugin		  = nullptr;

		// If the handle is valid, try to get the function address.
		if (hinstLib != NULL)
		{
			CreatePluginFunc createPluginAddr = (CreatePluginFunc)GetProcAddress(hinstLib, "CreatePlugin");

			// If the function address is valid, call the function.

			if (NULL != createPluginAddr)
			{
				plugin = (createPluginAddr)(path, app, hinstLib);
				plugin->OnLoaded();
			}
			else
			{
				SFG_ERR("Could not load plugin create function! {0}", path);
			}
		}
		else
		{
			SFG_ERR("Could not find plugin! {0}", path);
		}

		return plugin;
	}

	void Process::UnloadPlugin(Plugin* plugin)
	{
		HINSTANCE hinstLib = NULL;

		plugin->OnUnloaded();
		hinstLib = static_cast<HINSTANCE>(plugin->GetPlatformHandle());

		if (hinstLib == NULL)
		{
			SFG_ERR("Could not find the plugin to unload! {0}", plugin->GetPath());
			return;
		}

		DestroyPluginFunc destroyPluginAddr = (DestroyPluginFunc)GetProcAddress(hinstLib, "DestroyPlugin");
		if (destroyPluginAddr != NULL)
			destroyPluginAddr(plugin);

		BOOL fFreeResult = FreeLibrary(hinstLib);
	}

	void Process::OpenURL(const char* url)
	{
		ShellExecute(0, "open", url, NULL, NULL, SW_SHOWNORMAL);
	}

} // namespace SFG
