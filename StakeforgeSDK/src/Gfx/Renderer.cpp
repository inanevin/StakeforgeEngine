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

#include "SFG/Gfx/Renderer.hpp"

namespace SFG
{

	void Renderer::Initialize(String& errString)
	{
		m_backend.Create(errString);
	}

	void Renderer::Shutdown()
	{
		m_backend.Destroy();
	}

	void Renderer::Render(const RenderFrame& frame)
	{
	}

	Handle<uint16> Renderer::CreateRenderTarget(const RenderTargetDesc& desc)
	{
		Handle<uint16> hnd = m_renderTargets.Allocate(this);
		m_renderTargets.Get(hnd).Create(desc);
		return hnd;
	}

	void Renderer::DestroyRenderTarget(Handle<uint16> handle)
	{
		m_renderTargets.Get(handle).Destroy();
		m_renderTargets.Free(handle);
	}

	Handle<uint16> Renderer::CreateSwapchain(const SwapchainDesc& desc)
	{
		Handle<uint16> swp = m_swapchains.Allocate(m_backend.GetDevice());
		m_swapchains.Get(swp).Create(desc);
		return swp;
	}

	void Renderer::DestroySwapchain(Handle<uint16> handle)
	{
		m_swapchains.Get(handle).Destroy();
		m_swapchains.Free(handle);
	}

	Handle<uint16> Renderer::CreateTexture(const TextureDesc& desc)
	{
		Handle<uint16> txt = m_textures.Allocate(m_backend.GetDevice());
		m_textures.Get(txt).Create(desc);
		return txt;
	}

	void Renderer::DestroyTexture(Handle<uint16> handle)
	{
		m_textures.Get(handle).Destroy();
		m_textures.Free(handle);
	}

	Handle<uint16> Renderer::CreateSemaphore()
	{
		Handle<uint16> hnd = m_semaphores.Allocate(m_backend.GetDevice());
		return hnd;
	}

	void Renderer::DestroySemaphore(Handle<uint16> handle)
	{
		m_semaphores.Get(handle).Destroy();
		m_semaphores.Free(handle);
	}

	Handle<uint16> Renderer::CreateSampler(const SamplerDesc& desc)
	{
		Handle<uint16> hnd = m_samplers.Allocate(m_backend.GetDevice());
		m_samplers.Get(hnd).Create(desc);
		return hnd;
	}

	void Renderer::DestroySampler(Handle<uint16> handle)
	{
		m_samplers.Get(handle).Destroy();
		m_samplers.Free(handle);
	}

	Handle<uint16> Renderer::CreateResource(const ResourceDesc& desc)
	{
		Handle<uint16> hnd = m_resources.Allocate(m_backend.GetDevice());
		m_resources.Get(hnd).Create(desc);
		return hnd;
	}

	void Renderer::DestroyResource(Handle<uint16> handle)
	{
		m_resources.Get(handle).Destroy();
		m_resources.Free(handle);
	}

	Handle<uint16> Renderer::CreateShader(const ShaderDesc& desc)
	{
		Handle<uint16> hnd = m_shaders.Allocate(m_backend.GetDevice());
		m_shaders.Get(hnd).Create(desc);
		return hnd;
	}

	void Renderer::DestroyShader(Handle<uint16> handle)
	{
		m_shaders.Get(handle).Destroy();
		m_shaders.Free(handle);
	}

} // namespace SFG
