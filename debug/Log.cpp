#include "Log.h"

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Joker {
	std::shared_ptr<spdlog::logger> Log::coreLogger;
	std::shared_ptr<spdlog::logger> Log::clientLogger;

	void Log::init(const char* appName, spdlog::level::level_enum level) {
		// [time][name]: message
		spdlog::set_pattern("%^[%T][%n]: %v%$");
		coreLogger = spdlog::stdout_color_mt("Joker");
		coreLogger->set_level(level);
		clientLogger = spdlog::stdout_color_mt(appName);
		clientLogger->set_level(level);
	}
}