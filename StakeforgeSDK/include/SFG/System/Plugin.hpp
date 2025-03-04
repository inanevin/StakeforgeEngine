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

#include "SFG/Data/String.hpp"
#include "SFG/StakeforgeAPI.hpp"

namespace SFG
{
	class App;

	class SFG_API Plugin
	{
	public:
		Plugin()						 = delete;
		Plugin(const Plugin&)			 = delete;
		Plugin& operator=(const Plugin&) = delete;
		Plugin(const char* path, App* app, void* platformHandle) : m_path(path), m_app(app), m_platformHandle(platformHandle){};
		virtual ~Plugin() = default;

		/// <summary>
		///
		/// </summary>
		virtual void OnLoaded() = 0;

		/// <summary>
		///
		/// </summary>
		virtual void OnUnloaded() = 0;

		/// <summary>
		///
		/// </summary>
		inline void* GetPlatformHandle() const
		{
			return m_platformHandle;
		}

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		inline const String& GetPath() const
		{
			return m_path;
		}

	protected:
		String m_path			= "";
		App*   m_app			= nullptr;
		void*  m_platformHandle = nullptr;
	};
} // namespace SFG
