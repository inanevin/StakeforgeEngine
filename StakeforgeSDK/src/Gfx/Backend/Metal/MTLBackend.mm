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
#include "SFG/Gfx/Backend/Metal/MTLUtility.hpp"
#include "SFG/Gfx/Common/QueueDesc.hpp"
#include "SFG/Gfx/RenderFrame.hpp"
#include "SFG/Gfx/GfxResources.hpp"
#include "SFG/Gfx/CommandStream.hpp"
#include "SFG/Gfx/Commands/CommandType.hpp"
#include "SFG/Gfx/Commands/CMDRenderPass.hpp"
#include "SFG/Gfx/Commands/CMDBindPipeline.hpp"
#include "SFG/Gfx/Commands/CMDCopy.hpp"
#include "SFG/Gfx/Commands/CMDDraw.hpp"
#include "SFG/IO/Log.hpp"
#include "SFG/Data/VectorUtil.hpp"

#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Cocoa/Cocoa.h>
#include <dispatch/dispatch.h>

namespace SFG
{

	void MTLBackend::Initialize(String& errString, GfxResources* resources, uint32 maxCommandStreams)
	{
		SFG_INFO("************* Initializing Metal backend... *************");
        
        m_resources = resources;
        m_commandData.resize(maxCommandStreams);
        
        for(CommandData& data : m_commandData)
        {
            data.renderEncoders.reserve(10);
            data.computeEncoders.reserve(4);
            data.blitEncoders.reserve(10);
            data.swapchains.reserve(4);
        }
        
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

	void MTLBackend::Shutdown()
	{
        if(m_device)
        {
            id<MTLDevice> device = static_cast<id<MTLDevice>>(m_device);
            [device release];
        }
	}

    
    void MTLBackend::StartFrame()
    {
        m_frameIndex = (m_frameIndex + 1) % FRAMES_IN_FLIGHT;
        PerFrameData& pfd = m_pfd[m_frameIndex];
        
        // Busy spin.
        while(pfd.reachedSubmits.load() < pfd.totalSubmits)
        {
            
        }
        
    }
    
    void MTLBackend::Join()
    {
        for(uint8 i = 0; i <FRAMES_IN_FLIGHT; i++)
        {
            PerFrameData& pfd = m_pfd[i];
            while(pfd.reachedSubmits.load() < pfd.totalSubmits)
            {
                
            }
        }
    }

    void MTLBackend::Render(const RenderFrame &frame)
    {
        @autoreleasepool {
            StartFrame();
            
            const uint32 submissionsCount = frame.GetSubmissionCount();
            const RenderFrame::SubmitDesc* submissions = frame.GetSubmissions();
            
            for(uint32 i = 0; i < submissionsCount; i++)
            {
                const RenderFrame::SubmitDesc& submit = submissions[i];
                const uint8 streamCount = submit.streamsCount;
                CommandStream** streams = submit.streams;
                
                GfxQueue& submitQueue = m_resources->GetQueue(submit.queue);
                id<MTLCommandQueue> mtlQueue = static_cast<id<MTLCommandQueue>>(submitQueue.GetGPU());
                
                /* Execute each command stream in parallel per submission. */
                dispatch_group_t group = dispatch_group_create();
                dispatch_queue_t queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
                
                for (uint8_t j = 0; j < streamCount; ++j) {
                    dispatch_group_async(group, queue, ^{
                        CommandStream* stream = streams[j];
                        const uint32 commandsCount = stream->GetCommandsCount();
                        if(commandsCount == 0)
                            return;
                        
                        
                        id<MTLCommandBuffer> buffer = [mtlQueue commandBuffer];
                        [buffer retain];
                        
                        CommandData& cmdData = m_commandData[j];
                        ResetCommandData(cmdData);
                        cmdData.buffer = buffer;
                      
                        
                        uint8* head = stream->GetCommandsRaw();
                        uint32 consumedCommands = 0;
                        
                        auto endCurrentComputeEncoder = [&](){
                            id<MTLComputeCommandEncoder> computeEncoder = static_cast<id<MTLComputeCommandEncoder>>(cmdData.currentComputeEncoder);
                            [computeEncoder endEncoding];
                            cmdData.computeEncoders.push_back(cmdData.currentComputeEncoder);
                            cmdData.currentComputeEncoder = nullptr;
                        };
                        
                        auto endCurrentBlitEncoder = [&] () {
                            id<MTLBlitCommandEncoder> blitEncoder = static_cast<id<MTLBlitCommandEncoder>>(cmdData.currentBlitEncoder);
                            [blitEncoder endEncoding];
                            cmdData.blitEncoders.push_back(cmdData.currentBlitEncoder);
                            cmdData.currentBlitEncoder = nullptr;
                        };
                        
                        auto createComputeEncoder = [&](){
                            id<MTLComputeCommandEncoder> computeEncoder = [buffer computeCommandEncoder];
                            [computeEncoder retain];
                            cmdData.currentComputeEncoder = computeEncoder;
                        };
                        
                        auto createBlitEncoder = [&](){
                            id<MTLBlitCommandEncoder> blitEncoder = [buffer blitCommandEncoder];
                            [blitEncoder retain];
                            cmdData.currentBlitEncoder = blitEncoder;
                        };
                        
                        while(consumedCommands != commandsCount)
                        {
                            CommandType type = {};
                            SFG_MEMCPY(&type, head, sizeof(CommandType));
                            head += sizeof(CommandType);
                            
                            uint32 commandSize = 0;
                            SFG_MEMCPY(&commandSize, head, sizeof(uint32));
                            head += sizeof(uint32);
                            
                            if(type == CommandType::BeginRenderPass)
                            {
                                CMD_BeginRenderPass(cmdData, head);
                            }
                            else if(type == CommandType::EndRenderPass)
                            {
                                CMD_EndRenderPass(cmdData);
                            }
                            else if(type == CommandType::BindPipeline)
                            {
                                CMD_BindPipeline(cmdData,  head);
                            }
                            else if(type == CommandType::DrawInstanced)
                            {
                                CMD_DrawInstanced(cmdData, head);
                            }
                            else if(type == CommandType::DrawIndexedInstanced)
                            {
                                CMD_DrawIndexedInstanced(cmdData, head);
                            }
                            else if(type == CommandType::CopyBufferToBuffer)
                            {
                                CMD_CopyBufferToBuffer(cmdData, head);
                            }
                            else if(type == CommandType::CopyBufferToTexture)
                            {
                                CMD_CopyBufferToTexture(cmdData, head);
                            }
                            else if(type == CommandType::CopyTextureToBuffer)
                            {
                                CMD_CopyTextureToBuffer(cmdData, head);
                            }
                            
                            head += commandSize;
                            consumedCommands++;
                        }
                    });
                }
                
                dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
                
                auto& pfd = m_pfd[m_frameIndex];
                
                        
                /* Send work for each buffer */
                for(uint8 j = 0; j < streamCount; j++)
                {
                    CommandStream* stream = streams[j];
                    const uint32 commandsCount = stream->GetCommandsCount();
                    if(commandsCount == 0)
                        return;
                    
                    CommandData& cmdData = m_commandData[j];
                    id<MTLCommandBuffer> buffer = static_cast<id<MTLCommandBuffer>>(cmdData.buffer);
                    
                    for(uint8 k = 0; k < submit.waitSemaphoresCount; k++)
                    {
                        RenderSemaphore& renderSemaphore = m_resources->GetRenderSemaphore(submit.waitSemaphores[k]);
                        GfxSemaphore& gfxSemaphore = m_resources->GetSemaphore(renderSemaphore.GetGPU(m_frameIndex));
                        gfxSemaphore.EncodeWait(buffer, submit.waitValues[k]);
                    }
                    
                    for(uint8 k = 0; k < submit.signalSemaphoresCount; k++)
                    {
                        RenderSemaphore& renderSemaphore = m_resources->GetRenderSemaphore(submit.signalSemaphores[k]);
                        GfxSemaphore& gfxSemaphore = m_resources->GetSemaphore(renderSemaphore.GetGPU(m_frameIndex));
                        gfxSemaphore.EncodeSignal(buffer, submit.signalValues[k]);
                    }
                    
                    for(void* swpPtr : cmdData.swapchains)
                    {
                        GfxSwapchain* swp = static_cast<GfxSwapchain*>(swpPtr);
                        swp->Present(buffer);
                    }
                    
                    /* Bump total submits for each submission, then bump reached submits each time its completed. */
                    if(submitQueue.GetType() == QueueType::Graphics)
                    {
                        const uint32 frameIndex = m_frameIndex;
                        pfd.totalSubmits++;
                        
                        [buffer addCompletedHandler:^(id<MTLCommandBuffer> _Nonnull) {
                            m_pfd[frameIndex].reachedSubmits.fetch_add(1);
                        }];
                    }
                    
                    [buffer commit];
                    [buffer release];
                    
                    for(void* encoder : cmdData.renderEncoders)
                    {
                        id<MTLRenderCommandEncoder> mtlEncoder = static_cast<id<MTLRenderCommandEncoder>>(encoder);
                        [mtlEncoder release];
                    }
                    
                    for(void* encoder : cmdData.computeEncoders)
                    {
                        id<MTLComputeCommandEncoder> mtlEncoder = static_cast<id<MTLComputeCommandEncoder>>(encoder);
                        [mtlEncoder release];
                    }
                    
                    for(void* encoder : cmdData.blitEncoders)
                    {
                        id<MTLBlitCommandEncoder> mtlEncoder = static_cast<id<MTLBlitCommandEncoder>>(encoder);
                        [mtlEncoder release];
                    }
                    
                    ResetCommandData(cmdData);
                    
                }
            }
        }

    }

    void MTLBackend::ResetCommandData(CommandData & cmdData)
    {
        cmdData.currentRenderEncoder = nullptr;
        cmdData.currentBlitEncoder = nullptr;
        cmdData.currentComputeEncoder = nullptr;
        cmdData.currentShader = nullptr;
        cmdData.buffer = nullptr;
        cmdData.currentIndexBuffer16Bit = false;
        cmdData.renderEncoders.resize(0);
        cmdData.computeEncoders.resize(0);
        cmdData.blitEncoders.resize(0);
        cmdData.swapchains.resize(0);
    }


    void MTLBackend::CMD_BeginRenderPass(CommandData& data, uint8* head)
    {
        CMDBeginRenderPass* cmd = reinterpret_cast<CMDBeginRenderPass*>(head);
        
        id<MTLCommandBuffer> buffer = static_cast<id<MTLCommandBuffer>>(data.buffer);
        MTLRenderPassDescriptor* passDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];

        
        if(cmd->depthAttachment != nullptr)
        {
            passDescriptor.depthAttachment.loadAction = MTLUtility::GetLoadAction(cmd->depthAttachment->loadOp);
            passDescriptor.depthAttachment.storeAction = MTLUtility::GetStoreAction(cmd->depthAttachment->storeOp);
            passDescriptor.depthAttachment.clearDepth = cmd->depthAttachment->clearValue;
            
            RenderTarget& rt = m_resources->GetRenderTarget(cmd->depthAttachment->renderTarget);
            Handle<uint16> rtTargetHandle = rt.GetGPU(m_frameIndex);
            GfxTexture& texture = m_resources->GetTexture(rtTargetHandle);
            
            id<MTLTexture> mtlTexture = static_cast<id<MTLTexture>>(texture.GetGPUView(cmd->depthAttachment->viewIndex));
            passDescriptor.depthAttachment.texture = mtlTexture;
            
            if(cmd->depthAttachment->resolveMode != ResolveMode::None)
            {
                RenderTarget& resolveRt = m_resources->GetRenderTarget(cmd->depthAttachment->resolveTarget);
                Handle<uint16> resolveRtHandle = resolveRt.GetGPU(m_frameIndex);
                GfxTexture& resolveTxt = m_resources->GetTexture(resolveRtHandle);
                id<MTLTexture> mtlTextureResolve = static_cast<id<MTLTexture>>(resolveTxt.GetGPUView(cmd->depthAttachment->resolveIndex));
                passDescriptor.depthAttachment.storeAction = MTLStoreActionMultisampleResolve;
                passDescriptor.depthAttachment.resolveTexture = mtlTextureResolve;
            }
            
        }
        
        const uint32 colorAttachmentsCount = cmd->colorAttachmentCount;
        ColorAttachment* colorAttachments = cmd->colorAttachments;
        
        for(uint32 i = 0; i < colorAttachmentsCount; i++)
        {
            ColorAttachment& att = colorAttachments[i];
            
            passDescriptor.colorAttachments[i].loadAction = MTLUtility::GetLoadAction(att.loadOp);
            passDescriptor.colorAttachments[i].storeAction = MTLUtility::GetStoreAction(att.storeOp);
            passDescriptor.colorAttachments[i].clearColor = MTLClearColorMake(att.clearColor[0], att.clearColor[1], att.clearColor[2], att.clearColor[3]);
            
            if(att.resolveMode != ResolveMode::None)
            {
                RenderTarget& resolveRt = m_resources->GetRenderTarget(att.resolveTarget);
                Handle<uint16> resolveRtHandle = resolveRt.GetGPU(m_frameIndex);
                GfxTexture& resolveTxt = m_resources->GetTexture(resolveRtHandle);
                id<MTLTexture> mtlTextureResolve = static_cast<id<MTLTexture>>(resolveTxt.GetGPUView(att.resolveIndex));
                passDescriptor.colorAttachments[i].resolveTexture = mtlTextureResolve;
                passDescriptor.colorAttachments[i].storeAction = MTLStoreActionMultisampleResolve;
            }
            
            RenderTarget& targetRt = m_resources->GetRenderTarget(att.renderTarget);
            
            if(targetRt.GetIsSwapchain())
            {
                Handle<uint16> swpHandle = targetRt.GetGPU(0);
                GfxSwapchain& swp = m_resources->GetSwapchain(swpHandle);
                
                const bool contains = UtilVector::Contains(data.swapchains, (void*)&swp);
                
                if(!contains)
                    swp.RequestNextDrawable();
                
                id<CAMetalDrawable> drawable = static_cast<id<CAMetalDrawable>>(swp.GetCurrentDrawable());
                passDescriptor.colorAttachments[i].texture = drawable.texture;
                
                if(!contains)
                    data.swapchains.push_back(&swp);
            }
            else
            {
                Handle<uint16> txtHandle = targetRt.GetGPU(m_frameIndex);
                id<MTLTexture> mtlTxt = static_cast<id<MTLTexture>>(m_resources->GetTexture(txtHandle).GetGPUView(att.viewIndex));
                passDescriptor.colorAttachments[i].texture = mtlTxt;
            }
        }
        
        id<MTLRenderCommandEncoder> encoder = [buffer renderCommandEncoderWithDescriptor:passDescriptor];
        [encoder retain];
        data.currentRenderEncoder = encoder;
        
        
        MTLViewport viewport = {};
        viewport.width = static_cast<double>(cmd->viewport.width);
        viewport.height = static_cast<double>(cmd->viewport.height);
        viewport.originX = static_cast<double>(cmd->viewport.x);
        viewport.originY = static_cast<double>(cmd->viewport.y);
        viewport.znear = static_cast<double>(cmd->viewport.minDepth);
        viewport.zfar = static_cast<double>(cmd->viewport.maxDepth);
        [encoder setViewport:viewport];

        MTLScissorRect scissors = {};
        scissors.width = cmd->scissors.width;
        scissors.height = cmd->scissors.height;
        scissors.x = static_cast<NSUInteger>(cmd->scissors.x);
        scissors.y = static_cast<NSUInteger>(cmd->scissors.y);
        [encoder setScissorRect:scissors];
    }

