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

#include "SFG/Memory/PoolAllocator.hpp"

namespace SFG
{

	struct ResourceCache
	{
		void* Allocate()	  = 0;
		void  Free(void* obj) = 0;
	};

	template <class T> struct ResourceCacheImpl : ResourceCache
	{
		void* Allocate() override
		{
		}

		void Free(void* obj) override
		{
			T* t = static_cast<T*>(obj);
		}

		PoolAllocator16<T, 50> pool;
	};

	class ResourceManager
	{
	public:
		/// <summary>
		///
		/// </summary>
		void Initialize();

		/// <summary>
		///
		/// </summary>
		void Shutdown();

		/// <summary>
		///
		/// </summary>
		Handle<uint16> CreateShader();

		/// <summary>
		///
		/// </summary>
		void DestroyShader(Handle<uint16> handle);

		/// <summary>
		///
		/// </summary>
		Shader& GetShader(Handle<uint16> handle);

	private:
	};
} // namespace SFG
