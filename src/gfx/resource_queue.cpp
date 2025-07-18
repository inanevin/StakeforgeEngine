// Copyright (c) 2025 Inan Evin

#include "resource_queue.hpp"
#include "io/log.hpp"
#include "gfx/buffer.hpp"
#include "gfx/gfx_util.hpp"

namespace Game
{
	void resource_queue::add_texture_request(LinaGX::TextureBuffer* buffers, uint32 mip_levels, uint32 hw)
	{
		if (mip_levels == 0)
		{
			GAME_ERR("resource_queue::add_texture_request() -> Can't add a texture with mip level 0!");
			return;
		}

		const uint32 buffer_start = static_cast<uint32>(_reuse_buffers.size());

		for (uint32 i = 0; i < mip_levels; i++)
			_reuse_buffers.push_back(buffers[i]);

		_texture_requests.push_back({
			.buffers	   = buffers,
			.hw			   = hw,
			.mip_levels	   = mip_levels,
			._buffer_start = buffer_start,
		});
	}

	void resource_queue::add_buffer_request(const buffer_request& req)
	{
		_buffer_requests.push_back(req);
	}

	bool resource_queue::flush_all(LinaGX::CommandStream* cmd)
	{
		if (_texture_requests.empty() && _buffer_requests.empty()) return false;
		uint8 count = 0;

		if (!_texture_requests.empty())
		{
			LinaGX::CMDBarrier* barrier	 = cmd->AddCommand<LinaGX::CMDBarrier>();
			barrier->textureBarrierCount = static_cast<uint32>(_texture_requests.size());
			barrier->textureBarriers	 = cmd->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * _texture_requests.size());
			barrier->srcStageFlags		 = LinaGX::PipelineStageFlags::PSF_TopOfPipe;
			barrier->dstStageFlags		 = LinaGX::PipelineStageFlags::PSF_Transfer;

			for (const texture_request& req : _texture_requests)
			{
				auto& texture_barrier = barrier->textureBarriers[count];
				gfx_util::get_texture_barrier_undef_2_trasfer_dest(req.hw, texture_barrier);
				count++;
			}
		}

		for (const texture_request& req : _texture_requests)
		{
			LinaGX::CMDCopyBufferToTexture2D* copy = cmd->AddCommand<LinaGX::CMDCopyBufferToTexture2D>();
			copy->destTexture					   = req.hw;
			copy->mipLevels						   = req.mip_levels;
			copy->buffers						   = cmd->EmplaceAuxMemory<LinaGX::TextureBuffer>(&_reuse_buffers[req._buffer_start], req.mip_levels * sizeof(LinaGX::TextureBuffer));
		}

		count = 0;
		if (!_texture_requests.empty())
		{
			LinaGX::CMDBarrier* barrier	 = cmd->AddCommand<LinaGX::CMDBarrier>();
			barrier->textureBarrierCount = static_cast<uint32>(_texture_requests.size());
			barrier->textureBarriers	 = cmd->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * _texture_requests.size());
			barrier->srcStageFlags		 = LinaGX::PipelineStageFlags::PSF_Transfer;
			;
			barrier->dstStageFlags = LinaGX::PipelineStageFlags::PSF_FragmentShader;

			for (const texture_request& req : _texture_requests)
			{
				auto& texture_barrier = barrier->textureBarriers[count];
				gfx_util::get_texture_barrier_transfer_dest_2_sampled(req.hw, texture_barrier);
				count++;
			}
		}

		for (const buffer_request& req : _buffer_requests)
		{
			if (req.buffer->copy(cmd)) count++;
		}

		_texture_requests.resize(0);
		_buffer_requests.resize(0);
		_reuse_buffers.resize(0);

		return count != 0;
	}
} // namespace Lina
