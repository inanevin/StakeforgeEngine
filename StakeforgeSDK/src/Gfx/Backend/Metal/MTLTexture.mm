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

#include "SFG/Gfx/Backend/Metal/MTLTexture.hpp"
#include "SFG/Gfx/Backend/Metal/MTLUtility.hpp"
#include "SFG/Gfx/Common/TextureDesc.hpp"
#include "SFG/Gfx/Common/TextureFlags.hpp"
#include "SFG/Gfx/Common/TextureView.hpp"
#include <Metal/Metal.h>

namespace SFG
{
    void MTLTexture::Create(const TextureDesc& desc)
    {
        id<MTLDevice> device = static_cast<id<MTLDevice>>(m_device);
        
        const MTLPixelFormat format = MTLUtility::GetFormat(desc.format);
        
        /* Descriptor based on properties */
        MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:format width:desc.width height:desc.height mipmapped:desc.mipLevels > 1];
        textureDescriptor.mipmapLevelCount = desc.mipLevels;
        textureDescriptor.storageMode = MTLStorageModePrivate;
        textureDescriptor.arrayLength = desc.arrayLevels;
        
        if(desc.type == TextureType::Texture1D)
            textureDescriptor.textureType = desc.arrayLevels == 1 ? MTLTextureType1D : MTLTextureType1DArray;
        else if(desc.type == TextureType::Texture2D)
            textureDescriptor.textureType = desc.arrayLevels == 1 ? (desc.sampleCount > 1 ? MTLTextureType2DMultisample : MTLTextureType2D) : (desc.sampleCount > 1 ? MTLTextureType2DMultisampleArray : MTLTextureType2DArray);
        else if(desc.type == TextureType::Texture3D)
            textureDescriptor.textureType = MTLTextureType3D;
        
        /* Usage based on flags */
        MTLTextureUsage usage;
        if(desc.flags & TextureFlags::TEXTURE_FLAGS_SAMPLED)
            textureDescriptor.usage |= MTLTextureUsageShaderRead;
        if((desc.flags & TextureFlags::TEXTURE_FLAGS_COLOR_ATT) || (desc.flags & TextureFlags::TEXTURE_FLAGS_DEPTH_ATT) || (desc.flags & TextureFlags::TEXTURE_FLAGS_STENCIL_ATT))
            textureDescriptor.usage |= MTLTextureUsageRenderTarget;
        
        textureDescriptor.sampleCount = desc.sampleCount;
        
        /* Create texture*/
        id<MTLTexture> texture = [device newTextureWithDescriptor:textureDescriptor];
        m_texture = static_cast<void*>(texture);
        
        /* Create views */
        m_views.resize(static_cast<size_t>(desc.viewCount));
        for(uint32 i = 0; i < desc.viewCount; i++)
        {
            TextureView& view = desc.views[i];
            
            NSRange mipRng = NSMakeRange(view.baseMipLevel, view.mipLevels == 0 ? desc.mipLevels - view.baseMipLevel : view.mipLevels);
            NSRange layerRng = NSMakeRange(view.baseArrayLevel, view.arrayLevels == 0 ? desc.arrayLevels - view.baseArrayLevel : view.arrayLevels);

            MTLTextureType type = view.isCubemap ? MTLTextureTypeCube : textureDescriptor.textureType;
            
            m_views[i] = static_cast<void*>([texture newTextureViewWithPixelFormat:format textureType:type levels:mipRng slices:layerRng]);
            
        }
       
        NSString *debugNSString = [NSString stringWithUTF8String:desc.name];
        [texture setLabel:debugNSString];
    }

    void MTLTexture::Destroy()
    {
        for(void* view : m_views)
        {
            id<MTLTexture> mtlView = static_cast<id<MTLTexture>>(view);
            [mtlView release];
        }
        
        id<MTLTexture> mtlTexture = static_cast<id<MTLTexture>>(m_texture);
        [mtlTexture release];
        m_texture = nullptr;
        m_views.resize(0);
    }
} // namespace SFG
