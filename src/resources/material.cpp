// Copyright (c) 2025 Inan Evin

#include "material.hpp"
#include "common/linagx_incl.hpp"

namespace Game
{
	void material::create(const size_t size, const LinaGX::DescriptorSetDesc& set_desc)
	{
		_hw = LinaGX::instance()->CreateDescriptorSet({
			set_desc,
		});

		if (!set_desc.bindings.empty() && set_desc.bindings[0].type == LinaGX::DescriptorType::UBO && size != 0)
		{
			_buffer.create(LinaGX::ResourceTypeHint::TH_ConstantBuffer, size, "Material");
			LinaGX::instance()->DescriptorUpdateBuffer({
				.setHandle = _hw,
				.binding   = 0,
				.buffers   = {_buffer.get_gpu()},
			});
		}
	}

	void material::destroy()
	{
		LinaGX::instance()->DestroyDescriptorSet(_hw);
		if (_buffer.get_size()) _buffer.destroy();
	}

	void material::update_textures(uint32 binding, const vector<uint32>& hws)
	{
		LinaGX::instance()->DescriptorUpdateImage({
			.setHandle = _hw,
			.binding   = binding,
			.textures  = hws,
		});
	}

	void material::update_samplers(uint32 binding, const vector<uint32>& hws)
	{
		LinaGX::instance()->DescriptorUpdateImage({
			.setHandle = _hw,
			.binding   = binding,
			.samplers  = hws,
		});
	}

	void material::set_ubo_data(size_t padding, uint8* data, size_t size)
	{
		_buffer.set_data(padding, data, size);
	}
} // namespace
