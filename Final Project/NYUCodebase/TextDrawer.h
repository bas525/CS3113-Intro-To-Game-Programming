#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include "ShaderProgram.h"

void drawerInit();
void TextLocation(float x, float y);
void DrawText(std::string text, float size, float spacing, float x = 0, float y = 0);

