

#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ShaderProgram.h"

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
SDL_Window* displayWindow;

GLuint LoadTexture(const char *filePath) {
	int w, h, comp;
	unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
	
	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	GLuint retTexture;
	glGenTextures(1, &retTexture);
	glBindTexture(GL_TEXTURE_2D, retTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(image);

	return retTexture;
}

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Animaion Assignment", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	glViewport(0, 0, 640, 360);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	GLuint catTexture = LoadTexture(RESOURCE_FOLDER"cat.png");
	GLuint cat2Texture = LoadTexture(RESOURCE_FOLDER"cat2.png");
	GLuint ballTexture = LoadTexture(RESOURCE_FOLDER"ball.png");
	double ballX = -1.65;
	int dir = 1; 

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	
	glUseProgram(program.programID);

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);

		
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		program.setModelMatrix(modelMatrix);

		glBindTexture(GL_TEXTURE_2D, catTexture);


		float catVertices[] = {-0.75431, -0.5, 0.5, -0.5, 0.5, 0.5, -0.75431, -0.5, 0.5, 0.5, -0.75431, 0.5 };
		for (int i = 0; i < 12; i++) {
			if (i % 2 == 0) catVertices[i] = catVertices[i] - 2.33;
			else catVertices[i] = catVertices[i] - 1;
		}
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, catVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
	
		//modelMatrix.Translate(.05, 0, 0);
		
			
		glBindTexture(GL_TEXTURE_2D, cat2Texture);

		float cat2Vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		for (int i = 0; i < 12; i++) {
			if (i % 2 == 0) cat2Vertices[i] = cat2Vertices[i] + 2 ;
			else cat2Vertices[i] = cat2Vertices[i] - 1;
		}
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, cat2Vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);


		glBindTexture(GL_TEXTURE_2D, ballTexture);

		float ballVertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		for (int i = 0; i < 12; i++) {
			
			if (i % 2 == 0) ballVertices[i] = ballVertices[i] + ballX;
			else ballVertices[i] = ballVertices[i] - .5 + cos((abs(ballX + .2)));
			
		}
		if (ballX <= -1.75) dir = 1;
		if (ballX >= 1.35) dir = -1;
		ballX += .05*dir;
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ballVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
