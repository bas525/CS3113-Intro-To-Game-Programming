#include "SheetSprite.h"

SheetSprite::SheetSprite(unsigned int *textureIDp, float up, float vp, float widthp, float heightp, float sizep) {
	size = sizep;
	textureID = textureIDp;
	u = up;
	v = vp;
	width = widthp;
	height = heightp;
}

SheetSprite::SheetSprite()
{
	size = 0;
	textureID = 0;
	u = 0;
	v = 0;
	width = 0;
	height = 0;
}

void SheetSprite::scroll(float y, float elapsed) {
	v -= y*elapsed;
}

void SheetSprite::drawSprite(ShaderProgram *program) {
	glBindTexture(GL_TEXTURE_2D, *textureID);

	GLfloat texCoords[] = {
		u, v + height,
		u + width, v,
		u, v,
		u + width, v,
		u, v + height,
		u + width, v + height
	};

	float  aspect = width / height;
	float vertices[] = {
		-0.5f * size * aspect, -0.5f * size,
		0.5f * size * aspect, 0.5f * size,
		-0.5f * size * aspect, 0.5f * size,
		0.5f * size * aspect, 0.5f * size,
		-0.5f * size * aspect, -0.5f * size,
		0.5f * size * aspect, -0.5f * size
	};

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

SheetSprite::~SheetSprite()
{
}
