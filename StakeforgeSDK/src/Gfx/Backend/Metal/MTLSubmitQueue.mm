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

#include "SFG/Gfx/Backend/Metal/MTLSubmitQueue.hpp"
#include "SFG/Gfx/Common/QueueDesc.hpp"
#include <Metal/Metal.h>

namespace SFG
{
    void MTLSubmitQueue::Create(const QueueDesc &desc)
    {
        id<MTLDevice> device = static_cast<id<MTLDevice>>(m_device);
        id<MTLCommandQueue> q = [device newCommandQueue];
        [q retain];
        
        NSString* debugString = [NSString stringWithUTF8String:desc.debugName];
        [q setLabel:debugString];
        
        m_queue = static_cast<void*>(q);
        m_queueType = desc.type;
    }
    
    void MTLSubmitQueue::Destroy()
    {
        id<MTLCommandQueue> q = static_cast<id<MTLCommandQueue>>(m_queue);
        [q release];
        m_queue = nullptr;
    }
} // namespace SFG
