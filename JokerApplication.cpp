#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "AudioManager.h"
#include "DisplayManager.h"
#include "Loader.h"
#include "Object.h"
#include "Renderer.h"
#include "Shader.h"
#include "BasicShader.h"
#include "GuiShader.h"
#include "ShadowShader.h"
#include "TextShader.h"
#include "ParticleShader.h"
#include "InputHandler.h"
#include "Log.h"
#include "Entity.h"
#include "Components/TestComponent.h"

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
		JokerApplication() {
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
		Renderer renderer;
		Model earthModel;
		Model moonModel;
		Model atlasModel;
		Model waluigiModel;
		Model battlefieldModel;
		Model gui;
		Model font;
		Model particle;
		Text funText;
		Sound sound;
		BasicShader shader = BasicShader("res/basicShader.vert", "res/basicShader.frag");
		GuiShader guiShader = GuiShader("res/guiShader.vert", "res/guiShader.frag");
		ShadowShader shadozer = ShadowShader("res/shadowShader.vert", "res/shadowShader.frag");
		TextShader textShader = TextShader("res/textShader.vert", "res/textShader.frag");
		ParticleShader particleShader = ParticleShader("res/particleShader.vert", "res/particleShader.frag");
		InputHandler& input = display.input; // We want to use the same input as DisplayManager because it does some work for us
		AudioManager audio = AudioManager(loader);
		Framebuffer shadowFbo;
		Entity e;
		
		bool camLocked = false;
		float rotX = 0.0f;
		float rotY = 1.5f;
		glm::vec3 position = glm::vec3(-60.0f, 15.0f, 00.0f);
		glm::vec3 moonPosition = glm::vec3(0.0f);
		glm::vec3 moonMoon1 = glm::vec3(0.0f);
		glm::vec3 moonMoon2 = glm::vec3(0.0f);
		glm::vec3 waluigiPosition = glm::vec3(00.0f, 40.0f, 00.0f);
		glm::vec3 battlefieldPosition = glm::vec3(00.0f, -70.0f, 00.0f);
		glm::mat4 guiTransform = glm::mat4(1.0f);
		float t = 0.0f;

		void init() {
			shadowFbo = loader.loadFramebuffer(2000, 2000);

			int present = glfwJoystickIsGamepad(GLFW_JOYSTICK_1);
			if (present == 1) {
				std::cout<<"Joystick is pad"<<std::endl;
			}
			else {
				std::cout << "Joystick is not" << std::endl;
			}



			// Load some stuff
			sound.buffer = loader.loadFromWAV("res/buzz.wav");
			sound.position = &moonPosition;
			Mesh mesh = loader.loadFromOBJ("res/earth.obj");
			GLuint texture = loader.loadTexture("res/earth.png");
			earthModel.mesh = mesh;
			earthModel.texture = texture;
			texture = loader.loadTexture("res/moon.png");
			moonModel.mesh = mesh;
			moonModel.texture = texture;
			texture = loader.loadTexture("res/test2.png");
			atlasModel.mesh = mesh;
			atlasModel.texture = texture;
			Mesh waluigiMesh = loader.loadFromOBJ("res/waluigi.obj");
			GLuint waluigiTexture = loader.loadTexture("res/waluigi.png");
			waluigiModel.mesh = waluigiMesh;
			waluigiModel.texture = waluigiTexture;
			Mesh battlefieldMesh = loader.loadFromOBJ("res/battlefield.obj");
			GLuint battlefieldTexture = loader.loadTexture("res/test2.png");
			battlefieldModel.mesh = battlefieldMesh;
			battlefieldModel.texture = battlefieldTexture;
			input.registerKeyCallback(keyHandler);
			input.registerMouseButtonCallback(clickHandler);

			Mesh quad = loader.loadQuad();
			gui.mesh = quad;
			gui.texture = shadowFbo.colorbuffer;
			guiTransform = glm::translate(guiTransform, glm::vec3(0.75f, 0.75f, 0.75f));
			guiTransform = glm::scale(guiTransform, glm::vec3(0.25f));

			font.mesh = quad;
			font.texture = loader.loadTexture("res/font.png");
			funText.string = "Waluigi Time";
			funText.font = loader.loadFont("res/font.fnt");

			particle.mesh = quad;
			particle.texture = loader.loadTexture("res/test2.png");

			// Entity test
			e.addComponent(std::make_unique<TestComponent>());
		}

		void loop() {
			e.tick();
			audio.tick(position, rotX, rotY);
			t += display.dt;

			// Move the moon
			moonPosition.x = 50.0f * sinf(t + 1.5f);
			moonPosition.y = 20.0f * cosf(t * 1.3f + 1.5f);
			moonPosition.z = 50.0f * cosf(t + 1.5f);

			// Move the colorful moons
			moonMoon1.x = 7.0f * sinf(t) + moonPosition.x;
			moonMoon1.y = 7.0f * cosf(t / 0.9f) + moonPosition.y;
			moonMoon1.z = 7.0f * cosf(t) + moonPosition.z;
			moonMoon2.x = 7.0f * sinf(t + 1.5f) + moonPosition.x;
			moonMoon2.y = 7.0f * cosf(t / 1.1f + 1.0f) + moonPosition.y;
			moonMoon2.z = 7.0f * cosf(t + 1.5f) + moonPosition.z;

			// If gamepad press A, then move Waluigi
			GLFWgamepadstate state;

			if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
				if (state.buttons[GLFW_GAMEPAD_BUTTON_A]) {
					waluigiPosition.y = waluigiPosition.y + 1;
				}
				else {
					if (waluigiPosition.y > 40) {
						waluigiPosition.y = waluigiPosition.y - 1;
					}
				}

				if (abs(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]) > 0.15f) {
					waluigiPosition.z += state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
				}
				if (abs(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]) > 0.15f) {
					waluigiPosition.x -= state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
				}
			}

			// Camera controls
			if (camLocked) {
				// Rotation
				rotX += input.dy / 3000.0f;
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
				position += relativeVelocity * display.dt;
			}

			// Get the matrices and such in order
			glm::mat4 viewMatrix = glm::mat4(1.0f);
			viewMatrix = glm::rotate(viewMatrix, -rotX, glm::vec3(1.0f, 0.0f, 0.0f));
			viewMatrix = glm::rotate(viewMatrix, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
			viewMatrix = glm::translate(viewMatrix, -position);

			glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 8.0f / 5.0f, 1.0f, 1500.0f);

			glm::mat4 earthMatrix = glm::mat4(1.0f);
			earthMatrix = glm::translate(earthMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
			earthMatrix = glm::rotate(earthMatrix, t / 10.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			earthMatrix = glm::scale(earthMatrix, glm::vec3(40.0f));

			glm::mat4 moonMatrix = glm::mat4(1.0f);
			moonMatrix = glm::translate(moonMatrix, moonPosition);
			moonMatrix = glm::rotate(moonMatrix, t, glm::vec3(1.0f, 0.0f, -1.0f));
			moonMatrix = glm::scale(moonMatrix, glm::vec3(5.0f));

			glm::mat4 moonMoon1Matrix = glm::mat4(1.0f);
			moonMoon1Matrix = glm::translate(moonMoon1Matrix, moonMoon1);

			glm::mat4 waluigiMatrix = glm::mat4(1.0f);
			waluigiMatrix = glm::translate(waluigiMatrix, waluigiPosition);
			waluigiMatrix = glm::scale(waluigiMatrix, glm::vec3(0.2f));
			waluigiMatrix = glm::rotate(waluigiMatrix, -rotY, glm::vec3(0.0f, 3.0f, 0.0f));

			glm::mat4 battlefieldMatrix = glm::mat4(1.0f);
			battlefieldMatrix = glm::translate(battlefieldMatrix, battlefieldPosition);
			battlefieldMatrix = glm::scale(battlefieldMatrix, glm::vec3(20.0f));
			
			glm::mat4 textTransform = glm::mat4(1.0f);
			textTransform = glm::translate(textTransform, glm::vec3(-0.8f, 0.8f, 0.0f));
			textTransform = glm::scale(textTransform, glm::vec3(5.0f / 8.0f, 1.0f, 0.0f));

			glm::mat4 particleMatrix = glm::mat4(1.0f);
			particleMatrix = glm::translate(particleMatrix, moonMoon2);
			particleMatrix = glm::rotate(particleMatrix, -rotY, glm::vec3(0.0f, 1.0f, 0.0f));
			particleMatrix = glm::rotate(particleMatrix, rotX, glm::vec3(1.0f, 0.0f, 0.0f));

			glm::vec3 lightDirection = glm::vec3(1.0f, -0.3f, 0.0f);

			// Render to the shadow map
			renderer.bindFrameBuffer(shadowFbo);
			renderer.enableDepthTest();
			renderer.enableTransparency();
			renderer.prepare();
			shadozer.start();
			shadozer.calculateShadowMatrix(lightDirection, position);
			shadozer.render(earthModel, earthMatrix);
			shadozer.render(moonModel, moonMatrix);
			shadozer.render(atlasModel, moonMoon1Matrix);
			shadozer.render(waluigiModel, waluigiMatrix);
			shadozer.render(battlefieldModel, battlefieldMatrix);
			shadozer.stop();

			// Render the real scene
			renderer.bindDefaultFrameBuffer();
			renderer.enableDepthTest();
			renderer.enableTransparency();
			renderer.prepare();
			shader.start();
			shader.uploadLightDirection(lightDirection);
			shader.render(earthModel, earthMatrix, viewMatrix, projectionMatrix, shadowFbo.depthbuffer, shadozer.shadowMatrix);
			shader.render(moonModel, moonMatrix, viewMatrix, projectionMatrix, shadowFbo.depthbuffer, shadozer.shadowMatrix);
			shader.render(atlasModel, moonMoon1Matrix, viewMatrix, projectionMatrix, shadowFbo.depthbuffer, shadozer.shadowMatrix, 2, 2);
			shader.render(waluigiModel, waluigiMatrix, viewMatrix, projectionMatrix, shadowFbo.depthbuffer, shadozer.shadowMatrix);
			shader.render(battlefieldModel, battlefieldMatrix, viewMatrix, projectionMatrix, shadowFbo.depthbuffer, shadozer.shadowMatrix);
			shader.stop();
			particleShader.start();
			particleShader.render(particle, particleMatrix, viewMatrix, projectionMatrix, shadowFbo.depthbuffer, shadozer.shadowMatrix);
			particleShader.stop();

			// Render GUI
			renderer.disableDepthTest();
			guiShader.start();
			guiShader.render(gui, guiTransform);
			guiShader.stop();

			// Render text
			textShader.start();
			textShader.render(funText, textTransform, font);

			display.updateDisplay();
		}

		void stop() {
			shader.cleanUp();
			guiShader.cleanUp();
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
