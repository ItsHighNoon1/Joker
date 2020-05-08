#pragma once

#include <chrono>
#include <map>
#include <string>

namespace Joker {
	class Profiler {
		// Time various actions so we know where to optimize
	public:
		std::string flushProfiler();
		void beginSection(std::string section);
		void tick();
	private:
		std::chrono::steady_clock::time_point lastTime;
		std::string lastString;
		std::map<std::string, float> times;
		uint32_t frames;
		float totalTime;
	};
}