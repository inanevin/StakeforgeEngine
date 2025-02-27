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

#include "SFG/Gfx/Backend/Metal/MTLSemaphore.hpp"
#include "SFG/Platform/Time.hpp"

#include <Metal/Metal.h>

namespace SFG
{
    void MTLSemaphore::Create()
    {
        id<MTLDevice> device = static_cast<id<MTLDevice>>(m_device);
        id<MTLSharedEvent> ev = [device newSharedEvent];
        [ev retain];
        m_semaphore = static_cast<void*>(ev);
    }

    void MTLSemaphore::Destroy()
    {
        id<MTLSharedEvent> sem = static_cast<id<MTLSharedEvent>>(m_semaphore);
        [sem release];
        m_semaphore = nullptr;
    }

    void MTLSemaphore::Wait(uint64 value, uint32 sleepMS)
    {
        id<MTLSharedEvent> sem = static_cast<id<MTLSharedEvent>>(m_semaphore);
        while(sem.signaledValue < value)
            Time::Sleep(sleepMS);
    }

    void MTLSemaphore::EncodeWait(void *buffer, uint64 value)
    {
        id<MTLCommandBuffer> mtlBuffer = static_cast<id<MTLCommandBuffer>>(buffer);
        id<MTLSharedEvent> sem = static_cast<id<MTLSharedEvent>>(m_semaphore);
        [mtlBuffer encodeWaitForEvent:sem value:value];
    }

    void MTLSemaphore::EncodeSignal(void *buffer, uint64 value)
    {
        id<MTLCommandBuffer> mtlBuffer = static_cast<id<MTLCommandBuffer>>(buffer);
        id<MTLSharedEvent> sem = static_cast<id<MTLSharedEvent>>(m_semaphore);
        [mtlBuffer encodeSignalEvent:sem value:value];
    }
} // namespace SFG
