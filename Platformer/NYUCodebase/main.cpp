/**********
*Brandan Sankar
*CS 3113
*Pong
*Controls:
*	Side arrow keys to move
*   Up arrow to jump
*   Move to the left and touch the yellow
*   door to win
*   Press P at start, win, or lose screen
*   to start playing
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
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ShaderProgram.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
SDL_Window* displayWindow;

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
#define TILE_SIZE 1.0f
#define SCREEN_SIZE 1000

class Entity;
class SpriteSheet;

string levelFile = "PlatformerMap.txt";
float mapWidth = -1;
float mapHeight = -1;

float projectionScale = 5;
float playerX = 0, playerY = 0, playerSpeed = 1;
float entitySize = 2;

unsigned char ** levelData;

enum GameState { STATE_TITLE_SCREEN, GAME, GAME_OVER, WIN };
GameState state;
//SpriteSheet * globChar;

vector<int> playerSprites = { 0, 1, 2, 3 };// { 0, 1, 2, 3, 4, 5, 6, 7 };
vector<int> snakeSprites = { 25, 26, 27 };
vector<int> groundBlocks = { 0, 33, 51, 53, 60 };
vector<int> winBlocks = { 145, 169 };
//The sides are based on the sides of the sprite, not the size of the 
//texture coordinates used to get said sprites. These aren't perfect
//but the closest I could get in reasonable time. If it's .025
//{top, bottom, right, right}
vector<float> playerSides = { .025f, .025f, .18f, .22f };
vector<float> enemySides = { .025f, .025f, .025f, .025f };
vector<Entity> entities;


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

void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
	*gridX = (int)(worldX / TILE_SIZE);
	*gridY = (int)(-worldY / TILE_SIZE);
}

class Entity {
public:

	float x;
	float y;

	SpriteSheet *spriteSheet;
	std::vector<int> spriteIndexes;
	float animationElapsed = 0.0f;
	float framesPerSecond;
	int curIndex;

	float size;

	float speed;
	float velocity_x;
	float velocity_y;

	float acceleration_x;
	float acceleration_y;

	float direction_x;

	string type;
	vector<float> sides;

	int counter;

	Entity(float xp, float yp, float sizep, SpriteSheet *spriteSheetp, std::vector<int> &spriteIndexesp, float speedp, string typep) {
		x = xp;
		y = yp;
		size = sizep;

		spriteSheet = spriteSheetp;
		spriteIndexes = spriteIndexesp;
		animationElapsed = 0.0f;
		framesPerSecond = 9.0f;
		curIndex = 0;

		speed = speedp;
		velocity_y = 0;

		direction_x = 1;
		type = typep;

		counter = 0;
		
	}


	void DrawSpriteSheetSprite(ShaderProgram *program) {
		float u = (float)(((int)spriteIndexes[curIndex]) % spriteSheet->spriteCountX) / (float)spriteSheet->spriteCountX;
		float v = (float)(((int)spriteIndexes[curIndex]) / spriteSheet->spriteCountX) / (float)spriteSheet->spriteCountY;
		float spriteWidth = 1.0 / (float)spriteSheet->spriteCountX;
		float spriteHeight = 1.0 / (float)spriteSheet->spriteCountY;

		//I'm adding .0025 to v because the above sprite is getting shown otherwise
		GLfloat texCoords[] = {
			u, v + spriteHeight,
			//u + spriteWidth, v,
			u + spriteWidth, v + .0025,
			//u, v,
			u, v + .0025,
			//u + spriteWidth, v,
			u + spriteWidth, v + .0025,
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

	void drawSprite(ShaderProgram &program, Matrix &modelMatrix) {
		modelMatrix.identity();
		modelMatrix.Translate(x, y, 0);
		modelMatrix.Scale(size*direction_x, size, 0);
		program.setModelMatrix(modelMatrix);
		DrawSpriteSheetSprite(&program);
		//sprite->drawSprite(&program);

	}

	void updateSprite(float elapsed) {
		animationElapsed += elapsed;
		if (velocity_x != 0 && animationElapsed > 1.0 / framesPerSecond) {
			curIndex++;
			animationElapsed = 0.0;

			if (curIndex > spriteIndexes.size() - 1) {
				curIndex = 0;
			}
		}
		else if (velocity_x == 0) {
			curIndex = 0;
			animationElapsed = 0.0;
		}
	}
	void playerInput(int inX, bool inY, float elapsed) {
		if (inY) {
			velocity_y = .5;
		}
		if (inX == 1) {
			velocity_x += .01;
			//x += .2;
			direction_x = 1;
			if (velocity_x > .1) {
				velocity_x = .1;
			}
		}
		if (inX == -1) {
			velocity_x -= .01;
			direction_x = -1;
			//x -= .2;
			if (velocity_x < -.1) {
				velocity_x = -.1;
			}
		}
	}

	void update(float elapsed) {
		updateSprite(elapsed);
		wall();
		gravity();
		drag();
		y += velocity_y;
		x += velocity_x;
		if (left() <= 0) {
			x += -left() + .01;
		}
		if (right() >= 49) {
			x -= right()-49 - .01;
		}
	}

	void enemyUpdate(float elapsed) {
		updateSprite(elapsed);
		if (counter >= 350) {
			counter = 0;
			direction_x *= -1;
		}
		velocity_x = .02;
		x += velocity_x*direction_x;
		counter++;
	}

	//The following four functions are based on the
	//sides of the sprite that are gotten.
	//The exact measurements were approximations made
	//through just figuring out when the collisions occur
	float top() { return y + .5*size*.05; }
	float bottom() { return y - .5*size*.05; }
	float right() { return x - .18*size; }
	float left() { return x - .22*size; }
	float enemyTop() { return y + .6 * size; }
	float enemyBottom() { return y - .4 * size; }
	float enemyRight() { return x + .4*size; }
	float enemyLeft() { return x - .9*size; }
	

	//top, bottom, right, left
	vector<float> edges() {
		vector<float> edges;
		
		if (type == "Player") {
			edges.push_back(top());
			edges.push_back(bottom());
			edges.push_back(right());
			edges.push_back(left());
		}
		else if (type == "Snake") {
			edges.push_back(enemyTop());
			edges.push_back(enemyBottom());
			edges.push_back(enemyRight());
			edges.push_back(enemyLeft());
		}
		return edges;
	}


	bool grounded() {
		int gridX, gridY;
		worldToTileCoordinates(x, y, &gridX, &gridY);
		int belowTile = levelData[gridY+1][gridX];// cout << belowTile << endl;
		//cout << y << "  " << gridY << endl;
		//cout << velocity_y << endl;
		if (find(groundBlocks.begin(), groundBlocks.end(), belowTile) != groundBlocks.end() && bottom() < gridY) {
		//if (bottom() < - gridY) {
			y += -gridY -bottom();
			return true;
		}
		return false;
	}

	bool didWin() {
		int gridX, gridY;
		worldToTileCoordinates(x, y, &gridX, &gridY);
		int curBlock = levelData[gridY][gridX];
		worldToTileCoordinates(x, y, &gridX, &gridY);
		cout << curBlock << endl;
		if (find(winBlocks.begin(), winBlocks.end(), curBlock) != winBlocks.end()) {
			return true;
		}
		return false;
	}

	void wall() {
		int gridX, gridY;
		worldToTileCoordinates(x, y, &gridX, &gridY);
		int rightTile = levelData[gridY+1][gridX+1];
		if(find(groundBlocks.begin(), groundBlocks.end(), rightTile) != groundBlocks.end() && right() > gridX) {
			x -= right() - gridX;
		}
		int leftTile = levelData[gridY+1][gridX-1];
		if (find(groundBlocks.begin(), groundBlocks.end(), leftTile) != groundBlocks.end() && left() < gridX) {
			x += gridX - left();
		}
		
	}

	void gravity() {
		velocity_y -= .015;
		if (velocity_y <= -.4) {
			velocity_y = -.4;
		}
		
		//velocity_y = -.1;
	}

	
	void drag() {
		//cout << velocity_x << endl;
		if (velocity_x > 0) {
			velocity_x -= .003;
			if (velocity_x < 0) {
				velocity_x = 0;
			}
		}
		else if (velocity_x < 0) {
			velocity_x += .003;
			if (velocity_x > 0) {
				velocity_x = 0;
			}
		}

	}
};

//top, bottom, right, left
bool collision(Entity &a, Entity &b) {
	vector<float> aEdges = a.edges();
	vector<float> bEdges = b.edges();
	if (aEdges[1] < bEdges[0] && aEdges[0] > bEdges[1]) {
		if (aEdges[2] > bEdges[3] && aEdges[3] < bEdges[2]) {
			//DrawText(programs, fonts, "true", .4, 0);
			return true;
		}
	}
	return false;
}

bool readHeader(ifstream &stream) {
	string line;
	mapWidth = -1;
	mapHeight = -1;
	while (getline(stream, line)) {
		if (line == "") { break; }

		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);

		if (key == "width") {
			mapWidth = atoi(value.c_str());
		}
		else if (key == "height") {
			mapHeight = atoi(value.c_str());
		}
	}

	if (mapWidth == -1 || mapHeight == -1) {
		return false;
	}
	else {// allocate our map data
		levelData = new unsigned char*[mapHeight];
		for (int i = 0; i < mapHeight; ++i){
			levelData[i] = new unsigned char[mapWidth];
		}
		return true;
	}
}

bool readLayerData(ifstream &stream) {
	string line;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < mapHeight; y++) {
				getline(stream, line);
				istringstream lineStream(line);
				string title;

				for (int x = 0; x < mapWidth; x++) {
					getline(lineStream, title, ',');
					unsigned char val = (unsigned char)atoi(title.c_str());
					if (val > -1) {
						levelData[y][x] = val - 1;
					}
					else {
						levelData[y][x] = -1;
					}
				}
			}
		}
	}
	return true;
}

void placeEntity(string type, float x, float y, SpriteSheet *spriteSheet) {
	if (type == "Player") {
		Entity entity(x, y, entitySize, spriteSheet, playerSprites, playerSpeed, "Player");
		entities.push_back(entity);
	}
	if (type == "Snake") {
		Entity entity(x, y, entitySize, spriteSheet, snakeSprites, 1, "Snake");
		entities.push_back(entity);
	}
}

bool readEntityData(ifstream &stream, SpriteSheet *spriteSheet) {
	string line;
	string type;

	while (getline(stream, line)) {
		if (line == "") { break; }

		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);

		if (key == "type") {
			type = value;
		}
		else if (key == "location") {
			istringstream lineStream(value);
			string xPosition, yPosition;
			getline(lineStream, xPosition, ',');
			getline(lineStream, yPosition, ',');

			float placeX = atoi(xPosition.c_str())*TILE_SIZE;
			float placeY = atoi(yPosition.c_str())*-TILE_SIZE;

			placeEntity(type, placeX, placeY, spriteSheet);
		}
	}
	return true;
}

void readMapFile(SpriteSheet *charSheet) {
	ifstream infile(levelFile);
	string line;
	while (getline(infile, line)) {
		if (line == "[header]") {
			if (!readHeader(infile)) {
				return;
			}
		}
		else if (line == "[layer]") {
			readLayerData(infile);
		}
		else if (line == "[ObjectsLayer]") {
			readEntityData(infile, charSheet);
		}
	}
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

	glDrawArrays(GL_TRIANGLES, 0, 6 * text.size());

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}


void drawMap(SpriteSheet *spriteSheet, ShaderProgram *program, Matrix &modelMatrix) {
	modelMatrix.identity();
	program->setModelMatrix(modelMatrix);

	std::vector<float> vertexData;
	std::vector<float> texCoordData; 

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			float u = (float)(((int)levelData[y][x]) % spriteSheet->spriteCountX) / (float)spriteSheet->spriteCountX;
			float v = (float)(((int)levelData[y][x]) / spriteSheet->spriteCountX) / (float)spriteSheet->spriteCountY;
			float spriteWidth = 1.0 / (float)spriteSheet->spriteCountX;
			float spriteHeight = 1.0 / (float)spriteSheet->spriteCountY;

			vertexData.insert(vertexData.end(), {
				TILE_SIZE * x, -TILE_SIZE * y,
				TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
				(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
				TILE_SIZE * x, -TILE_SIZE * y,
				(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
				(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
			});
			texCoordData.insert(texCoordData.end(), {
				u + .0025f, v + .0025f,
				u + .0025f, v + (spriteHeight) - .0025f,
				u + spriteWidth - .0025f, v + (spriteHeight) - .0025f,
				u + .0025f, v + .0025f,
				u + spriteWidth - .0025f, v + (spriteHeight) - .0025f,
				u + spriteWidth - .0025f, v + .0025f
			});
		}
	}
	//float vertices[];
	glBindTexture(GL_TEXTURE_2D, spriteSheet->sheet);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, &vertexData[0]);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, &texCoordData[0]);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6*mapHeight*mapWidth);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}


void input(Entity &player, float elapsed) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	bool ground = player.grounded();
	if (ground && keys[SDL_SCANCODE_UP]) {
		player.playerInput(0, true, elapsed);
	}
	if (keys[SDL_SCANCODE_RIGHT]) {
		player.playerInput(1, false, elapsed);
	}
	if (keys[SDL_SCANCODE_LEFT]) {
		player.playerInput(-1, false, elapsed);
	}
}

int l = 0;
void Update(float elapsed, std::vector<Entity> &entities, ShaderProgram &program, Matrix &modelMatrix) {
	//player.update(elapsed);
	Entity *player = &entities[2];
	player->update(elapsed);
	for (Entity &entity : entities) {
		if (entity.type == "Snake") {
			entity.enemyUpdate(elapsed);
			if (collision(*player, entity)) {
				state = GAME_OVER;
				//cout << "TRUE" << endl;
			}
			//if (l > 0) { cout << player->bottom() << "   " << entity.enemyBottom() << "   " << entity.y << endl; }
			//else { cout << endl; }
			/*
			if(l > 0 && player -> right() > entity.enemyRight()) { cout << player->right() << "   " << entity.enemyRight() << "   " << entity.x << endl; }
			else { cout << endl; }
			//if (l > 0) { cout << player->right() << "   " << entity.enemyLeft() << "   " << entity.x <<  endl; */
			//l++;
			
		}
	}
	l = 0;
	//player->grounded();

	input(*player, elapsed);

	if (player->didWin()) state = WIN;
}

