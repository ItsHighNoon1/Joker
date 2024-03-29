#include "MasterRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "io/DisplayManager.h"

namespace Joker {
	MasterRenderer::MasterRenderer(int width, int height, Allocator& allocator) :
		// Shader initializer list
		staticShader("res/shader/staticShader.vert", "res/shader/staticShader.frag"),
		particleShader("res/shader/particleShader.vert", "res/shader/particleShader.frag"),
		guiShader("res/shader/guiShader.vert", "res/shader/guiShader.frag"),
		textShader("res/shader/textShader.vert", "res/shader/textShader.frag"),
		shadowShader("res/shader/shadowShader.vert", "res/shader/shadowShader.frag"),
		skyboxShader("res/shader/skyboxShader.vert", "res/shader/skyboxShader.frag"),

		// Screen size initializer list
		width(width),
		height(height),

		// Other initializer list
		loader(allocator),
		viewMatrix(1.0f),
		viewProjectionMatrix(1.0f),
		skyboxMatrix(1.0f),
		shadowMatrix(1.0f),
		particleRotationMatrix(1.0f),
		lightDirection(0.0f) {

		// Allocate some things that we need for rendering
		shadowFramebuffer.width = 2000;
		shadowFramebuffer.height = 2000;
		shadowFramebuffer.buffer = allocator.loadFramebuffer(shadowFramebuffer.width, shadowFramebuffer.height, nullptr, &shadowFramebuffer.depth);;
		quadMesh = allocator.loadQuad();
		cubeMesh = allocator.loadCube();
		skyboxTexture = 0;
		shadowsOn = true;
		postPipeline = true;

		// Rendering globals
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // We need to clear to black because of the post processing pipeline
		glDepthFunc(GL_LEQUAL); // Pass fragments that are closer or equal distance to depth buffer
		glEnable(GL_CULL_FACE); // Enable backface culling
		glCullFace(GL_BACK); // Set backface culling to cull the back face

		// The post processing pipeline needs 2 framebuffers to ping-pong between
		prePost.buffer = loader.loadFramebuffer(width, height, &prePost.color, &prePost.depth);
		postFboA.buffer = loader.loadFramebuffer(width, height, &postFboA.color, &postFboA.depth);
		postFboB.buffer = loader.loadFramebuffer(width, height, &postFboB.color, &postFboB.depth);
	}

	MasterRenderer::~MasterRenderer() {
		shadowShader.deleteShader();
		guiShader.deleteShader();
		textShader.deleteShader();
		staticShader.deleteShader();
		particleShader.deleteShader();
		skyboxShader.deleteShader();
	}

	void MasterRenderer::submit(GUIRenderable& r) {
		// Calculate the model matrix before pushing it to the queue
		glm::mat4 transformationMatrix = glm::mat4(1.0f);
		transformationMatrix = glm::translate(transformationMatrix, glm::vec3(r.position.x, r.position.y, 0.0f));
		transformationMatrix = glm::scale(transformationMatrix, glm::vec3(r.scale.x, r.scale.y, 0.0f));
		r.transformationMatrix = transformationMatrix;
		guiRenderables[r.texture.texture].push_back(r);
	}

	void MasterRenderer::submit(TextRenderable& r) {
		glm::mat4 transformationMatrix = glm::mat4(1.0f);
		transformationMatrix = glm::translate(transformationMatrix, glm::vec3(r.position.x, r.position.y, 0.0f));
		transformationMatrix = glm::scale(transformationMatrix, glm::vec3(r.scale.x, r.scale.y, 0.0f));
		r.transformationMatrix = transformationMatrix;
		textRenderables[r.font.texture].push_back(r);
	}

	void MasterRenderer::submit(StaticRenderable& r) {
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, r.position);
		modelMatrix = glm::rotate(modelMatrix, r.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, r.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, r.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::scale(modelMatrix, r.scale);
		r.modelMatrix = modelMatrix;
		staticRenderables[r.model.texture.texture << 16 | r.model.mesh.vaoID].push_back(r);
	}

