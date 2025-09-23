// Copyright (c) 2025 Inan Evin

#include "skin.hpp"
#include "memory/chunk_allocator.hpp"

namespace SFG
{
#ifdef SFG_TOOLMODE
	void skin::create_from_loaded(const skin_loaded& loaded, chunk_allocator32& alloc)
	{
		_root		  = loaded.root_joint;
		_joints_count = static_cast<uint16>(loaded.joints.size());

		if (!loaded.name.empty())
		{
			_name = alloc.allocate<uint8>(loaded.name.size());
			strcpy((char*)alloc.get(_name.head), loaded.name.data());
		}

		_joints = alloc.allocate<skin_joint>(loaded.joints.size());

		skin_joint*	 ptr   = reinterpret_cast<skin_joint*>(alloc.get(_joints.head));
		const uint32 count = static_cast<uint32>(loaded.joints.size());

		for (uint32 i = 0; i < count; i++)
			ptr[i] = loaded.joints[i];
	}

	void skin::destroy(chunk_allocator32& alloc)
	{
		if (_name.size != 0)
			alloc.free(_name);

		alloc.free(_joints);

		_name	= {};
		_joints = {};
	}
#endif

}