    void MTLBackend::CMD_EndRenderPass(CommandData& data)
    {
        id<MTLRenderCommandEncoder> encoder = static_cast<id<MTLRenderCommandEncoder>>(data.currentRenderEncoder);
        [encoder endEncoding];
        data.renderEncoders.push_back(encoder);
        data.currentRenderEncoder = nullptr;
    }

    void MTLBackend::CMD_BindPipeline(CommandData& data, uint8* head)
    {
        CMDBindPipeline* cmd = reinterpret_cast<CMDBindPipeline*>(head);
        GfxShader& shader = m_resources->GetShader(cmd->shader);
        
        if(shader.IsCompute())
        {
            id<MTLComputeCommandEncoder> encoder = static_cast<id<MTLComputeCommandEncoder>>(data.currentComputeEncoder);
            id<MTLComputePipelineState> pso = static_cast<id<MTLComputePipelineState>>(shader.GetPSO());
            [encoder setComputePipelineState:pso];
            return;
        }
        
        data.currentShader = &shader;
        
        id<MTLRenderCommandEncoder> encoder = static_cast<id<MTLRenderCommandEncoder>>(data.currentRenderEncoder);
        id<MTLRenderPipelineState> pso = static_cast<id<MTLRenderPipelineState>>(shader.GetPSO());
        id<MTLDepthStencilState> dsso = static_cast<id<MTLDepthStencilState>>(shader.GetDSSO());
        [encoder setRenderPipelineState:pso];
        [encoder setDepthStencilState:dsso];
        [encoder setDepthBias:shader.GetDepthBias() slopeScale:shader.GetDepthSlope() clamp:shader.GetDepthClamp()];
        
        const MTLCullMode cull = static_cast<MTLCullMode>(shader.GetCullMode());
        const MTLWinding winding = static_cast<MTLWinding>(shader.GetWinding());
        [encoder setCullMode:cull];
        [encoder setFrontFacingWinding:winding];
    }

