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

#include "SFG/Gfx/GfxResources.hpp"
#include "SFG/Gfx/Common/QueueDesc.hpp"

namespace SFG
{

	void GfxResources::Initialize(GfxBackend* backend)
	{
		m_backend			   = backend;
		m_primaryQueueGfx	   = CreateQueue({.debugName = "PrimaryGfxQueue", .type = QueueType::Graphics});
		m_primaryQueueTransfer = CreateQueue({.debugName = "PrimaryTransferQueue", .type = QueueType::Transfer});
		m_primaryQueueCompute  = CreateQueue({.debugName = "PrimaryComputeQueue", .type = QueueType::Compute});
	}

	void GfxResources::Shutdown()
	{
		DestroyQueue(m_primaryQueueGfx);
		DestroyQueue(m_primaryQueueTransfer);
		DestroyQueue(m_primaryQueueCompute);

		SFG_ASSERT(m_swapchains.IsEmpty());
		SFG_ASSERT(m_textures.IsEmpty());
		SFG_ASSERT(m_renderTargets.IsEmpty());
		SFG_ASSERT(m_samplers.IsEmpty());
		SFG_ASSERT(m_shaders.IsEmpty());
		SFG_ASSERT(m_semaphores.IsEmpty());
		SFG_ASSERT(m_renderSemaphores.IsEmpty());
		SFG_ASSERT(m_resources.IsEmpty());
	}

	Handle<uint16> GfxResources::CreateRenderTarget(const RenderTargetDesc& desc)
	{
		Handle<uint16> hnd = m_renderTargets.Allocate(*this);
		m_renderTargets.Get(hnd).Create(desc);
		return hnd;
	}

	void GfxResources::DestroyRenderTarget(Handle<uint16> handle)
	{
		m_renderTargets.Get(handle).Destroy();
		m_renderTargets.Free(handle);
	}

	Handle<uint16> GfxResources::CreateSwapchain(const SwapchainDesc& desc)
	{
		Handle<uint16> swp = m_swapchains.Allocate(m_backend->GetDevice());
		m_swapchains.Get(swp).Create(desc);
		return swp;
	}

	void GfxResources::DestroySwapchain(Handle<uint16> handle)
	{
		m_swapchains.Get(handle).Destroy();
		m_swapchains.Free(handle);
	}

	Handle<uint16> GfxResources::CreateTexture(const TextureDesc& desc)
	{
		Handle<uint16> txt = m_textures.Allocate(m_backend->GetDevice());
		m_textures.Get(txt).Create(desc);
		return txt;
	}

	void GfxResources::DestroyTexture(Handle<uint16> handle)
	{
		m_textures.Get(handle).Destroy();
		m_textures.Free(handle);
	}

	Handle<uint16> GfxResources::CreateSemaphore()
	{
		Handle<uint16> hnd = m_semaphores.Allocate(m_backend->GetDevice());
		return hnd;
	}

	void GfxResources::DestroySemaphore(Handle<uint16> handle)
	{
		m_semaphores.Get(handle).Destroy();
		m_semaphores.Free(handle);
	}

	Handle<uint16> GfxResources::CreateRenderSemaphore()
	{
		Handle<uint16> hnd = m_renderSemaphores.Allocate(*this);
		return hnd;
	}

	void GfxResources::DestroyRenderSemaphore(Handle<uint16> handle)
	{
		m_renderSemaphores.Get(handle).Destroy();
		m_renderSemaphores.Free(handle);
	}

	Handle<uint16> GfxResources::CreateSampler(const SamplerDesc& desc)
	{
		Handle<uint16> hnd = m_samplers.Allocate(m_backend->GetDevice());
		m_samplers.Get(hnd).Create(desc);
		return hnd;
	}

	void GfxResources::DestroySampler(Handle<uint16> handle)
	{
		m_samplers.Get(handle).Destroy();
		m_samplers.Free(handle);
	}

	Handle<uint16> GfxResources::CreateResource(const ResourceDesc& desc)
	{
		Handle<uint16> hnd = m_resources.Allocate(m_backend->GetDevice());
		m_resources.Get(hnd).Create(desc);
		return hnd;
	}

	void GfxResources::DestroyResource(Handle<uint16> handle)
	{
		m_resources.Get(handle).Destroy();
		m_resources.Free(handle);
	}

	Handle<uint16> GfxResources::CreateShader(const ShaderDesc& desc)
	{
		Handle<uint16> hnd = m_shaders.Allocate(m_backend->GetDevice());
		m_shaders.Get(hnd).Create(desc);
		return hnd;
	}

	void GfxResources::DestroyShader(Handle<uint16> handle)
	{
		m_shaders.Get(handle).Destroy();
		m_shaders.Free(handle);
	}

	Handle<uint16> GfxResources::CreateQueue(const QueueDesc& desc)
	{
		Handle<uint16> hnd = m_queues.Allocate(m_backend->GetDevice());
		m_queues.Get(hnd).Create(desc);
		return hnd;
	}

	void GfxResources::DestroyQueue(Handle<uint16> handle)
	{
		m_queues.Get(handle).Destroy();
		m_queues.Free(handle);
	}

} // namespace SFG
