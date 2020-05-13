#pragma once

// Standard types
#include <map>
#include <string>
#include <vector>

// Standard utilities
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

// OpenGL/OpenAL related headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

// spdlog headers
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// stb
#include <stb/stb_image.h>
#include <stb/stb_perlin.h>

// Anything GLM related that we are likely to use
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Joker headers
#include "component/Component.h"
#include "debug/Log.h"
#include "debug/Profiler.h"
#include "game/Entity.h"
#include "game/Sound.h"
#include "io/AudioManager.h"
#include "io/DisplayManager.h"
#include "io/InputHandler.h"
#include "render/MasterRenderer.h"
#include "render/Renderable.h"
#include "render/ShaderProgram.h"
#include "render/gui/GUIShader.h"
#include "render/particle/ParticleShader.h"
#include "render/post/PostShader.h"
#include "render/shadow/ShadowShader.h"
#include "render/skybox/SkyboxShader.h"
#include "render/static/StaticShader.h"
#include "render/text/TextShader.h"
#include "util/Allocator.h"
#include "util/GeometryGenerator.h"
#include "util/Util.h"