// Copyright (c) 2025 Inan Evin

#include "texture_sampler.hpp"
#include "texture_sampler_raw.hpp"
#include "math/math_common.hpp"
#include "io/assert.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"

namespace SFG
{
	texture_sampler::~texture_sampler()
	{
		SFG_ASSERT(_hw == std::numeric_limits<gfx_id>::max());
	}

	void texture_sampler::create_from_raw(const texture_sampler_raw& raw)
	{
		gfx_backend* backend = gfx_backend::get();
		_hw					 = backend->create_sampler(raw.desc);
	}

	void texture_sampler::destroy()
	{
		SFG_ASSERT(_hw != std::numeric_limits<gfx_id>::max());
		gfx_backend* backend = gfx_backend::get();
		backend->destroy_sampler(_hw);
		_hw = std::numeric_limits<gfx_id>::max();
	}

}
