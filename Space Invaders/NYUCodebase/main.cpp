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
#include <vector>

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
float gameHeight = 720;

float playerX = 0.0f;
float playerY = -3.5f;
float playerSpeed = 4.0f;
float playerSpriteX = 0.0f;
float playerSpriteY = 941.0f;
float playerWidth = 112.0f;
float playerHeight = 75.0f;
float playerSize = 0.4f;

float enemySpeed = 0.2f;
float enemySpriteX = 734.0f;
float enemySpriteY = 977.0f;
float enemyWidth = 40.0f;
float enemyHeight = 40.0f;
float enemySize = 0.4f;
int enemiesKilled = 0;

float playerLaserSpeed = 5.0f;
float playerLaserSpriteX = 858.0f;
float playerLaserSpriteY = 230.0f;
float playerLaserWidth = 9.0f;
float playerLaserHeight = 54.0f;
float playerLaserSize = 0.4f;

float enemyLaserSpeed = 5.0f;
float enemyLaserSpriteX = 740.0f;
float enemyLaserSpriteY = 686.0f;
float enemyLaserWidth = 37.0f;
float enemyLaserHeight = 38.0f;
float enemyLaserSize = 0.1f;

enum GameState {STATE_TITLE_SCREEN, GAME, GAME_OVER, WIN};
GameState state;
GLuint fonts;
ShaderProgram *programs;
class SheetSprite {
	public:
		float size;
		unsigned int *textureID;
		float u;
		float v;
		float width;
		float height;

		SheetSprite() {}

		SheetSprite(unsigned int *textureIDp, float up, float vp, float widthp, float heightp, float sizep) {
			size = sizep;
			textureID = textureIDp;
			u = up;
			v = vp;
			width = widthp;
			height = heightp;
		}

		void drawSprite(ShaderProgram *program) {
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
};

class Entity {
public:

	float x;
	float y;

	SheetSprite sprite;

	float width;
	float height;
	float size;
	float  aspect;
	
	float speed;
	float direction_x;
	float direction_y;
	bool active;

	Entity() {
	}

	Entity(float xp, float yp, float widthp, float heightp, SheetSprite spritep, float speedp, float sizep, float dxp = 1, float dyp = 1) {
		x = xp;
		y = yp;
		width = widthp;
		height = heightp;

		sprite = spritep;

		size = sizep;

		speed = speedp;
		direction_x = dxp;
		direction_y = dyp;
		active = true;
		aspect = width / height;
	}
	
	float top() { return y + .5*size; }
	float bottom() { return y - .5*size; }
	float right() { return x + .5*size*aspect; }
	float left() { return x - .5*size*aspect; }
	
	
	//void Draw(ShaderProgram &program, GLuint &texture, Matrix &modelMatrix) {
	/*
		program.setModelMatrix(modelMatrix);
		modelMatrix.identity();

		glBindTexture(GL_TEXTURE_2D, texture);
				
		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		modelMatrix.Translate(x, y, 0);
		modelMatrix.Scale(width, height, 0);

	}
	*/

