#include "TextShader.h"

#include <map>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "Object.h"
#include "Shader.h"

namespace Joker {
	TextShader::TextShader(const char* vertex, const char* fragment) : Shader(vertex, fragment) {
		location_transformationMatrix = glGetUniformLocation(programID, "u_transformationMatrix");
		location_charPosition = glGetUniformLocation(programID, "u_charPosition");
		location_charSize = glGetUniformLocation(programID, "u_charSize");
		location_charOffset = glGetUniformLocation(programID, "u_charOffset");
	}

	void TextShader::render(Text& text, glm::mat4& transformationMatrix, Model& textModel) {
		// Bind model
		glBindVertexArray(textModel.mesh.vaoID);
		glBindTexture(GL_TEXTURE_2D, textModel.texture);

		// Enable buffers
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// Iterate over the text and render each character
		glm::mat4 charTransMatrix = transformationMatrix;
		uint8_t character;
		uint32_t index = 0;
		while ((character = *(text.string + index)) != '\0') {
			// Upload information about the character we are drawing
			TextChar charInfo = text.font[character];
			uploadCharPosition(charInfo.position);
			uploadCharSize(charInfo.size);
			uploadCharOffset(charInfo.offset);

			// Upload the position we should draw at, then prepare the next character's position
			uploadTransformationMatrix(charTransMatrix);
			charTransMatrix = glm::translate(charTransMatrix, glm::vec3(charInfo.xAdvance * 2.0f, 0.0f, 0.0f));

			// Draw the character
			glDrawElements(GL_TRIANGLES, textModel.mesh.vertexCount, GL_UNSIGNED_INT, 0);

			// Increment the index
			index++;
		}

		// Disable buffers
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Unbind model
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void TextShader::uploadTransformationMatrix(glm::mat4& m) {
		uploadUniformMat4(location_transformationMatrix, m);
	}

	void TextShader::uploadCharPosition(glm::vec2& pos) {
		uploadUniformVec2(location_charPosition, pos);
	}

	void TextShader::uploadCharSize(glm::vec2& size) {
		uploadUniformVec2(location_charSize, size);
	}

	void TextShader::uploadCharOffset(glm::vec2& off) {
		uploadUniformVec2(location_charOffset, off);
	}
}