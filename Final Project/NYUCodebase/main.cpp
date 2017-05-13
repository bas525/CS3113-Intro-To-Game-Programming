

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "main.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ShaderProgram.h"
#include "Vect.h"
//#include "Entity1.h"
#include "Player.h"
#include "Enemy.h"
#include "Collision.h"
#include "TextDrawer.h"
#include "SheetSprite.h"
#include "TitleScreen.h"
#include "LevelOne.h"
#include "LevelTwo.h"
#include "LevelThree.h"
#include "GameOverScreen.h"
#include "WinScreen.h"
#include <SDL_mixer.h>
using namespace std;

SDL_Window* displayWindow;
float gameWidth = 640;
float gameHeight = 720;
float windowWidth = 7.1;

enum GameState { STATE_TITLE_SCREEN, LEVEL_ONE, LEVEL_TWO, LEVEL_THREE, GAME, GAME_OVER, WIN, COLLISION_ASSIGNMENT };
GameState state;

bool done;

ShaderProgram *program;
Matrix *shortcut;

Player *player;

float pi = 3.1415926;


Mix_Chunk *laserSound;
Mix_Chunk *playerDieSound;
Mix_Chunk *enemyDieSound;
Mix_Chunk *reflectSound;
Mix_Chunk *enemy2DeathSound;
Mix_Chunk *swordSound;
Mix_Chunk *playerLaserSound;
Mix_Chunk *meteorDeathSound;
Mix_Chunk *playerDeathSound;
Mix_Chunk *enemy2LaserSound;
Mix_Music *music;

SheetSprite * backgroundSprite;

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

class SpriteSheet {
public:
	GLuint sheet;
	int spriteCountX;
	int spriteCountY;
	int spriteHeight;
	int spriteWidth;

	SpriteSheet(GLuint &sheetp, int x, int y, int h, int w) {
		sheet = sheetp;
		spriteCountX = x;
		spriteCountY = y;
		spriteHeight = h;
		spriteWidth = w;
	}
};

class Explosions {
public:
	float x;
	float y;
	float size;
	SpriteSheet *spriteSheet;
	std::vector<int> spriteIndexes;
	float animationElapsed = 0.0f;
	float framesPerSecond;
	int curIndex;
	bool active;
	Matrix matrix;

	Explosions(float xp, float yp, float sizep, SpriteSheet *spriteSheetp) {
		float x = xp;
		float y = yp;
		size = sizep;
		spriteSheet = spriteSheetp;
		spriteIndexes = {12, 13, 14, 15, 16, 17};
		animationElapsed = 0.0f;
		framesPerSecond = 9.0f;
		curIndex = 0;
		active = false;
	}
	void DrawSpriteSheetSprite(ShaderProgram *program) {
		float u = (float)(((int)spriteIndexes[curIndex]) % spriteSheet->spriteCountX) / (float)spriteSheet->spriteCountX;
		float v = (float)(((int)spriteIndexes[curIndex]) / spriteSheet->spriteCountX) / (float)spriteSheet->spriteCountY;
		float spriteWidth = 1.0 / (float)spriteSheet->spriteCountX;
		float spriteHeight = 1.0 / (float)spriteSheet->spriteCountY;

		GLfloat texCoords[] = {
			u, v + spriteHeight,
			u + spriteWidth, v,
			u, v,
			u + spriteWidth, v,
			u, v + spriteHeight,
			u + spriteWidth, v + spriteHeight
		};
		float vertices[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };

		glBindTexture(GL_TEXTURE_2D, spriteSheet->sheet);

		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);
		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}
	void drawSprite(ShaderProgram &program) {
		matrix.identity();
		matrix.Translate(x, y, 0);
		matrix.Scale(size, size, 0);
		program.setModelMatrix(matrix);
		DrawSpriteSheetSprite(&program);
		//sprite->drawSprite(&program);
	}
	void updateSprite(float elapsed) {
		if (!active) return;
		animationElapsed += elapsed;
		if (animationElapsed > 1.0 / framesPerSecond) {
			curIndex++;
			animationElapsed = 0.0;

			if (curIndex > spriteIndexes.size() - 1) {
				curIndex = 0;
				turnOff();
			}
		}
	}

	void turnOn(float xp, float yp, float sizep) {
		x = xp;
		y = yp;
		size = sizep;
		active = true;
		curIndex = 0;
	}
	void turnOff() {
		active = false;
		curIndex = 0;
	}
};

