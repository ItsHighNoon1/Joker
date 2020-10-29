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
#include <random>
#include <sstream>

// OpenGL/OpenAL related headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

// stb
#include <stb/stb_image.h>

// Anything GLM related that we are likely to use
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Joker headers
#include "debug/Profiler.h"
#include "external/discord/DiscordManager.h"
#include "game/Sound.h"
#include "io/AudioManager.h"
#include "io/DisplayManager.h"
#include "io/InputHandler.h"
#include "util/Util.h"
/*
Component based architecture, probably want to revisit this at a later date
#include "component/Component.h"
#include "game/Entity.h"

Old renderer, new renderer will probably just be straight up shaders
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

New engine will use RAII
#include "util/Allocator.h"
*/
#include "util/Model.h"