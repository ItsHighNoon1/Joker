#include <iostream>

#include "DisplayManager.h"
#include "Loader.h"
#include "Model.h"
#include "Renderer.h"
#include "Shader.h"

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
		Shader shader = Shader("vertexShader.glsl", "fragmentShader.glsl");

		void init() {
			GLfloat data[] = {
				-1.0f, -1.0f, 0.0f,
				 1.0f, -1.0f, 0.0f,
				 0.0f,  1.0f, 0.0f,
			};
			GLuint indices[] = { 0, 1, 2 };
			mesh = loader.loadToVAO(data, indices, 3);
		}

		void loop() {
			renderer.prepare();
			renderer.render(shader, mesh);
			display.updateDisplay();
		}

		void stop() {
			shader.cleanUp();
			loader.cleanUp();
		}
	};
}

int main() {
	Joker::JokerApplication app;
	app.run();
	return 0;
}