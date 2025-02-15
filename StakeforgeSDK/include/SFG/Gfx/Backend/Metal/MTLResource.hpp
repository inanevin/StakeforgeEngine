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
    struct ResourceDesc;

	class MTLResource
	{
	public:
        
        MTLResource() = delete;
        MTLResource(void* device) : m_device(device) {};
        
        /// <summary>
        ///
        /// </summary>
        void Create(const ResourceDesc& desc);
        
        /// <summary>
        ///
        /// </summary>
        void Destroy();
        
        /// <summary>
        ///
        /// </summary>
        void Map(uint8*& ptr);
        
        /// <summary>
        ///
        /// </summary>
        void Unmap() {};
        
        
    private:
        
        void* m_resource = nullptr;
        void* m_device = nullptr;
	};
}; // namespace SFG