void renderTitleScreen(Matrix &modelMatrix, ShaderProgram &program, GLuint &font) {
	modelMatrix.identity();
	modelMatrix.Translate(-3, 1, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Platformer", 1, -.55);

	modelMatrix.identity();
	modelMatrix.Translate(-2.5, 0, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Press P to play the game", .25, -.12);

	modelMatrix.identity();
	modelMatrix.Translate(-2.5, -.5, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Arrow Keys to move left and right", .25, -.12);

	modelMatrix.identity();
	modelMatrix.Translate(-2.5, -1, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Up Arrow to jump", .25, -.12);

	modelMatrix.identity();
	modelMatrix.Translate(-2.5, -1.5, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Reach the goal at the end", .25, -.12);
}

void processTitleScreen() {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_P]) {
		state = GAME;
	}
}

void renderGameOver(Matrix &modelMatrix, ShaderProgram &program, GLuint &font) {
	modelMatrix.identity();
	modelMatrix.Translate(-2.25, 0, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Game Over You Lost", .5, -.25);
	modelMatrix.identity();
	modelMatrix.Translate(-2, -1, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Press P to play again", .25, -.12);
}

void processGameOver() {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_P]) {
		entities[2].x = 2; entities[2].y = -11;
		state = GAME;
	}
}
void renderWin(Matrix &modelMatrix, ShaderProgram &program, GLuint &font) {
	modelMatrix.identity();
	modelMatrix.Translate(-2.75, 0, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Congratulations You Win", .5, -.25);
	modelMatrix.identity();
	modelMatrix.Translate(-2, -1, 0);
	program.setModelMatrix(modelMatrix);
	DrawText(&program, font, "Press P to play again", .25, -.12);
}

int main(int argc, char *argv[]) {


	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Platformer Assignment", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	//displayWindow = SDL_CreateWindow("Platformer Assignment", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 320, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	glViewport(0, 0, 640, 360);
	//glViewport(0, 0, 1000, 320);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	//projectionMatrix.setOrthoProjection(0, 50, -16, 0, -1.0f, 1.0f);

	GLuint font = LoadTexture(RESOURCE_FOLDER"font.png");
	GLuint mapTextures = LoadTexture(RESOURCE_FOLDER"dirt-tiles.png");
	GLuint characterTextures = LoadTexture(RESOURCE_FOLDER"characters_3.png");
	SpriteSheet characterSheet(characterTextures, 8, 4, 32, 32);
	SpriteSheet mapSheet(mapTextures, 24, 16, 16, 16);
	
	readMapFile(&characterSheet);

	state = STATE_TITLE_SCREEN;
	float lastFrameTicks = 0.0f;
	float fixedElapsed = 0;
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

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		program.setModelMatrix(modelMatrix);
		switch (state) {

			case STATE_TITLE_SCREEN:
				projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
				renderTitleScreen(modelMatrix, program, font);
				processTitleScreen();
				break;

			case GAME:
				
				projectionMatrix.setOrthoProjection(entities[2].x - 14.22, entities[2].x + 14.22, -16, 0, -1.0f, 1.0f);
				fixedElapsed = elapsed;
				if (fixedElapsed > FIXED_TIMESTEP *  MAX_TIMESTEPS) {
					fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
				}
				while (fixedElapsed >= FIXED_TIMESTEP) {
					fixedElapsed -= FIXED_TIMESTEP;
					Update(FIXED_TIMESTEP, entities, program, modelMatrix);
				}
				Update(fixedElapsed, entities, program, modelMatrix);
				drawMap(&mapSheet, &program, modelMatrix);
				for (Entity &entity : entities) {
					entity.drawSprite(program, modelMatrix);
					//entity.updateSprite(elapsed);
				}
				break;
			case GAME_OVER:
				projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
				renderGameOver(modelMatrix, program, font);
				processGameOver();
				break;
			case WIN:
				projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
				renderWin(modelMatrix, program, font);
				processGameOver();
				break;

		}


		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
