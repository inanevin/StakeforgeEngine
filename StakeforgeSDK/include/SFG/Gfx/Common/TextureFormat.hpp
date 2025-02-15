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

#pragma once

namespace SFG
{
    enum class TextureFormat
    {
        UNDEFINED = 0,

        // 8 bit
        R8_SINT,
        R8_UINT,
        R8_UNORM,
        R8_SNORM,

        R8G8_SINT,
        R8G8_UINT,
        R8G8_UNORM,
        R8G8_SNORM,

        R8G8B8A8_SINT,
        R8G8B8A8_UINT,
        R8G8B8A8_UNORM,
        R8G8B8A8_SNORM,
        R8G8B8A8_SRGB,

        B8G8R8A8_UNORM,
        B8G8R8A8_SRGB,

        // 16 bit
        R16_SINT,
        R16_UINT,
        R16_UNORM,
        R16_SNORM,
        R16_SFLOAT,

        R16G16_SINT,
        R16G16_UINT,
        R16G16_UNORM,
        R16G16_SNORM,
        R16G16_SFLOAT,

        R16G16B16A16_SINT,
        R16G16B16A16_UINT,
        R16G16B16A16_UNORM,
        R16G16B16A16_SNORM,
        R16G16B16A16_SFLOAT,

        // 32 bit
        R32_SINT,
        R32_UINT,
        R32_SFLOAT,

        R32G32_SINT,
        R32G32_UINT,
        R32G32_SFLOAT,

        R32G32B32_SFLOAT,
        R32G32B32_SINT,
        R32G32B32_UINT,

        R32G32B32A32_SINT,
        R32G32B32A32_UINT,
        R32G32B32A32_SFLOAT,

        // depth-stencil
        D32_SFLOAT,
        D24_UNORM_S8_UINT,
        D16_UNORM,

        // Misc
        R11G11B10_SFLOAT,
        R10G0B10A2_INT,
        BC3_BLOCK_SRGB,
        BC3_BLOCK_UNORM,
        FORMAT_MAX,
    };
}; // namespace SFG
