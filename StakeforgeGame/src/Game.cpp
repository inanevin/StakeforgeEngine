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

#include <random>
#include <chrono>
#include <thread>

namespace SFG
{
	Game::Game(App* app) : AppDelegate(), m_app(app)
	{
		/* App Init */
		SFG::App::Settings& settings = app->GetAppSettings();
		settings.throttleCPU		 = true;
		settings.delegate			 = this;

		/* Main window */
		Window* window = app->CreateAppWindow(0, SFG_APPNAME, {}, Vector2ui(256, 256), WindowStyle::ApplicationWindow);
		window->CenterToMonitor();
		// window->SetHighFrequencyInputMode(true);
	}

	Game::~Game()
	{
	}

	void Game::OnWindowEvent(const WindowEvent& ev)
	{
		if (ev.type != WindowEventType::MouseDelta)
			return;

		// SFG_TRACE("HUH");
		// SFG_TRACE("POS {0} - {1}", ev.window->GetMousePositionAbs().x, ev.window->GetMousePositionAbs().y);
		// SFG_TRACE("VALUE {0} - {1}", ev.value.x, ev.value.y);
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
	}
} // namespace SFG
