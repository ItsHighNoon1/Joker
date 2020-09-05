#pragma once

#include <map>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "util/Allocator.h"
#include "render/Renderable.h"
#include "render/gui/GUIShader.h"
#include "render/particle/ParticleShader.h"
#include "render/post/PostShader.h"
#include "render/shadow/ShadowShader.h"
#include "render/skybox/SkyboxShader.h"
#include "render/static/StaticShader.h"
#include "render/text/TextShader.h"

namespace Joker {
	class MasterRenderer {
		// The interface between the developer and OpenGL
	public:
		MasterRenderer(Allocator& allocator);
		~MasterRenderer();
		void submit(StaticRenderable&);
		void submit(ParticleRenderable&);
		void submit(GUIRenderable&);
		void submit(TextRenderable&);
		void renderScene();
		void setCamera(glm::vec3 position, glm::vec3 rotation, float fov);
		void setLight(glm::vec3 light);
		void setSkybox(uint32_t skybox);
		void setShadows(bool useShadows);
		void setPost(bool usePost);
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
		bool shadowsOn;
		bool postPipeline;
		glm::mat4 viewMatrix;
		glm::mat4 viewProjectionMatrix;
		glm::mat4 skyboxMatrix;
		glm::mat4 shadowMatrix;
		glm::mat4 particleRotationMatrix;
		glm::vec3 lightDirection;
	};
}