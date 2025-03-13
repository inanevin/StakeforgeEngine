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

#include "Game.hpp"

#include <SFG/Core/App.hpp>
#include <SFG/IO/Log.hpp>
#include <SFG/Platform/Window.hpp>
#include <SFG/Platform/WindowStyle.hpp>

#ifdef SFG_EDITOR
#include <SFG/Editor/Editor.hpp>
#endif

/* DEBUG DEBUG DEBUG */

#include <random>
#include <chrono>
#include <thread>

#include <SFG/Gfx/Common/ShaderDesc.hpp>
#include <SFG/Gfx/Common/TextureDesc.hpp>
#include <SFG/Gfx/Common/TextureFormat.hpp>
#include <SFG/Gfx/Common/TextureView.hpp>
#include <SFG/Gfx/Common/SwapchainDesc.hpp>
#include <SFG/Gfx/Common/RenderTargetDesc.hpp>
#include <SFG/Gfx/CommandStream.hpp>
#include <SFG/Gfx/GfxResources.hpp>
#include <SFG/Gfx/Commands/CMDRenderPass.hpp>
#include <SFG/Gfx/Commands/CMDBindPipeline.hpp>
#include <SFG/Gfx/Commands/CMDDraw.hpp>
#include <SFG/Platform/InputAction.hpp>
#include <SFG/Platform/InputMappings.hpp>
#include <SFG/Memory/BumpAllocator.hpp>

namespace SFG
{
	Game::Game(App& app) : AppDelegate(), m_app(app)
	{
		/* App Init */
		SFG::App::Settings& settings = app.GetAppSettings();
		settings.throttleCPU		 = true;
		settings.delegate			 = this;
	}

	void Game::OnInitialize()
	{
		/* Main window */
		m_window = m_app.CreateAppWindow(0, SFG_APPNAME, {}, Vector2ui(256, 256), WindowStyle::ApplicationWindow);
		m_window->CenterToMonitor();
		// window->SetHighFrequencyInputMode(true);

		GfxResources& resources = m_app.GetGfxResources();

		m_swapchain = resources.CreateSwapchain({
			.name		   = "MainSwapchain",
			.window		   = (void*)m_window,
			.osHandle	   = m_window->GetOSHandle(),
			.format		   = TextureFormat::B8G8R8A8_UNORM,
			.x			   = 0,
			.y			   = 0,
			.width		   = m_window->GetSize().x,
			.height		   = m_window->GetSize().y,
			.scalingFactor = 1.0f,
		});

		m_renderTarget = resources.CreateRenderTarget({
			.swapchainHandle = m_swapchain,
			.isSwapchain	 = true,
		});

		ShaderColorAttachment attachment0 = {
			.format = TextureFormat::R8G8B8A8_SRGB,
			.blendAttachment =
				{
					.blendEnabled = false,
				},
		};

		m_shaderGPU = resources.CreateShader({
			.debugName = "TestShader",
			.depthStencil =
				{
					.depthCompare = CompareOp::LEqual,
					.format		  = TextureFormat::D32_SFLOAT,
					.depthWrite	  = true,
					.depthTest	  = true,
				},
			.colorAttachments	  = &attachment0,
			.colorAttachmentCount = 1,
			.polygonMode		  = PolygonMode::Fill,
			.cullMode			  = CullMode::Back,
			.frontFace			  = FrontFace::CCW,
			.topology			  = Topology::TriangleList,
			.samples			  = 1,
		});
	}

	void Game::OnShutdown()
	{
		GfxResources& resources = m_app.GetGfxResources();
		resources.DestroySwapchain(m_swapchain);
		resources.DestroyRenderTarget(m_renderTarget);
		resources.DestroyShader(m_shaderGPU);

		Window* window = m_app.GetWindow(0);
		if (window)
		{
			m_app.DestroyAppWindow(window);
		}
	}

	void Game::OnWindowEvent(const WindowEvent& ev)
	{
		if (ev.type == WindowEventType::Resized)
		{
			m_app.GetGfxResources().DestroyRenderTarget(m_renderTarget);

			m_app.GetGfxResources()
				.GetSwapchain(m_swapchain)
				.Recreate({
					.name		   = "MainSwapchain",
					.window		   = (void*)ev.window,
					.osHandle	   = ev.window->GetOSHandle(),
					.format		   = TextureFormat::B8G8R8A8_UNORM,
					.x			   = 0,
					.y			   = 0,
					.width		   = ev.window->GetSize().x,
					.height		   = ev.window->GetSize().y,
					.scalingFactor = 1.0f,
				});

			m_renderTarget = m_app.GetGfxResources().CreateRenderTarget({
				.swapchainHandle = m_swapchain,
				.isSwapchain	 = true,
			});

			return;
		}

		if (ev.type == WindowEventType::Key)
		{
			if (ev.action == InputAction::Pressed && ev.button == InputCode::KeySpace)
			{
			}
		}
	}

	void Game::OnTick()
	{
		// SFG_TRACE("Game ticking {0}", delta);
	}

	void Game::OnSimulate(double delta)
	{
		static std::mt19937				   rng(std::random_device{}());
		std::uniform_int_distribution<int> sleepDist(10, 15); // Milliseconds

		int sleepTime = sleepDist(rng);
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
	}

	void Game::OnGenerateFrame(RenderFrame& frame, double interpolation)
	{
		CommandStream& stream = frame.GetCommandStream();

		const ColorAttachment attachment0 = {
			.renderTarget = m_renderTarget,
			.format		  = TextureFormat::B8G8R8A8_UNORM,
			.storeOp	  = StoreOp::Store,
			.loadOp		  = LoadOp::Clear,
			.clearColor	  = {1.0f, 1.0f, 0.1f, 1.0f},
		};

		stream.AddCommand<CMDBeginRenderPass>({
			.viewport =
				{
					.x		= 0,
					.y		= 0,
					.width	= m_window->GetSize().x,
					.height = m_window->GetSize().y,
				},
			.scissors =
				{
					.x		= 0,
					.y		= 0,
					.width	= m_window->GetSize().x,
					.height = m_window->GetSize().y,
				},
			.colorAttachments	  = frame.GetAllocator()->EmplaceAux<ColorAttachment>(attachment0),
			.colorAttachmentCount = 1,
		});

		stream.AddCommand<CMDBindPipeline>({
			.shader = m_shaderGPU,
		});

		stream.AddCommand<CMDDrawInstanced>({
			.vertexCount   = 6,
			.instanceCount = 1,
			.firstVertex   = 0,
			.firstInstance = 0,
		});

		stream.AddCommand<CMDEndRenderPass>({});

		frame.Submit({
			.queue		  = m_app.GetGfxResources().GetPrimaryQueueGfx(),
			.streams	  = frame.GetAllocator()->EmplaceAux<CommandStream*>(&stream),
			.streamsCount = 1,
		});
	}
} // namespace SFG
