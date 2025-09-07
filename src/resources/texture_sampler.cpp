// Copyright (c) 2025 Inan Evin

#include "texture_sampler.hpp"
#include "math/math_common.hpp"
#include "io/assert.hpp"
#include "gfx/backend/backend.hpp"
#include "gfx/common/descriptions.hpp"

#ifdef SFG_TOOLMODE
#include "io/file_system.hpp"
#include <fstream>
#include <vendor/nhlohmann/json.hpp>
using json = nlohmann::json;
#endif

namespace SFG
{
	texture_sampler::~texture_sampler()
	{
		const bool exists = CHECK_BIT(_hw, 15);
		SFG_ASSERT(!exists);
	}

#ifdef SFG_TOOLMODE
	bool texture_sampler::create_from_file(const char* file)
	{

		if (file_system::exists(file))
		{
			SFG_ERR("File doesn't exist! {0}", file);
			return false;
		}
		SFG_ASSERT(!CHECK_BIT(_hw, 15));

		std::ifstream f(file);
		json		  json_data = json::parse(f);
		sampler_desc  meta		= json_data;
		f.close();

		gfx_backend* backend = gfx_backend::get();
		_hw					 = backend->create_sampler(meta);
		_hw					 = SET_BIT(_hw, 15);
		return true;
	}
#endif

	void texture_sampler::destroy()
	{
		SFG_ASSERT(CHECK_BIT(_hw, 15));

		_hw					 = UNSET_BIT(_hw, 15);
		gfx_backend* backend = gfx_backend::get();
		backend->destroy_sampler(_hw);
	}

	gfx_id texture_sampler::get_hw() const
	{
		return UNSET_BIT(_hw, 15);
	}
}
