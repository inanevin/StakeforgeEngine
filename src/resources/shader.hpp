// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/string.hpp"

namespace LinaGX
{
	struct ShaderDesc;
}

namespace Game
{
	class shader
	{
	public:
		bool create_from_file_default(const char* file, const LinaGX::ShaderDesc& desc);
		void destroy();

		inline uint16 get_hw() const
		{
			return _hw;
		}

		static void get_shader_block(const string& text, const string& block_ident, const string& end_ident, string& out_block);

	private:
		uint16 _hw = 0;
	};

}