std::vector<Explosions> explosionSet;

ShaderProgram * getProgram() {
	return program;
}

vector<Vect> playerPoints() {
	vector<Vect> points = player->globPoints();
	return points;
}

void drawPlayer() {
	player->drawLasers(*program);
	player->drawSword(*program);
	player->drawSprite(*program);
}

void processPlayer(float x, float y, float elapsed) {
	player->Input(x, y, elapsed);
}

void updatePlayer(float elapsed) {
	player->Update(elapsed);
}

float playerX()
{
	return player->position.x;
}

void playerLaserFire() {
	if (player->fireLaser()) {
		Mix_PlayChannel(-1, playerLaserSound, 0);
	}

}

std::vector<float> laserXPos()
{
	return player->getAllLaserX();
}

std::vector<std::vector<Vect>> playerLaserCord()
{
	return player->laserCoordinates();
}

std::vector<bool> playerLaserActive()
{
	return player->activeLasers();
}

std::vector<bool> playerDangerousLasers()
{
	return player->dangerousLasers();
}

void playerSetLaserOff(int i) {
	player->setLaserOff(i);
}
void playerLaserBounce(int i) {
	player->laserBounce(i);
}
void playerSword()
{
	if (player->swordCanPlaySound()) {
		Mix_PlayChannel(-1, swordSound, 0);
	}
	player->swordAttack();
}

std::vector<Vect> playerSwordCord()
{
	return player->swordCord();
}

bool playerSwordActive()
{
	return player->swordActive();
}


void resetPlayer() {
	player->position = Vect(0, -3, 0);
	player->resetAllLasers();
	player->resetSword();
}

void setStateCollision() {
	state = COLLISION_ASSIGNMENT;
}
void setStateTitleScreen() {
	turnAllExplosionsOff();
	state = STATE_TITLE_SCREEN;
	TitleScreenInit();
}
void setStateLevelOne() {
	turnAllExplosionsOff();
	state = LEVEL_ONE;
	initLevelOne();
	resetPlayer();
}
void setStateLevelTwo()
{
	turnAllExplosionsOff();
	state = LEVEL_TWO;
	initLevelTwo();
	resetPlayer();
}
void setStateLevelThree()
{
	turnAllExplosionsOff();
	state = LEVEL_THREE;
	initLevelThree();
	resetPlayer();
}
void setStateWin() {
	turnAllExplosionsOff();
	state = WIN;
}
void setStateGameOver() {
	turnAllExplosionsOff();
	Mix_PlayChannel(-1, playerDeathSound, 0);
	state = GAME_OVER;
}

void playReflect()
{
	Mix_PlayChannel(-1, reflectSound, 0);
}

void playEnemy3Death()
{
	Mix_PlayChannel(-1, meteorDeathSound, 0);
}

void playEnemy2Laser()
{
	Mix_PlayChannel(-1, enemy2LaserSound, 0);
}

void playEnemy2Death()
{
	Mix_PlayChannel(-1, enemy2DeathSound, 0);
}

void playEnemy1Laser()
{
	Mix_PlayChannel(-1, laserSound, 0);
}

void playEnemy1Death()
{
	Mix_PlayChannel(-1, enemyDieSound, 0);
}

void SetupGame() {
	drawerInit();
	state = STATE_TITLE_SCREEN;
	Mix_PlayMusic(music, -1);
}

void drawBackground(ShaderProgram *program) {
	backgroundSprite->drawSprite(program);
}

void scrollBackground(float speed, float elapsed) {
	backgroundSprite->scroll(speed, elapsed);
}

void startExplosionHere(float x, float y, float size) {
	for (Explosions &exp : explosionSet) {
		if (!exp.active) {
			exp.turnOn(x, y, size);
			return;
		}

	}
}

void turnAllExplosionsOff() {
	for (Explosions &exp : explosionSet) {
		exp.turnOff();
		exp.active = false;
		exp.curIndex = 0;
	}
}

void drawExplosions() {
	for (Explosions &exp : explosionSet) {
		if (exp.active) {
			exp.drawSprite(*program);
		}
	}
}

