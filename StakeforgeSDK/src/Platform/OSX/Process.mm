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
#include "SFG/System/Plugin.hpp"

#include <Cocoa/Cocoa.h>
#include <dlfcn.h>


namespace SFG
{
    void Process::PumpOSMessages()
    {
        NSEvent* ev;
        do
        {
            ev = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES];
            if (ev)
            {
                // handle events here
                [NSApp sendEvent:ev];
                [NSApp updateWindows];
            }
        } while (ev);
    }
    
    void Process::OpenURL(const char* url)
    {
        const String urlStr = url;
        const String command = "open " + urlStr;
        system(command.c_str());
    }
    
    Plugin* Process::LoadPlugin(const char* path, App* app)
    {
        void* handle = dlopen(path, RTLD_LAZY);
        if (!handle)
        {
            SFG_ERR("Could not find plugin! {}", path);
            return nullptr;
        }

        using CreatePluginFunc = Plugin*(*)(const String&, void*, App*);
        CreatePluginFunc createPluginAddr = (CreatePluginFunc)dlsym(handle, "CreatePlugin");
        if (!createPluginAddr)
        {
            SFG_ERR("Could not load plugin create function! {}", path);
            dlclose(handle);
            return nullptr;
        }

        Plugin* plugin = createPluginAddr(path, handle, app);
        if (plugin)
            plugin->OnLoaded();
        
        return plugin;
    }
    
    void Process::UnloadPlugin(Plugin* plugin)
    {
        if (!plugin)
        {
            SFG_ERR("Plugin is null, cannot unload.");
            return;
        }

        void* handle = plugin->GetPlatformHandle();
        if (!handle)
        {
            SFG_ERR("Could not find the plugin to unload! {}", plugin->GetPath());
            return;
        }

        plugin->OnUnloaded();
        
        using DestroyPluginFunc = void(*)(Plugin*);
        DestroyPluginFunc destroyPluginAddr = (DestroyPluginFunc)dlsym(handle, "DestroyPlugin");
        if (destroyPluginAddr)
            destroyPluginAddr(plugin);

        dlclose(handle);
    }
    
} // namespace SFG
