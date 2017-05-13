#pragma once
#include "ShaderProgram.h"
class SheetSprite
{

public:
	float size;
	unsigned int *textureID;
	float u;
	float v;
	float width;
	float height;

	SheetSprite(unsigned int *textureIDp, float up, float vp, float widthp, float heightp, float sizep);
	
	void drawSprite(ShaderProgram *program);
	

	SheetSprite();
	void scroll(float y, float elapsed);
	~SheetSprite();
};