	void MasterRenderer::submit(ParticleRenderable& r) {
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, r.position);
		modelMatrix = modelMatrix * particleRotationMatrix;
		modelMatrix = glm::scale(modelMatrix, glm::vec3(r.scale.x, r.scale.y, 0.0f));
		r.modelMatrix = modelMatrix;
		particleRenderables[r.texture.texture].push_back(r);
	}

	void MasterRenderer::renderScene() {
		// Create the shadow map
		if (shadowsOn) {
			glEnable(GL_DEPTH_TEST);
			glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer.buffer);
			glViewport(0, 0, shadowFramebuffer.width, shadowFramebuffer.height);
			glClear(GL_DEPTH_BUFFER_BIT);
			renderShadow();
			glDisable(GL_DEPTH_TEST);
		}

		// Check to see if there is a post processing pipeline and set the appropriate framebuffer
		bool usePost = postEffects.size() > 0 && postPipeline;
		if (usePost) {
			glBindFramebuffer(GL_FRAMEBUFFER, prePost.buffer);
		} else {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render the skybox behind anything; depth test should be off at this point
		renderSkybox();

		// Enable the depth test and render the 3D things
		glEnable(GL_DEPTH_TEST);
		renderStatic();
		renderParticle();

		// Disable the depth test to draw a bunch of 2D things
		glDisable(GL_DEPTH_TEST);
		if (usePost) {
			postProcessing();
		}
		renderGUI();
		renderText();
		
		// Clear the render queue for next frame
		guiRenderables.clear();
		textRenderables.clear();
		staticRenderables.clear();
		particleRenderables.clear();
	}

	void MasterRenderer::setCamera(glm::vec3 position, glm::vec3 rotation, float fov) {
		// Particles are just 3D models that always face the camera, so we will need to rotate them properly
		particleRotationMatrix = glm::mat4(1.0f);
		particleRotationMatrix = glm::rotate(particleRotationMatrix, -rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		particleRotationMatrix = glm::rotate(particleRotationMatrix, -rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));

		// Calculate the projection matrix
		glm::mat4 projectionMatrix = glm::perspective(fov, 8.0f / 5.0f, 1.0f, 1500.0f);

		// Calculate the view matrix
		viewMatrix = glm::mat4(1.0f);
		viewMatrix = glm::rotate(viewMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		viewMatrix = glm::rotate(viewMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		viewMatrix = glm::rotate(viewMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		viewMatrix = glm::translate(viewMatrix, -position);

		// Calculate some specialized matrices
		viewProjectionMatrix = projectionMatrix * viewMatrix;
		skyboxMatrix = projectionMatrix * glm::mat4(glm::mat3(viewMatrix)); // Translation be gone

		// Calculate the shadow matrix
		glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 300.0f); // Orthographic projection matrix
		glm::mat4 lightView = glm::lookAt(200.0f * glm::normalize(-lightDirection) + position, position, glm::vec3(0.0f, 1.0f, 0.0f)); // Look at something
		shadowMatrix = lightProjection * lightView;
	}
	
	// Scene variable setters
	void MasterRenderer::setLight(glm::vec3 light) {
		lightDirection = light;
	}

	void MasterRenderer::setSkybox(uint32_t skybox) {
		skyboxTexture = skybox;
	}

	void MasterRenderer::setShadows(bool useShadows) {
		shadowsOn = useShadows;
	}

	void MasterRenderer::setPost(bool usePost) {
		postPipeline = usePost;
	}

	void MasterRenderer::resizeFramebuffers(int width, int height) {
		// Delete the old framebuffers, because framebuffers are big and resizing the window can obliterate VRAM if we aren't careful
		glDeleteFramebuffers(1, &prePost.buffer);
		glDeleteFramebuffers(1, &postFboA.buffer);
		glDeleteFramebuffers(1, &postFboB.buffer);
		glDeleteTextures(1, &prePost.color);
		glDeleteTextures(1, &postFboA.color);
		glDeleteTextures(1, &postFboB.color);
		glDeleteTextures(1, &prePost.depth);
		glDeleteTextures(1, &postFboA.depth);
		glDeleteTextures(1, &postFboB.depth);

		// Allocate new framebuffers that are the size of the screen
		prePost.buffer = loader.loadFramebuffer(width, height, &prePost.color, &prePost.depth);
		postFboA.buffer = loader.loadFramebuffer(width, height, &postFboA.color, &postFboA.depth);
		postFboB.buffer = loader.loadFramebuffer(width, height, &postFboB.color, &postFboB.depth);

		glViewport(0, 0, width, height);
	}

	void MasterRenderer::renderShadow() {
		// We will assume the framebuffer is bound because that isn't our job here
		glUseProgram(shadowShader.programID);

		// Iterate over static models and draw them to the shadow map
		for (auto iterator : staticRenderables) {
			// Bind anything that is specific to the model
			Model& model = iterator.second[0].model;
			glBindVertexArray(model.mesh.vaoID);
			glBindTexture(GL_TEXTURE_2D, model.texture.texture);
			glUniform1i(shadowShader.texRows, model.texture.numRows);
			for (auto renderable : iterator.second) {
				// Upload texture atlas (we need textures for transparency)
				if (model.texture.numRows > 1) {
					float xOffset = (float)(renderable.texIndex % model.texture.numRows) / model.texture.numRows;
					float yOffset = (float)(renderable.texIndex / model.texture.numRows) / model.texture.numRows;
					glUniform2f(shadowShader.texOffset, xOffset, yOffset);
				}

				// Upload transformation matrix
				glm::mat4 modelMatrix = renderable.modelMatrix;
				glm::mat4 modelShadowMatrix = shadowMatrix * modelMatrix;
				glUniformMatrix4fv(shadowShader.modelShadowMatrix, 1, GL_FALSE, glm::value_ptr(modelShadowMatrix));
				glDrawElements(GL_TRIANGLES, model.mesh.vertexCount, GL_UNSIGNED_INT, 0);
			}
		}
	}

	void MasterRenderer::renderSkybox() {
		glUseProgram(skyboxShader.programID);

		// Simple, because we will only ever draw 1 skybox
		glBindVertexArray(cubeMesh);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glUniformMatrix4fv(skyboxShader.viewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(skyboxMatrix));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	void MasterRenderer::renderGUI() {
		glUseProgram(guiShader.programID);

		// Same principle as particles
		glBindVertexArray(quadMesh);

		// Iterate over GUIs
		for (auto iterator : guiRenderables) {
			// Bind the texture
			Texture& texture = iterator.second[0].texture;
			glBindTexture(GL_TEXTURE_2D, texture.texture);
			glUniform1i(guiShader.texRows, texture.numRows);
			for (auto renderable : iterator.second) {
				// Upload instance data and draw
				if (texture.numRows > 1) {
					float xOffset = (float)(renderable.texIndex % texture.numRows) / texture.numRows;
					float yOffset = (float)(renderable.texIndex / texture.numRows) / texture.numRows;
					glUniform2f(guiShader.texOffset, xOffset, yOffset);
				}
				glUniformMatrix4fv(guiShader.transformationMatrix, 1, GL_FALSE, glm::value_ptr(renderable.transformationMatrix));
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}
	}

	void MasterRenderer::renderText() {
		glUseProgram(textShader.programID);
		glBindVertexArray(quadMesh);

		// Iterate over texts
		for (auto iterator : textRenderables) {
			// Bind the texture
			glBindTexture(GL_TEXTURE_2D, iterator.second[0].font.texture);
			for (auto renderable : iterator.second) {
				glm::mat4 leftTransformationMatrix = renderable.transformationMatrix;
				glm::mat4 transformationMatrix = leftTransformationMatrix;
				glUniform3f(textShader.textColor, renderable.color.x, renderable.color.y, renderable.color.z);
				for (char character : renderable.string) {
					if (character == '\n') {
						// Newline is unique, we need to move to the original position but then down
						leftTransformationMatrix = glm::translate(leftTransformationMatrix, glm::vec3(0.0f, -renderable.font.data['\n'].xAdvance, 0.0f));
						transformationMatrix = leftTransformationMatrix;
					} else if (character == ' ') {
						// We still need to advance if it's a space, but we can skip the draw
						transformationMatrix = glm::translate(transformationMatrix, glm::vec3(renderable.font.data[' '].xAdvance, 0.0f, 0.0f));
					} else {
						// Upload char data and draw
						FontChar charData = renderable.font.data[character];
						glUniform2f(textShader.charPosition, charData.position.x, charData.position.y);
						glUniform2f(textShader.charSize, charData.size.x, charData.size.y);
						glUniform2f(textShader.charOffset, charData.offset.x, charData.offset.y);
						glUniformMatrix4fv(textShader.transformationMatrix, 1, GL_FALSE, glm::value_ptr(transformationMatrix));
						glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

						// Advance the transformation matrix for the next character
						transformationMatrix = glm::translate(transformationMatrix, glm::vec3(charData.xAdvance, 0.0f, 0.0f));
					}
				}
			}
		}
	}

	void MasterRenderer::renderStatic() {
		// Set up the static shader
		glUseProgram(staticShader.programID);
		glUniform3f(staticShader.lightDirection, lightDirection.x, lightDirection.y, lightDirection.z);

		// Enable the shadow map if shadows are enabled
		if (shadowsOn) {
			glUniform1f(staticShader.shadowDistance, 100.0f); // Closest distance that shadows will break down
			glUniform1i(staticShader.shadowMapSize, shadowFramebuffer.width);
			glUniform1i(staticShader.useShadows, 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, shadowFramebuffer.depth);
		} else {
			glUniform1i(staticShader.useShadows, 0);
		}

		glActiveTexture(GL_TEXTURE0); // Move back to unit 0 for other textures
		
		// Since we have multiple textures, we need to use uniforms
		glUniform1i(staticShader.tex, 0);
		glUniform1i(staticShader.shadowMap, 1);

		// Iterate over static models and render them
		for (auto iterator : staticRenderables) {
			// Bind the model
			Model& model = iterator.second[0].model;
			glBindVertexArray(model.mesh.vaoID);
			glBindTexture(GL_TEXTURE_2D, model.texture.texture);
			glUniform1i(staticShader.texRows, model.texture.numRows);
			for (auto renderable : iterator.second) {
				// Upload instance data and draw
				glm::mat4 modelMatrix = renderable.modelMatrix;
				glm::mat4 modelView = viewMatrix * modelMatrix;
				glm::mat4 modelViewProjection = viewProjectionMatrix * modelMatrix;
				glm::mat4 modelShadowMatrix = shadowMatrix * modelMatrix;
				if (model.texture.numRows > 1) {
					float xOffset = (float)(renderable.texIndex % model.texture.numRows) / model.texture.numRows;
					float yOffset = (float)(renderable.texIndex / model.texture.numRows) / model.texture.numRows;
					glUniform2f(staticShader.texOffset, xOffset, yOffset);
				}
				glUniformMatrix4fv(staticShader.modelMatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
				glUniformMatrix4fv(staticShader.modelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelView));
				glUniformMatrix4fv(staticShader.modelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(modelViewProjection));
				glUniformMatrix4fv(staticShader.modelShadowMatrix, 1, GL_FALSE, glm::value_ptr(modelShadowMatrix));
				glDrawElements(GL_TRIANGLES, model.mesh.vertexCount, GL_UNSIGNED_INT, 0);
			}
		}
	}

	void MasterRenderer::renderParticle() {
		glUseProgram(particleShader.programID);

		// We use the shadow map again here
		if (shadowsOn) {
			// We don't deal with uploading shadow fade stuff because particles are too small
			glUniform1i(particleShader.useShadows, 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, shadowFramebuffer.depth);
		} else {
			glUniform1i(particleShader.useShadows, 0);
		}
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(particleShader.tex, 0);
		glUniform1i(particleShader.shadowMap, 1);

		// There will only ever be 1 model for particles, and that's a unit quad
		glBindVertexArray(quadMesh);

		// Iterate over particles
		for (auto iterator : particleRenderables) {
			// Bind the texture
			Texture& texture = iterator.second[0].texture;
			glBindTexture(GL_TEXTURE_2D, texture.texture);
			glUniform1i(particleShader.texRows, texture.numRows);
			for (auto renderable : iterator.second) {
				// Upload instance data and draw
				glm::mat4 modelMatrix = renderable.modelMatrix;
				glm::mat4 modelViewProjection = viewProjectionMatrix * modelMatrix;
				glm::mat4 modelShadowMatrix = shadowMatrix * modelMatrix;
				if (texture.numRows > 1) {
					float xOffset = (float)(renderable.texIndex % texture.numRows) / texture.numRows;
					float yOffset = (float)(renderable.texIndex / texture.numRows) / texture.numRows;
					glUniform2f(particleShader.texOffset, xOffset, yOffset);
				}
				glUniformMatrix4fv(particleShader.modelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(modelViewProjection));
				glUniformMatrix4fv(particleShader.modelShadowMatrix, 1, GL_FALSE, glm::value_ptr(modelShadowMatrix));
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}
	}

	void MasterRenderer::postProcessing() {
		// Go through the post processing pipeline
		glBindVertexArray(quadMesh);
		bool aIsTexture = false;

		// Some shaders may be interested in the original scene, so provide that
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, prePost.color);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, prePost.depth);
		glActiveTexture(GL_TEXTURE0);
		
		for (uint32_t i = 0; i < postEffects.size(); i++) {
			if (i == 0) {
				// This is the first effect in the pipeline, so we will be pulling from the prePost FBO
				glBindTexture(GL_TEXTURE_2D, prePost.color);
				if (i == postEffects.size() - 1) {
					// Additionally, if this is the last effect we need to render to the screen
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
				} else {
					glBindFramebuffer(GL_FRAMEBUFFER, postFboA.buffer);
				}
			} else if (i == postEffects.size() - 1) {
				// If this is the last effect in the pipeline, it's time to render to the screen
				glBindTexture(GL_TEXTURE_2D, aIsTexture ? postFboA.color : postFboB.color);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			} else if (aIsTexture) {
				// If we are using A as the texture, we must draw to B, and vice versa
				glBindTexture(GL_TEXTURE_2D, postFboA.color);
				glBindFramebuffer(GL_FRAMEBUFFER, postFboB.buffer);
			} else {
				glBindTexture(GL_TEXTURE_2D, postFboB.color);
				glBindFramebuffer(GL_FRAMEBUFFER, postFboA.buffer);
			}
			glClear(GL_COLOR_BUFFER_BIT);
			glUseProgram(postEffects[i].programID);
			glUniform1i(postEffects[i].currentColor, 0);
			glUniform1i(postEffects[i].sceneColor, 1);
			glUniform1i(postEffects[i].sceneDepth, 2);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			aIsTexture = !aIsTexture;
		}
	}
}
