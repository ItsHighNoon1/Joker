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
			Mesh mesh = loader.loadFromOBJ("res/earth.obj");
			GLuint texture = loader.loadTexture("res/earth.png");
			model.mesh = mesh;
			model.texture = texture;
		}

		void loop() {
			z -= 0.01f;
			renderer.prepare();

			shader.start();
			glm::vec3 lightDirection = glm::vec3(1.0f, -1.0f, 0.0f);
			shader.uploadLightDirection(lightDirection);
			glm::mat4 modelMatrix = glm::rotate(z, glm::vec3(1.0f, 0.0f, 0.0f));
			glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -3.0f));
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