    void MTLBackend::CMD_DrawInstanced(CommandData &data, uint8 *head)
    {
        CMDDrawInstanced* cmd = reinterpret_cast<CMDDrawInstanced*>(head);
        id<MTLRenderCommandEncoder> encoder = static_cast<id<MTLRenderCommandEncoder>>(data.currentRenderEncoder);
        GfxShader* shader = static_cast<GfxShader*>(data.currentShader);
        [encoder drawPrimitives:static_cast<MTLPrimitiveType>(shader->GetTopology()) vertexStart:cmd->firstVertex vertexCount:cmd->vertexCount instanceCount:cmd->instanceCount baseInstance:cmd->firstInstance];
    }

    void MTLBackend::CMD_DrawIndexedInstanced(CommandData &data, uint8 *head)
    {
        CMDDrawIndexedInstanced* cmd = reinterpret_cast<CMDDrawIndexedInstanced*>(head);
        id<MTLRenderCommandEncoder> encoder = static_cast<id<MTLRenderCommandEncoder>>(data.currentRenderEncoder);
        id<MTLBuffer> buffer = static_cast<id<MTLBuffer>>(data.currentIndexBuffer);
        GfxShader* shader = static_cast<GfxShader*>(data.currentShader);
        
        const MTLIndexType ibType = data.currentIndexBuffer16Bit ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32;
        const uint32 ibOffset = data.currentIndexBuffer16Bit ? (cmd->firstIndex * sizeof(uint16)) : (cmd->firstIndex * sizeof(uint32));
        [encoder drawIndexedPrimitives:static_cast<MTLPrimitiveType>(shader->GetTopology()) indexCount:cmd->indexCount indexType:ibType indexBuffer:buffer indexBufferOffset:ibOffset  instanceCount:cmd->instanceCount baseVertex:cmd->vertexOffset baseInstance:cmd->firstInstance];
    }

    void MTLBackend::CMD_CopyBufferToBuffer(CommandData& data, uint8* head)
    {
        CMDCopyBufferToBuffer* cmd = reinterpret_cast<CMDCopyBufferToBuffer*>(head);
    }
    
    void MTLBackend::CMD_CopyBufferToTexture(CommandData& data, uint8* head)
    {
        CMDCopyBufferToTexture* cmd = reinterpret_cast<CMDCopyBufferToTexture*>(head);
    }
    
    void MTLBackend::CMD_CopyTextureToBuffer(CommandData& data, uint8* head)
    {
        CMDCopyTextureToBuffer* cmd = reinterpret_cast<CMDCopyTextureToBuffer*>(head);
    }


} // namespace SFG
