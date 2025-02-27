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
#include "SFG/Memory/PoolAllocator.hpp"
#include "SFG/Gfx/RenderTarget.hpp"
#include "SFG/Gfx/RenderSemaphore.hpp"
#include "SFG/StakeforgeAPI.hpp"

#ifdef SFG_PLATFORM_WINDOWS
#include "SFG/Gfx/Backend/Vulkan/VulkanBackend.hpp"
#elif SFG_PLATFORM_OSX
#include "SFG/Gfx/Backend/Metal/MTLBackend.hpp"
#include "SFG/Gfx/Backend/Metal/MTLSwapchain.hpp"
#include "SFG/Gfx/Backend/Metal/MTLTexture.hpp"
#include "SFG/Gfx/Backend/Metal/MTLSemaphore.hpp"
#include "SFG/Gfx/Backend/Metal/MTLSampler.hpp"
#include "SFG/Gfx/Backend/Metal/MTLResource.hpp"
#include "SFG/Gfx/Backend/Metal/MTLShader.hpp"
#include "SFG/Gfx/Backend/Metal/MTLSubmitQueue.hpp"
#endif

namespace SFG
{
	class GfxResources
	{
	public:
		/// <summary>
		///
		/// </summary>
		void Initialize(GfxBackend* backend);

		/// <summary>
		///
		/// </summary>
		void Shutdown();

		/// <summary>
		///
		/// </summary>
		SFG_API Handle<uint16> CreateRenderTarget(const RenderTargetDesc& desc);

		/// <summary>
		///
		/// </summary>
		SFG_API void DestroyRenderTarget(Handle<uint16> handle);

		/// <summary>
		///
		/// </summary>
		SFG_API Handle<uint16> CreateSwapchain(const SwapchainDesc& desc);

		/// <summary>
		///
		/// </summary>
		SFG_API void DestroySwapchain(Handle<uint16> handle);

		/// <summary>
		///
		/// </summary>
		SFG_API Handle<uint16> CreateTexture(const TextureDesc& desc);

		/// <summary>
		///
		/// </summary>
		SFG_API void DestroyTexture(Handle<uint16> handle);

		/// <summary>
		///
		/// </summary>
		SFG_API Handle<uint16> CreateSemaphore();

		/// <summary>
		///
		/// </summary>
		SFG_API void DestroySemaphore(Handle<uint16> handle);

		/// <summary>
		///
		/// </summary>
		SFG_API Handle<uint16> CreateRenderSemaphore();

		/// <summary>
		///
		/// </summary>
		SFG_API void DestroyRenderSemaphore(Handle<uint16> handle);

		/// <summary>
		///
		/// </summary>
		SFG_API Handle<uint16> CreateSampler(const SamplerDesc& desc);

		/// <summary>
		///
		/// </summary>
		SFG_API void DestroySampler(Handle<uint16> handle);

		/// <summary>
		///
		/// </summary>
		SFG_API Handle<uint16> CreateResource(const ResourceDesc& desc);

		/// <summary>
		///
		/// </summary>
		SFG_API void DestroyResource(Handle<uint16> handle);

		/// <summary>
		///
		/// </summary>
		SFG_API Handle<uint16> CreateShader(const ShaderDesc& desc);

		/// <summary>
		///
		/// </summary>
		SFG_API void DestroyShader(Handle<uint16> handle);

		/// <summary>
		///
		/// </summary>
		SFG_API Handle<uint16> CreateQueue(const QueueDesc& desc);

		/// <summary>
		///
		/// </summary>
		SFG_API void DestroyQueue(Handle<uint16> handle);

		/// <summary>
		///
		/// </summary>
		inline RenderTarget& GetRenderTarget(Handle<uint16> handle)
		{
			return m_renderTargets.Get(handle);
		}

		/// <summary>
		///
		/// </summary>
		inline GfxSwapchain& GetSwapchain(Handle<uint16> handle)
		{
			return m_swapchains.Get(handle);
		}

		/// <summary>
		///
		/// </summary>
		inline GfxTexture& GetTexture(Handle<uint16> handle)
		{
			return m_textures.Get(handle);
		}

		/// <summary>
		///
		/// </summary>
		inline GfxSemaphore& GetSemaphore(Handle<uint16> handle)
		{
			return m_semaphores.Get(handle);
		}

		/// <summary>
		///
		/// </summary>
		inline RenderSemaphore& GetRenderSemaphore(Handle<uint16> handle)
		{
			return m_renderSemaphores.Get(handle);
		}

		/// <summary>
		///
		/// </summary>
		inline GfxSampler& GetSampler(Handle<uint16> handle)
		{
			return m_samplers.Get(handle);
		}

		/// <summary>
		///
		/// </summary>
		inline GfxResource& GetResource(Handle<uint16> handle)
		{
			return m_resources.Get(handle);
		}

		/// <summary>
		///
		/// </summary>
		inline GfxShader& GetShader(Handle<uint16> handle)
		{
			return m_shaders.Get(handle);
		}

		/// <summary>
		///
		/// </summary>
		inline GfxQueue& GetQueue(Handle<uint16> handle)
		{
			return m_queues.Get(handle);
		}

		/// <summary>
		///
		/// </summary>
		inline Handle<uint16> GetPrimaryQueueGfx() const
		{
			return m_primaryQueueGfx;
		}

		/// <summary>
		///
		/// </summary>
		inline Handle<uint16> GetPrimaryQueueTransfer() const
		{
			return m_primaryQueueTransfer;
		}

		/// <summary>
		///
		/// </summary>
		inline Handle<uint16> GetPrimaryQueueCompute() const
		{
			return m_primaryQueueCompute;
		}

	private:
		GfxBackend*							 m_backend = nullptr;
		PoolAllocator16<RenderTarget, 32>	 m_renderTargets;
		PoolAllocator16<GfxSwapchain, 4>	 m_swapchains;
		PoolAllocator16<GfxTexture, 3>		 m_textures;
		PoolAllocator16<GfxSemaphore, 20>	 m_semaphores;
		PoolAllocator16<RenderSemaphore, 20> m_renderSemaphores;
		PoolAllocator16<GfxSampler, 20>		 m_samplers;
		PoolAllocator16<GfxResource, 50>	 m_resources;
		PoolAllocator16<GfxShader, 5>		 m_shaders;
		PoolAllocator16<GfxQueue, 3>		 m_queues;
		Handle<uint16>						 m_primaryQueueGfx		= {};
		Handle<uint16>						 m_primaryQueueTransfer = {};
		Handle<uint16>						 m_primaryQueueCompute	= {};
	};
}; // namespace SFG
