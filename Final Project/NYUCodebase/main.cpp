

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
using namespace std;

SDL_Window* displayWindow;
float gameWidth = 640;
float gameHeight = 720;
float windowWidth = 7.1;

enum GameState { STATE_TITLE_SCREEN, LEVEL_ONE, LEVEL_TWO, LEVEL_THREE, GAME, GAME_OVER, WIN, COLLISION_ASSIGNMENT };
GameState state;

ShaderProgram *program;

Player *player;


float pi = 3.1415926;

float redX = 0, redY = -1, redW = 1.5, redH = 1, redS = .5, redR = 45, redSp = 3,
blueX = -2, blueY = 1, blueW = 3, blueH = .5, blueS = .3, blueR = -30, blueSp = 1,
whiteX = 2, whiteY = 1, whiteW = 2, whiteH = .7, whiteS = .7, whiteR = 30, whiteSp = 1;

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
	player->fireLaser();
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
}

void setStateCollision() {
	state = COLLISION_ASSIGNMENT;
}
void setStateTitleScreen() {
	state = STATE_TITLE_SCREEN;
}
void setStateLevelOne() {
	state = LEVEL_ONE;
	initLevelOne();
	resetPlayer();
}
void setStateLevelTwo()
{
	state = LEVEL_TWO;
	initLevelTwo();
	resetPlayer();
}
void setStateLevelThree()
{
	state = LEVEL_THREE;
	initLevelThree();
	resetPlayer();
}
void setStateWin() {
	state = WIN;
}
void setStateGameOver() {
	state = GAME_OVER;
}


void Start(GLuint &redT, GLuint &blueT, GLuint &whiteT, Entity1 &red, Entity1 &blue, Entity1 &white) {


	redT = LoadTexture(RESOURCE_FOLDER"red.png");
	blueT = LoadTexture(RESOURCE_FOLDER"blue.png");
	whiteT = LoadTexture(RESOURCE_FOLDER"white.png");

	red = Entity1(redX, redY, redW, redH, redS, redR, redSp, redT);
	blue = Entity1(blueX, blueY, blueW, blueH, blueS, blueR, blueSp, blueT);
	white = Entity1(whiteX, whiteY, whiteW, whiteH, whiteS, whiteR, whiteSp, whiteT);
}

void Draw(Entity1 &red, Entity1 &blue, Entity1 &white, ShaderProgram &program, Matrix &modelMatrix) {
	blue.Draw(program, modelMatrix);
	white.Draw(program, modelMatrix);
	red.Draw(program, modelMatrix);
	DrawText("test", 1, 0);
}

void Input(Entity1 &red, float elapsed) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	float x = 0, y = 0;
	if (keys[SDL_SCANCODE_LEFT]) { x = -1; }
	else if (keys[SDL_SCANCODE_RIGHT]) { x = 1; }
	if (keys[SDL_SCANCODE_UP]) { y = 1; }
	else if (keys[SDL_SCANCODE_DOWN]) { y = -1; }

	red.Input(x, y, elapsed);
	player->Input(x, y, elapsed);
}

void Update(Entity1 &red, Entity1 &blue, Entity1& white, GLuint texture) {
	Vect penetration;
	red.Update();
	blue.Update();
	white.Update();
	if (checkSATCollision(red.globPoints(), blue.globPoints(), penetration)) {
		red.collisionMove(penetration);
		Vect opp;
		opp.x = -penetration.x;
		opp.y = -penetration.y;
		blue.collisionMove(opp);
		//red.collisionAction(blue.texture);
	}
	else if (checkSATCollision(red.globPoints(), white.globPoints(), penetration)) {
		red.collisionAction(white.texture);
	}
	else {
		red.collisionAction(texture);
	}
	if (checkSATCollision(player->globPoints(), blue.globPoints(), penetration)) {
		cout << "works" << endl;
	}
	else cout << endl;
}

void SetupGame() {
	drawerInit();
	state = STATE_TITLE_SCREEN;
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



	GLuint redTexture, blueTexture, whiteTexture;
	Entity1 red, blue, white;
	Start(redTexture, blueTexture, whiteTexture, red, blue, white);

	GLuint spriteSheet = LoadTexture("spriteSheet.png");
	player = new Player(&spriteSheet);

	SetupGame();


	float lastFrameTicks = 0.0f;

	glUseProgram(program->programID);

	SDL_Event event;
	bool done = false;
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

		program->setProjectionMatrix(projectionMatrix);
		program->setViewMatrix(viewMatrix);
		program->setModelMatrix(modelMatrix);


		switch (state) {
		case STATE_TITLE_SCREEN:
			drawTitleScreen();
			processTitleScreen();
			updateTitleScreen();
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
		case COLLISION_ASSIGNMENT:
			Draw(red, blue, white, *program, modelMatrix);
			player->drawSprite(*program);
			Input(red, elapsed);
			Update(red, blue, white, redTexture);
			break;
		}
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}