	void drawSprite(ShaderProgram &program, Matrix &modelMatrix) {
		modelMatrix.identity();
		modelMatrix.Translate(x, y, 0);
		program.setModelMatrix(modelMatrix);
		//DrawSpriteSheetSprite(&program, spriteSheet, textureID, spriteCountX, spriteCountY);
		sprite.drawSprite(&program);
		
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

void DrawSpriteSheetSprite(ShaderProgram *program, GLuint *textureSheet, int index, int spriteCountX, int spriteCountY) {
	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0 / (float)spriteCountX;
	float spriteHeight = 1.0 / (float)spriteCountY;

	GLfloat texCoords[] = {
		u, v + spriteHeight,
		u + spriteWidth, v,
		u, v,
		u + spriteWidth, v,
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight
	};
	float vertices[] = {-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f};

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing) {
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

	glDrawArrays(GL_TRIANGLES, 0, 6*text.size());

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void renderTitleScreen(Matrix &modelMatrix, ShaderProgram &program, GLuint &font) {
	modelMatrix.identity();
	modelMatrix.Translate(-3, 1, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Space Invaders", 1, -.55);

	modelMatrix.identity();
	modelMatrix.Translate(-2.5, -1, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Press P to play the game", .25, -.12);

	modelMatrix.identity();
	modelMatrix.Translate(-2.5, -1.5, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Arrow Keys to move left and right", .25, -.12);

	modelMatrix.identity();
	modelMatrix.Translate(-2.5, -2.0, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Use the Space Bar to shoot", .25, -.12);
}
void processTitleScreen() {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_P]) {
		state = GAME;
	}
}


void renderGame(Matrix &modelMatrix, ShaderProgram &program, Entity &player, std::vector<Entity> &enemies, Entity &playerLaser, Entity &enemyLaser) {
	player.drawSprite(program, modelMatrix);
	//enemy.drawSprite(program, modelMatrix);
	for (Entity enemy : enemies) {
		if(enemy.active) enemy.drawSprite(program, modelMatrix);
	}
	if(playerLaser.active) playerLaser.drawSprite(program, modelMatrix);
	if (enemyLaser.active) enemyLaser.drawSprite(program, modelMatrix);
}

//Used for debugging
std::string double_to_string(float f)
{
	std::ostringstream s;
	s << f;
	return s.str();
}
void processGame(Entity &player, Entity &playerLaser, float elapsed) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_LEFT] && player.left() >= -3.55) {
		player.x -= player.speed*elapsed;
		
	}
	if (keys[SDL_SCANCODE_RIGHT] && player.right() <= 3.55) {
		player.x += player.speed*elapsed;
	}

	if (keys[SDL_SCANCODE_SPACE] && !playerLaser.active) {
		playerLaser.x = player.x;
		playerLaser.y = player.y;
		playerLaser.active = true;
	};
}

bool collision(Entity &a, Entity &b) {
	if (a.bottom() < b.top() && a.top() > b.bottom()) {
		if (a.right() > b.left() && a.left() < b.right()) {
			//DrawText(programs, fonts, "true", .4, 0);
			return true;
		}
	}
	return false;
}

void updateGame(Entity& player, Entity &playerLaser, std::vector<Entity> &enemies, Entity &enemyLaser, float elapsed) {
	if (playerLaser.active) {
		playerLaser.y += playerLaser.speed * elapsed;
	}
	if (playerLaser.y >= 4.0f) {
		playerLaser.active = false;
	}
	if (enemyLaser.active) {
		enemyLaser.y -= enemyLaser.speed * elapsed;
	}
	if (enemyLaser.y <= -4.0f) {
		enemyLaser.active = false;
	}

	float changeDirection = 0;
	for (Entity &enemy : enemies) {
		if (enemy.active) {
			if (playerLaser.active && collision(playerLaser, enemy)) {
				playerLaser.active = false;
				enemy.active = false;
				enemiesKilled++;
				if (enemiesKilled == 30) state = WIN;
			}
			else if (!enemyLaser.active) {
				if (std::rand() % 100 < 5) {
					enemyLaser.active = true;
					enemyLaser.x = enemy.x;
					enemyLaser.y = enemy.y;
				}
			}
			if (collision(enemy, player)) {
				state = GAME_OVER;
			}
		}
		enemy.x += enemy.speed*enemy.direction_x*elapsed;
		if (enemy.right() > 3.55) {
			changeDirection = -1.0f;
		}
		else if (enemy.left() < -3.55) {
			changeDirection = 1.0f;
		}
	}
	if (changeDirection == -1.0f) {
		for (Entity &enemy : enemies) {
			enemy.direction_x = -1.0f;
			//Didn't used elapsed on purpose since this is an
			//event based movement instead of a movement
			//that is relative to time
			enemy.y -= .25;
		}
	}
	else if (changeDirection == 1.0f) {
		for (Entity &enemy : enemies) {
			enemy.direction_x = 1.0f;
			//Didn't used elapsed on purpose since this is an
			//event based movement instead of a movement
			//that is relative to time
			enemy.y -= .25;
		}
	}

	if (collision(player, enemyLaser)) {
		state = GAME_OVER;
	}
}

void renderGameOver(Matrix &modelMatrix, ShaderProgram &program, GLuint &font) {
	modelMatrix.identity();
	modelMatrix.Translate(-2.25, 0, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Game Over You Lost", .5, -.25);
}

void renderWin(Matrix &modelMatrix, ShaderProgram &program, GLuint &font) {
	modelMatrix.identity();
	modelMatrix.Translate(-2.75, 0, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Congratulations You Win", .5, -.25);
}

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gameWidth, gameHeight, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, (int)gameWidth, (int)gameHeight);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	programs = &program;

	GLuint spriteSheet = LoadTexture(RESOURCE_FOLDER"spriteSheet.png");
	GLuint font = LoadTexture(RESOURCE_FOLDER"font.png");

	fonts = font;
	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	
	SheetSprite playerSprite(&spriteSheet, playerSpriteX / 1024.0f, playerSpriteY / 1024.0f, playerWidth / 1024.0f, playerHeight / 1024.0f, playerSize);
	SheetSprite enemySprite(&spriteSheet, enemySpriteX / 1024.0f, enemySpriteY / 1024.0f, enemyWidth / 1024.0f, enemyHeight / 1024.0f, enemySize);
	SheetSprite playerLaserSprite(&spriteSheet, playerLaserSpriteX / 1024.0f, playerLaserSpriteY / 1024.0f, playerLaserWidth / 1024.0f, playerLaserHeight / 1024.0f, playerLaserSize);
	SheetSprite enemyLaserSprite(&spriteSheet, enemyLaserSpriteX / 1024.0f, enemyLaserSpriteY / 1024.0f, enemyLaserWidth / 1024.0f, enemyLaserHeight / 1024.0f, enemyLaserSize);

	Entity player(playerX, playerY, playerWidth, playerHeight, playerSprite, playerSpeed, playerSize);
	std::vector<Entity> enemies;
	for (float j = -3.0; j <= 3.4; j += 6.4 / 10.0) {
		for (float i = 3.5; i >= 2.5; i -= .5) {
			enemies.push_back(Entity(j, i, enemyWidth, enemyHeight, enemySprite, enemySpeed, enemySize));
		}
	}
	//Entity enemy(0, 0, enemyWidth, enemyHeight, &enemySprite, enemySpeed, playerSize);
	Entity playerLaser(0, 0, playerLaserWidth, playerLaserHeight, playerLaserSprite, playerLaserSpeed, playerLaserSize);
	playerLaser.active = false;
	Entity enemyLaser(0, 0, enemyLaserWidth, enemyLaserHeight, enemyLaserSprite, enemyLaserSpeed, enemyLaserSize);
	enemyLaser.active = false;


	projectionMatrix.setOrthoProjection(-3.55, 3.55, -4.0f, 4.0f, -1.0f, 1.0f);

	float lastFrameTicks = 0.0f;
	state = STATE_TITLE_SCREEN;

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
		program.setModelMatrix(modelMatrix);


		switch (state) {
			case STATE_TITLE_SCREEN:
				renderTitleScreen(modelMatrix, program, font);
				processTitleScreen();
				break;
			case GAME:
				renderGame(modelMatrix, program, player, enemies, playerLaser, enemyLaser);
				processGame(player, playerLaser, elapsed);
				updateGame(player, playerLaser, enemies, enemyLaser, elapsed);
				break;
			case GAME_OVER:
				renderGameOver(modelMatrix, program, font);
				break;
			case WIN:
				renderWin(modelMatrix, program, font);
				break;
		}

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}