// Copyright (c) 2025 Inan Evin
#include "gfx_util.hpp"
#include "common/linagx_incl.hpp"
#include "io/log.hpp"
#include <regex>
#include <stdarg.h>

namespace Game
{
	namespace
	{
		LINAGX_STRING FormatString(const char* fmt, va_list args)
		{
			// Determine the required size
			va_list args_copy;
			va_copy(args_copy, args);
			int size = vsnprintf(nullptr, 0, fmt, args_copy) + 1; // +1 for the null terminator
			va_end(args_copy);

			// Allocate a buffer and format the string
			std::vector<char> buffer(size);
			vsnprintf(buffer.data(), size, fmt, args);

			return std::string(buffer.data());
		}

		void LinaGX_ErrorCallback(const char* err, ...)
		{
			va_list args;
			va_start(args, err);

			// Use vsnprintf to safely format the string into a buffer.
			char buffer[4096]; // Adjust buffer size as needed.
			vsnprintf(buffer, sizeof(buffer), err, args);

			// Convert buffer to std::string for regex replacement.
			std::string formattedMessage(buffer);

			// Escape curly braces by replacing `{` with `{{` and `}` with `}}`.
			formattedMessage = std::regex_replace(formattedMessage, std::regex("\\{"), "{{");
			formattedMessage = std::regex_replace(formattedMessage, std::regex("\\}"), "}}");

			// Pass the formatted and sanitized message to LINA_ERR.
			GAME_ERR(formattedMessage.c_str());

			va_end(args);
		}

		void LinaGX_LogCallback(const char* err, ...)
		{
			va_list args;
			va_start(args, err);

			// Use vsnprintf to safely format the string into a buffer.
			char buffer[4096]; // Adjust buffer size as needed.
			vsnprintf(buffer, sizeof(buffer), err, args);

			// Convert buffer to std::string for regex replacement.
			std::string formattedMessage(buffer);

			// Escape curly braces by replacing `{` with `{{` and `}` with `}}`.
			formattedMessage = std::regex_replace(formattedMessage, std::regex("\\{"), "{{");
			formattedMessage = std::regex_replace(formattedMessage, std::regex("\\}"), "}}");

			// Pass the formatted and sanitized message to LINA_ERR.
			GAME_INFO(formattedMessage.c_str());

			va_end(args);
		}
	} // namespace

	void gfx_util::init()
	{
		LinaGX::set_instance(new LinaGX::Instance());
		LinaGX::Config.api			   = LinaGX::BackendAPI::Vulkan;
		LinaGX::Config.backbufferCount = BACK_BUFFER_COUNT;
		LinaGX::Config.framesInFlight  = FRAMES_IN_FLIGHT;
		LinaGX::Config.errorCallback   = LinaGX_ErrorCallback;
		LinaGX::Config.infoCallback	   = LinaGX_LogCallback;

		LinaGX::instance()->Initialize();
	}

	void gfx_util::uninit()
	{
		delete LinaGX::instance();
		LinaGX::set_instance(nullptr);
	}

	void gfx_util::get_texture_barrier_undef_2_trasfer_dest(uint32 hw, LinaGX::TextureBarrier& out_barrier)
	{
		out_barrier = {
			.texture		= hw,
			.isSwapchain	= false,
			.toState		= LinaGX::TextureState::TransferDestination,
			.srcAccessFlags = 0,
			.dstAccessFlags = LinaGX::AF_TransferWrite,
		};
	}

	void gfx_util::get_texture_barrier_transfer_dest_2_sampled(uint32 hw, LinaGX::TextureBarrier& out_barrier)
	{
		out_barrier = {
			.texture		= hw,
			.isSwapchain	= false,
			.toState		= LinaGX::TextureState::ShaderRead,
			.srcAccessFlags = LinaGX::AF_TransferWrite,
			.dstAccessFlags = LinaGX::AF_ShaderRead,
		};
	}

	void gfx_util::get_texture_barrier_color_att_2_present(uint32 hw, LinaGX::TextureBarrier& out_barrier)
	{
		out_barrier = {
			.texture		= hw,
			.isSwapchain	= true,
			.toState		= LinaGX::TextureState::Present,
			.srcAccessFlags = LinaGX::AF_ColorAttachmentWrite,
			.dstAccessFlags = 0,
		};
	}

	void gfx_util::get_texture_barrier_present_2_color_att(uint32 hw, LinaGX::TextureBarrier& out_barrier)
	{
		out_barrier = {
			.texture		= hw,
			.isSwapchain	= true,
			.toState		= LinaGX::TextureState::ColorAttachment,
			.srcAccessFlags = LinaGX::AF_MemoryRead | LinaGX::AF_MemoryWrite,
			.dstAccessFlags = LinaGX::AF_ColorAttachmentRead,
		};
	}

}