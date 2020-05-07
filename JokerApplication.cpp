#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "AudioManager.h"
#include "DisplayManager.h"
#include "Loader.h"
#include "InputHandler.h"
#include "Log.h"
#include "render/MasterRenderer.h"

class LogInitializerLol {
public:
	LogInitializerLol() {
		// Initialize the logger object before we do ANYTHING
		Joker::Log::init("Test app", spdlog::level::trace);
	}
};
LogInitializerLol a;

void keyHandler(GLFWwindow* w, int key, int scancode, int action, int mods);
void clickHandler(GLFWwindow* w, int button, int action, int mods);

namespace Joker {
	class JokerApplication {
	public:
		JokerApplication() : renderer(loader.loadQuad(), loader.loadFramebuffer(2000, 2000)) {
			JK_CORE_INFO("Initialized core and client loggers");
		};
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
		Sound sound;
		InputHandler& input = display.input; // We want to use the same input as DisplayManager because it does some work for us
		AudioManager audio = AudioManager(loader);

		MasterRenderer renderer;
		StaticRenderable earth;
		StaticRenderable moon;
		StaticRenderable atlas;
		StaticRenderable waluigi;
		StaticRenderable battlefield;
		GUIRenderable gui;
		ParticleRenderable particle;
		
		bool camLocked = false;
		float rotX = 0.0f;
		float rotY = 1.5f;
		glm::vec3 cameraPosition = glm::vec3(-60.0f, 15.0f, 0.0f);
		float t = 0.0f;

		void init() {
			int present = glfwJoystickIsGamepad(GLFW_JOYSTICK_1);
			if (present == 1) {
				std::cout<<"Joystick is pad"<<std::endl;
			} else {
				std::cout << "Joystick is not" << std::endl;
			}

			// Earth
			Texture earthTexture;
			earthTexture.texture = loader.loadTexture("res/earth.png");
			Model earthModel;
			earthModel.mesh = loader.loadFromOBJ("res/earth.obj");
			earthModel.texture = earthTexture;
			earth.model = earthModel;
			earth.scale = glm::vec3(40.0f);

			// Moon
			Texture moonTexture;
			moonTexture.texture = loader.loadTexture("res/moon.png");
			Model moonModel;
			moonModel.mesh = earthModel.mesh;
			moonModel.texture = moonTexture;
			moon.model = moonModel;
			moon.scale = glm::vec3(5.0f);

			// Atlas
			Texture atlasTexture;
			atlasTexture.texture = loader.loadTexture("res/test2.png");
			atlasTexture.numRows = 2;
			Model atlasModel;
			atlasModel.mesh = earthModel.mesh;
			atlasModel.texture = atlasTexture;
			atlas.model = atlasModel;
			atlas.texIndex = 0;
			atlas.scale = glm::vec3(1.0f);

			// Waluigi
			Texture waluigiTexture;
			waluigiTexture.texture = loader.loadTexture("res/waluigi.png");
			Model waluigiModel;
			waluigiModel.mesh = loader.loadFromOBJ("res/waluigi.obj");
			waluigiModel.texture = waluigiTexture;
			waluigi.model = waluigiModel;
			waluigi.position = glm::vec3(0.0f, 40.0f, 0.0f);
			waluigi.scale = glm::vec3(0.2f);

			// Battlefield
			Model battlefieldModel;
			battlefieldModel.mesh = loader.loadFromOBJ("res/battlefield.obj");
			battlefieldModel.texture = atlasTexture;
			battlefield.model = battlefieldModel;
			battlefield.position = glm::vec3(0.0f, -70.0f, 0.0f);
			battlefield.scale = glm::vec3(20.0f);

			// GUI
			gui.texture.texture = 1;
			gui.position = glm::vec3(0.75f, 0.75f, 0.0f);
			gui.scale = glm::vec2(0.25f, 0.25f);

			// Particle
			particle.texture = atlasTexture;
			particle.texIndex = 2;
			particle.scale = glm::vec2(1.0f);

			// Misc stuff
			sound.buffer = loader.loadFromWAV("res/buzz.wav");
			sound.position = &moon.position;
			input.registerKeyCallback(keyHandler);
			input.registerMouseButtonCallback(clickHandler);
		}

