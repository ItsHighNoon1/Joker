#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "DisplayManager.h"
#include "Loader.h"
#include "Model.h"
#include "Renderer.h"
#include "Shader.h"
#include "BasicShader.h"

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
		BasicShader shader = BasicShader("basicShader.vert", "basicShader.frag");

		float z = 0.0f;

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
			z -= 0.01f;
			renderer.prepare();

			shader.start();
			glm::mat4 modelMatrix = glm::rotate(z, glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, z));
			glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 8.0f/5.0f, 0.1f, 100.0f);
			shader.render(mesh, modelMatrix, viewMatrix, projectionMatrix);
			shader.stop();

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