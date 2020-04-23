#include <spdlog/spdlog.h>

#include "DisplayManager.h"
#include "Loader.h"
#include "Model.h"
#include "Renderer.h"

namespace Joker {
	class JokerApplication {
	public:
		void run() {
			init();
			while (!display.shouldClose()) {
				loop();
			}
			stop();
		}
	private:
		DisplayManager display;
		Loader loader;
		Renderer renderer;
		Mesh mesh;

		void init() {
			float data[] = {
				-1.0f, -1.0f, 0.0f,
				 1.0f, -1.0f, 0.0f,
				 0.0f,  1.0f, 0.0f,
			};
			int indices[] = { 0, 1, 2 };
			mesh = loader.loadToVAO(data, indices, 3);
		}

		void loop() {
			renderer.prepare();
			renderer.render(mesh);
			display.updateDisplay();
		}

		void stop() {
			loader.cleanUp();
		}
	};
}

int main() {
	Joker::JokerApplication app;
	app.run();
	return 0;
}