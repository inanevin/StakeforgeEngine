/*
This file is a part of: Stakeforge Engine
https://github.com/inanevin/StakeforgeEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2025-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "SFG/IO/Filewatcher.hpp"
#include "SFG/IO/FileSystem.hpp"

namespace SFG
{
	FileWatcher::FileWatcher(const char* path, uint32_t frames)
	{
		m_path	 = path;
		m_frames = frames;

		if (m_path.empty())
			return;

		if (!FS::Exists(m_path.c_str()))
			return;

		const String date = FS::GetLastModifiedDate(path);
		m_sid			  = TO_SID(date);
	}

	bool FileWatcher::Watch()
	{
		if (m_path.empty())
			return false;

		if (m_frameCtr < m_frames)
		{
			m_frameCtr++;
			return false;
		}

		if (!FS::Exists(m_path.c_str()))
			return false;

		m_frameCtr			= 0;
		const String   date = FS::GetLastModifiedDate(m_path.c_str());
		const StringID sid	= TO_SID(date);

		if (m_sid == sid)
			return false;

		m_sid = sid;
		return true;
	}

} // namespace SFG
