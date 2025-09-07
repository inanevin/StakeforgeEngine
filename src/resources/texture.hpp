// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/string.hpp"
#include "data/bitmask.hpp"
#include "gfx/common/gfx_constants.hpp"

#ifdef SFG_TOOLMODE
#include "vendor/nhlohmann/json_fwd.hpp"
#endif

namespace SFG
{
	struct texture_desc;
	struct vector2ui16;
	struct texture_buffer;

	struct texture_meta
	{
		string source	= "";
		uint8  gen_mips = 0;
		uint8  fmt		= 0;
	};

#ifdef SFG_TOOLMODE

	void to_json(nlohmann::json& j, const texture_meta& s);
	void from_json(const nlohmann::json& j, texture_meta& s);

#endif

	class texture
	{

	public:
		enum flags
		{
			hw_exists			= 1 << 0,
			intermediate_exists = 1 << 1,
			upload_pending		= 1 << 2,
		};

		~texture();

#ifdef SFG_TOOLMODE
		bool create_from_file(const char* file);
#endif

		void create_from_buffer(uint8* data, const vector2ui16& size, uint8 bpp, uint8 fmt, const char* debug_name = "");
		void destroy_cpu();
		void destroy();
		void destroy_intermediate();
		void populate_mips(uint8 channels, bool is_linear);

		uint8  get_bpp() const;
		uint16 get_width() const;
		uint16 get_height() const;

		gfx_id get_hw() const;

		inline bitmask<uint8>& get_flags()
		{
			return _flags;
		}

		inline texture_buffer* get_cpu() const
		{
			return _cpu_buffers;
		}

		inline uint8 get_cpu_count() const
		{
			return _cpu_count;
		}

		inline gfx_id get_intermediate() const
		{
			return _intermediate;
		}

	private:
		void create_intermediate();

	private:
		texture_buffer* _cpu_buffers  = nullptr;
		gfx_id			_hw			  = 0;
		gfx_id			_intermediate = 0;
		uint8			_cpu_count	  = 0;
		bitmask<uint8>	_flags		  = 0;
	};

}