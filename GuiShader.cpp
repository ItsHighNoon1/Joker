#include "GuiShader.h"

#include <glm/glm.hpp>

#include "Object.h"
#include "Shader.h"

namespace Joker {
	GuiShader::GuiShader(const char* vertex, const char* fragment) : Shader(vertex, fragment) {
		location_transformationMatrix = glGetUniformLocation(programID, "u_transformationMatrix");
	}
	
	void GuiShader::render(Model& gui, glm::mat4& transformationMatrix) {
		// Bind model
		glBindVertexArray(gui.mesh.vaoID);
		glBindTexture(GL_TEXTURE_2D, gui.texture);

		// Upload uniforms
		uploadTransformationMatrix(transformationMatrix);

		// Enable buffers
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// Draw command
		glDrawElements(GL_TRIANGLES, gui.mesh.vertexCount, GL_UNSIGNED_INT, 0);

		// Disable buffers
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Unbind model
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void GuiShader::uploadTransformationMatrix(glm::mat4& m) {
		uploadUniformMat4(location_transformationMatrix, m);
	}
}