		void loop() {
			audio.tick(cameraPosition, rotX, rotY);
			t += display.dt;

			// Move stuff around
			earth.rotation.y = t / 10.0f;
			moon.position.x = 50.0f * sinf(t + 1.5f);
			moon.position.y = 20.0f * cosf(t * 1.3f + 1.5f);
			moon.position.z = 50.0f * cosf(t + 1.5f);
			moon.rotation.x = t;
			moon.rotation.z = -t;
			atlas.position.x = 7.0f * sinf(t) + moon.position.x;
			atlas.position.y = 7.0f * cosf(t / 0.9f) + moon.position.y;
			atlas.position.z = 7.0f * cosf(t) + moon.position.z;
			particle.position.x = 7.0f * sinf(t + 1.5f) + moon.position.x;
			particle.position.y = 7.0f * cosf(t / 1.1f + 1.0f) + moon.position.y;
			particle.position.z = 7.0f * cosf(t + 1.5f) + moon.position.z;

			// Controller stuff
			GLFWgamepadstate state;
			if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
				if (state.buttons[GLFW_GAMEPAD_BUTTON_A]) {
					waluigi.position.y += 1;
				} else {
					if (waluigi.position.y > 40) {
						waluigi.position.y -= 1;
					}
				}

				if (abs(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]) > 0.15f) {
					waluigi.position.z += state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
				}
				if (abs(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]) > 0.15f) {
					waluigi.position.x -= state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
				}
			}

			// Camera controls
			if (camLocked) {
				// Rotation
				rotX -= input.dy / 3000.0f;
				rotY += input.dx / 3000.0f;

				// Motion Keyboard
				glm::vec3 relativeVelocity = glm::vec3(0.0f);
				if (input.getKeyState(GLFW_KEY_W) == GLFW_PRESS) {
					relativeVelocity.z -= cosf(rotY) * 15.0f;
					relativeVelocity.x += sinf(rotY) * 15.0f;
				}
				if (input.getKeyState(GLFW_KEY_A) == GLFW_PRESS) {
					relativeVelocity.x -= cosf(rotY) * 15.0f;
					relativeVelocity.z -= sinf(rotY) * 15.0f;
				}
				if (input.getKeyState(GLFW_KEY_S) == GLFW_PRESS) {
					relativeVelocity.z += cosf(rotY) * 15.0f;
					relativeVelocity.x -= sinf(rotY) * 15.0f;
				}
				if (input.getKeyState(GLFW_KEY_D) == GLFW_PRESS) {
					relativeVelocity.x += cosf(rotY) * 15.0f;
					relativeVelocity.z += sinf(rotY) * 15.0f;
				}

				// Motion Gamepad
				if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] > 0.15f) {
					relativeVelocity.z -= cosf(rotY) * 15.0f;
					relativeVelocity.x += sinf(rotY) * 15.0f;
				}
				if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] < -0.15f) {
					relativeVelocity.x -= cosf(rotY) * 15.0f;
					relativeVelocity.z -= sinf(rotY) * 15.0f;
				}
				if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] < -0.15f) {
					relativeVelocity.z += cosf(rotY) * 15.0f;
					relativeVelocity.x -= sinf(rotY) * 15.0f;
				}
				if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] > 0.15f) {
					relativeVelocity.x += cosf(rotY) * 15.0f;
					relativeVelocity.z += sinf(rotY) * 15.0f;
				}
				cameraPosition += relativeVelocity * display.dt;
			}

			// Render
			glm::vec3 lightDirection = glm::vec3(1.0f, -0.3f, 0.0f);
			renderer.setEnvironment(lightDirection);
			renderer.setCamera(cameraPosition, glm::vec3(rotX, rotY, 0.0f), glm::radians(90.0f));

			renderer.submit(earth);
			renderer.submit(moon);
			renderer.submit(atlas);
			renderer.submit(waluigi);
			renderer.submit(battlefield);
			renderer.submit(gui);
			renderer.submit(particle);

			renderer.renderScene();

			display.updateDisplay();
		}

		void stop() {
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
