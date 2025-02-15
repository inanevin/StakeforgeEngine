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

#include "SFG/Gfx/Backend/Metal/MTLBackend.hpp"
#include "SFG/IO/Log.hpp"

#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Cocoa/Cocoa.h>

namespace SFG
{

	void MTLBackend::Create(String& errString)
	{
		SFG_INFO("************* Initializing Metal backend... *************");
        
        /*
            Device enumeration and selection.
         */
        NSArray<id<MTLDevice>> *availableDevices = MTLCopyAllDevices();
        id<MTLDevice> selectedDevice = nil;

        for (id<MTLDevice> device in availableDevices) {
            
            SFG_INFO("Metal: Device found: {0}, Architecture: {1}, ID: {2} Is Low Power: {3}", device.name.UTF8String, device.architecture.name.UTF8String, device.registryID, device.isLowPower);
            
            if(!device.isLowPower)
                selectedDevice = device;
            break;  // Found a device that meets our criteria
        }
        
        [availableDevices release];
        
        if(selectedDevice == nil)
           selectedDevice = MTLCreateSystemDefaultDevice();

        if(!selectedDevice)
        {
            errString = "Metal: Failed creating Metal device!";
            return;
        }
        
        [selectedDevice retain];
        
        SFG_INFO("Metal: Selected device: {0}", selectedDevice.name.UTF8String);
        m_device = static_cast<void*>(selectedDevice);
	
		SFG_INFO("************* Successfuly initialized Metal backend. *************");
	}

	void MTLBackend::Destroy()
	{
        if(m_device)
        {
            id<MTLDevice> device = static_cast<id<MTLDevice>>(m_device);
            [device release];
        }
	}
} // namespace SFG
