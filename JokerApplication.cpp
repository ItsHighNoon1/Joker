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
		Model model;
		BasicShader shader = BasicShader("res/basicShader.vert", "res/basicShader.frag");

		float z = 0.0f;

		void init() {
			GLfloat positions[] = {
				-1.0f, -1.0f, 0.0f,
				 1.0f, -1.0f, 0.0f,
				 0.0f,  1.0f, 0.0f,
			};
			GLfloat texCoords[] = {
				0.0f, 0.0f,
				1.0f, 0.0f,
				0.5f, 1.0f,
			};
			GLuint indices[] = { 0, 1, 2 };
			Mesh mesh = loader.loadToVAO(positions, texCoords, indices, 3);
			GLuint texture = loader.loadTexture("res/test.png");
			model.mesh = mesh;
			model.texture = texture;
		}

		void loop() {
			z -= 0.01f;
			renderer.prepare();

			shader.start();
			glm::mat4 modelMatrix = glm::rotate(z, glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, -2.0f + sinf(z)));
			glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 8.0f/5.0f, 0.1f, 100.0f);
			shader.render(model, modelMatrix, viewMatrix, projectionMatrix);
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