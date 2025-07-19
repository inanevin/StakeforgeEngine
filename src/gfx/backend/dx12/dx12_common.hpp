// Copyright (c) 2025 Inan Evin

#pragma once
#include "gfx/backend/dx12/sdk/d3dx12.h"
#include "gfx/backend/dx12/sdk/d3d12shader.h"
#include "data/string.hpp"
#include "common/size_definitions.hpp"
#include <dxcapi/dxcapi.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <stdexcept>

namespace Game
{
	inline string HrToString(HRESULT hr)
	{
		char s_str[64] = {};
		sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		return string(s_str);
	}

	class HrException : public std::runtime_error
	{
	public:
		HrException(HRESULT hr) : std::runtime_error(HrToString(hr).c_str()), m_hr(hr)
		{
		}
		HRESULT Error() const
		{
			return m_hr;
		}

	private:
		const HRESULT m_hr;
	};

	inline void throw_if_failed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw HrException(hr);
		}
	}

	struct descriptor_handle
	{
		size_t cpu	 = NULL;
		uint64 gpu	 = NULL;
		uint32 index = 0;
		uint32 count = 0;
	};
} // namespace LinaGX
