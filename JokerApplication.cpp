#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "AudioManager.h"
#include "DisplayManager.h"
#include "Loader.h"
#include "Model.h"
#include "Renderer.h"
#include "Shader.h"
#include "BasicShader.h"
#include "InputHandler.h"

void keyHandler(GLFWwindow* w, int key, int scancode, int action, int mods);
void clickHandler(GLFWwindow* w, int button, int action, int mods);

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

		void lockCursor() {
			input.setInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			camLocked = true;
		}

		void unlockCursor() {
			input.setInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			camLocked = false;
		}

		void playSound() {
			audio.playSound(sound);
		}
	private:
		DisplayManager display;
		Loader loader;
		Renderer renderer;
		Model model;
		Sound sound;
		BasicShader shader = BasicShader("res/basicShader.vert", "res/basicShader.frag");
		InputHandler& input = display.input; // We want to use the same input as DisplayManager because it does some work for us
		AudioManager audio = AudioManager(loader);
		
		bool camLocked = false;
		float rotX = 0.0f;
		float rotY = 0.0f;
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 earthPosition = glm::vec3(0.0f);
		float t = 0.0f;

		void init() {
			Mesh mesh = loader.loadFromOBJ("res/earth.obj");
			GLuint texture = loader.loadTexture("res/earth.png");
			sound.buffer = loader.loadFromWAV("res/crunch.wav");
			sound.position = &earthPosition;
			model.mesh = mesh;
			model.texture = texture;
			input.registerKeyCallback(keyHandler);
			input.registerMouseButtonCallback(clickHandler);
		}

		void loop() {
			audio.tick(position, rotX, rotY);
			renderer.prepare();
			t += display.dt;

			// Move the Earth
			earthPosition.x = 5.0f * sinf(t);
			earthPosition.z = 5.0f * cosf(t);

			// Camera controls
			if (camLocked) {
				// Rotation
				rotX += input.dy / 3000.0f;
				rotY += input.dx / 3000.0f;

				// Motion
				glm::vec3 relativeVelocity = glm::vec3(0.0);
				if (input.getKeyState(GLFW_KEY_W) == GLFW_PRESS) {
					relativeVelocity.z -= cosf(rotY) * 0.1f;
					relativeVelocity.x += sinf(rotY) * 0.1f;
				}
				if (input.getKeyState(GLFW_KEY_A) == GLFW_PRESS) {
					relativeVelocity.x -= cosf(rotY) * 0.1f;
					relativeVelocity.z -= sinf(rotY) * 0.1f;
				}
				if (input.getKeyState(GLFW_KEY_S) == GLFW_PRESS) {
					relativeVelocity.z += cosf(rotY) * 0.1f;
					relativeVelocity.x -= sinf(rotY) * 0.1f;
				}
				if (input.getKeyState(GLFW_KEY_D) == GLFW_PRESS) {
					relativeVelocity.x += cosf(rotY) * 0.1f;
					relativeVelocity.z += sinf(rotY) * 0.1f;
				}
				position += relativeVelocity;
			}

			// Rendering
			shader.start();
			glm::vec3 lightDirection = glm::vec3(1.0f, -1.0f, 0.0f);
			shader.uploadLightDirection(lightDirection);
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, earthPosition);
			modelMatrix = glm::rotate(modelMatrix, t, glm::vec3(1.0f, 0.0f, -1.0f));
			glm::mat4 viewMatrix = glm::mat4(1.0f);
			viewMatrix = glm::rotate(viewMatrix, -rotX, glm::vec3(1.0f, 0.0f, 0.0f));
			viewMatrix = glm::rotate(viewMatrix, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
			viewMatrix = glm::translate(viewMatrix, -position);
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

Joker::JokerApplication app;

int main() {
	app.run();
	return 0;
}

void keyHandler(GLFWwindow* w, int key, int scancode, int action, int mods) {
	// Test function
	if (action != GLFW_PRESS) {
		return;
	}
	if (key == GLFW_KEY_ESCAPE) {
		app.unlockCursor();
	} else if (key == GLFW_KEY_SPACE) {
		app.playSound();
	}
}

void clickHandler(GLFWwindow* w, int button, int action, int mods) {
	// Test function
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		app.lockCursor();
	}
}