#include "LevelOne.h"

GLuint spriteSheetOne;
std::vector<std::vector<Enemy>> enemies;
std::vector<EnemyLaser1> enemyLasers;

float levelOneCooldown;

float enemyCountX = 6;
float enemyCountY = 3;
float levelOneTime;
int totalEnemiesAlive;
void initLevelOne()
{
	enemies = *new std::vector<std::vector<Enemy>>;
	enemyLasers = *new std::vector<EnemyLaser1>;
	levelOneTime = 0;
	totalEnemiesAlive = 0;

	float enemyRight = 2.9;
	float enemyLeft = -2.1;
	float enemyTop = 4;
	float enemyBottom = 2;
	float enemyX = enemyLeft, enemyY = enemyBottom;
	spriteSheetOne = LoadTexture("spriteSheet.png");
	for (int i = 0; i < enemyCountX; i++) {
		std::vector<Enemy> row;
		enemyY = enemyBottom;
		for (int j = 0; j < enemyCountY; j++) {
			row.push_back(Enemy(enemyX, enemyY, &spriteSheetOne));
			enemyY += (enemyTop - enemyBottom) / enemyCountY;
			totalEnemiesAlive++;
		}
		enemyX += (enemyRight - enemyLeft) / enemyCountX;
		enemies.push_back(row);
	}
	for (int i = 0; i < 3; i++) {
		enemyLasers.push_back(EnemyLaser1(&spriteSheetOne));
	}
	/*
	for (float i = enemyLeft; i <= enemyRight; i+=(enemyRight-enemyLeft)/enemyCountX) {
		for (float j = enemyBottom; j < enemyTop; j+=(enemyTop-enemyBottom)/enemyCountY) {
			enemies.push_back(Enemy(i, j, &spriteSheetOne));
		}
	}*/
	levelOneCooldown = 1;
}

void drawLevelOne(ShaderProgram *program)
{
	if (levelOneCooldown > 0) DrawText("Level 1", .8, -.4, -1, 0);
	//DrawText("Space Striker", .8, -.4, -2.5, -3);
	/*for (Enemy &enemy : enemies) {
		enemy.drawSprite(*program);
	}*/
	for (int i = 0; i < enemyCountX; i++) {
		for (int j = 0; j < enemyCountY; j++) {
			if (enemies[i][j].active) {
				enemies[i][j].drawSprite(*program);
			}
		}
	}
	for (EnemyLaser1 &enemyLaser : enemyLasers) {

		if (enemyLaser.active) {
			enemyLaser.drawSprite(*program);
		}
	}
	drawPlayer();
	
}

void processLevelOne(float elapsed)
{
	if (levelOneCooldown > 0) return;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_P]) {
		setStateLevelOne();
	}
	if (keys[SDL_SCANCODE_R]) setStateTitleScreen();

	float x = 0, y = 0;
	if (keys[SDL_SCANCODE_LEFT]) { x = -1; }
	else if (keys[SDL_SCANCODE_RIGHT]) { x = 1; }
	if (keys[SDL_SCANCODE_UP]) { y = 1; }
	else if (keys[SDL_SCANCODE_DOWN]) { y = -1; }

	if (keys[SDL_SCANCODE_C]) { playerLaserFire(); }
	if (keys[SDL_SCANCODE_X]) { playerSword(); }

	processPlayer(x, y, elapsed);
}

void updateLevelOne(float elapsed)
{
	if (levelOneCooldown > 0) {
		levelOneCooldown -= elapsed;
		return;
	}
	scrollBackground(.1, elapsed);
	float direction = 1;
	levelOneTime += elapsed;
	std::vector<std::vector<Vect>> laserCords = playerLaserCord();
	std::vector<bool> laserActive = playerLaserActive();
	for (int i = 0; i < enemyCountX; i++) {
		for (int j = 0; j < enemyCountY; j++) {
			enemies[i][j].Update(sin(levelOneTime)*.55*direction, cos(levelOneTime * 2)*direction, elapsed);
			direction *= -1;
			if (enemies[i][j].active) {
				Vect penetration;
				for (EnemyLaser1 &enemyLaser : enemyLasers) {

					if (!enemyLaser.active) {
						int random = rand() % 100;
						//std::cout << enemyLasers.size() << std::endl;
						if (random == 0) {
							enemyLaser.turnOn(enemies[i][j].position);
							playEnemy1Laser();
							break;
						}
					}
				}

				if (checkSATCollision(playerPoints(), enemies[i][j].globPoints(), penetration)) {
					setStateGameOver();
				}

				for (int k = 0; k < laserActive.size(); k++) {
					if (laserActive[k]) {
						if (checkSATCollision(laserCords[k], enemies[i][j].globPoints(), penetration)) {
							playerSetLaserOff(k);
							enemies[i][j].Die();
							playEnemy1Death();
						}
					}
				}
			}
		}
	}
	for (EnemyLaser1 &enemyLaser : enemyLasers) {
		if (enemyLaser.active) {
			enemyLaser.Update(elapsed);
			Vect penetration;
			if (checkSATCollision(playerPoints(), enemyLaser.globPoints(), penetration)) {
				setStateGameOver();
				//std::cout << "True" << std::endl;
			}
		}
	}
	updatePlayer(elapsed);
	int counter = 0;
	for (int i = 0; i < enemyCountX; i++) {
		for (int j = 0; j < enemyCountY; j++) {
			if (enemies[i][j].active) counter++;
		}
	}
	if (counter == 0) {
		setStateLevelTwo();
	}
}
