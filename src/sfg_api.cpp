#include "sfg_api.hpp"
#include "app/game.hpp"
#include "io/log.hpp"
#

extern "C"
{
	SFG::game_app* create_app(uint32 width, uint32 height)
	{
		SFG::game_app* app = new SFG::game_app();
		app->init({width, height});
		// app->tick();
		return app;
	}

	void destroy_app(SFG::game_app* app)
	{
		// app->uninit();
		delete app;
	}

	void set_log_callback(log_fn cb)
	{
		static constexpr uint32 EXT_API_CB = 28273;
		SFG::log::instance().add_listener(EXT_API_CB, [cb](SFG::log_level level, const char* msg) { cb(static_cast<int32>(level), msg); });
	}
}