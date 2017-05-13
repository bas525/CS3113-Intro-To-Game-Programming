#include "LevelTwo.h"

GLuint spriteSheetTwo;
std::vector<std::vector<Enemy2>> enemies2;
std::vector<EnemyLaser2> enemyLasers2;

float enemyCountX2 = 4;
float enemyCountY2 = 2;
float levelTwoTime;
void initLevelTwo()
{
	enemies2 = *new std::vector<std::vector<Enemy2>>;
	enemyLasers2 = *new std::vector<EnemyLaser2>;
	levelTwoTime = 0;

	float enemyRight = 2.9;
	float enemyLeft = -2.1;
	float enemyTop = 4;
	float enemyBottom = 2;
	float enemyX = enemyLeft, enemyY = enemyBottom;
	spriteSheetTwo = LoadTexture("spriteSheet.png");
	for (int i = 0; i < enemyCountX2; i++) {
		std::vector<Enemy2> row;
		enemyY = enemyBottom;
		for (int j = 0; j < enemyCountY2; j++) {
			row.push_back(Enemy2(enemyX, enemyY, &spriteSheetTwo));
			enemyY += (enemyTop - enemyBottom) / enemyCountY2;

		}
		enemyX += (enemyRight - enemyLeft) / enemyCountX2;
		enemies2.push_back(row);
	}
	for (int i = 0; i < 3; i++) {
		enemyLasers2.push_back(EnemyLaser2(&spriteSheetTwo));
	}
	/*
	for (float i = enemyLeft; i <= enemyRight; i+=(enemyRight-enemyLeft)/enemyCountX2) {
	for (float j = enemyBottom; j < enemyTop; j+=(enemyTop-enemyBottom)/enemyCountY2) {
	enemies2.push_back(Enemy2(i, j, &spriteSheetTwo));
	}
	}*/

}

void drawLevelTwo(ShaderProgram *program)
{

	DrawText("Space Striker", .8, -.4, -2.5, -3);
	/*for (Enemy2 &enemy : enemies2) {
	enemy.drawSprite(*program);
	}*/
	for (int i = 0; i < enemyCountX2; i++) {
		for (int j = 0; j < enemyCountY2; j++) {
			if (enemies2[i][j].active) {
				enemies2[i][j].drawSprite(*program);
			}
		}
	}
	for (EnemyLaser2 &enemyLaser : enemyLasers2) {

		if (enemyLaser.active) {
			enemyLaser.drawSprite(*program);
		}
	}
	drawPlayer();

}

void processLevelTwo(float elapsed)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_P]) {
		setStateLevelTwo();
	}

	float x = 0, y = 0;
	if (keys[SDL_SCANCODE_LEFT]) { x = -1; }
	else if (keys[SDL_SCANCODE_RIGHT]) { x = 1; }
	if (keys[SDL_SCANCODE_UP]) { y = 1; }
	else if (keys[SDL_SCANCODE_DOWN]) { y = -1; }

	if (keys[SDL_SCANCODE_C]) { playerLaserFire(); }
	if (keys[SDL_SCANCODE_X]) { playerSword(); }
	processPlayer(x, y, elapsed);
}

void updateLevelTwo(float elapsed)
{
	float direction = 1;
	levelTwoTime += elapsed;
	std::vector<std::vector<Vect>> laserCords = playerLaserCord();
	std::vector<bool> laserActive = playerLaserActive();
	std::vector<bool> dangerousLasers = playerDangerousLasers();
	int temp = 0;
	for (bool ds : laserActive) {
		if (ds) temp++;
	}
	//std::cout << temp << std::endl;
	for (int i = 0; i < enemyCountX2; i++) {
		for (int j = 0; j < enemyCountY2; j++) {

			if (enemies2[i][j].active) {
				Vect penetration;
				for (EnemyLaser2 &enemyLaser : enemyLasers2) {

					if (!enemyLaser.active) {
						int random = rand() % 100;
						//std::cout << enemyLasers2.size() << std::endl;
						if (random == 0) {
							enemyLaser.turnOn(enemies2[i][j].position);
							break;
						}
					}
				}

				if (checkSATCollision(playerPoints(), enemies2[i][j].globPoints(), penetration)) {
					setStateGameOver();
				}

				for (int k = 0; k < laserActive.size(); k++) {
					if (laserActive[k]) {
						if (checkSATCollision(laserCords[k], enemies2[i][j].globPoints(), penetration)) {
							//playerSetLaserOff(k);
							//enemies2[i][j].Die();
							playerLaserBounce(k);
						}
					}
				}
				for (EnemyLaser2 &enemyLaser : enemyLasers2) {
					if (enemyLaser.active && enemyLaser.hurtUser) {
						Vect penetration;
						if (checkSATCollision(enemies2[i][j].globPoints(), enemyLaser.globPoints(), penetration)) {
							enemies2[i][j].Die();
							enemyLaser.damagedSelf();
						}
					}
				}
			}
		}
	}
	for (EnemyLaser2 &enemyLaser : enemyLasers2) {
		enemyLaser.Update(elapsed);
		if (enemyLaser.active && !enemyLaser.hurtUser) {
			
			Vect penetration;
			if (checkSATCollision(playerPoints(), enemyLaser.globPoints(), penetration)) {
				setStateGameOver();
			}
			if (playerSwordActive()) {
				if (checkSATCollision(playerSwordCord(), enemyLaser.globPoints(), penetration)) {
					enemyLaser.bounceBack();
				}
			}
		}

	}	updatePlayer(elapsed);
	int counter = 0;
	for (int i = 0; i < enemyCountX2; i++) {
		for (int j = 0; j < enemyCountY2; j++) {
			if (enemies2[i][j].active) counter++;
		}
	}
	for (int i = 0; i < dangerousLasers.size(); i++) {
		if (laserActive[i] && dangerousLasers[i]) {
			Vect penetration;
			if (checkSATCollision(playerPoints(), laserCords[i], penetration)) {
				setStateGameOver();
			}
		}
	}
	if (counter == 0) {
		setStateLevelThree();
	}
}
