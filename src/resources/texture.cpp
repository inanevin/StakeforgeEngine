// Copyright (c) 2025 Inan Evin
#include "texture.hpp"
#include "memory/memory.hpp"
#include "io/log.hpp"
#include "io/assert.hpp"

namespace Game
{
	texture::~texture()
	{
		SFG_ASSERT(_cpu.empty(), "");
	}

	// void texture::copy(LinaGX::CommandStream* cmd)
	//{
	//	if (_cpu.empty())
	//	{
	//		SFG_ERR("Trying to copy a texture with empty buffers!");
	//		return;
	//	}
	//
	//	LinaGX::CMDCopyBufferToTexture2D* copy = cmd->AddCommand<LinaGX::CMDCopyBufferToTexture2D>();
	//	copy->destTexture					   = _hw;
	//	copy->mipLevels						   = 1;
	//	copy->buffers						   = cmd->EmplaceAuxMemory<LinaGX::TextureBuffer>(_cpu.data(), _cpu.size() * sizeof(LinaGX::TextureBuffer));
	//
	//	if (_clear_buffers_after_copy) { destroy_cpu(); }
	// }

	void texture::destroy()
	{
		// if (_owns_hw)
		//{
		//	LinaGX::instance()->DestroyTexture(_hw);
		//	_hw = 0;
		// }
		//
		// destroy_cpu();
	}

	void texture::create_cpu(uint8* data, uint32 width, uint32 height, uint32 bpp, uint8 channels, bool linear)
	{
		//_mip_levels = 1;
		//_channels	= channels;
		//_is_linear	= linear;
		//
		// destroy_cpu();
		//
		// const size_t				data_size = static_cast<size_t>(width * height * bpp);
		// const LinaGX::TextureBuffer b		  = {
		//			.pixels		   = reinterpret_cast<uint8*>(SFG_MALLOC(data_size)),
		//			.width		   = width,
		//			.height		   = height,
		//			.bytesPerPixel = bpp,
		//};
		//
		// if (b.pixels) SFG_MEMCPY(b.pixels, data, data_size);
		//_cpu.push_back(b);
	}

	// void texture::create_hw(const LinaGX::TextureDesc& desc)
	//{
	//	_hw		 = LinaGX::instance()->CreateTexture(desc);
	//	_owns_hw = true;
	// }

	void texture::create_hw_default()
	{
		// if (_cpu.empty())
		//{
		//	SFG_ERR("Failed creating hw_default as no cpu data is available!");
		//	return;
		// }
		//
		//_hw		 = LinaGX::instance()->CreateTexture({
		//		 .type		= LinaGX::TextureType::Texture2D,
		//		 .format	= determine_format_from_cpu(),
		//		 .width		= _cpu[0].width,
		//		 .height	= _cpu[0].height,
		//		 .mipLevels = static_cast<uint32>(_cpu.size()),
		//		 .debugName = "texture",
		//  });
		//_owns_hw = true;
	}

	void texture::destroy_cpu()
	{
		// for (LinaGX::TextureBuffer& b : _cpu)
		//{
		//	SFG_FREE(b.pixels);
		// }
		//
		//_cpu.resize(0);
	}

	void texture::populate_mips_from_cpu(uint32 max_level)
	{
		// if (_cpu.empty())
		//{
		//	SFG_ERR("Can't generate mipmaps as cpu data is empty!");
		//	return;
		// }
		//
		// LinaGX::GenerateMipmaps(_cpu[0], _cpu, LinaGX::MipmapFilter::Box, _cpu[0].bytesPerPixel, true, max_level, false);
	}

	format texture::determine_format_from_cpu()
	{
		if (_cpu.empty())
			return format::undefined;

		const uint32 bpp = _cpu[0].bpp;
		const uint32 ch	 = _channels;

		if (ch == 1)
		{
			if (bpp == 1)
				return format::r8_unorm;
			else if (bpp == 2)
				return format::r16_unorm;
		}
		else if (ch == 2)
		{
			if (bpp == 2)
				return format::r8g8_unorm;
			else if (bpp == 4)
				return format::r16g16_unorm;
		}
		else if (ch == 3)
		{
			if (bpp == 12)
				return format::r32g32b32_sfloat;
		}
		else if (ch == 4)
		{
			if (bpp == 4)
				return _is_linear ? format::r8g8b8a8_unorm : format::b8g8r8a8_srgb;
			else if (bpp == 8)
				return format::r16g16b16a16_unorm;
			else if (bpp == 16)
				return format::r32g32b32a32_sfloat;
		}

		return format::undefined;
	}

}