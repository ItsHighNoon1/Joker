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
#include "InputHandler.h"
#include "Log.h"

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
		Model gui;
		Model font;
		Text funText;
		Sound sound;
		BasicShader shader = BasicShader("res/basicShader.vert", "res/basicShader.frag");
		GuiShader guiShader = GuiShader("res/guiShader.vert", "res/guiShader.frag");
		ShadowShader shadozer = ShadowShader("res/shadowShader.vert", "res/shadowShader.frag");
		TextShader textShader = TextShader("res/textShader.vert", "res/textShader.frag");
		InputHandler& input = display.input; // We want to use the same input as DisplayManager because it does some work for us
		AudioManager audio = AudioManager(loader);
		Framebuffer shadowFbo;
		
		bool camLocked = false;
		float rotX = 0.0f;
		float rotY = 1.5f;
		glm::vec3 position = glm::vec3(-60.0f, 15.0f, 00.0f);
		glm::vec3 moonPosition = glm::vec3(0.0f);
		glm::vec3 moonMoon1 = glm::vec3(0.0f);
		glm::vec3 moonMoon2 = glm::vec3(0.0f);
		glm::mat4 guiTransform = glm::mat4(1.0f);
		float t = 0.0f;

		void init() {
			shadowFbo = loader.loadFramebuffer(800, 500);

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
			input.registerKeyCallback(keyHandler);
			input.registerMouseButtonCallback(clickHandler);

			gui.mesh = loader.loadGUI();
			gui.texture = shadowFbo.colorbuffer;
			guiTransform = glm::translate(guiTransform, glm::vec3(0.75f, 0.75f, 0.75f));
			guiTransform = glm::scale(guiTransform, glm::vec3(0.25f));

			font.mesh = gui.mesh;
			font.texture = loader.loadTexture("res/font.png");
			funText.string = "Pickle Chungus";
			funText.font = loader.loadFont("res/font.fnt");
		}

		void loop() {
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

			// Camera controls
			if (camLocked) {
				// Rotation
				rotX += input.dy / 3000.0f;
				rotY += input.dx / 3000.0f;

				// Motion
				glm::vec3 relativeVelocity = glm::vec3(0.0);
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
				position += relativeVelocity * display.dt;
			}

			// Get the matrices and such in order
			glm::mat4 viewMatrix = glm::mat4(1.0f);
			viewMatrix = glm::rotate(viewMatrix, -rotX, glm::vec3(1.0f, 0.0f, 0.0f));
			viewMatrix = glm::rotate(viewMatrix, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
			viewMatrix = glm::translate(viewMatrix, -position);
			glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 8.0f / 5.0f, 1.0f, 150.0f);
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
			glm::mat4 moonMoon2Matrix = glm::mat4(1.0f);
			moonMoon2Matrix = glm::translate(moonMoon2Matrix, moonMoon2);
			glm::mat4 textTransform = glm::mat4(1.0f);
			textTransform = glm::translate(textTransform, glm::vec3(-0.8f, 0.8f * cosf(t), 0.0f));
			textTransform = glm::rotate(textTransform, sinf(t), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::vec3 lightDirection = glm::vec3(1.0f, -0.3f, 0.0f);

			// Render to the shadow map
			renderer.bindFrameBuffer(shadowFbo);
			renderer.enableDepthTest();
			renderer.prepare();
			shadozer.start();
			shadozer.calculateShadowMatrix(lightDirection, position);
			shadozer.render(earthModel, earthMatrix);
			shadozer.render(moonModel, moonMatrix);
			shadozer.render(atlasModel, moonMoon1Matrix);
			shadozer.render(atlasModel, moonMoon2Matrix);
			shadozer.stop();

			// Render the real scene
			renderer.bindDefaultFrameBuffer();
			renderer.enableDepthTest();
			renderer.prepare();
			shader.start();
			shader.uploadLightDirection(lightDirection);
			shader.render(earthModel, earthMatrix, viewMatrix, projectionMatrix, shadowFbo.depthbuffer, shadozer.shadowMatrix);
			shader.render(moonModel, moonMatrix, viewMatrix, projectionMatrix, shadowFbo.depthbuffer, shadozer.shadowMatrix);
			shader.render(atlasModel, moonMoon1Matrix, viewMatrix, projectionMatrix, shadowFbo.depthbuffer, shadozer.shadowMatrix, 2, 2);
			shader.render(atlasModel, moonMoon2Matrix, viewMatrix, projectionMatrix, shadowFbo.depthbuffer, shadozer.shadowMatrix, 0, 2);
			shader.stop();

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
