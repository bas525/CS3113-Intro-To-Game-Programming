#include "LevelThree.h"

GLuint spriteSheetThree;
Enemy3 meteor;
std::vector<float> startXs;
std::vector<Enemy3> meteors;
float lineY;
float counter;
float startX;
float startY;
bool nextOne;
int curMeteor;
float levelThreeTime;

float levelThreeCooldown;

float randomNum(float left, float right) {
	float random = (float)(rand() % 1000);
	random = random / 1000;
	random = random*(right - left);
	random += left;
	return random;
}

void initLevelThree()
{
	lineY = -2.4;
	startXs = *new std::vector<float>;
	meteors = *new std::vector<Enemy3>;
	curMeteor = 0;
	levelThreeTime = 0;
	counter = 0;
	spriteSheetThree = LoadTexture("spriteSheet.png");
	startX = -3;
	startY = 4.5;
	for (int i = 0; i < 10; i++) {
		startXs.push_back(randomNum(-3.2, 3.2));
	}
	float nSpeed = 3;
	for (int i = 0; i < 10; i++) {
		meteors.push_back(Enemy3(startXs[i], startY, nSpeed, &spriteSheetThree));
		nSpeed += .2;
	}

	meteor = *new Enemy3(startX, startY, 3, &spriteSheetThree);

	levelThreeCooldown = 2;
}



void drawLevelThree(ShaderProgram *program)
{
	if (levelThreeCooldown > 0) DrawText("Level 3", .8, -.4, -1, 0);
	//DrawText("Space Striker", .8, -.4, -2.5, -3);
	drawExplosions();
	if(meteors[curMeteor].active) meteors[curMeteor].drawSprite(*program);
	meteor.drawSprite(*program);

	drawPlayer();

}

void processLevelThree(float elapsed)
{
	if (levelThreeCooldown > 0) return;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_P]) {
		setStateLevelThree();
	}
	if (keys[SDL_SCANCODE_R]) setStateTitleScreen();

	float x = 0, y = 0;
	if (keys[SDL_SCANCODE_LEFT]) { x = -1; }
	else if (keys[SDL_SCANCODE_RIGHT]) { x = 1; }
	if (keys[SDL_SCANCODE_UP]) { y = 1; }
	else if (keys[SDL_SCANCODE_DOWN]) { y = -1; }

	if (keys[SDL_SCANCODE_C]) { playerLaserFire(); }
	if (keys[SDL_SCANCODE_X]) { playerSword(); }
	if (keys[SDL_SCANCODE_Q]) quiteGame();

	processPlayer(x, y, elapsed);
}

float easeIn(float from, float to, float time) {
	float tVal = time*time;
	return (1.0f - tVal)*from + tVal*to;
}

float easeOut(float from, float to, float time) {
	float oneMinusT = 1.0f - time;
	float tVal = 1.0f - (oneMinusT * oneMinusT);
	return (1.0f - tVal)*from + tVal*to;
}

float easeInOut(float from, float to, float time) {
	float tVal;
	if (time > 0.5) {
		float oneMinusT = 1.0f - ((0.5f - time)*-2.0f);
		tVal = 1.0f - ((oneMinusT * oneMinusT) * 0.5f);
	}
	else {
		time *= 2.0;
		tVal = (time*time) / 2.0;
	}
	return (1.0f - tVal)*from + tVal*to;
}

float easeOutElastic(float from, float to, float time) {
	float p = 0.3f;
	float s = p / 4.0f;
	float diff = (to - from);
	return from + diff + (diff*pow(2.0f, -5.0f*time) * sin((time - s)*(2 * 3.14159) / p));
}

float timeBasedY(float start, float cur, float end) {
	float num = abs(end - cur);
	float den = abs(end - start);
	float q = num / den;
	return 1 - q;
}

void updateLevelThree(float elapsed)
{
	if (levelThreeCooldown > 0) {
		levelThreeCooldown -= elapsed;
		return;
	}
	scrollBackground(.2, elapsed);
	updateExplosions(elapsed);

	float direction = 1;
	levelThreeTime += elapsed;
	std::vector<std::vector<Vect>> laserCords = playerLaserCord();
	std::vector<bool> laserActive = playerLaserActive();

	float time = timeBasedY(startY, meteors[curMeteor].position.y, -3.2);
	float newX = 0;
	if (curMeteor <= 2) {
		newX = easeOut(startXs[curMeteor], playerX(), time);
	}
	else if (curMeteor <= 5) {
		newX = easeInOut(startXs[curMeteor], playerX(), time);
	}
	else if (curMeteor <= 7) {
		newX = easeIn(startXs[curMeteor], playerX(), time);
	}
	else {
		newX = easeOutElastic(startXs[curMeteor], playerX(), time);
	}

	meteors[curMeteor].Update(newX, 0, elapsed);

	Vect penetration;
	if (meteors[curMeteor].active) {
		if (checkSATCollision(playerPoints(), meteors[curMeteor].globPoints(), penetration)) {
			//std::cout << "true" << std::endl;
			setStateGameOver();
		}
	}
	if (playerSwordActive() && meteors[curMeteor].active) {
		if (checkSATCollision(playerSwordCord(), meteors[curMeteor].globPoints(), penetration)) {
			startExplosionHere(meteors[curMeteor].position.x, meteors[curMeteor].position.y, meteors[curMeteor].size);
			meteors[curMeteor].active = false;
			playEnemy3Death();
		}
	}
	//std::cout << std::endl;
	updatePlayer(elapsed);
	int counter = 1;
	//float time = timeBasedY(startY, meteor.position.y, -3);
	//float newX = easeIn(startX, playerX(), time);
	//float newX = easeOut(startX, playerX(), time);
	//float newX = easeInOut(startX, playerX(), time);
	//float newX = easeOutElastic(startX, playerX(), time);
	//float currDiffX = abs(meteor.position.x - playerX());
	//newX = std::min(newX, currDiffX);
	//meteor.Update(newX, 0, elapsed);
	//std::cout << meteor.position.x << "  " << playerX() << std::endl;
	if (meteors[curMeteor].position.y <= -4.5) {
		curMeteor++;
	}



	if (curMeteor == 10) {
		setStateWin();
	}
}
