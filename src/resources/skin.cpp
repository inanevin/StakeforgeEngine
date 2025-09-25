// Copyright (c) 2025 Inan Evin

#include "skin.hpp"
#include "skin_raw.hpp"
#include "memory/chunk_allocator.hpp"

namespace SFG
{
	void skin::create_from_raw(const skin_raw& raw, chunk_allocator32& alloc)
	{
		_root		  = raw.root_joint;
		_joints_count = static_cast<uint16>(raw.joints.size());

		if (!raw.name.empty())
		{
			_name = alloc.allocate<uint8>(raw.name.size());
			strcpy((char*)alloc.get(_name.head), raw.name.data());
			strcpy((char*)(alloc.get(_name.head + _name.size)), "\0");
		}

		_joints = alloc.allocate<skin_joint>(raw.joints.size());

		skin_joint*	 ptr   = reinterpret_cast<skin_joint*>(alloc.get(_joints.head));
		const uint32 count = static_cast<uint32>(raw.joints.size());

		for (uint32 i = 0; i < count; i++)
			ptr[i] = raw.joints[i];
	}

	void skin::destroy(chunk_allocator32& alloc)
	{
		if (_name.size != 0)
			alloc.free(_name);

		alloc.free(_joints);

		_name	= {};
		_joints = {};
	}

}