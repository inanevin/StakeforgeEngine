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

#include "SFG/Type/SizeDefinitions.hpp"

namespace SFG
{

enum class TextureFormat;

    enum class PolygonMode
    {
        Fill,
        Line,
        Point
    };
    
    enum class CullMode
    {
        None,
        Front,
        Back,
    };
    
    enum class FrontFace
    {
        CCW,
        CW,
    };

    enum class Topology
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        TriangleFan,
        TriangleListAdjacency,
        TriangleStripAdjacency,
    };
    
    enum class LogicOp
    {
        Clear,
        And,
        AndReverse,
        Copy,
        AndInverted,
        NoOp,
        XOR,
        OR,
        NOR,
        Equivalent
    };

    enum class BlendOp
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };
    
    enum class BlendFactor
    {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
    };

    enum ColorComponentFlags
    {
        CCF_R = 1 << 0,
        CCF_G = 1 << 1,
        CCF_B = 1 << 2,
        CCF_A = 1 << 3,
    };

    struct ColorBlendAttachment
    {
        bool                            blendEnabled        = false;
        BlendFactor                     srcColorBlendFactor = BlendFactor::SrcAlpha;
        BlendFactor                     dstColorBlendFactor = BlendFactor::OneMinusSrcAlpha;
        BlendOp                         colorBlendOp        = BlendOp::Add;
        BlendFactor                     srcAlphaBlendFactor = BlendFactor::One;
        BlendFactor                     dstAlphaBlendFactor = BlendFactor::Zero;
        BlendOp                         alphaBlendOp        = BlendOp::Add;
        uint32 componentFlags = ColorComponentFlags::CCF_R | ColorComponentFlags::CCF_G | ColorComponentFlags::CCF_B | ColorComponentFlags::CCF_A;
    };

    struct ShaderColorAttachment
    {
        TextureFormat               format          = {};
        ColorBlendAttachment blendAttachment = {};
    };

    struct ShaderDesc
    {
        const char* debugName = "";
        ShaderColorAttachment* colorAttachments = nullptr;
        PolygonMode polygonMode = PolygonMode::Fill;
        CullMode cullMode = CullMode::Back;
        FrontFace frontFace = FrontFace::CCW;
        Topology topology = Topology::TriangleList;
        LogicOp logicOp = LogicOp::Copy;
        uint32 samples = 1;
        uint32 colorAttachmentCount = 0;
        float depthBiasConstant = 0.0f;
        float depthBiasClamp = 0.0f;
        float depthBiasSlope = 0.0f;
        bool enableSampleShading = false;
        bool enableBlendLogicOp = false;
        bool enableDepthBias = false;
        bool enableAlphaToCoverage = false;
        
    };
}; // namespace SFG
