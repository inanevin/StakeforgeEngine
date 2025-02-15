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

#include "SFG/Gfx/Backend/Metal/MTLSampler.hpp"
#include "SFG/Gfx/Backend/Metal/MTLUtility.hpp"
#include "SFG/Gfx/Common/SamplerDesc.hpp"

#include <Metal/Metal.h>

namespace SFG
{
    void MTLSampler::Create(const SamplerDesc& desc)
{
        id<MTLDevice> device = static_cast<id<MTLDevice>>(m_device);
        
        MTLSamplerDescriptor *samplerDesc = [[MTLSamplerDescriptor alloc] init];
        samplerDesc.mipFilter = MTLUtility::GetSamplerMipmapMode(desc.mipmapMode);
        samplerDesc.minFilter = MTLUtility::GetSamplerFilter(desc.minFilter);
        samplerDesc.magFilter = MTLUtility::GetSamplerFilter(desc.magFilter);
        samplerDesc.rAddressMode = samplerDesc.sAddressMode = samplerDesc.tAddressMode = MTLUtility::GetSamplerAddressMode(desc.mode);
        samplerDesc.lodMinClamp = desc.minLod;
        samplerDesc.maxAnisotropy = desc.anisotropy;
        samplerDesc.lodMaxClamp = desc.maxLod;
        // samplerDesc.supportArgumentBuffers = true;
                
        if(samplerDesc.maxAnisotropy < 1)
            samplerDesc.maxAnisotropy = 1;
        else if(samplerDesc.maxAnisotropy > 16)
            samplerDesc.maxAnisotropy = 16;
        
        NSString* debugString = [NSString stringWithUTF8String:desc.debugName];
        [samplerDesc setLabel:debugString];
        
        id<MTLSamplerState> sampler = [device newSamplerStateWithDescriptor:samplerDesc];
        [sampler retain];
        m_sampler = static_cast<void*>(sampler);
        [samplerDesc release];
    }

    void MTLSampler::Destroy()
    {
        id<MTLSamplerState> sampler = static_cast<id<MTLSamplerState>>(m_sampler);
        [sampler release];
        m_sampler = nullptr;
    }
} // namespace SFG
