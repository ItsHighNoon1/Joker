#include "Profiler.h"

#include <chrono>
#include <map>
#include <string>

#include "Log.h"

namespace Joker {
	std::string Profiler::flushProfiler() {
		std::string finalString;
		// Iterate over the map
		for (auto iterator : times) {
			finalString += iterator.first; // Show the category
			finalString += ": "; // Colon for formatting
			finalString += std::to_string(iterator.second / totalTime * 100.0f); // Show the percent of total time
			finalString += "%\n"; // Show percent symbol and then newline
		}

		// Add framerate just because
		finalString += std::to_string(frames / totalTime);
		finalString += " FPS";

		// Prepare for next profiling
		frames = 0;
		totalTime = 0.0f;
		times.clear();

		return finalString;
	}

	void Profiler::beginSection(std::string section) {
		// Calculate the length of the previous section
		std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();
		auto nanosElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastTime).count();
		lastTime = now; // Set time ASAP because the profiler itself takes time

		// Add the elapsed time to the total time spent on this section
		float secondsElapsed = (float)nanosElapsed / (float)1000000000;
		times[lastString] += secondsElapsed;
		totalTime += secondsElapsed;

		// Prepare the next section
		lastString = section;
	}

	void Profiler::tick() {
		frames++;
	}
}