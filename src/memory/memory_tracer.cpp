// Copyright (c) 2025 Inan Evin

#include "memory_tracer.hpp"

#ifdef ENABLE_MEMORY_TRACER

#include "memory.hpp"
#include "io/assert.hpp"
#include "io/file_system.hpp"
#include "io/log.hpp"
#include "platform/process.hpp"
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <DbgHelp.h>

#include <iostream>

#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "DbgHelp.lib")

namespace Game
{
	uint8 memory_tracer::s_category_counter = 0;

	namespace
	{
		void print(uint32 n)
		{
			const size_t bufferSize = 256;
			char*		 buffer		= static_cast<char*>(malloc(bufferSize));
			if (!buffer)
				return;
			const int written = snprintf(buffer, bufferSize, " %d\n", n);
			if (written > 0 && static_cast<size_t>(written) < bufferSize)
			{
				WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buffer, static_cast<DWORD>(written), NULL, NULL);
			}
		}
	}
	void memory_tracer::on_allocation(void* ptr, size_t sz)
	{
		LOCK_GUARD(_category_mtx);

		memory_track& track = _allocations[ptr];
		track.ptr			= ptr;
		track.size			= sz;
		capture_trace(track);

		if (_categories.empty() || _current_active_category == 0)
			return;

		memory_category& cat = _categories[_current_active_category - 1];
		cat.total_size += track.size;

		capture_trace(track);
	}

	void memory_tracer::on_free(void* ptr)
	{
		LOCK_GUARD(_category_mtx);

		auto it = _allocations.find(ptr);
		if (it != _allocations.end())
		{
			_allocations.erase(it);

			if (!_categories.empty() && _current_active_category != 0)
			{
				memory_category& cat = _categories[_current_active_category - 1];
				cat.total_size -= it->second.size;
				GAME_ASSERT(cat.total_size >= 0);
			}

			return;
		}
	}

	void memory_tracer::push_category(const char* name)
	{
		LOCK_GUARD(_category_mtx);

		auto it = std::find_if(_categories.begin(), _categories.end(), [&](const memory_category& saved) -> bool { return strcmp(saved.name, name) == 0; });
		if (it != _categories.end())
		{
			_current_active_category = it->id;
			_category_ids.push_back(_current_active_category);
			return;
		}

		memory_category cat = {};
		const size_t	sz	= strlen(name) + 1;
		cat.name			= reinterpret_cast<const char*>(malloc(sz));

		if (cat.name)
			GAME_MEMCPY((void*)cat.name, (void*)name, sz);
		cat.id = ++s_category_counter;
		_category_ids.push_back(s_category_counter);
		_current_active_category = s_category_counter;
		_categories.push_back(cat);
	}

	void memory_tracer::pop_category()
	{
		LOCK_GUARD(_category_mtx);

		if (_category_ids.size() > 1)
		{
			// pop current active one.
			_category_ids.pop_back();
			const uint8 id = _category_ids.back();
			_category_ids.pop_back();
			_current_active_category = id;
		}
		else
		{
			_current_active_category = 1;
		}
	}

	void memory_tracer::capture_trace(memory_track& track)
	{
		track.stack_size = CaptureStackBackTrace(3, MEMORY_STACK_TRACE_SIZE, track.stack, nullptr);
	}

	void memory_tracer::destroy()
	{
		HANDLE process = GetCurrentProcess();
		SymCleanup(process);

		for (const memory_category& cat : _categories)
			free((void*)cat.name);

		check_leaks();
	}

	void memory_tracer::check_leaks()
	{
		for (auto& [ptr, alloc] : _allocations)
		{
			std::ostringstream ss;

			ss << "****************** LEAK DETECTED ******************\n";
			ss << "Size: " << alloc.size << " bytes \n";

			HANDLE		process = GetCurrentProcess();
			static bool inited	= false;

			if (!inited)
			{
				inited = true;
				SymInitialize(process, nullptr, TRUE);
			}

			void* symbolAll = calloc(sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR), 1);

			if (symbolAll == NULL)
				return;

			SYMBOL_INFO* symbol	 = static_cast<SYMBOL_INFO*>(symbolAll);
			symbol->MaxNameLen	 = 255;
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

			DWORD			 displacement;
			IMAGEHLP_LINE64* line = NULL;
			line				  = (IMAGEHLP_LINE64*)std::malloc(sizeof(IMAGEHLP_LINE64));

			if (line == NULL)
				return;

			line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);

			bool not_valid = false;

			for (int i = 0; i < alloc.stack_size; ++i)
			{
				ss << "------ Stack Trace " << i << "------\n";

				DWORD64 address = (DWORD64)(alloc.stack[i]);

				SymFromAddr(process, address, NULL, symbol);

				if (SymGetLineFromAddr64(process, address, &displacement, line))
				{
					const string fn = line->FileName;

					if (fn.find("LinaGX") != string::npos)
					{
						not_valid = true;
						break;
					}

					ss << "Location:" << line->FileName << "\n";
					ss << "Smybol:" << symbol->Name << "\n";
					ss << "Line:" << line->LineNumber << "\n";
					ss << "SymbolAddr:" << symbol->Address << "\n";
				}
				else
				{
					ss << "Smybol:" << symbol->Name << "\n";
					ss << "SymbolAddr:" << symbol->Address << "\n";
				}

				IMAGEHLP_MODULE64 moduleInfo;
				moduleInfo.SizeOfStruct = sizeof(moduleInfo);
				if (::SymGetModuleInfo64(process, symbol->ModBase, &moduleInfo))
					ss << "Module:" << moduleInfo.ModuleName << "\n";
			}

			if (not_valid)
				continue;

			std::free(line);
			std::free(symbolAll);

			ss << "\n";
			ss << "\n";

			process::message_box(ss.str().c_str());
			ss.clear();
		}
	}
} // namespace Lina

#endif
