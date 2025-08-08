// Copyright (c) 2025 Inan Evin

#pragma once

#include "data/string.hpp"
#include "common/size_definitions.hpp"
#include "io/log.hpp"
#include "io/assert.hpp"
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
			SFG_FATAL("DX12 Exception! {0}", hr);
			DBG_BRK;
			throw HrException(hr);
		}
	}

} // namespace LinaGX
