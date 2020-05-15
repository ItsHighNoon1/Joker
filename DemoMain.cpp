#include "Joker.h"

// Helper classes
Joker::DisplayManager display;
Joker::InputHandler& input = display.input;
Joker::Allocator loader;
Joker::Profiler profiler;
Joker::AudioManager audio = Joker::AudioManager(loader);
Joker::GeometryGenerator generator = Joker::GeometryGenerator(loader);
Joker::MasterRenderer renderer = Joker::MasterRenderer(loader);

// Game objects


// Other globals
glm::vec3 cameraPosition = glm::vec3(-60.0f, 15.0f, 0.0f);
float rotX = 0.0f;
float rotY = 1.5f;
float t = 0.0f;
float profileTimer = 0.0f;
bool camLocked = false;
bool useShadows = true;
bool usePost = true;

// Event handlers
void keyHandler(GLFWwindow* w, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS) {
		return; // We don't really care about anything besides key down
	}
	if (key == GLFW_KEY_ESCAPE) {
		// Unlock cursor on escape
		input.setInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		camLocked = false;
	} else if (key == GLFW_KEY_R) {
		// Refresh post processing buffers on R
		renderer.resizeFramebuffers();
	}
}

void clickHandler(GLFWwindow* w, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		input.setInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		camLocked = true;
	}
}

// Flow control
void init() {
	// Check if there is a gamepad present
	int present = glfwJoystickIsGamepad(GLFW_JOYSTICK_1);
	if (present == 1) {
		std::cout << "Joystick is pad" << std::endl;
	} else {
		std::cout << "Joystick is not" << std::endl;
	}

	// Pass environment variables to the renderer
	uint32_t skyboxTexture = loader.loadCubeMap(
		"res/texture/top.jpg",
		"res/texture/bottom.jpg",
		"res/texture/left.jpg",
		"res/texture/right.jpg",
		"res/texture/front.jpg",
		"res/texture/back.jpg"
	);
	renderer.setLight(glm::vec3(0.7f, -0.7f, 1.0f));
	renderer.setSkybox(skyboxTexture);
	renderer.setShadows(useShadows);
	renderer.setPost(usePost);

	// Earth
	Joker::Texture earthTexture;
	earthTexture.texture = loader.loadTexture("res/texture/earth.png");
	Joker::Model earthModel;
	earthModel.mesh = loadFromOBJ("res/model/earth.obj", loader);
	earthModel.texture = earthTexture;
	earth.model = earthModel;
	earth.scale = glm::vec3(40.0f);

	// Moon
	Joker::Texture moonTexture;
	moonTexture.texture = loader.loadTexture("res/texture/moon.png");
	Joker::Model moonModel;
	moonModel.mesh = earthModel.mesh;
	moonModel.texture = moonTexture;
	moon.model = moonModel;
	moon.scale = glm::vec3(5.0f);

	// Atlas
	Joker::Texture atlasTexture;
	atlasTexture.texture = loader.loadTexture("res/texture/test2.png", false);
	atlasTexture.numRows = 2;
	Joker::Model atlasModel;
	atlasModel.mesh = earthModel.mesh;
	atlasModel.texture = atlasTexture;
	atlas.model = atlasModel;
	atlas.texIndex = 0;
	atlas.scale = glm::vec3(1.0f);

	// Waluigi
	Joker::Texture waluigiTexture;
	waluigiTexture.texture = loader.loadTexture("res/texture/waluigi.png");
	Joker::Model waluigiModel;
	waluigiModel.mesh = loadFromOBJ("res/model/waluigi.obj", loader);
	waluigiModel.texture = waluigiTexture;
	waluigi.model = waluigiModel;
	waluigi.position = glm::vec3(0.0f, 40.0f, 0.0f);
	waluigi.scale = glm::vec3(0.2f);

	// Terrain
	Joker::Model terrainModel;
	terrainModel.mesh = generator.generateTerrain(500.0f, 50, 1);
	terrainModel.texture = atlasTexture;
	terrain.model = terrainModel;
	terrain.position = glm::vec3(250.0f, -100.0f, 250.0f);
	terrain.scale = glm::vec3(1.0f);
	terrain.texIndex = 0;

	// GUI
	gui.texture.texture = 2; // OpenGL happens to always allocate my shadow map here, but this is bad practice
	gui.position = glm::vec3(0.75f, 0.75f, 0.0f);
	gui.scale = glm::vec2(0.25f);

	// Particle
	particle.texture = atlasTexture;
	particle.texIndex = 2;
	particle.scale = glm::vec2(1.0f);

	// Text
	Joker::Font ransomFont;
	ransomFont.texture = loader.loadTexture("res/font/monospace.png", false);
	ransomFont.data = Joker::loadFontData("res/font/monospace.fnt");
	profileText.font = ransomFont;
	profileText.position = glm::vec2(-0.95f, 0.95f);
	profileText.scale = glm::vec2(0.3f);
	profileText.color = glm::vec3(1.0f, 1.0f, 0.0f);

	// Misc stuff
	sound.buffer = loadFromWAV("res/sound/buzz.wav", loader);
	sound.position = &moon.position;
	input.registerKeyCallback(keyHandler);
	input.registerMouseButtonCallback(clickHandler);

	// Post processing shaders
	renderer.postEffects.push_back(Joker::PostShader("res/shader/postShader.vert", "res/shader/post/bloom1.frag"));
	renderer.postEffects.push_back(Joker::PostShader("res/shader/postShader.vert", "res/shader/post/gaussianX.frag"));
	renderer.postEffects.push_back(Joker::PostShader("res/shader/postShader.vert", "res/shader/post/gaussianY.frag"));
	renderer.postEffects.push_back(Joker::PostShader("res/shader/postShader.vert", "res/shader/post/bloom2.frag"));
}

