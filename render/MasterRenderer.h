#pragma once

#include <map>
#include <vector>

#include "Renderable.h"

#include "gui/GUIShader.h"
#include "particle/ParticleShader.h"
#include "post/PostShader.h"
#include "shadow/ShadowShader.h"
#include "skybox/SkyboxShader.h"
#include "text/TextShader.h"
#include "static/StaticShader.h"

#include "util/Allocator.h"

namespace Joker {
	class MasterRenderer {
		// The interface between the developer and OpenGL
	public:
		MasterRenderer(Allocator& allocator);
		void submit(StaticRenderable&);
		void submit(ParticleRenderable&);
		void submit(GUIRenderable&);
		void submit(TextRenderable&);
		void renderScene();
		void setCamera(glm::vec3 position, glm::vec3 rotation, float fov);
		void setEnvironment(glm::vec3 light, uint32_t skybox);
		void resizeFramebuffers();
		
		// TODO introduce proper methods to modify the post processing pipeline
		std::vector<PostShader> postEffects;
	private:
		// Individual render pipelines
		void renderShadow();
		void renderSkybox();
		void renderGUI();
		void renderText();
		void renderStatic();
		void renderParticle();
		void postProcessing();

		// Rendering objects
		std::map<uint32_t, std::vector<GUIRenderable>> guiRenderables;
		std::map<uint32_t, std::vector<TextRenderable>> textRenderables;
		std::map<uint32_t, std::vector<StaticRenderable>> staticRenderables;
		std::map<uint32_t, std::vector<ParticleRenderable>> particleRenderables;
		GLuint quadMesh; // GUI, text, and particles all use the same mesh
		GLuint cubeMesh;
		GLuint skyboxTexture;
		Framebuffer shadowFramebuffer;
		Framebuffer prePost;
		Framebuffer postFboA;
		Framebuffer postFboB;

		// Shaders
		ShadowShader shadowShader;
		GUIShader guiShader;
		TextShader textShader;
		StaticShader staticShader;
		ParticleShader particleShader;
		SkyboxShader skyboxShader;

		// Miscellaneous
		Allocator& loader;
		glm::mat4 viewMatrix;
		glm::mat4 viewProjectionMatrix;
		glm::mat4 skyboxMatrix;
		glm::mat4 shadowMatrix;
		glm::mat4 particleRotationMatrix;
		glm::vec3 lightDirection;
	};
}