void updateExplosions(float elapsed) {
	for (Explosions &exp : explosionSet) {
		if (exp.active) {
			exp.updateSprite(elapsed);
		}
	}
}

void quiteGame() {
	turnAllExplosionsOff();
	done = true;
}

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	//displayWindow = SDL_CreateWindow("Collisions", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	displayWindow = SDL_CreateWindow("Space Striker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gameWidth, gameHeight, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	music = Mix_LoadMUS("Background.mp3");
	laserSound = Mix_LoadWAV("Laser.wav");
	enemyDieSound = Mix_LoadWAV("enemyExplode.wav");
	playerDieSound = Mix_LoadWAV("playerExplode.wav");
	reflectSound = Mix_LoadWAV("reflect.wav");
	swordSound = Mix_LoadWAV("swordSlash.wav");
	playerLaserSound = Mix_LoadWAV("playerLaser.wav");
	enemy2DeathSound = Mix_LoadWAV("enemy2Death.wav");
	playerDeathSound = Mix_LoadWAV("playerDeath.wav");
	meteorDeathSound = Mix_LoadWAV("meteorDestroy.wav");
	enemy2LaserSound = Mix_LoadWAV("enemy2Laser.wav");

	//glViewport(0, 0, 640, 360);
	glViewport(0, 0, (int)gameWidth, (int)gameHeight);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ShaderProgram gameProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	program = &gameProgram;

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	//projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	projectionMatrix.setOrthoProjection(-windowWidth / 2, windowWidth / 2, -(windowWidth*gameHeight) / (2 * gameWidth), (windowWidth*gameHeight) / (2 * gameWidth), -1.0f, 1.0f);

	shortcut = &modelMatrix;

	GLuint spriteSheet = LoadTexture("spriteSheet.png");
	player = new Player(&spriteSheet);

	GLuint backgroundSheet = LoadTexture("background.png");
	backgroundSprite = new SheetSprite(&backgroundSheet, 0.0f/ 1024.0f, 0.0f/1024.0f, 600.0f/ 1024.0f, 800.0f/ 1024.0f, 10.0f);

	GLuint explosionTextures = LoadTexture(RESOURCE_FOLDER"explosion.png");
	SpriteSheet explosionSheet(explosionTextures, 18, 1, 32, 32);

	for (int i = 0; i < 10; i++) {
		explosionSet.push_back(Explosions(0, 0, 0, &explosionSheet));
	}

	SetupGame();


	float lastFrameTicks = 0.0f;

	glUseProgram(program->programID);

	SDL_Event event;
	done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		shortcut = &modelMatrix;

		program->setProjectionMatrix(projectionMatrix);
		program->setViewMatrix(viewMatrix);
		program->setModelMatrix(modelMatrix);

		drawBackground(program);


		switch (state) {
		case STATE_TITLE_SCREEN:
			drawTitleScreen();
			processTitleScreen();
			updateTitleScreen(elapsed);
			break;
		case LEVEL_ONE:
			drawLevelOne(program);
			processLevelOne(elapsed);
			updateLevelOne(elapsed);
			break;
		case LEVEL_TWO:
			drawLevelTwo(program);
			processLevelTwo(elapsed);
			updateLevelTwo(elapsed);
			break;
		case LEVEL_THREE:
			drawLevelThree(program);
			processLevelThree(elapsed);
			updateLevelThree(elapsed);
			break;
		case GAME_OVER:
			drawGameOverScreen();
			processGameOverScreen();
			updateGameOverScreen();
			break;
		case WIN:
			drawWinScreen();
			processWinScreen();
			updateWinScreen(elapsed);
			break;
		}
		SDL_GL_SwapWindow(displayWindow);
	}
	std::cout << "Quiting" << endl;
	Mix_FreeMusic(music);
	Mix_FreeChunk(laserSound);
	Mix_FreeChunk(playerDieSound);
	Mix_FreeChunk(enemyDieSound);
	std::cout << "Quiting" << endl;
	//Doing this return before SDL_Quit() because SDL_Quit() just freezes and doesn't close
	return 0;
	SDL_Quit();
	std::cout << "Quiting" << endl;
	return 0;
}