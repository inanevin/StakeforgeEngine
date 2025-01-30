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
	struct MouseEvent;
	struct MouseDeltaEvent;
	struct MouseWheelEvent;
	struct KeyEvent;
	class Vector2i;

	class AppDelegate
	{
	public:
		AppDelegate() = delete;
		AppDelegate(uint32 frameRate, uint32 gameFrameRate) : m_targetFrameRate(frameRate), m_targetGameRate(gameFrameRate) {};
		virtual ~AppDelegate() = default;

		/// <summary>
		///
		/// </summary>
		/// <param name="event"></param>
		virtual void OnMouse(const MouseEvent& event) = 0;

		/// <summary>
		///
		/// </summary>
		/// <param name="event"></param>
		virtual void OnKey(const KeyEvent& event) = 0;

		/// <summary>
		///
		/// </summary>
		/// <param name="event"></param>
		virtual void OnMouseDelta(const MouseDeltaEvent& event) = 0;

		/// <summary>
		///
		/// </summary>
		/// <param name="event"></param>
		virtual void OnMouseWheel(const MouseWheelEvent& event) = 0;

		/// <summary>
		///
		/// </summary>
		/// <param name="delta"></param>
		virtual void OnTick(double delta) = 0;

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline uint32 GetTargetFrameRate() const
		{
			return m_targetFrameRate;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline uint32 GetTargetGameRate() const
		{
			return m_targetGameRate;
		}

	protected:
		uint32 m_targetFrameRate = 60;
		uint32 m_targetGameRate	 = 0;
	};
} // namespace SFG
