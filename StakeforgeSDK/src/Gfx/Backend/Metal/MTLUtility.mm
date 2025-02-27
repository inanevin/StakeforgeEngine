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

#include "SFG/Gfx/Backend/Metal/MTLUtility.hpp"
#include "SFG/Gfx/Common/TextureFormat.hpp"
#include "SFG/Gfx/Common/SamplerDesc.hpp"
#include "SFG/Gfx/Common/LoadStoreOp.hpp"

namespace SFG
{
    MTLPixelFormat MTLUtility::GetFormat(TextureFormat format)
    {
        switch(format)
        {
            case TextureFormat::UNDEFINED:
                return MTLPixelFormatInvalid;

                // 8 bit
            case TextureFormat::R8_SINT:
                return MTLPixelFormatR8Sint;
            case TextureFormat::R8_UINT:
                return MTLPixelFormatR8Uint;
            case TextureFormat::R8_UNORM:
                return MTLPixelFormatR8Unorm;
            case TextureFormat::R8_SNORM:
                return MTLPixelFormatR8Snorm;

            case TextureFormat::R8G8_SINT:
                    return MTLPixelFormatRG8Sint;
            case TextureFormat::R8G8_UINT:
                    return MTLPixelFormatRG8Uint;
            case TextureFormat::R8G8_UNORM:
                    return MTLPixelFormatRG8Unorm;
            case TextureFormat::R8G8_SNORM:
                    return MTLPixelFormatRG8Snorm;

            case TextureFormat::R8G8B8A8_SINT:
                    return MTLPixelFormatRGBA8Sint;
            case TextureFormat::R8G8B8A8_UINT:
                    return MTLPixelFormatRGBA8Uint;
            case TextureFormat::R8G8B8A8_UNORM:
                    return MTLPixelFormatRGBA8Unorm;
            case TextureFormat::R8G8B8A8_SNORM:
                    return MTLPixelFormatRGBA8Snorm;
            case TextureFormat::R8G8B8A8_SRGB:
                    return MTLPixelFormatRGBA8Unorm_sRGB;

            case TextureFormat::B8G8R8A8_UNORM:
                    return MTLPixelFormatBGRA8Unorm;
            case TextureFormat::B8G8R8A8_SRGB:
                    return MTLPixelFormatBGRA8Unorm_sRGB;

                // 16 bit
            case TextureFormat::R16_SINT:
                    return MTLPixelFormatR16Sint;
            case TextureFormat::R16_UINT:
                    return MTLPixelFormatR16Uint;
            case TextureFormat::R16_UNORM:
                    return MTLPixelFormatR16Unorm;
            case TextureFormat::R16_SNORM:
                    return MTLPixelFormatR16Snorm;
            case TextureFormat::R16_SFLOAT:
                    return MTLPixelFormatR16Float;

            case TextureFormat::R16G16_SINT:
                    return MTLPixelFormatRG16Sint;
            case TextureFormat::R16G16_UINT:
                    return MTLPixelFormatRG16Uint;
            case TextureFormat::R16G16_UNORM:
                    return MTLPixelFormatRG16Unorm;
            case TextureFormat::R16G16_SNORM:
                    return MTLPixelFormatRG16Snorm;
            case TextureFormat::R16G16_SFLOAT:
                    return MTLPixelFormatRG16Float;

            case TextureFormat::R16G16B16A16_SINT:
                    return MTLPixelFormatRGBA16Sint;
            case TextureFormat::R16G16B16A16_UINT:
                    return MTLPixelFormatRGBA16Uint;
            case TextureFormat::R16G16B16A16_UNORM:
                    return MTLPixelFormatRGBA16Unorm;
            case TextureFormat::R16G16B16A16_SNORM:
                    return MTLPixelFormatRGBA16Snorm;
            case TextureFormat::R16G16B16A16_SFLOAT:
                    return MTLPixelFormatRGBA16Float;

                // 32 bit
            case TextureFormat::R32_SINT:
                    return MTLPixelFormatR32Sint;
            case TextureFormat::R32_UINT:
                    return MTLPixelFormatR32Uint;
            case TextureFormat::R32_SFLOAT:
                    return MTLPixelFormatR32Float;

            case TextureFormat::R32G32_SINT:
                    return MTLPixelFormatRG32Sint;
            case TextureFormat::R32G32_UINT:
                    return MTLPixelFormatRG32Uint;
            case TextureFormat::R32G32_SFLOAT:
                    return MTLPixelFormatRG32Float;

            case TextureFormat::R32G32B32_SFLOAT:
                    return MTLPixelFormatInvalid;
            case TextureFormat::R32G32B32_SINT:
                    return MTLPixelFormatInvalid;
            case TextureFormat::R32G32B32_UINT:
                    return MTLPixelFormatInvalid;

            case TextureFormat::R32G32B32A32_SINT:
                    return MTLPixelFormatRGBA32Sint;
            case TextureFormat::R32G32B32A32_UINT:
                    return MTLPixelFormatRGBA32Uint;
            case TextureFormat::R32G32B32A32_SFLOAT:
                    return MTLPixelFormatRGBA32Float;
                    
                // depth-stencil
            case TextureFormat::D32_SFLOAT:
                    return MTLPixelFormatDepth32Float;
            case TextureFormat::D24_UNORM_S8_UINT:
                    return MTLPixelFormatDepth32Float_Stencil8;
                case TextureFormat::D16_UNORM:
                    return MTLPixelFormatDepth16Unorm;

                // misc
            case TextureFormat::R11G11B10_SFLOAT:
                    return MTLPixelFormatRG11B10Float;
            case TextureFormat::R10G0B10A2_INT:
                return MTLPixelFormatRGB10A2Uint;
            case TextureFormat::BC3_BLOCK_SRGB:
                    return MTLPixelFormatBC3_RGBA_sRGB;
            case TextureFormat::BC3_BLOCK_UNORM:
                return MTLPixelFormatBC3_RGBA;
            default:
                    return MTLPixelFormatInvalid;
        }

        return MTLPixelFormatInvalid;
    }

