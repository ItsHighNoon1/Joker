#pragma once

#include <map>
#include <vector>

#include "Renderable.h"
#include "static/StaticShader.h"
#include "particle/ParticleShader.h"
#include "gui/GUIShader.h"
#include "text/TextShader.h"
#include "shadow/ShadowShader.h"

namespace Joker {
	class MasterRenderer {
		// The interface between the developer and OpenGL
	public:
		MasterRenderer(Mesh quad, Framebuffer shadow);
		~MasterRenderer();
		void submit(StaticRenderable&);
		void submit(ParticleRenderable&);
		void submit(GUIRenderable&);
		void renderScene();
		void setCamera(glm::vec3 position, glm::vec3 rotation, float fov);
		void setEnvironment(glm::vec3& light);
	private:
		// Individual render pipelines
		void renderShadow();
		void renderGUI();
		void renderText();
		void renderStatic();
		void renderParticle();

		// Rendering objects
		std::map<uint32_t, std::vector<GUIRenderable>> guiRenderables;
		std::map<uint32_t, std::vector<StaticRenderable>> staticRenderables;
		std::map<uint32_t, std::vector<ParticleRenderable>> particleRenderables;
		Mesh quadMesh; // GUI, text, and particles all use the same mesh
		Framebuffer shadowFramebuffer;

		// Shaders
		ShadowShader shadowShader;
		GUIShader guiShader;
		TextShader textShader;
		StaticShader staticShader;
		ParticleShader particleShader;

		// Miscellaneous global variables
		glm::mat4 viewMatrix;
		glm::mat4 shadowMatrix;
		glm::mat4 particleRotationMatrix;
		glm::vec3 lightDirection;
	};
}