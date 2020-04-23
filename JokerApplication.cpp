#include <spdlog/spdlog.h>

#include "APIWrapper.h"

namespace Joker {
	class JokerApplication {
	public:
		void run() {
			init();
			while (!gl.shouldClose()) {
				loop();
			}
			stop();
		}
	private:
		void init() {
		}

		void loop() {
			gl.poll();
		}

		void stop() {
		}

		APIWrapper gl;
	};
}

int main() {
	Joker::JokerApplication app;
	app.run();
	return 0;
}