    MTLSamplerMipFilter MTLUtility::GetSamplerMipmapMode(SamplerMipmapMode samplerMipMapMode)
    {
        switch(samplerMipMapMode)
        {
            case SamplerMipmapMode::Linear:
                return MTLSamplerMipFilterLinear;
            case SamplerMipmapMode::Nearest:
                return MTLSamplerMipFilterNearest;
                
        }
    }
    
    MTLSamplerMinMagFilter MTLUtility::GetSamplerFilter(SamplerFilter samplerFilter)
    {
        switch(samplerFilter)
        {
            case SamplerFilter::Anisotropic:
            case SamplerFilter::Linear:
                return MTLSamplerMinMagFilterLinear;
            case SamplerFilter::Nearest:
                return MTLSamplerMinMagFilterNearest;
                
        }
    }
    
    MTLSamplerAddressMode MTLUtility::GetSamplerAddressMode(SamplerAddressMode samplerAddressMode)
    {
        switch(samplerAddressMode)
        {
            case SamplerAddressMode::ClampToEdge:
                return MTLSamplerAddressModeClampToEdge;
            case SamplerAddressMode::ClampToBorder:
                return MTLSamplerAddressModeClampToBorderColor;
            case SamplerAddressMode::MirrorClampToEdge:
                return MTLSamplerAddressModeMirrorClampToEdge;
            case SamplerAddressMode::Repeat:
                return MTLSamplerAddressModeRepeat;
            case SamplerAddressMode::MirroredRepeat:
                return MTLSamplerAddressModeMirrorRepeat;
                
        }
    }

    MTLLoadAction MTLUtility::GetLoadAction(LoadOp op)
    {
        switch(op)
        {
            case LoadOp::Clear:
                return MTLLoadActionClear;
            case LoadOp::Load:
                return MTLLoadActionLoad;
            default:
                return MTLLoadActionDontCare;
        }
    }

    MTLStoreAction MTLUtility::GetStoreAction(StoreOp op)
    {
        switch(op)
        {
            case StoreOp::Store:
                return MTLStoreActionStore;
            default:
                return MTLStoreActionDontCare;
        }
    }

} // namespace SFG
