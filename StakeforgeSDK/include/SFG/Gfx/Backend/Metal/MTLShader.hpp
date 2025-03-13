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
	struct ShaderDesc;

	class MTLShader
	{
	public:
		MTLShader() = delete;
		MTLShader(void* device) : m_device(device){};

		/// <summary>
		///
		/// </summary>
		void Create(const ShaderDesc& desc);

		/// <summary>
		///
		/// </summary>
		void Destroy();

		/// <summary>
		///
		/// </summary>
		inline bool IsCompute() const
		{
			return m_isCompute;
		}

		/// <summary>
		///
		/// </summary>
		inline void* GetPSO() const
		{
			return m_pso;
		}

		/// <summary>
		///
		/// </summary>
		inline void* GetDSSO() const
		{
			return m_dsso;
		}

		/// <summary>
		///
		/// </summary>
		inline uint32 GetWinding() const
		{
			return m_winding;
		}

		/// <summary>
		///
		/// </summary>
		inline uint32 GetCullMode() const
		{
			return m_cullMode;
		}

		/// <summary>
		///
		/// </summary>
		inline uint32 GetTopology() const
		{
			return m_topology;
		}

		/// <summary>
		///
		/// </summary>
		inline float GetDepthBias() const
		{
			return m_depthBias;
		}

		/// <summary>
		///
		/// </summary>
		inline float GetDepthSlope() const
		{
			return m_depthSlope;
		}

		/// <summary>
		///
		/// </summary>
		inline float GetDepthClamp() const
		{
			return m_depthClamp;
		}

	private:
		void*  m_device		= nullptr;
		void*  m_pso		= nullptr;
		void*  m_dsso		= nullptr;
		bool   m_isCompute	= false;
		uint32 m_winding	= 0;
		uint32 m_cullMode	= 0;
		uint32 m_topology	= 0;
		float  m_depthBias	= 0.0f;
		float  m_depthSlope = 0.0f;
		float  m_depthClamp = 0.0f;
	};

	typedef MTLShader GfxShader;

}; // namespace SFG
