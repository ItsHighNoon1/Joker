#pragma once

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Joker {
	class Log {
		// Contains our loggers
	public:
		static void init(const char* appName, spdlog::level::level_enum level);

		inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return clientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> coreLogger;
		static std::shared_ptr<spdlog::logger> clientLogger;
	};

}

// #defines make logging much easier
#ifdef _DEBUG
#define JK_CORE_TRACE(...) ::Joker::Log::getCoreLogger()->trace(__VA_ARGS__)
#define JK_CORE_INFO(...) ::Joker::Log::getCoreLogger()->info(__VA_ARGS__)
#define JK_CORE_WARN(...) ::Joker::Log::getCoreLogger()->warn(__VA_ARGS__)
#define JK_CORE_ERROR(...) ::Joker::Log::getCoreLogger()->error(__VA_ARGS__)
#define JK_CORE_CRITICAL(...) ::Joker::Log::getCoreLogger()->critical(__VA_ARGS__)
#define JK_CORE_ASSERT(x, ...) { if(!(x)) { JK_CORE_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

#define JK_TRACE(...) ::Joker::Log::getClientLogger()->trace(__VA_ARGS__)
#define JK_INFO(...) ::Joker::Log::getClientLogger()->info(__VA_ARGS__)
#define JK_WARN(...) ::Joker::Log::getClientLogger()->warn(__VA_ARGS__)
#define JK_ERROR(...) ::Joker::Log::getClientLogger()->error(__VA_ARGS__)
#define JK_CRITICAL(...) ::Joker::Log::getClientLogger()->critical(__VA_ARGS__)
#define JK_ASSERT(x, ...) { if(!(x)) { JK_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else 
#define JK_CORE_TRACE(...)
#define JK_CORE_INFO(...)
#define JK_CORE_WARN(...)
#define JK_CORE_ERROR(...)
#define JK_CORE_CRITICAL(...)
#define JK_CORE_ASSERT(x, ...)

#define JK_TRACE(...)
#define JK_INFO(...)
#define JK_WARN(...)
#define JK_ERROR(...)
#define JK_CRITICAL(...)
#define JK_ASSERT(x, ...)
#endif
