/**********
*Brandan Sankar
*CS 3113
*Pong
*Controls:
*	P1 (Left Paddle):
*		Up -> W key
*		Down -> S key
*
*	P2 (Right Paddle):
*		Up -> Up key
*		Down -> Down key
*
***********/

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

float PI = 3.1415926;

float gameWidth = 640;
float gameHeight = 360;
float playerOffset = 3.3;
float playerWidth = .10;
float playerHeight = .80;
float playerSpeed = 3;
float ballSize = .1;
float ballSpeed = 3.5;
float borderBarOffset;

class Entity {
public:

	float x;
	float y;
	float rotation;

	//int textureID;
	float width;
	float height;

	float speed;
	float direction_x;
	float direction_y;

	Entity(float xp, float yp, float widthp, float heightp, float speedp = 0, float rotationp = 0) {
		x = xp;
		y = yp;
		width = widthp;
		height = heightp;
		speed = speedp;
		direction_x = 1;
		direction_y = 1;
		rotation = rotationp;
	}

	void Draw(ShaderProgram &program, GLuint &texture, Matrix &modelMatrix) {
		glBindTexture(GL_TEXTURE_2D, texture);
		
		/*float vertices[12];
		vertices[0] = x;
		vertices[1] = y;
		vertices[2] = x;
		vertices[3] = y - height;
		vertices[4] = x + width;
		vertices[5] = y - height;
		vertices[6] = x;
		vertices[7] = y;
		vertices[8] = x + width;
		vertices[9] = y - height;
		vertices[10] = x + width;
		vertices[11] = y;*/
		
		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		//float vertices[] = { -0.75431, -0.5, 0.5, -0.5, 0.5, 0.5, -0.75431, -0.5, 0.5, 0.5, -0.75431, 0.5 };

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		program.setModelMatrix(modelMatrix);
		modelMatrix.identity();

		//modelMatrix.Translate(.005, y, 0);
		modelMatrix.Translate(x, y, 0);
		modelMatrix.Scale(width, height, 0);

	}
};

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

	glViewport(0, 0, (int) gameWidth, (int) gameHeight);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	GLuint texture = LoadTexture(RESOURCE_FOLDER"texture.png");
	GLuint p1WinTexture = LoadTexture(RESOURCE_FOLDER"P1Wins.png");
	GLuint p2WinTexture = LoadTexture(RESOURCE_FOLDER"P2Wins.png");

	//Entity p1 = Entity( -playerOffset, 0, playerWidth, playerHeight, playerSpeed);
	Entity p1 = Entity(-playerOffset, 0, playerWidth, playerHeight, playerSpeed);
	Entity p2 = Entity(playerOffset, 0, playerWidth, playerHeight, playerSpeed);
	Entity ball = Entity(0, 0, ballSize, ballSize, ballSpeed, 45);
	Entity topBar = Entity(0, 2, (playerOffset+playerWidth)*2, .2, 0, 0);
	Entity bottomBar = Entity(0, -2, ((playerOffset + playerWidth) * 2), .2, 0, 0);
	Entity Win = Entity(0, 0, 2, 2, 0, 0);
	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);

	float lastFrameTicks = 0.0f;
	
	glUseProgram(program.programID);

	SDL_Event event;
	bool done = false;
	while (!done) {

		

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			else if (event.type == SDL_KEYDOWN) {
				
			}

		}

		glClear(GL_COLOR_BUFFER_BIT);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		//program.setModelMatrix(modelMatrix);


		//Collisions
		if (ball.y + ball.height / 2 >= topBar.y - topBar.height / 2) {
			ball.direction_y = -1;
		}
		if (ball.y - ball.height/2 <= bottomBar.y + bottomBar.height / 2) {
			ball.direction_y = 1;
		}
		if (ball.x - ball.width / 2 <= p1.x + p1.width / 2 &&
			ball.x - ball.width / 2 >= p1.x &&
			ball.y - ball.height / 2 <= p1.y + p1.height / 2 &&
			ball.y + ball.height / 2 >= p1.y - p1.height / 2) {
			ball.direction_x = 1;
		}
		else if (ball.x + ball.width/2 >= p2.x - p1.width / 2 &&
			ball.x + ball.width/2 <= p2.x &&
			ball.y - ball.height/2 <= p2.y + p2.height / 2 &&
			ball.y + ball.height/2 >= p2.y - p2.height / 2) {
			ball.direction_x = -1;
		}

		//Moving
		ball.x += elapsed*ball.direction_x*ball.speed*cos(ball.rotation*PI/180.0f);
		ball.y += elapsed*ball.direction_y*ball.speed*sin(ball.rotation*PI / 180.0f);

		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		if (keys[SDL_SCANCODE_W] && p1.y+p1.height/2 < topBar.y - topBar.height/2) {
			p1.y += elapsed*p1.speed;
		}
		if (keys[SDL_SCANCODE_S] && p1.y - p1.height / 2 > bottomBar.y + bottomBar.height / 2) {
			p1.y -= elapsed*p1.speed;
		}
		if (keys[SDL_SCANCODE_UP] && p2.y+p2.height/2 < topBar.y - topBar.height / 2) {
			p2.y += elapsed*p2.speed;
		}
		if (keys[SDL_SCANCODE_DOWN] && p2.y - p2.height / 2 > bottomBar.y + bottomBar.height / 2) {
			p2.y -= elapsed*p2.speed;
		}

		//Drawing

		
		if (ball.x < -4) {
			Win.Draw(program, p2WinTexture, modelMatrix);
		}
		else if (ball.x > 4) {
			Win.Draw(program, p1WinTexture, modelMatrix);
		}

		else {
			p1.Draw(program, texture, modelMatrix);
			p2.Draw(program, texture, modelMatrix);
			ball.Draw(program, texture, modelMatrix);
			topBar.Draw(program, texture, modelMatrix);
			bottomBar.Draw(program, texture, modelMatrix);
		}
		

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