void loop() {
	// Begin profiling for non-render events, and edit the profiler text if it's time
	profiler.beginSection(std::string("Other"));
	profiler.tick();
	profileTimer += display.dt;
	if (profileTimer > 1.0f) {
		profileTimer -= 1.0f;
		profileText.string = profiler.flushProfiler();
	}

	// Global timer
	t += display.dt;

	// Have the audio manager recalculate positions and such
	audio.tick(cameraPosition, rotX, rotY);

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

	// Move Waluigi in accordance with the controller
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
			relativeVelocity.z -= cosf(rotY) * 25.0f;
			relativeVelocity.x += sinf(rotY) * 25.0f;
		}
		if (input.getKeyState(GLFW_KEY_A) == GLFW_PRESS) {
			relativeVelocity.x -= cosf(rotY) * 25.0f;
			relativeVelocity.z -= sinf(rotY) * 25.0f;
		}
		if (input.getKeyState(GLFW_KEY_S) == GLFW_PRESS) {
			relativeVelocity.z += cosf(rotY) * 15.0f;
			relativeVelocity.x -= sinf(rotY) * 15.0f;
		}
		if (input.getKeyState(GLFW_KEY_D) == GLFW_PRESS) {
			relativeVelocity.x += cosf(rotY) * 15.0f;
			relativeVelocity.z += sinf(rotY) * 15.0f;
		}
		if (input.getKeyState(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			relativeVelocity.y -= 15.0f;
		}
		if (input.getKeyState(GLFW_KEY_SPACE) == GLFW_PRESS) {
			relativeVelocity.y += 15.0f;
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

	// Submit objects to the render queue
	profiler.beginSection(std::string("Render submission"));
	renderer.setCamera(cameraPosition, glm::vec3(rotX, rotY, 0.0f), glm::radians(90.0f));
	renderer.submit(earth);
	renderer.submit(moon);
	renderer.submit(atlas);
	renderer.submit(waluigi);
	renderer.submit(terrain);
	//renderer.submit(gui);
	renderer.submit(particle);
	renderer.submit(profileText);

	// Render the scene
	profiler.beginSection(std::string("Rendering"));
	renderer.renderScene();
	display.updateDisplay();
}

// Entry point
int main() {
	Joker::Log::init("Test app", spdlog::level::trace);
	init();
	while (!display.shouldClose()) {
		loop();
	}
	return 0;
}