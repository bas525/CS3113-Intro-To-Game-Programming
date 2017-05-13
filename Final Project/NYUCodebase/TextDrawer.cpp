#include "TextDrawer.h"
#include "main.h"

GLuint fontTexture;
//ShaderProgram *program;
Matrix textMatrix;

void drawerInit() {
	fontTexture = LoadTexture(RESOURCE_FOLDER"font.png");
}

void TextLocation(float x=0, float y=0) {
	textMatrix.identity();
	textMatrix.Translate(x, y, 0);
}

void DrawText(std::string text, float size, float spacing, float x, float y) {
	TextLocation(x, y);
	ShaderProgram * program = getProgram();
	program->setModelMatrix(textMatrix);

	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;

	for (int i = 0; i < text.size(); i++) {
		int spriteIndex = (int)text[i];

		float texture_x = (float)(spriteIndex % 16) / 16.0f;
		float texture_y = (float)(spriteIndex / 16) / 16.0f;

		vertexData.insert(vertexData.end(), {
			((size + spacing)*i) + (-0.5f * size), 0.5f * size,
			((size + spacing)*i) + (-0.5f * size), -0.5f * size,
			((size + spacing)*i) + (0.5f * size), 0.5f * size,
			((size + spacing)*i) + (0.5f * size), -0.5f * size,
			((size + spacing)*i) + (0.5f * size), 0.5f * size,
			((size + spacing)*i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}

	glBindTexture(GL_TEXTURE_2D, fontTexture);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6 * text.size());

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

