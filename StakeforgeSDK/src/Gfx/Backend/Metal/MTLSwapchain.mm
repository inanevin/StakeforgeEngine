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

#include "SFG/Gfx/Backend/Metal/MTLSwapchain.hpp"
#include "SFG/Gfx/Backend/Metal/MTLUtility.hpp"
#include "SFG/Gfx/Common/SwapchainDesc.hpp"
#include "SFG/Gfx/Common/GfxConstants.hpp"
#include <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Cocoa/Cocoa.h>

namespace SFG
{
    void MTLSwapchain::Create(const SwapchainDesc& desc)
    {
        id<MTLDevice> device = static_cast<id<MTLDevice>>(m_device);
        CAMetalLayer* layer = [CAMetalLayer layer];
        [layer retain];
        layer.device = device;
        layer.pixelFormat = MTLUtility::GetFormat(desc.format);
        layer.maximumDrawableCount = BACK_BUFFER_COUNT;
        
        m_layer = static_cast<void*>(layer);
        
        CGSize sz;
        sz.width = desc.width * desc.scalingFactor;
        sz.height = desc.height * desc.scalingFactor;
        layer.drawableSize = sz;
        layer.contentsScale = desc.scalingFactor;
        
        NSView* view = static_cast<NSView*>(desc.osHandle);
        [view setWantsLayer:YES];
        [view setLayer:layer];
    }

    void MTLSwapchain::Destroy()
    {
        CAMetalLayer* layer = static_cast<CAMetalLayer*>(m_layer);
        [layer release];
        m_layer = nullptr;
    }

    void MTLSwapchain::Recreate(const SwapchainDesc &desc)
    {
        CAMetalLayer* layer = static_cast<CAMetalLayer*>(m_layer);
        const CGSize sz = NSMakeSize(desc.width * desc.scalingFactor, desc.height * desc.scalingFactor);
        const CGRect rect = NSMakeRect(0, 0, desc.width, desc.height);
        [layer setDrawableSize:sz];
        [layer setContentsScale:desc.scalingFactor];
    }
 
} // namespace SFG
